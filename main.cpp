#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include "tools.h"

#define WIN_WIDTH 1012
#define WIN_HEIGHT 396
#define HERO_JUMP_SPEED	8

IMAGE image_bg[3];
IMAGE image_hero[12];
IMAGE tortoise[7];
int bgX[3];			
int bgSpeed[3] = { 1,2,4 };
int heroX;
int heroY;
int heroYInit;
int heroIndex;
int tortoiseIndex;
int heroJumpMax;
int heroJumpFlag;
int jumpOffset;
int tortoiseX;
int tortoiseY;
bool startUpdate;
bool isTortoise;

void init() {
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	char name[64];
	for (int i = 0; i < 12; i++)
	{
		if (i < 3) {
			sprintf(name, "material/res/res/bg%03d.png", i + 1);
			// material/res/res/bg001.png
			loadimage(&image_bg[i], name);
			bgX[i] = 0;
		}
		if (i < 7) {
			sprintf(name, "material/res/res/t%d.png", i + 1);
			loadimage(&tortoise[i], name);
		}
		sprintf(name, "material/res/res/hero%d.png", i + 1);
		loadimage(&image_hero[i], name);
	}
	heroX = WIN_WIDTH / 2 - image_hero[0].getwidth() / 2;
	heroYInit = heroY = 345 - image_hero[0].getheight();
	heroIndex = 0;
	heroJumpFlag = false;
	jumpOffset = -HERO_JUMP_SPEED;
	heroJumpMax = heroYInit - 120;
	startUpdate = true;
	isTortoise = false;
	tortoiseY = 345 - tortoise[0].getheight() + 5;
	tortoiseIndex = 0;
}

void fly() {
	for (int i = 0; i < 3; i++)
	{
		if (bgX[i] <= -WIN_WIDTH) 
			bgX[i] = 0;
		bgX[i] -= bgSpeed[i];
	}
	if (heroJumpFlag) {
		if (heroY <= heroJumpMax) {
			jumpOffset = HERO_JUMP_SPEED;
		}
		heroY += jumpOffset;
		if (heroY >= heroYInit) {
			heroJumpFlag = false;
			jumpOffset = -HERO_JUMP_SPEED;
		}
	}
	else {
		heroIndex = (heroIndex + 1) % 12;
	}
	static int frameCount = 0;
	static int frame_tor = 100;
	frameCount++;
	if (frameCount > frame_tor) {
		frameCount = 0;
		if (!isTortoise) {
			isTortoise = true;
			tortoiseX = WIN_WIDTH;
			frame_tor = 200 + rand() % 300;
		}
	}
	if (isTortoise) {
		tortoiseX -= bgSpeed[2];
		tortoiseIndex = (tortoiseIndex + 1) % 7;
		if (tortoiseX < -tortoise[0].getwidth()) {
			isTortoise = false;
		}
	}
}
void updateBg() {
	putimagePNG2(bgX[0], 0,&image_bg[0]);
	putimagePNG2(bgX[1], 119,&image_bg[1]);
	putimagePNG2(bgX[2],330,&image_bg[2]);
}

void updateEnemy() {
	if (isTortoise) {
		putimagePNG2(tortoiseX, tortoiseY, WIN_WIDTH, &tortoise[tortoiseIndex]);
	}
}

void keyBoard() {
	char c;
	if (_kbhit()) {
		c = _getch();
		if (c == ' ')
		{
			heroJumpFlag = true;
			startUpdate = true;
		}
	 }
}
int main()
{
	// 初始化游戏
	init();
	int timer = 0;
	while (1)
	{
		keyBoard();
		timer += getDelay();
		if (timer > 30) {
			startUpdate = true;
			timer = 0;
		}
		if (startUpdate) {
			startUpdate = false;
			// 更新背景
			BeginBatchDraw();// 让 putimagePNG2不一条条打印,而是先缓存好所以片段,再打印,解决闪烁问题
			updateBg();
			putimagePNG2(heroX, heroY, &image_hero[heroIndex]);
			updateEnemy();
			EndBatchDraw();
			fly();
		}

	}
	system("pause");

	return 0;
}