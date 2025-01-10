#pragma once

#include "user.h"

#define MAXHITFILA 12

struct PACOTE
{
	DWORD alvo;
	WORD skill;
	BYTE distancia;
	BYTE x;
	BYTE y;
	BYTE direcao;
	BYTE posicao;
	WORD angulo;
	BYTE acao;
	BYTE count;
	BYTE serial;
	BYTE magicKey;
};

struct HitFila
{
	DWORD tick_liberacao;
	BYTE tipo;
	PACOTE pacote;
};

struct HitUser
{
	int acumulo;
	int penalidades;
	bool flag;
	DWORD timer;
	DWORD penTimer;
};

class FilaHit
{
public:
	FilaHit();
	~FilaHit();

	int tamanhoFila;
	bool processando;

	void InicializarElemento(HitFila* elemento);
	void Add(BYTE tipo, int index, BYTE* pacoteOriginal);
	void InserirElemento(HitFila elemento, int index);
	void RemoverElemento(int posicao, int index);
	void List();
	DWORD ProximaLiberacao(int index, int skill, int tipo);
	void Velocidade(LPOBJ lpObj);
	void LimparFila(int index=-1);

	HitFila HIT_FILA[MAX_OBJECT_USER][MAXHITFILA];
	HitUser HIT_USER[MAX_OBJECT_USER];
	HANDLE threadHandle;

	static DWORD WINAPI FilaThread(LPVOID lpdwThreadParam);
	static DWORD WINAPI PotionThread(LPVOID lpdwThreadParam);
};

extern FilaHit g_FilaHit;