#include "stdafx.h"
#include "MemScript.h"
#include "Util.h"
#include "CalFormula.h"

CalFormula gCalFormula;

CalFormula::CalFormula()
{
	this->m_FormulaInfo.clear();
}


CalFormula::~CalFormula()
{
}
void CalFormula::Load(char* path) // OK
{
#if(GAMESERVER_UPDATE>=1220)

	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_FormulaInfo.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while (true)
			{
				if (section == 0)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					FORMULA_INFO info;

					info.ID = lpMemScript->GetNumber();

					strcpy_s(info.value, lpMemScript->GetAsString());

					this->m_FormulaInfo.insert(std::pair<int, FORMULA_INFO>(info.ID, info));
				}

				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

#endif
}
bool CalFormula::GetFormulaInfo(int ID, FORMULA_INFO * lpInfo)
{
	std::map<int, FORMULA_INFO>::iterator it = this->m_FormulaInfo.find(ID);

	if (it == this->m_FormulaInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}
float  CalFormula::CalculationFormula(int ID, int level)
{
	char *str = (char*)calloc(sizeof(char) * 100, sizeof(char));
	FORMULA_INFO info;
	if (this->GetFormulaInfo(ID, &info) == 0)
	{
		return 0;
	}

	int a = 0;
	int b = 0;
	while (true)
	{
		if (info.value[a]=='\0')
		{
			break;
		}
		if (info.value[a] == 'x')
		{
			_itoa_s(level, &str[b],sizeof(str) ,10);
			//_itoa(level, &str[b], 10);
			while (true)
			{
				if (str[b] != '\0')
				{
					b++;
				}
				else
				{
					a++;
					break;
				}
			}

		}
		else
		{
			str[b] = info.value[a];
			a++;
			b++;
		}

	}
	float operand[1024] = { 0 };                /*数据栈，初始化*/
	int  top_num = -1;

	char oper[1024] = { 0 };                  /*操作符栈，初始化*/
	int top_oper = -1;

	char* temp;
	char dest[1024];

	float num = 0;
	int i = 0;

	while (*str != '\0')
	{
		temp = dest;
		while ((*str >= '0' && *str <= '9') || *str=='.')        /*判断是否是数据*/
		{
			*temp = *str;
			str++;
			temp++;
		}                               /*遇到符号退出*/

		if (*str != '(' && *(temp - 1) != '\0')      /*判断符号是否为'('*/
		{
			*temp = '\0';
			//num = atoi(dest);               /*将字符串转为数字*/
			num = atof(dest);
			this->insert_operand(operand, &top_num, num);      /*将数据压入数据栈*/
		}

		while (1)
		{
			i = this->compare(oper, &top_oper, *str);      /*判断操作符优先级*/
			if (i == 0)
			{
				this->insert_oper(oper, &top_oper, *str);   /*压入操作符*/
				break;
			}

			else if (i == 1)                         /*判断括号内的表达式是否结束*/
			{
				str++;
			}

			else if (i == -1)                        /*进行数据处理*/
			{
				this->deal_date(operand, oper, &top_num, &top_oper);
			}
		}
		str++;                 /*指向表达式下一个字符*/
	}
	return operand[0];
}
int  CalFormula::insert_operand(float *operand, int * top_num, float num)           /*数据压入数据栈*/
{
	(*top_num)++;
	operand[*top_num] = num;                    /*保存数据*/

	return 0;                           /*正常退出*/
}


int  CalFormula::insert_oper(char * oper, int *top_oper, char ch)             /*操作符压入符号栈*/
{
	(*top_oper)++;
	oper[*top_oper] = ch;                       /*保存操作符*/
	return 0;                           /*正常退出*/
}

int  CalFormula::compare(char *oper, int *top_oper, char ch)                   /*比较操作服优先级*/
{
	if ((oper[*top_oper] == '-' || oper[*top_oper] == '+')           /*判断当前优先级是否比栈顶操作符优先级高*/
		&& (ch == '*' || ch == '/'))
	{
		return 0;                      /*操作符压入栈*/
	}
	else if (*top_oper == -1 || ch == '(' || (oper[*top_oper] == '(' && ch != ')'))  /*判断操作符栈是否为空；栈顶操作   符是否为'('*/
	{
		return 0;                       /*操作符压入栈*/
	}
	else if (oper[*top_oper] == '(' && ch == ')')       /*判断括号内的表达式是否计算完毕*/
	{
		(*top_oper)--;
		return 1;                       /*对（）进行处理*/
	}
	else
	{
		return -1;                                          /*进行操作符的运算*/
	}
}


int  CalFormula::deal_date(float *operand, char *oper, int *top_num, int *top_oper)    /*进行数据运算*/
{
	float num_1 = operand[*top_num];              /*取出数据栈中两个数据*/
	float num_2 = operand[*top_num - 1];

	float value = 0;

	if (oper[*top_oper] == '+')                  /*加法操作*/
	{
		value = num_1 + num_2;
	}

	else if (oper[*top_oper] == '-')             /*减法操作*/
	{
		value = num_2 - num_1;
	}

	else if (oper[*top_oper] == '*')             /*乘法操作*/
	{
		value = num_2 * num_1;
	}

	else if (oper[*top_oper] == '/')             /*除法操作*/
	{
		value = num_2 / num_1;
	}
	else if (oper[*top_oper] == '^')
	{
		value = pow(num_2, num_1);
	}
	(*top_num)--;                              /*将数据栈顶下移一位*/
	operand[*top_num] = value;                  /*将得到的值压入数据栈*/
	(*top_oper)--;                             /*将操作符栈顶下移一位*/
	return 0;
}
