#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXSIZE 100
typedef int SElemType;

//数组方式实现栈 
typedef struct
{
	SElemType data[MAXSIZE];
	int top;	//用于栈顶指针 
}SqStack;
typedef SqStack* LinkStack;

/* 遍历栈 */
void display(LinkStack linkStack){
	int i;
	printf("栈：\n");
	for(i=linkStack->top;i>=0;i--){
		printf("%d\n",linkStack->data[i]);
	}
}

/* 初始化，建立一个空栈 */
LinkStack initStack(){
	LinkStack linkStack = (LinkStack)malloc(sizeof(SqStack));	//指针，指向栈
	linkStack->top = -1;	//表示空栈
	return linkStack; 
}
 
/* 销毁栈 */
void destroyStack(LinkStack linkStack){
	free(linkStack);
}

/* 清空栈 */
void clearStack(LinkStack linkStack){
	linkStack->top = -1;	//即置空栈 
}

/* 判断栈是否为空 */
bool stackEmpty(LinkStack linkStack){
	if(linkStack->top == -1)
		return true;
	else
		return false;
}

/* 判断栈是否满 */
bool stackFull(LinkStack linkStack){
	if(linkStack->top == MAXSIZE-1)
		return true;
	else
		return false;
}

/* 返回栈顶元素 */
bool getTop(LinkStack linkStack, SElemType *e){
	if(linkStack->top == -1){
		return false;
	}else{
		*e = linkStack->data[linkStack->top];
		return true;
	}
} 
/* 压栈 */
bool push(LinkStack linkStack, SElemType e){
	if(linkStack->top ==  MAXSIZE-1){
		printf("栈满！\n");
		return false;
	}else{
		linkStack->top++;
		linkStack->data[linkStack->top] =  e;
		return true;
	}
}

/* 出栈 */
bool pop(LinkStack linkStack, SElemType* e){
	if(linkStack->top == -1){
		printf("栈空！\n");
		return false;
	}else{
		*e = linkStack->data[linkStack->top];
		linkStack->top--;
		return true;
	}
}

/* 返回栈元素的个数 */
int stackLength(LinkStack linkStack){
	return linkStack->top+1;
}

/* 主函数 */
int main(){
	SElemType top;
	SElemType *topElem = &top;
	LinkStack linkStack = NULL;
	bool continueFlag = true;
	SElemType pushNum,popNum;
	int functionNum;	//功能号 
	while(continueFlag){
		printf("--------------菜单---------------\n");
		printf("------------1.初始化-------------\n");
		printf("------------2.销毁栈-------------\n");
		printf("------------3.清空栈-------------\n");
		printf("------------4.判断栈空-----------\n");
		printf("------------5.判断栈满-----------\n");
		printf("------------6.返回栈顶元素-------\n");
		printf("------------7.压栈---------------\n");
		printf("------------8.出栈---------------\n");
		printf("------------9.栈元素个数---------\n");
		printf("------------10.遍历栈------------\n");
		printf("------------11.退出\n"); 
		printf("输入功能号：\n");
		scanf("%d",&functionNum);
		switch(functionNum){
			case 1:
				linkStack = initStack();
				printf("初始化成功！\n");
				display(linkStack); 
				break;
			case 2:
				destroyStack(linkStack);
				printf("销毁成功！\n");
				display(linkStack); 
				break;
			case 3:
				clearStack(linkStack);
				printf("栈已置空\n");
				display(linkStack); 
				break;
			case 4:
				if(stackEmpty(linkStack)){
					printf("栈空！\n");
				}else{
					printf("栈非空！\n");
				}
				break;
			case 5:
				if(stackFull(linkStack)){
					printf("栈满！\n");
				}else{
					printf("栈未满！\n");
				} 
				break;
			case 6:
				if(getTop(linkStack, topElem)){
					printf("栈顶元素是：%d\n",top);
				}else{
					printf("栈空！\n");
				}
				break;
			case 7:
				printf("输入要压入栈的值：\n");
				scanf("%d",&pushNum);
				if(push(linkStack,pushNum)){
					printf("压栈成功！\n");
					display(linkStack);
				}
				break;
			case 8:
				if(pop(linkStack,topElem)){
					printf("出栈成功，栈顶元素是：%d\n",*topElem);
				}
				break;
			case 9:
				printf("栈内元素个数：%d\n",stackLength(linkStack));
				break;
			case 10:
				display(linkStack);
				break;
			case 11:
				printf("感谢使用！\n");
				continueFlag = false; 
				break; 
			default:
				printf("功能号不存在！\n");
				break;
		}
	}
	return 0;
} 
