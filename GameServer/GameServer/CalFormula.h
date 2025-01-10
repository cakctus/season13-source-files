#pragma once

struct FORMULA_INFO
{
	int ID;
	char value[100];
};
class CalFormula
{
public:
	CalFormula();
	~CalFormula();
	void Load(char* path); // OK
	bool GetFormulaInfo(int ID, FORMULA_INFO * lpInfo);
	float CalculationFormula(int ID, int level);
	int insert_operand(float *operand, int * top_num, float num);
	int insert_oper(char * oper, int *top_oper, char ch);
	int compare(char *oper, int *top_oper, char ch);
	int deal_date(float *operand, char *oper, int *top_num, int *top_oper);
private:
	std::map<int, FORMULA_INFO> m_FormulaInfo;
};
extern CalFormula gCalFormula;
