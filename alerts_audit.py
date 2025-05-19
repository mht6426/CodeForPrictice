# -*- coding: utf-8 -*-
import xml.etree.ElementTree as ET
import smtplib
import subprocess
import mysql.connector
import os
import sys
import socket
import struct
from datetime import datetime, timedelta
from email.mime.text import MIMEText
from email.header import Header
from email.utils import formataddr

class ConfigReader:

    CONFIG_FILE_PATH = '/etc/log_alert.conf'
    TIME_FILE_PATH = '/etc/log_alert_timestamp.conf'

    def __init__(self):
        self.sys_log_levels = []
        self.alert_way_emails = []
        self.alert_T = None
        self.admin_log_levels = []
        self.load_config()

    def load_config(self):
        try:
            with open(self.CONFIG_FILE_PATH, 'r') as f:
                lines = f.readlines()
                for line in lines:
                    line = line.strip() 
                    if not line:
                        continue  

                    if line.startswith('alert sys_log'):
                        parts = line.split()  
                        if len(parts) < 3:  
                            raise ValueError("'alert sys_log' configuration line format error, example: 'alert sys_log 1' or 'alert sys_log 1,2,3'")
                        levels_part = parts[2].strip()
                        self.sys_log_levels = [int(level) for level in levels_part.split(',')]
                        if not self.sys_log_levels:
                            raise ValueError("'alert sys_log' parsed log level list is empty")
                    elif line.startswith('alert admin_log'):
                        parts = line.split()  
                        if len(parts) < 3:
                            raise ValueError("'alert admin_log' configuration line format error, example: 'alert admin_log 4' or 'alert admin_log 4,5,6'")
                        levels_part = parts[2].strip()
                        self.admin_log_levels = [int(level) for level in levels_part.split(',')]
                        if not self.admin_log_levels:
                            raise ValueError("'alert admin_log' parsed log level list is empty")
                    elif line.startswith('alert_way'):
                        parts = line.split()  
                        if len(parts) < 2:  
                            raise ValueError("'alert_way' configuration line format error, example: 'alert_way desktop email/xxx@163.com/'")
                        email_part = None
                        for part in parts[1:]:  
                            if 'email/' in part:
                                email_part = part
                                break
                        if email_part is None:
                            raise ValueError("'alert_way' configuration line missing 'email/' prefix")
                        self.alert_way_emails = email_part.split('email/')[1].split('/')[0].split(';')
                        if not self.alert_way_emails:
                            raise ValueError("'alert_way' parsed recipient email list is empty")
                    elif line.startswith('alert_T'):
                        parts = line.split()  
                        if len(parts) < 2:  
                            raise ValueError("'alert_T' configuration line format error, example: 'alert_T 7'")
                        self.alert_T = int(parts[1].strip())  
                        if self.alert_T <= 0:
                            raise ValueError("Configuration item 'alert_T' value {} is invalid, must be a positive integer".format(self.alert_T))
                
                if self.alert_T is None:
                    raise ValueError("Configuration item 'alert_T' not found in config file")
                if not self.alert_way_emails:
                    raise ValueError("Configuration item 'alert_way' not found in config file")
                if not self.sys_log_levels:
                    raise ValueError("Configuration item 'alert sys_log' not found in config file")
                if not self.admin_log_levels:
                    raise ValueError("Configuration item 'alert admin_log' not found in config file")
        except FileNotFoundError:
            raise FileNotFoundError("Configuration file {} not found, please ensure it exists".format(self.CONFIG_FILE_PATH))
        except Exception as e:
            print("Error reading config file: {}".format(e))
            raise SystemExit(1)

    def timestamp_file(self, table_name):
        current_time = datetime.now()
        alert_days_ago = current_time - timedelta(days=self.alert_T)
        current_time_str = current_time.strftime('%Y-%m-%d %H:%M:%S')

        try:
            if not os.path.exists(self.TIME_FILE_PATH):
                print("File {} does not exist, creating...".format(self.TIME_FILE_PATH))
                with open(self.TIME_FILE_PATH, 'w') as f:
                    f.write("{} {}\n".format(table_name, current_time_str))
                return alert_days_ago.strftime('%Y-%m-%d %H:%M:%S')

            with open(self.TIME_FILE_PATH, 'r') as f:
                lines = f.readlines()

            result_time = alert_days_ago
            found = False
            updated_lines = []

            for line in lines:
                stripped_line = line.strip()
                if not stripped_line:
                    continue

                parts = stripped_line.split(maxsplit=1)
                if len(parts) != 2:
                    continue

                name, timestamp_str = parts
                if name == table_name:
                    found = True
                    try:
                        original_timestamp = datetime.strptime(timestamp_str, '%Y-%m-%d %H:%M:%S')
                        if original_timestamp > alert_days_ago:
                            result_time = original_timestamp
                    except ValueError:
                        print("Invalid timestamp format in line: {}. Using alert_T days ago.".format(line))

                    updated_lines.append("{} {}\n".format(table_name, current_time_str))
                else:
                    updated_lines.append(line)

            if not found:
                print("Table {} not found in {}, appending new line...".format(table_name, self.TIME_FILE_PATH))
                updated_lines.append("{} {}\n".format(table_name, current_time_str))

            with open(self.TIME_FILE_PATH, 'w') as f:
                f.writelines(updated_lines)

            return result_time.strftime('%Y-%m-%d %H:%M:%S')

        except Exception as e:
            print("Error operating file {}: {}".format(self.TIME_FILE_PATH, e))
            return alert_days_ago.strftime('%Y-%m-%d %H:%M:%S')

class MySQLConnector:

    def __init__(self, database, config_reader):
        self.host = 'localhost'
        self.user = 'root'
        self.password = ''
        self.database = database
        self.connection = None
        self.config_reader = config_reader  

    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.connection:
            self.connection.close()

    def connect(self):
        try:
            self.connection = mysql.connector.connect(
                host=self.host,
                user=self.user,
                password=self.password,
                database=self.database
            )
            print("Successfully connected to MySQL database")
        except mysql.connector.Error as e:
            print("Error connecting to MySQL database: {}".format(e))
            raise

    def _build_query(self, table_name):
        timestamp_str = self.config_reader.timestamp_file(table_name)

        if table_name == 'sys_log':
            log_levels = self.config_reader.sys_log_levels
            create_time = '`time`'
        elif table_name == 'opt_log':
            log_levels = self.config_reader.admin_log_levels
            create_time = 'opt_time'
        else:
            raise ValueError("Unsupported table name: {}, cannot build query".format(table_name))

        level_conditions = ""
        if log_levels:
            level_conditions = " AND level IN ({})".format(','.join(map(str, log_levels)))

        return """SELECT * FROM {} WHERE {} >= '{}' {} ORDER BY {} DESC;""".format(table_name, create_time, timestamp_str, level_conditions, create_time).strip()  

    def get_data(self, table_name):
        if self.connection and self.connection.is_connected():
            try:
                mysql_query = self._build_query(table_name)
                print("Executing MySQL query: {}".format(mysql_query))
                with self.connection.cursor(dictionary=True) as cursor:
                    cursor.execute(mysql_query)
                    return cursor.fetchall()
            except mysql.connector.Error as e:
                print("Error fetching data from {} table: {}".format(table_name, e))
                raise
        else:
            raise RuntimeError("MySQL connection not established or disconnected")
            
class SMTPConnector:
    def __init__(self, config_reader, config_command=['superfw', 'smtpconf', 'list']):
        self.server = None
        self.port = None
        self.username = None
        self.password = None
        self.smtp_server = None
        self.alias = None
        self.encryption = None
        self.config_command = config_command
        self.config_reader = config_reader

    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.disconnect()

    def _convert_level(self, level_value, table_name):
        try:
            level = int(level_value)
            if table_name == 'sys_log':
                if level == 1:
                    return "严重异常"
                elif level == 2:
                    return "错误信息"
                elif level == 3:
                    return "普通信息"
                else:
                    return str(level_value)
            elif table_name == 'opt_log':
                if level in {0, 1}:
                    return "低"
                elif level in {2, 3}:
                    return "中"
                elif level >= 4:
                    return "高"
                else:
                    return str(level_value)
            else:
                return str(level_value)
        except (ValueError, TypeError):
            return "N/A"

    def _convert_varbinary_ip(self, ip_data):
        try:
            if isinstance(ip_data, (bytes, bytearray)):
                ip_bytes = bytes(ip_data)  
                if len(ip_bytes) == 4:
                    try:
                        ip_int = struct.unpack('!I', struct.pack('I', int.from_bytes(ip_bytes, byteorder='little')))[0]
                        ip_bytes = ip_int.to_bytes(4, byteorder='big')
                        return socket.inet_ntoa(ip_bytes)
                    except Exception as e:
                        print("Failed to convert byte order: {}".format(e))
                elif len(ip_bytes) == 16:
                    return socket.inet_ntop(socket.AF_INET6, ip_bytes)
        except socket.error as se:
            print("Socket error while converting IP data: {} - {}".format(ip_data, se))
        except Exception as e:
            print("Unexpected error while converting IP data: {} - {}".format(ip_data, e))
        print("Warning: Failed to convert IP data: {}".format(ip_data))
        return str(ip_data)

    def send_email(self, sys_log_data, opt_log_data):
        recipient = self.config_reader.alert_way_emails
        if not recipient:
            print("Warning: Recipient email not configured")
            return
        
        log_content = ""
        
        if sys_log_data:
            sys_log_count = len(sys_log_data)
            sys_header = "系统日志（共 {} 条）:\n{}  {}  {}  {}  {}  {}".format(
                sys_log_count,
                "序号", "时间", "进程名", "进程PID", "描述", "日志级别"
            )
            sys_log_lines = []
            for i, log in enumerate(sys_log_data):
                try:
                    log_line = "{}  {}  {}  {}  {}  {}".format(
                        i + 1,
                        str(log.get('time', 'N/A')),
                        str(log.get('progname', 'N/A')),
                        str(log.get('pid', 'N/A')),
                        str(log.get('msg', 'N/A')),
                        self._convert_level(log.get('level', 'N/A'), 'sys_log')
                    )
                    sys_log_lines.append(log_line)
                except Exception as e:
                    sys_log_lines.append("{}  处理出错: {}".format(i + 1, str(e).encode('utf-8', 'replace').decode('utf-8')))
            sys_log_content = "\n".join([sys_header] + sys_log_lines)
        else:
            sys_log_content = "无新系统日志"
        
        if opt_log_data:
            opt_log_count = len(opt_log_data)
            opt_header = "操作日志（共 {} 条）:\n{} {}  {}  {}  {}  {}  {}".format(
                opt_log_count,
                "序号", "时间", "用户", "访问来源", "操作模块", "描述", "日志级别"
            )
            opt_log_lines = []
            for i, log in enumerate(opt_log_data):
                try:
                    log_line = "{} {}  {}  {}  {}  {}  {}".format(
                        i + 1,
                        str(log.get('opt_time', 'N/A')),
                        str(log.get('user_name', 'N/A')),
                        self._convert_varbinary_ip(log.get('client_ip', 'N/A')),
                        str(log.get('module', 'N/A')),
                        str(log.get('desc', 'N/A')),
                        self._convert_level(log.get('level', 'N/A'), 'opt_log')
                    )
                    opt_log_lines.append(log_line)
                except Exception as e:
                    print("处理操作日志第 {} 条记录时出错: {}".format(i + 1, str(e).encode('utf-8', 'replace').decode('utf-8')))
                    opt_log_lines.append("{}  处理出错: {}".format(i + 1, str(e).encode('utf-8', 'replace').decode('utf-8')))
            opt_log_content = "\n".join([opt_header] + opt_log_lines)
        else:
            opt_log_content = "无新操作日志"
        
        log_content = sys_log_content + "\n\n" + opt_log_content
        
        try:
            msg = MIMEText(log_content, 'plain', 'utf-8')
            msg['From'] = formataddr((str(Header(self.alias, 'utf-8')), self.username))
            msg['To'] = Header('; '.join(recipient), 'utf-8')
            msg['Subject'] = Header('系统日志与操作日志', 'utf-8')
            
            failed_recipients = self.smtp_server.sendmail(
                from_addr=self.username,
                to_addrs=recipient,
                msg=msg.as_string()
            )
            if failed_recipients:
                print("部分邮件发送失败:")
                for email, error in failed_recipients.items():
                    print("收件人 {} 发送失败，错误信息: {}".format(email, error))
            else:
                print("系统与操作日志 email 成功发送给所有收件人: {}".format(recipient))
        except UnicodeEncodeError as ue:
            print("Unicode 编码错误: {}".format(ue))
            print("出错的内容: ", log_content.encode('utf-8', 'replace').decode('utf-8'))
        except Exception as e:
            print("系统与操作日志 email sending failed: {}".format(str(e).encode('utf-8', 'replace').decode('utf-8')))
            
    def load_config(self):
        try:
            process = subprocess.Popen(self.config_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
            stdout, stderr = process.communicate()
            if process.returncode != 0:
                raise subprocess.CalledProcessError(process.returncode, self.config_command, output=stdout, stderr=stderr)
            root = ET.fromstring(stdout)

            if root.find('smtp_conf_enable').text.lower() == 'on':
                values = root.find('smtp_conf_value').text.split(',')
                if len(values) >= 6:
                    self.server, self.port, self.username, self.alias, self.password, self.encryption = values
                    self.port = int(self.port)
                    #print("SMTP server configuration loaded successfully{},{},{},{},{},{}".format(self.server,self.port,self.username,self.alias,self.password,self.encryption))
                    return True
        except subprocess.CalledProcessError as e:
            print("Error executing command: {}".format(e)) 
        except AttributeError as e:
            print("Error parsing XML: {}".format(e))
        except IndexError as e:
            print("Insufficient configuration values: {}".format(e))
        return False

    def connect(self):
        if self.load_config():
            try:
                if self.encryption == 'PLAIN':
                    self.smtp_server = smtplib.SMTP(self.server, self.port)
                elif self.encryption == 'OVERTLS':
                    self.smtp_server = smtplib.SMTP_SSL(self.server, self.port)
                elif self.encryption == 'STARTTLS':
                    self.smtp_server = smtplib.SMTP(self.server, self.port)
                    self.smtp_server.starttls()
                else:
                    self.smtp_server = smtplib.SMTP_SSL(self.server, self.port)
                self.smtp_server.login(self.username, self.password)
                print("Successfully connected to SMTP server")  
                return self.smtp_server
            except smtplib.SMTPConnectError as e:
                raise RuntimeError("Failed to connect to SMTP server: {}".format(e))  
            except smtplib.SMTPAuthenticationError as e:
                raise RuntimeError("SMTP authentication failed: Please check email account or authorization code")  
            except Exception as e:
                print("Runtime error: {}".format(e))  
        else:
            raise RuntimeError("SMTP configuration loading failed")

    def disconnect(self):
        if self.smtp_server:
            try:
                self.smtp_server.quit()
                print("Successfully disconnected from SMTP server")
            except Exception as e:
                print("Error disconnecting: {}".format(e))  
        else:
            print("Not connected to SMTP server, no need to disconnect")

    def connectivity_test(self, recipient_email):
        try:
            msg = MIMEText('你好,这是一封测试邮件', 'plain', 'utf-8')
            msg['From'] = formataddr((str(Header(self.alias, 'utf-8')), self.username))
            msg['To'] = Header(recipient_email, 'utf-8')
            msg['Subject'] = Header('Mailbox Connectivity Test'.encode('utf-8'), 'utf-8')
            self.smtp_server.sendmail(self.username, [recipient_email], msg.as_string())
            print("Mailbox connectivity test succeeded: {}".format(recipient_email))
            return True
        except Exception as e:
            print("Mailbox connectivity test failed: {}, error: {}".format(recipient_email, e))
            return False

def main():
    try:
        shared_config_reader = ConfigReader()
        args = sys.argv[1:]
        if args:
            if args[0] == 'audit_log':
                with SMTPConnector(shared_config_reader) as smtp_connector, \
                        MySQLConnector('firewall_pfw', shared_config_reader) as sys_mysql_connector, \
                        MySQLConnector('sysinfo', shared_config_reader) as opt_mysql_connector:
                    sys_log_data = sys_mysql_connector.get_data('sys_log')
                    opt_log_data = opt_mysql_connector.get_data('opt_log')
                    smtp_connector.send_email(sys_log_data, opt_log_data)
            elif args[0] == 'connectivity_test' and len(args) == 2:
                with SMTPConnector(shared_config_reader) as smtp_connector:
                    smtp_connector.connectivity_test(args[1])
            else:
                print("Parameter error. Usage:\n  audit_log\n  connectivity_test recipient_email")
        else:
            print("Please provide parameter: audit_log or connectivity_test recipient_email")
    except Exception as e:
        print("Runtime error: {}".format(e))

if __name__ == "__main__":
    main()
