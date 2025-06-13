#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <Windows.h>
#include <conio.h>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

#pragma comment(lib, "MSIMG32.LIB")

using namespace std;

// 游戏常量
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 700;
const int PLAYER_SPEED = 5;
const int BULLET_SPEED = 8;
const int ENEMY_SPEED = 3;
const int ENEMY_BULLET_SPEED = 5;
const int MAX_ENEMIES = 20;
const int MAX_BULLETS = 50;
const int MAX_ENEMY_BULLETS = 30;

// 游戏对象结构
struct Point {
    int x, y;
};

struct GameObject {
    Point position;
    int width;
    int height;
    bool active;
};

// 全局变量
IMAGE bg_img, plane_img, enemy_img, bullet_img, enemy_bullet_img, explosion_img;
int playerX = 200, playerY = 500; // 玩家飞机位置
int score = 0;        // 当前得分
int hp = 100;         // 剩余血量
int gameState = 1;    // 1:游戏中, 0:游戏结束

// 游戏对象池
vector<GameObject> enemies(MAX_ENEMIES);
vector<GameObject> bullets(MAX_BULLETS);
vector<GameObject> enemyBullets(MAX_ENEMY_BULLETS);

// 加载资源
void loadImages() {
    loadimage(&bg_img, "images/background.png", SCREEN_WIDTH, SCREEN_HEIGHT);
    loadimage(&plane_img, "images/me1.png");
    loadimage(&enemy_img, "images/enemy1.png");
    loadimage(&bullet_img, "images/bullet1.png");
    loadimage(&enemy_bullet_img, "images/bullet2.png");
    loadimage(&explosion_img, "images/explosion.png");
}

// 初始化游戏
void initGame() {
    playerX = SCREEN_WIDTH / 2 - plane_img.getwidth() / 2;
    playerY = SCREEN_HEIGHT - 150;
    score = 0;
    hp = 100;
    gameState = 1;

    // 重置所有敌人
    for (auto& enemy : enemies) {
        enemy.active = false;
    }

    // 重置所有子弹
    for (auto& bullet : bullets) {
        bullet.active = false;
    }

    for (auto& ebullet : enemyBullets) {
        ebullet.active = false;
    }
}

// 透明贴图函数
void transparentimage(int x, int y, IMAGE* srcimg) {
    HDC dstDC = GetImageHDC(NULL);
    HDC srcDC = GetImageHDC(srcimg);
    int w = srcimg->getwidth();
    int h = srcimg->getheight();
    TransparentBlt(dstDC, x, y, w, h, srcDC, 0, 0, w, h, BLACK);
}

// 生成敌人
void spawnEnemy() {
    for (auto& enemy : enemies) {
        if (!enemy.active) {
            enemy.position.x = rand() % (SCREEN_WIDTH - enemy_img.getwidth());
            enemy.position.y = -enemy_img.getheight();
            enemy.width = enemy_img.getwidth();
            enemy.height = enemy_img.getheight();
            enemy.active = true;
            return;
        }
    }
}

// 玩家发射子弹
void fireBullet() {
    for (auto& bullet : bullets) {
        if (!bullet.active) {
            bullet.position.x = playerX + plane_img.getwidth() / 2 - bullet_img.getwidth() / 2;
            bullet.position.y = playerY - bullet_img.getheight();
            bullet.width = bullet_img.getwidth();
            bullet.height = bullet_img.getheight();
            bullet.active = true;
            return;
        }
    }
}

// 敌人发射子弹
void enemyFireBullet(int x, int y) {
    for (auto& ebullet : enemyBullets) {
        if (!ebullet.active) {
            ebullet.position.x = x + enemy_img.getwidth() / 2 - enemy_bullet_img.getwidth() / 2;
            ebullet.position.y = y + enemy_img.getheight();
            ebullet.width = enemy_bullet_img.getwidth();
            ebullet.height = enemy_bullet_img.getheight();
            ebullet.active = true;
            return;
        }
    }
}

// 碰撞检测
bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return x1 < x2 + w2 &&
        x1 + w1 > x2 &&
        y1 < y2 + h2 &&
        y1 + h1 > y2;
}

// 绘制游戏界面
void draw() {
    // 绘制背景
    putimage(0, 0, &bg_img);

    // 绘制玩家飞机
    transparentimage(playerX, playerY, &plane_img);

    // 绘制敌人
    for (const auto& enemy : enemies) {
        if (enemy.active) {
            transparentimage(enemy.position.x, enemy.position.y, &enemy_img);
        }
    }

    // 绘制玩家子弹
    for (const auto& bullet : bullets) {
        if (bullet.active) {
            transparentimage(bullet.position.x, bullet.position.y, &bullet_img);
        }
    }

    // 绘制敌人子弹
    for (const auto& ebullet : enemyBullets) {
        if (ebullet.active) {
            transparentimage(ebullet.position.x, ebullet.position.y, &enemy_bullet_img);
        }
    }

    // 绘制爆炸效果
    for (const auto& enemy : enemies) {
        if (!enemy.active && enemy.position.y < SCREEN_HEIGHT) {
            transparentimage(enemy.position.x, enemy.position.y, &explosion_img);
        }
    }

    // 设置文字样式
    settextcolor(WHITE);
    settextstyle(20, 0, "微软雅黑");
    setbkmode(TRANSPARENT);

    // 绘制得分
    char scoreStr[32];
    sprintf(scoreStr, "得分: %d", score);
    outtextxy(20, 20, scoreStr);

    // 绘制血量
    char hpStr[32];
    sprintf(hpStr, "血量: %d%%", hp);
    outtextxy(20, 50, hpStr);

    // 绘制血条背景
    setfillcolor(DARKGRAY);
    fillrectangle(90, 55, 290, 70);

    // 绘制当前血量（红色血条）
    setfillcolor(0xAA0000);
    fillrectangle(90, 55, 90 + hp * 2, 70);

    // 绘制血条边框
    setlinecolor(WHITE);
    rectangle(90, 55, 290, 70);

    // 绘制游戏标题
    settextcolor(0x00FFFF);
    settextstyle(28, 0, "微软雅黑");
    outtextxy(SCREEN_WIDTH / 2 - 80, 10, "飞机大战");

    // 绘制操作提示
    settextstyle(14, 0, "宋体");
    settextcolor(0x00FF00);
    outtextxy(SCREEN_WIDTH - 200, 20, "方向键移动");
    outtextxy(SCREEN_WIDTH - 200, 40, "空格键发射");

    // 游戏结束画面
    if (gameState == 0) {
        settextcolor(0xFF0000);
        settextstyle(48, 0, "微软雅黑");
        outtextxy(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, "游戏结束");

        settextstyle(24, 0, "微软雅黑");
        char finalScore[64];
        sprintf(finalScore, "最终得分: %d", score);
        outtextxy(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 20, finalScore);

        settextstyle(20, 0, "微软雅黑");
        outtextxy(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 80, "按R键重新开始");
    }
}

// 更新游戏状态
void updateGame() {
    // 更新敌人位置
    for (auto& enemy : enemies) {
        if (enemy.active) {
            enemy.position.y += ENEMY_SPEED;

            // 敌人超出屏幕
            if (enemy.position.y > SCREEN_HEIGHT) {
                enemy.active = false;
            }

            // 随机发射子弹
            if (rand() % 100 < 2) {
                enemyFireBullet(enemy.position.x, enemy.position.y);
            }
        }
    }

    // 更新玩家子弹位置
    for (auto& bullet : bullets) {
        if (bullet.active) {
            bullet.position.y -= BULLET_SPEED;

            // 子弹超出屏幕
            if (bullet.position.y < -bullet.height) {
                bullet.active = false;
            }
        }
    }

    // 更新敌人子弹位置
    for (auto& ebullet : enemyBullets) {
        if (ebullet.active) {
            ebullet.position.y += ENEMY_BULLET_SPEED;

            // 子弹超出屏幕
            if (ebullet.position.y > SCREEN_HEIGHT) {
                ebullet.active = false;
            }
        }
    }

    // 碰撞检测：玩家子弹与敌人
    for (auto& bullet : bullets) {
        if (bullet.active) {
            for (auto& enemy : enemies) {
                if (enemy.active &&
                    checkCollision(bullet.position.x, bullet.position.y, bullet.width, bullet.height,
                        enemy.position.x, enemy.position.y, enemy.width, enemy.height)) {
                    bullet.active = false;
                    enemy.active = false;
                    score += 10;
                    break;
                }
            }
        }
    }

    // 碰撞检测：敌人子弹与玩家
    for (auto& ebullet : enemyBullets) {
        if (ebullet.active) {
            if (checkCollision(ebullet.position.x, ebullet.position.y, ebullet.width, ebullet.height,
                playerX, playerY, plane_img.getwidth(), plane_img.getheight())) {
                ebullet.active = false;
                hp -= 5;
                if (hp <= 0) {
                    hp = 0;
                    gameState = 0;
                }
            }
        }
    }

    // 碰撞检测：敌人与玩家
    for (auto& enemy : enemies) {
        if (enemy.active &&
            checkCollision(enemy.position.x, enemy.position.y, enemy.width, enemy.height,
                playerX, playerY, plane_img.getwidth(), plane_img.getheight())) {
            enemy.active = false;
            hp -= 15;
            if (hp <= 0) {
                hp = 0;
                gameState = 0;
            }
        }
    }

    // 随机生成敌人
    if (rand() % 100 < 3) {
        spawnEnemy();
    }
}

int main() {
    // 初始化随机数生成器
    srand(static_cast<unsigned int>(time(nullptr)));

    // 创建游戏窗口
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
    BeginBatchDraw();

    // 加载图片资源
    loadImages();

    // 初始化游戏
    initGame();

    // 游戏循环变量
    int frameCount = 0;
    bool firing = false;
    ExMessage msg;
    // 游戏主循环
    while (true) {
        // 处理输入
        if (peekmessage(&msg, EX_KEY)) {

            // 游戏进行中
            if (gameState == 1) {
                // 玩家移动
                if (msg.message != WM_KEYDOWN) continue;
                switch (msg.vkcode) {

                    case VK_UP: {
                        playerY -= PLAYER_SPEED;
                        if (playerY < 0) playerY = 0;
                        break;
                    }
                    case VK_DOWN: {
                        playerY += PLAYER_SPEED;
                        if (playerY > SCREEN_HEIGHT - plane_img.getheight())
                            playerY = SCREEN_HEIGHT - plane_img.getheight();
                        break;
                    }
                    case VK_LEFT: {
                        playerX -= PLAYER_SPEED;
                        if (playerX < 0) playerX = 0;
                        break;
                    }
                    case VK_RIGHT: {
                        playerX += PLAYER_SPEED;
                        if (playerX > SCREEN_WIDTH - plane_img.getwidth())
                            playerX = SCREEN_WIDTH - plane_img.getwidth();
                        break;
                    }

                    case ' ': {
                        if (frameCount % 5 == 0) { // 限制发射频率
                            fireBullet();
                        }
                    }
                }
            }

            // 游戏结束后重新开始
            if (msg.vkcode == 'r' || msg.vkcode == 'R') {
                if (gameState == 0) {
                    initGame();
                }
            }

            // 退出游戏
            if (msg.vkcode == 27) { // ESC键
                break;
            }
        }

        // 更新游戏状态
        if (gameState == 1) {
            updateGame();
        }

        // 绘制游戏场景
        FlushBatchDraw();
        draw();

        // 控制帧率
        Sleep(16); // 约60FPS
        frameCount++;
    }

    EndBatchDraw();
    closegraph();
    return 0;
}