#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include  <vector>
#include "tools.h"

using namespace std;

#define WIN_WIDTH 1012
#define WIN_HEIGHT 396
#define HERO_JUMP_SPEED	8
#define OBSTACLES_COUNT 10

IMAGE image_bg[3];
IMAGE image_hero[12];

int bgX[3];			
int bgSpeed[3] = { 1,2,4 };
int heroX;
int heroY;
int heroYInit;
int heroIndex;
int heroJumpMax;
int heroJumpFlag;
int jumpOffset;
bool startUpdate;

typedef enum {
	TORTOISE,
	LION,
	OBSTACLE_COUNT
} obstacle_type;

vector<vector<IMAGE>> obstacleImages;
IMAGE imgHeroDown[2];
bool heroDownFlag;

typedef struct obstacle {
	obstacle_type type;
	int imgIndex;
	int x, y;
	int speed;
	int power;
	bool exist;
} obstacle_t;

obstacle_t obstacles[OBSTACLES_COUNT];
void init() {
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	char name[64];
	IMAGE imgType1;
	IMAGE imgType2;
	vector<IMAGE> imgTorArray;
	vector<IMAGE> imgLionArray;
	for (int i = 0; i < 12; i++)
	{
		if (i < 2) {
			sprintf(name, "material/res/res/d%d.png", i + 1);
			loadimage(&imgHeroDown[i], name);
		}
		if (i < 3) {
			sprintf(name, "material/res/res/bg%03d.png", i + 1);
			// material/res/res/bg001.png
			loadimage(&image_bg[i], name);
			bgX[i] = 0;
		}
		if (i < 6) {
			sprintf(name, "material/res/res/p%d.png", i + 1);
			loadimage(&imgType1, name);
			imgLionArray.push_back(imgType1);
		}
		if (i < 7) {
			sprintf(name, "material/res/res/t%d.png", i + 1);
			loadimage(&imgType2, name);
			imgTorArray.push_back(imgType2);
		}
		sprintf(name, "material/res/res/hero%d.png", i + 1);
		loadimage(&image_hero[i], name);
	}
	obstacleImages.push_back(imgTorArray);
	obstacleImages.push_back(imgLionArray);
	for (int i = 0; i < OBSTACLES_COUNT; i++)
	{
		obstacles[i].exist = false;
	}
	heroX = WIN_WIDTH / 2 - image_hero[0].getwidth() / 2;
	heroYInit = heroY = 345 - image_hero[0].getheight();
	heroIndex = 0;
	heroJumpFlag = false;
	heroDownFlag = false;
	jumpOffset = -HERO_JUMP_SPEED;
	heroJumpMax = heroYInit - 120;
	startUpdate = true;
}

void createObstacles() {
	int i;
	for (i = 0; i < OBSTACLES_COUNT; i++) {
		if (!obstacles[i].exist) break;
	}
	if (i >= OBSTACLES_COUNT) return;
	obstacles[i].exist = true;
	obstacles[i].imgIndex = 0;
	obstacles[i].x = WIN_WIDTH;
	obstacles[i].type = (obstacle_type)(rand() % OBSTACLE_COUNT);
	obstacles[i].y = 345 + 5 - obstacleImages[obstacles[i].type][0].getheight();
	if (obstacles[i].type == TORTOISE) {
		obstacles[i].speed = 0;
		obstacles[i].power = 5;
	}
	else if (obstacles[i].type == LION) {
		obstacles[i].speed = 5;
		obstacles[i].power = 20;
	}
}

void todo() {
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
	else if (heroDownFlag) {
		static int count = 0;
		int delays[2] = { 4,10 };
		count++;
		if (count >= delays[heroIndex]) {
			count = 0;
			heroIndex++;
			if (heroIndex >= 2) {
				heroDownFlag = false;
				heroIndex = 0;
			}
		}
	} else {
		heroIndex = (heroIndex + 1) % 12;
	}
	static int frameCount = 0;
	static int frame_enemy = 50;
	frameCount++;
	if (frameCount > frame_enemy) {
		frameCount = 0;
		frame_enemy = 50 + rand() % 50;
		createObstacles();
	}
}
void updateBg() {
	putimagePNG2(bgX[0], 0,&image_bg[0]);
	putimagePNG2(bgX[1], 119,&image_bg[1]);
	putimagePNG2(bgX[2],330,&image_bg[2]);
}

void updateEnemy() {
	for (int i = 0; i < OBSTACLES_COUNT; i++) {
		if (obstacles[i].exist) {
			putimagePNG2(obstacles[i].x, obstacles[i].y, WIN_WIDTH,&obstacleImages[obstacles[i].type][obstacles[i].imgIndex]);
			obstacles[i].x -= obstacles[i].speed + bgSpeed[2];
			if (obstacles[i].x <= -obstacleImages[obstacles[i].type][0].getwidth() * 2) {
			obstacles[i].exist = false;
			}
			int len = (int)obstacleImages[obstacles[i].type].size();
			obstacles[i].imgIndex = (obstacles[i].imgIndex + 1) % len;
		}
	}

}

void jump() {
	heroJumpFlag = true;
	startUpdate = true;
}

void down() {
	heroDownFlag = true;
	startUpdate = true;
	heroIndex = 0;
}

void keyBoard() {
	char c;
	if (_kbhit()) {
		c = _getch();
		if (c == ' ')
		{
			jump();
		}
		else if (c == 'z')
		{
			down();
		}
	 }
}

void updateHero() {
	if (!heroDownFlag) {
		putimagePNG2(heroX, heroY, &image_hero[heroIndex]);
	}
	else {
		int y = 345 - imgHeroDown[heroIndex].getheight();
		putimagePNG2(heroX, y, &imgHeroDown[heroIndex]);
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
			updateHero();
			updateEnemy();
			EndBatchDraw();
			todo();
		}

	}
	system("pause");

	return 0;
}