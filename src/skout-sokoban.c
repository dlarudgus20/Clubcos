// Copyright (c) 2014, 임경현 (dlarudgus20)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/*
 * @file skout-sokoban.c
 * @author skout123 (http://blog.naver.com/skout123/50132005532)
 *         ported & modifed by dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include "terminal.h"
#include "timer.h"
#include "task.h"
#include "keyboard.h"
#include "string.h"
#include "memory.h"

static void delay(unsigned ms)
{
	// bochs' timer is not real-time
	// http://wiki.osdev.org/Bochs#Differences_between_Bochs_and_real_hardware
	unsigned fu = g_TimerStruct.TickCountLow + ms;
	while (fu > g_TimerStruct.TickCountLow)
		ckTaskSchedule();
}
static void gotoxy(int x, int y)
{
	ckTerminalGotoXY((uint16_t)x, (uint16_t)y);
}
static uint8_t getch(void)
{
	static uint16_t prev = 0xffff;
	uint8_t cascii;

	if (prev != 0xffff)
	{
		cascii = (uint8_t)prev;
		prev = 0xffff;

		switch (cascii)
		{
			case CASCII_NUM_UP: return CASCII_UP;
			case CASCII_NUM_DOWN: return CASCII_DOWN;
			case CASCII_NUM_LEFT: return CASCII_LEFT;
			case CASCII_NUM_RIGHT: return CASCII_RIGHT;
			default: return cascii;
		}
	}

	cascii = ckTerminalGetch();
	if (cascii < 0x80)
	{
		return cascii;
	}
	else
	{
		prev = cascii;
		return 0xe0;
	}
}

void SokobanMain(void);

void ckCoshellCmdSkoutSokoban(const char *param)
{
	ckTerminalSetStatus(TERMINAL_STATUS_WAITING);
	ckTerminalClearAllBuffers();
	ckTerminalCls();

	void *stack = ckMemoryAllocateBuddy(4 * 1024);
	uint32_t id = ckTaskCreate(
		(uint32_t)SokobanMain, (uint32_t)stack + 4 * 1024, stack,
		ckProcessGetCurrentId(), TASK_PRIORITY_NORMAL);

	ckTaskJoin(id);
	ckMemoryFreeBuddy(stack);
	ckTerminalCls();

	ckTerminalClearAllBuffers();
	ckTerminalSetStatus(TERMINAL_STATUS_STRING_INPUTING);
	ckTerminalSetCursorType(TERMINAL_CURSOR_NORMAL);
}

#define printf(format, ...) ckTerminalPrintStringF(format, ##__VA_ARGS__)
#define getchar() ckTerminalGetChar()
#define clrscr() ckTerminalCls()
#define exit(i) ckTaskExit()

typedef bool BOOL;
#define TRUE true
#define FALSE false

/*=====================================================================*/

#define ESC CASCII_ESC
#define LEFT CASCII_LEFT
#define RIGHT CASCII_RIGHT
#define UP CASCII_UP
#define DOWN CASCII_DOWN
#define MAXSTAGE 2

#define putchxy(x,y,c) ckTerminalPutCharEntryAt((c), (x), (y))

/*=====================================================================*/

void LoadingStage(void);
void DrawStage(void);
void PlayerMoveAction(void);
BOOL TestStageClear(void);
void PrintInfo(void);
void Restart(void);

/*=====================================================================*/

char stage[18][21];

int RoundNum;
int Roundx, Roundy;
int NumMove;
int OnRestart;

/*=====================================================================*/

const char StageData[MAXSTAGE][18][21] =
{
	{
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"##   O            .##",
		"## @ O            .##",
		"##   O            .##",
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"#####################"
	},
	{
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"##########.##########",
		"########## ##########",
		"##########O@O.#######",
		"#######.  OO ########",
		"########### #########",
		"###########.#########",
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"#####################",
		"#####################"
	}
};

/*=====================================================================*/

void SokobanMain(void)
{
	int x, y;

	ckTerminalSetCursorType(TERMINAL_CURSOR_NOCURSOR);

	LoadingStage();

	RoundNum = 0;

	while (1)
	{
		memcpy(stage, StageData[RoundNum], sizeof(stage));
		for (y = 0; y < 18; y++)
		{
			for (x = 0; x < 20; x++)
			{
				if (stage[y][x] == '@')
				{
					Roundx = x;			// 플레이어 좌표 지정
					Roundy = y;			// 플레이어 좌표 지정
					stage[y][x] = ' ';	// 공백으로 지워주자
				}
			}
		}

		clrscr();
		NumMove = 0;

		while (1)
		{
			DrawStage();
			PrintInfo();
			PlayerMoveAction();

			if (OnRestart == 1)
			{
				OnRestart = 0;
				break;
			}

			if (TestStageClear())
			{
				clrscr();
				if (RoundNum < MAXSTAGE - 1)
				{
					gotoxy(30,4);
					printf("><Congratulation!><");
					gotoxy(29,5);
					printf("%d stage clear!", RoundNum+1);
					gotoxy(34,8);
					printf("Let's move to the next stage~");
					gotoxy(34,10);
					printf("press enter...");
					getchar();
					RoundNum++;
				}
				else
				{
					gotoxy(30,4);
					printf("><Congratulation!><");
					gotoxy(29,5);
					printf("all stage clear!");
					gotoxy(34,8);
					printf("press enter to exit...");
					getchar();
					exit(0);
				}
				break;
			}
		}
	}

	ckTaskExit();
}

/*=====================================================================*/

void LoadingStage(void)
{
	int i;

	for (i = 0; i < 45; i++)
	{
		putchxy(10+i,5,'-');
		delay(10);
	}
	for (i = 0; i < 45; i++)
	{
		putchxy(70-i,11,'-');
		delay(10);
	}

	delay(800);
	gotoxy(35,7);
	printf("So");
	delay(600);
	gotoxy(38,7);
	printf("ko");
	delay(600);
	gotoxy(41,7);
	printf("ban");
	delay(600);
	putchxy(48,8,'M');
	delay(40);
	putchxy(49,8,'a');
	delay(40);
	putchxy(50,8,'d');
	delay(40);
	putchxy(51,8,'e');
	delay(40);
	putchxy(53,8,'b');
	delay(40);
	putchxy(54,8,'y');
	delay(40);
	gotoxy(56,8);
	printf("Skout");
	delay(200);
	gotoxy(37,9);
	printf("Ported & Modified by ikh");

	delay(2500);
}

/*=====================================================================*/

void DrawStage(void)
{
	int stagex, stagey;

	for (stagey = 0; stagey < 18; stagey++)
	{
		for (stagex = 0; stagex < 20; stagex++)
		{
			putchxy(stagex, stagey, stage[stagey][stagex]);
		}
	}

	putchxy(Roundx,Roundy,'@');
}

/*=====================================================================*/

void PlayerMoveAction(void)
{
	int dx, dy;
	uint8_t ch;

	ch = getch();

	if (ch == ESC)
	{
		exit(0);
	}
	else if (ch == 'r' || ch == 'R')
	{
		Restart();
	}
	else if (ch == 0xe0)
	{
		ch = getch();
		switch(ch)
		{
			case LEFT:
				dx = -1; dy = 0; break;
			case RIGHT:
				dx = 1; dy = 0; break;
			case UP:
				dx = 0; dy = -1; break;
			case DOWN:
				dx = 0; dy = 1; break;
			default:
				return;
		}

		if (stage[Roundy+dy][Roundx+dx] == '#')
		{
			return;
		}

		if (stage[Roundy+dy][Roundx+dx] == 'O')
		{
			if (stage[Roundy+dy*2][Roundx+dx*2] == ' ')
			{
				stage[Roundy+dy][Roundx+dx] = ' ';
				stage[Roundy+dy*2][Roundx+dx*2] = 'O';
			}
			else if (StageData[RoundNum][Roundy+dy*2][Roundx+dx*2] == '.')
			{
				stage[Roundy+dy][Roundx+dx] = ' ';
				stage[Roundy+dy*2][Roundx+dx*2] = 'O';
			}
			else if (stage[Roundy+dy*2][Roundx+dx*2] == '#')
			{
				return;
			}
		}

		Roundx += dx;
		Roundy += dy;

		NumMove++;
	}
}

/*=====================================================================*/

BOOL TestStageClear(void)
{
	int cx, cy;

	for (cy = 0; cy < 18; cy++)
	{
		for (cx = 0; cx < 20; cx++)
		{
			if (StageData[RoundNum][cy][cx] == '.' && stage[cy][cx] != 'O')
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

/*=====================================================================*/

void PrintInfo(void)
{
	gotoxy(59,1);	printf("  /-----------------\\");
	gotoxy(59,2);	printf("  |  S O K O B A N  |");
	gotoxy(59,3);	printf("  \\-----------------/");

	gotoxy(61,5);	printf("Stage : %d", RoundNum + 1);
	gotoxy(61,7);	printf("Move Count : %d", NumMove);

	gotoxy(67,10);	printf("UP");
	gotoxy(67,10);	printf("/\\");
	gotoxy(60,11);	printf("LEFT <- | -> RIGHT");
	gotoxy(67,12);	printf("\\/");
	gotoxy(66,13);	printf("DOWN");

	gotoxy(61,15);	printf("Restart : R");
}

/*=====================================================================*/

void Restart(void)
{
	OnRestart = 1;
}

