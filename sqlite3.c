/*
*gcc student.c -lsqlite3
*arm-fsl-linux-gnueabi-gcc student.c -lsqlite3
*arm-fsl-linux-gnueabi-gcc student.c -L/root/sqlite/arm_build/lib -lsqlite3
*/
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#define DATABASE "terminal.db"

//插入函数
int do_insert(sqlite3 * db)
{
	int id;
	char name[32] = {};
	int score;
	char sql[128] = {};    //拼接
	char *errmsg;
	
	printf("Input id:");
	scanf("%d", &id);
	getchar();
	
	printf("Input name:");
	scanf("%s", name);
	getchar();

	printf("Input score:");
	scanf("%d", &score);
	getchar();
	
	sprintf(sql, "insert into stu values(%d, '%s', %d);", id, name, score); //拼接完成
	
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg ) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("Insert done.\n");	
	}
	
	return 0;	
}

//删除函数按照id
int do_delete(sqlite3 * db)
{
	int id;
	char sql[128] = {};    //拼接
	char *errmsg;
	
	printf("Input id:");
	scanf("%d", &id);
	getchar();
		
	sprintf(sql, "delete from stu where id = %d", id); //拼接完成
	
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg ) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("Delete done.\n");	
	}
	
	return 0;	
}

//更新函数按照输入id
int do_update(sqlite3 * db)
{
	int id;
	char sql[128] = {};    //拼接操作命令
	int score;
	char *errmsg;
	
	printf("Input update id:");
	scanf("%d", &id);
	getchar();
	
	printf("Update score:");
	scanf("%d", &score);
	getchar();
	
	sprintf(sql, "update stu set score = %d where id = %d", score, id); //拼接完成
	
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg ) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("Update done.\n");	
	}
	
	return 0;	
}

//查询函数用到的回调函数
int callback(void *para, int f_num, char **f_value, char **f_name)
{
	int i = 0;
	
	for(i = 0; i < f_num; i++)
	{
		printf("%-11s", f_value[i]); //%-11s是格式符,左对齐，右边填充空格
	}
	putchar(10);   //输出码表中ASCII码10对应的字符，也就是换行

	return 0;
}

//查询函数
int do_query(sqlite3 *db)
{
	char sql[128] = {};
	char *errmsg;
	
	sprintf(sql, "select * from stu;"); //查看整张表
	
	if(sqlite3_exec(db, sql, callback, NULL, &errmsg ) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("Query done.\n");	
	}
	
	return 0;
}

//查询函数，不使用回调函数
int do_query1(sqlite3 *db)
{
	char sql[128] = {};
	char *errmsg;
	char **resultp;  //二级指针取地址
	int nrow;  //定义一行
	int ncloumn;  //定义一列
	int index;
	
	int i, j;
	
	sprintf(sql, "select * from stu;"); //查看整张表
	
	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg) != SQLITE_OK)
	{
		printf("%s", errmsg);
	}
	else
	{
		printf("Query done.\n");
	}
	for(j = 0; j < ncloumn; j++)
	{
		printf("%-11s ", resultp[j]);
	}
	putchar(10);
	
	index = ncloumn;  //跨过第一行的列开始
	for(i = 0; i < nrow; i++)
	{
		for(j = 0; j < ncloumn; j++)
		{
			printf("%-11s ", resultp[index++]);  //第一行的列都不打
		}
		putchar(10);
	}
	
	return 0;
}


int main(int argc, const char *argv[])
{
	sqlite3 * db;
	char * errmsg;
	int cmd;
	
	if(sqlite3_open(DATABASE, &db) != SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	else
	{
		printf("Open DATABASE success.\n");
	}
	
	//创建一张数据库的表格，如果表格存在打印errmsg消息，不进行重复创建操作
	if(sqlite3_exec(db, "create table stu (id Integer, name char, score Integer);", NULL, NULL, &errmsg) != SQLITE_OK)                                     
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("create table or open success.\n");		
	}
	
	//输入cmd进行增、删、查、改、退出操作
	while(1)
	{
		printf("********************************\n");
		printf("1:insert 2:delete 3:query 4:update 5:quit.\n");
		printf("********************************\n");
	
		printf("Input cmd: ");
		scanf("%d", &cmd);
		getchar();     //吃掉回车，吃掉多余字符
		
		switch(cmd)
		{
			case 1:
				do_insert(db);
				break;
			case 2:
				do_delete(db);
				break;
			case 3:
				//do_query(db);     //调用回调函数进行查阅
				do_query1(db);      //不调用回调函数进行查阅
				break;				
			case 4:
				do_update(db);
				break;		
			case 5:
				sqlite3_close(db);
				exit(0);		
			default:
				printf("Error cmd.\n");		
		}
	}	
}
