#pragma once

#include "User.h"
#include "Protocol.h"

/*
// Index = n�mero do monstro ou -1 para qualquer monstro
// Mapa = n�mero do mapa, ou -1 para qualquer mapa
// HP = multiplicador do HP do monstro
// Damage = multiplicador de dano do monstro
// Defense = multiplicador de defesa do monstro
// AtkRate = multiplicador de taxa de sucesso de ataque
// DefRate = multiplicador de taxa de sucesso de defesa
// DmgMin> = Valor em pontos para o dano m�nimo que o monstro far� no jogador
// DmgMi%> e DmgMa%< = Valores de dano em porcento do life m�ximo do jogador atingido
//   * = n�o alterar o padr�o
//   Bonus de ataque e defesa dos buffs e pets ser�o calculados em cima desse dano
// DmgMin< e DmgMax< = Valores reais, em pontos de dano
//   * = n�o alterar o padr�o
//   Bonus de ataque e defesa dos buffs e pets ser�o calculados em cima desse dano
// HPRgAmt = quantidade de HP a recuperar em %
// HPRgTim = timer do recuperador de life (recuperar a cada X segundos)
// ExpRate = multiplicador de taxa de experi�ncia
// KillMsg = n�mero da mensagem ao mostrar na tela quanto o monstro for detonado (%s, %s e %d = player, monstro e moedas)
// Players = quantidade de jogadores que receber�o premia��o
// Levels = quantos levels o jogador ganhar�
// Points = quantos pontos de stat o jogador ganhar�
// Resets = quantos resets o jogador ganhar�
// AL_rst = nivel vip minimo para ganhar os resets
// GResets = quantos grand resets o jogador ganhar�
// AL_Grst = nivel vip minimo para ganhar os grand resets
// GoblinP = quantos goblin points o jogador ganhar�
// WcoinP = quantos Wcoin (P)  o jogador ganhar�
// WcoinC = quantos Wcoin (C)  o jogador ganhar�
// AL_Coin = nivel vip minimo para ganhar coins
// Rank = categoria do monstro (aparecer� no jogo)
// Color = cor da legenda para a categoria do monstro
*/
struct CUSTOM_MONSTER_INFO
{
	int Index;
	int MapNumber;
	float MaxLife;
	int Element;
	int MinDmgMakePoints;
	int MinDmgMakePerc;
	int MaxDmgMakePerc;
	int MinDmgGetPoints;
	int MaxDmgGetPoints;
	int PlayerNumber;
	int TimerRandom;
	int Points;
	int Resets;
	int MinVipResets;
	int Ruud;
	int WcoinP;
	int WcoinC;
	int MinVIPCoin;
	char Rank[32];
	int RankColor;
	int HitDelay;
};

#pragma pack(push, 1)
struct PMSG_TARGET_BOSS_DATA
{
	PSBMSG_HEAD h;

	int Life;
	int MaxLife;
	short Level;
	char Rank[32];
	int RankColor;

	int sDamage;
	int sRank;
	int Count;
	int Damage[10];
	char Name[10][11];

	PMSG_TARGET_BOSS_DATA()
	{
		for (int i = 0; i < 10; i++)
		{
			Name[i][0] = 0;
			Damage[i] = 0;
		}
		sDamage = 0;
		sRank = -1;
		Count = 0;
	}
};
#pragma pack(pop)

class CCustomMonster
{
public:
	CCustomMonster();
	virtual ~CCustomMonster();
	void Load(char* path);
	void SetCustomMonsterInfo(LPOBJ lpObj);
	void MonsterDieProc(LPOBJ lpObj,LPOBJ lpTarget);
	bool GetCustomMonsterInfo(int index,int map,CUSTOM_MONSTER_INFO* lpInfo);
	void SendDamageRanking(LPOBJ lpObj);
	void SendBossChat(LPOBJ lpMonster, LPOBJ lpTarget);
private:
	std::vector<CUSTOM_MONSTER_INFO> m_CustomMonsterInfo;
};

extern CCustomMonster gCustomMonster;
