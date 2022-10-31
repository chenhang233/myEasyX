#include <stdio.h>
#include <graphics.h>
#include "tools.h"

#define WIN_WIDTH 1012
#define WIN_HEIGHT 396

IMAGE image_bg[3];
IMAGE image_hero[12];
int bgX[3];			
int bgSpeed[3] = { 1,2,4 };
int heroX;
int heroY;
int heroIndex;

void init() {
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	char name[64];
	for (int i = 0; i < 3; i++)
	{
		sprintf(name,"material/res/res/bg%03d.png",i+1);
		// material/res/res/bg001.png
		loadimage(&image_bg[i], name);
		bgX[i] = 0;
	}
	for (int i = 0; i < 12; i++)
	{
		sprintf(name, "material/res/res/hero%d.png", i + 1);
		loadimage(&image_hero[i], name);
		heroX = WIN_WIDTH / 2 - image_hero[i].getwidth() / 2;
		heroY = 345 - image_hero[i].getheight();
		heroIndex = 0;
	}
}

void fly() {
	for (int i = 0; i < 3; i++)
	{
		if (bgX[i] <= -WIN_WIDTH) 
			bgX[i] = 0;
		bgX[i] -= bgSpeed[i];
	}
	heroIndex = (heroIndex + 1) % 12;
}
void updateBg() {
	putimagePNG2(bgX[0], 0,&image_bg[0]);
	putimagePNG2(bgX[1], 119,&image_bg[1]);
	putimagePNG2(bgX[2],330,&image_bg[2]);
}
int main()
{
	// 初始化游戏
	init();
	while (1)
	{
		// 更新背景
		BeginBatchDraw();// 让 putimagePNG2不一条条打印,而是先缓存好所以片段,再打印,解决闪烁问题
		updateBg();
		putimagePNG2(heroX, heroY, &image_hero[heroIndex]);
		EndBatchDraw();
		fly();
		Sleep(30);
	}
	system("pause");

	return 0;
}