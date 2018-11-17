#include <stdio.h>
#include <math.h>
#include <malloc.h>
#pragma warning(disable:4996)；
struct yuansu { char data; double num; struct yuansu *next; struct yuansu *father; };//定义一个链表结构的数据类型，用来做栈计算

struct yuansu *pop(struct yuansu *R)//出栈，返回next
{
	struct yuansu *temp;
	temp = R->father;
	free(R);
	return temp;
}

struct yuansu *push(struct yuansu *zz, char a, double i)//入栈，返回新地址
{
	struct yuansu *temp;
	temp = (struct yuansu*)malloc(sizeof(struct yuansu));
	zz->next = temp;
	temp->father = zz;
	temp->next = NULL;
	temp->num = i;
	temp->data = a;
	return temp;

}

double sum(struct yuansu *S)//提供后缀表达式链表，计算最终结果
{
	struct yuansu *H;//计算时用到的计算栈
	H = (struct yuansu*)malloc(sizeof(struct yuansu));//初始化第一个地址
	while (S != NULL)
	{
		if (S->data == ' ')//如果数据为空格则内容为数字
		{
			H = push(H, ' ', S->num);//操作数入计算栈
		}
		else//否则为运算符
		{
			double A, B;//运算数暂时变量
			switch (S->data)
			{
			case '+': B = H->num; H = pop(H); A = H->num; H = pop(H); H = push(H, ' ', A + B); break;
			case '-':B = H->num; H = pop(H); A = H->num; H = pop(H); H = push(H, ' ', A - B); break;
			case '*':B = H->num; H = pop(H); A = H->num; H = pop(H); H = push(H, ' ', A * B); break;
			case '/':B = H->num; H = pop(H); A = H->num; H = pop(H); H = push(H, ' ', A / B); break;
			case '^':B = H->num; H = pop(H); A = H->num; H = pop(H); H = push(H, ' ', pow(A, B)); break;
			}

		}
		S = S->next;
	}

	return H->num;
}

int panduan(struct yuansu *S, char suanshi)//比较提供的链表中的符号情况，决定参数符号是否应该直接入栈。应该直接入栈返回1，不应该返回0,如是右括号连续输出直到左括号为止返回2.
{

	if (suanshi == '+' || suanshi == '-')//如果参数符号为加减
	{
		if (S->data == '*' || S->data == '^' || S->data == '/')//如果加减前面是乘除
		{
			return 0;
		}
		else if (S->data == '(')//如果加减前面是左括号
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (suanshi == '*' || suanshi == '/' || suanshi == '^')//如果参数是乘除次方
	{
		if (S->data == '+' || S->data == '-' || S->data == '(')//如果乘除前面是加减左括号
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (suanshi == ')')
	{
		return 2;
	}
	else if (suanshi == '(')//如果是左括号，直接入栈
	{
		return 1;
	}
	else
	{
		printf("未知错误！！-判断\n");
	}
}

struct yuansu *zhuanhuan(char suanshi[])//提供字符串类型的中缀表达式转换为双项链表结构的后缀表达式
{
	struct yuansu *F, *S, *r;
	double h = 0;
	F = (struct yuansu*)malloc(sizeof(struct yuansu));//正式后缀表达式链表
	r = F;
	F->father = NULL;
	F->next = NULL;
	S = (struct yuansu*)malloc(sizeof(struct yuansu));//运算符号堆栈链表
	S->father = NULL;
	while (*suanshi != '\0')//遍历整个字符串
	{
		if (*suanshi >= '0'&&*suanshi <= '9')//处理运算数字入F链表
		{
			int d = 0;
			h += (*suanshi - '0');
			suanshi++;
			while (*suanshi == '.' || *suanshi >= '0'&&*suanshi <= '9')
			{
				if (*suanshi == '.'&&d == 0)
				{
					d++;
				}
				else
				{
					if (d == 0)
					{
						h = h * 10 + (*suanshi - '0');
					}
					else if (d > 0)
					{
						h += (*suanshi - '0') / (pow(10, d));
						d++;
					}
				}
				suanshi++;
			}
			F = push(F, ' ', h);
			h = 0, d = 0;
		}
		else//处理运算符号入F链表
		{
			if (S->father == NULL)
			{

				S = push(S, *suanshi, 0);
			}
			else
			{
				int jg;
				jg = panduan(S, *suanshi);
				if (jg == 1)
				{

					S = push(S, *suanshi, 0);
				}
				else if (jg == 0)
				{

					F = push(F, S->data, 0);
					S = pop(S);
					S = push(S, *suanshi, 0);
				}
				else if (jg == 2)
				{

					for (; S->data != '(';)
					{

						F = push(F, S->data, 0);
						S = pop(S);
					}

					S = pop(S);
				}
				else
				{
					printf("未知错误！！！\n");
				}
			}
			suanshi++;
		}

	}
	while (S->father != NULL)
	{
		F = push(F, S->data, 0);
		S = pop(S);
	}
	return r;
}





int main()
{
	printf("科学计算器，请输入算式:\n");
	char cm[80];
	scanf("%s",&cm);
	for (;;)
	{
		struct yuansu *p, *q;
		p = zhuanhuan(cm);
		p = p->next;
		q = p;
		printf("转换为后缀表达式：");
		while (p != NULL)
		{
			if (p->data == ' ')
			{
				printf("%.4lf", p->num);
				p = p->next;
			}
			else
			{
				printf("%c", p->data);
				p = p->next;
			}
		}

		printf("\n计算结果：%lf\n", sum(q));
		scanf("%s",&cm);
	}
	return 0;
}
