//trx
#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE(buf) (sizeof(buf)/sizeof(buf[0]))
#define DEBUG 0
#define MAXNUM 10

typedef int SElemType;

typedef struct{
	SElemType data[MAXNUM];
	int top;
}SqStack;
typedef SqStack* LinkStack;

typedef struct {
	int a:16;//申请几位
	int b:16;
	int c:16;
}test;

void swap(int *a,int *b){
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void show(int *buf,int max){
	int i;
	for(i=0; i<max; i++) printf(" %d ",buf[i]);
	printf("\n");
}

//冒泡排序
void bubble(int *buf,int max){
	int i,j;
	for(i=0;i<max-1;i++){
		for(j=0;j<max-i-1;j++){
			if(buf[j] > buf[j+1])
				swap(&buf[j],&buf[j+1]);
		}
	}    
}

//快排排序
void quick(int *buf,int start,int end){
	int i,j;
	printf("---------\nstart = %d end = %d\n*********\n",start,end);
	if(start < end){
		i = start;
		j = end;
		while(i<j){
			while(i<j && buf[i]<buf[j]){
				j--;
				printf("i = %d\t j-- = %d\n",i,j);
			}
			if(i<j){
				swap(&buf[i],&buf[j]);
				i++;
				printf("i++ = %d\t j = %d\n",i,j);
			}
			
			while(i<j && buf[i]<buf[j]){
				i++;
				printf("i++ = %d\t j = %d\n",i,j);
			}
			if(i<j){
				swap(&buf[i],&buf[j]);
				j--;
				printf("i = %d\t j-- = %d\n",i,j);
			}
			show(buf,9);
		}
		printf("---------\nstart i-1\n*********\n");
		quick(buf,start,i-1);
		printf("---------\nstart i+1\n*********\n");
		quick(buf,i+1,end);
	}
}

//选择排序
void choice(int *buf,int max){
	int i,j;
	for(i=0;i<max;i++){
		for(j=i;j<max;j++){
			if(buf[i]>buf[j]){
				swap(&buf[i],&buf[j]);
			}
		}
	}
}
/*quick print
3       5       6       1       7       2       4       9       8
*/
int main(){
	
	int i = 0;
	/*
	第三位置 1 和 0
		int a = 1;
		printf("a = %d\n",a);
		a = a | (1 << 3);
		printf("a = %d\n",a);
		a = a & (~(1 << 3));
		printf("a = %d\n",a);
	*/

	/* 
	查看结构体申请位数 
		test t;
		t.a = 3;
		t.b = 2;
		t.c = 3;
		printf("a = %d\n",t.a);
		printf("b = %d\n",t.b);
		printf("c = %d\n",t.c);
		printf("sizeof test = %ld\n",sizeof(t));
	*/
	/* 
	冒泡排序
		int bubble_buf[9] = {3,5,6,1,7,2,4,9,8};
		bubble(bubble_buf,ARRAY_SIZE(bubble_buf));
		show(bubble_buf,9);
	*/
	/*
	选择排序
		int choice_buf[9] = {3,5,6,1,7,2,4,9,8};
		choice(choice_buf,ARRAY_SIZE(choice_buf));
		show(choice_buf,9);
	*/
	/*
	 快速排序
		int quick_buf[9] = {3,5,6,1,7,2,4,9,8};
		quick(quick_buf,0,ARRAY_SIZE(quick_buf));
		show(quick_buf,9);
	*/

	/* 
	TCP如何实现可靠通信的？
		TCP的可靠就是保证每次数据按序、按时、不丢数据，顺利的交付给对端，
			但是可靠不等于安全，TCP是明文通信的(明文传输：只发生在HTTP协议，HTTPS协议是在应用层与传输层之间加密的，所以在网络层还能明文传输)(通俗来说就是未加密)
		1.字节编号机制：TCP数据是按序的，接受完后按序组装好，再交给上层
		2.数据段确认机制：TCP确认应答就是每一个数据段发送都会收到接收端返回的一个确认号，收到的确认号表示该号前面的数据全部接收
			TCP可以一次连续发送多个数据段：TCP可以连续发送多个数据段，具体发送数据段的多少取决于对方返回的窗口大小。
				只要满足窗口大小可容纳，Negale算法处于关闭状态就可以连续发送多个数据段。
		3.TCP超时重传机制：比如说发送了 1,2,3,4 全部接收就反馈 4 若接收的为 1,2,4 则反馈2，缓存4,等接受到了3立马反馈4
	*/

	/*
	变量内存			32位    64位
	bool				1		1
	char				1		1
	unsigned char		1		1
	short int			2		2
	int					4		4
	unsigned int		4		4
	float				4		4
	long				4		8
	unsigned long		4		8
	double				8		8
	long long			8		8
	指针  				 4		 8
	*/

	//深度遍历用栈，广度遍历同队列
	//链表不支持随机访问，随机访问是顺序表的特性
	//顺序存储：存取第N个数据时，必须先访问前N-1个数据
	//N个节点的二叉链表，非空链域的个数为N-1个，因为根节点没人指向他
	//常用的线性结构：线性表，堆栈，队列，双队列，数组，串
	//常用的非线性结构：二位数组，多维数组，广义表，树，图
	//稀疏矩阵压缩的存储方法为三元组和十字链表
	//.org 非盈利组织,.net网络服务公司，.gov政府，.com商业公司,.edu教育
	//1××临时相应，2xx成功，3xx重新定向(301永久重定向，302临时重定向)，4xx请求错误(400,请求失败，401为授权，403已禁止，404,找不到网页)，
	//物理层主要功能为在终端设备间传送比特流
	//RJ11 电话接口，RJ45 网络接口
	
	/*
	关键字
	auto 
	static：
			修饰局部变量：提升局部变量的生存周期
			修饰全局变量：作用域被限制在源文件中
			修饰函数：作用域被限制在源文件中
	extern:用来声明全局变量，程序运行期间一直存在
	register：声明寄存器变量，也为局部变量，只在声明他的函数中有效，
			  对于需要频繁使用的变量使用他来声明会提高程序运行速度
	volatile:防止编译器优化
	*/

	/*
		指定内存地址存储数据，真tm的偏，了解了解就完事
		char * test = NULL;
		test = (char *)0x0312ff7c;//假设指定地址为0x0312ff7c
		*test = value;//自己赋值就好了
	*/

	/*
		#define OFFSET(s,p)      (size_t)(&(((st *)0)->p))  //偏移地址宏	
		typedef struct u{
			char q;
			int p;
		} st;		
		st s;
		size_t：unsigned int
		typedef：结构体起别名
		st s：定义结构体变量
		(st *)0：将0强转成结构体指针，地址为0
		&(st *)0->p：访问结构体指针成员p，最后在取地址就是偏移地址
		
		示例：
		int *add = (st *)0;
		printf("%p\n",add);      //结果是：nil  就是0
		printf("%p\n",(add+1));  //结果是：0x4
	*/

	/*
	#include<assert.h>
	char * strcpy(char *dst,const char *src)
	{
		assert(dst != NULL && src != NULL); //assert:错误判断
		char *ret = dst;
		while ((*dst++=*src++)!='\0');
		return ret;
	}
	*/

	/*与memcpy实现基本一致
	char *strncpy(char *dst, const char *src, size_t len)  
	{  
		assert(dst != NULL && src != NULL);  
		char *res = dst;  
		while (len--)  
		{  
			*dst++ = *src++;  
		}  
		return res;  
	}  
	*/

	/*
	strlen与sizeof的区别：
		strlen为函数，sizeof为关键字
		strlen 测量的是字符的实际长度，以'\0' 结束（不包含'\0' ）。而sizeof 测量的是字符的分配大小，如果未分配大小，则遇到'\0' 结束（包含'\0' ，也就是strlen测量的长度加1）
			，如果已经分配内存大小，返回的就是分配的内存大小。
	*/
	/*
	首先我们讨论一下什么是系统调用（system calls）？
		用户借助UNIX/linux直接提供的少量函数可以对文件和设备进行访问和控制，这些函数就是系统调用。
	*/

	/*
		Linux系统执行ls，执行了哪些系统调用？
		
	*/

	return 0;
}
