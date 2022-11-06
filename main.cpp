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
int heroBlood;
int jumpOffset;
bool startUpdate;

typedef enum {
	TORTOISE,
	LION,
	HOOK1,
	HOOK2,
	HOOK3,
	HOOK4,
	OBSTACLE_COUNT
} obstacle_type;

vector<vector<IMAGE>> obstacleImages;
IMAGE imgHeroDown[2];
bool heroDownFlag;

typedef struct obstacle {
	int type;
	int imgIndex;
	int x, y;
	int speed;
	int power;
	bool exist;
	bool hited;
} obstacle_t;

obstacle_t obstacles[OBSTACLES_COUNT];
void init() {
	initgraph(WIN_WIDTH, WIN_HEIGHT, EW_SHOWCONSOLE);
	char name[64];
	IMAGE imgType1;
	IMAGE imgType2;
	IMAGE imgType3;
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
			loadimage(&imgType2, name);
			imgLionArray.push_back(imgType2);
		}
		if (i < 7) {
			sprintf(name, "material/res/res/t%d.png", i + 1);
			loadimage(&imgType1, name);
			imgTorArray.push_back(imgType1);
		}
		sprintf(name, "material/res/res/hero%d.png", i + 1);
		loadimage(&image_hero[i], name);
	}
	obstacleImages.push_back(imgTorArray);
	obstacleImages.push_back(imgLionArray);
	for (int i = 0; i < 4; i++)
	{
			vector<IMAGE> imageHookArray;
			sprintf(name, "material/res/res/h%d.png",i +1);
			loadimage(&imgType3, name, 63, 260, true);
			imageHookArray.push_back(imgType3);
			obstacleImages.push_back(imageHookArray);
	}
	for (int i = 0; i < OBSTACLES_COUNT; i++)
	{
		obstacles[i].exist = false;
		obstacles[i].hited = false;
		obstacles[i].imgIndex = 0;
	}
	heroX = WIN_WIDTH / 2 - image_hero[0].getwidth() / 2;
	heroYInit = heroY = 345 - image_hero[0].getheight();
	heroIndex = 0;
	heroJumpFlag = false;
	heroDownFlag = false;
	heroBlood = 100;
	jumpOffset = -HERO_JUMP_SPEED;
	heroJumpMax = heroYInit - 120;
	startUpdate = true;
	preLoadSound("material/res/res/hit.mp3");
	mciSendString("play material/res/res/bg.mp3",0,0,0);
}

void createObstacles() {
	int i;
	for (i = 0; i < OBSTACLES_COUNT; i++) {
		if (!obstacles[i].exist) break;
	}
	if (i >= OBSTACLES_COUNT) return;
	obstacles[i].exist = true;
	obstacles[i].x = WIN_WIDTH;
	obstacles[i].type = rand() % 3;
	if (obstacles[i].type == HOOK1) {
		obstacles[i].type += rand() % 4;
	}
	obstacles[i].y = 345 + 5 - obstacleImages[obstacles[i].type][0].getheight();
	if (obstacles[i].type == TORTOISE) {
		obstacles[i].speed = 0;
		obstacles[i].power = 5;
	}
	else if (obstacles[i].type == LION) {
		obstacles[i].speed = 5;
		obstacles[i].power = 20;
	}
	else if (obstacles[i].type >= HOOK1 && obstacles[i].type <= HOOK4) {
		obstacles[i].speed = 0;
		obstacles[i].power = 50;
		obstacles[i].y = 0;
	}
}

void checkHit() {
	for (int i = 0; i < OBSTACLES_COUNT; i++)
	{
		if (obstacles[i].exist && !obstacles[i].hited) {
			int a1X, a1Y, a2X, a2Y;
			int offset = 30;
			if (!heroDownFlag) {
				a1X = heroX + offset;
				a1Y = heroY + offset;
				a2X = heroX + image_hero[i].getwidth() - offset;
				a2Y = heroY + image_hero[i].getheight();
			}
			else
			{
				a1X = heroX + offset;
				a1Y = 345 - image_hero[i].getheight();
				a2X = heroX + image_hero[i].getwidth() - offset;
				a2Y = 345;
			}
			int b1X, b1Y, b2X, b2Y;
			IMAGE img = obstacleImages[obstacles[i].type][obstacles[i].imgIndex];
			b1X = obstacles[i].x + offset;
			b1Y = obstacles[i].y + offset;
			b2X = obstacles[i].x + img.getwidth() - offset;
			b2Y = obstacles[i].y + img.getheight() - 10;
			if (rectIntersect(a1X, a1Y, a2X, a2Y, b1X, b1Y, b2X, b2Y)) {
				heroBlood -= obstacles[i].power;
				printf("当前血量: %d", heroBlood);
				playSound("material/res/res/hit.mp3");
				obstacles[i].hited = true;
			}
	  }
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
		int delays[2] = { 8,30 };
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
	checkHit();
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

void updateBloodBar() {
	drawBloodBar(10, 10, 200, 10, 2, BLUE, DARKGRAY,RED, heroBlood / 100.0);
}

void checkOver() {
	if (heroBlood <= 0) {
		loadimage(0, "material/res/res/over.png");
		FlushBatchDraw();
		mciSendString("stop material/res/res/bg.mp3", 0, 0, 0);
		system("pause");
		heroBlood = 100;
		mciSendString("play material/res/res/bg.mp3", 0, 0, 0);
	}
}

int main()
{
	// 初始化游戏
	init();
	loadimage(0, "material/res/res/over.png");
	system("pause");
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
			updateBloodBar();
			EndBatchDraw();
			checkOver();
			todo();
		}

	}
	system("pause");
	return 0;
}