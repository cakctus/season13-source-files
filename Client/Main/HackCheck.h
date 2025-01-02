#pragma once

#define MIN_PORT 55901
#define MAX_PORT 55950

#define PORT_RANGE(x) (((x)<MIN_PORT)?0:((x)>MAX_PORT)?0:1)

void InitHackCheck();
void TestMoveHack();
bool CheckSocketPort(SOCKET s);

extern BYTE CoordX;
extern BYTE CoordY;
extern int stuckCount;
extern ULONGLONG avoidByProtocol;
extern int durationSkillCount;
extern int attackSkillCount;
extern int attackCount;