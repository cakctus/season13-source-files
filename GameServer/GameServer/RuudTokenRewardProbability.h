#pragma once
#define MAX_RUUD_PROBABILITIES 100
struct _RuudProbability
{
	int nRuudBoxBagNumber;
	int nRuudProbability;
	int nRuudInserCount;
};
class RuudTokenRewardProbability
{
public:
	RuudTokenRewardProbability();
	~RuudTokenRewardProbability();

	void InsertRuudPoint(int aIndex, char byType); // idb
	int GetSelectedRuudPoint(int aIndex, char byType); // idb
	void Load_RuudBoxProbability(char *pchFileName); // idb
private:
	_RuudProbability RuudProbabilities[MAX_RUUD_PROBABILITIES];
};

extern RuudTokenRewardProbability g_RuudTokenRewardProbability;