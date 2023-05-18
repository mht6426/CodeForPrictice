#include <stdio.h>
#include <string.h>
 
//输入 I like beijing.
//输出 beijing. like I
void reverse(char* left, char* right)
{
	while (left<right)
	{
		char tmp = 0;
		tmp = *left;
		*left = *right;
		*right = tmp;
		left++;
		right--;
	}
}
 
int main()
{
	char arr[100] = { 0 };
	gets(arr);//获取字符串
	//将字符串倒置
	int len = strlen(arr);//计算字符串长度，以求最后一位的地址
	reverse(arr, arr+len-1);//进行倒置
 
	//将每个单词再倒置
	char* start = arr;//一个单词起始位置
	while (*start != '\0')
	{
		char* end = start;
		while (*end != ' ' && *end != '\0')//一个单词结束位置
		{
			end++;
		}
		reverse(start, end - 1);
		if (*end != '\0')
			start = end + 1;
		else
			start = end;
	}
	printf("%s\n", arr);
 
	return 0;
}