/**

* Author: Ren Liao

* Assignment: Pong Clone

* Date due: 10/16/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

#include "raylib.h"
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;
class Ball;
// Enums
enum AppStatus { TERMINATED, RUNNING };
enum PlayerMode { SINGLE, DOUBLE };
enum Direction { RIGHT, LEFT, UP, DOWN };
// Global Constants
constexpr int SCREEN_WIDTH  = 1000 * 1.5f,
              SCREEN_HEIGHT = 600 * 1.5f,
              FPS           = 60,
              SIZE          = 100;
float BALL_SPEED = 400.0f;
float PADDLE_SPEED = 250.0f;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 ORIGINOFFSET1 = {SCREEN_WIDTH / 2 + 10.0f, SCREEN_HEIGHT / 2 - 12.0f};
constexpr Vector2 ORIGINOFFSET2 = {SCREEN_WIDTH / 2 -5.0f, SCREEN_HEIGHT / 2 + 7.0f};
constexpr Vector2 LEFT_START = {100.0f, SCREEN_HEIGHT / 2};
constexpr Vector2 RIGHT_START = {SCREEN_WIDTH - 100.0f, SCREEN_HEIGHT / 2};
constexpr Vector2 BASE_SIZE   = { static_cast<float>(SIZE), static_cast<float>(SIZE) };


constexpr char DITTOBALL[] = "assets/ditto.png";
constexpr char DITTOPADDLE[] = "assets/dittoPaddle.png";
//constexpr char DITTOPADDLE2[] = "assets/dittoPaddle.png";




//constexpr char DITTO[] = "assets/ditto.png";
// Global Variables
AppStatus gAppStatus = RUNNING;
Vector2 gScale = BASE_SIZE;
Vector2 gPositionDitto = ORIGIN;
float gPreviousTicks = 0.0f;

float gAngle = 0.0f;

PlayerMode mode = DOUBLE;
Direction paddle2Dir = UP;

int numBalls = 1;


Vector2 gDittoPaddle1Position = LEFT_START,
        gDittoPaddle1Movement = {0.0f, 0.0f},
        gDittoPaddle1Scale = {27.0f, 100.0f},

        gDittoPaddle2Position = RIGHT_START,
        gDittoPaddle2Movement = {0.0f, 0.0f},
        gDittoPaddle2Scale = {27.0f, 100.0f};

int player1Score = 0;
int player2Score = 0;

Texture2D gDittoPaddle1Texture;
Texture2D gDittoPaddle2Texture;
Texture2D gDittoBallTexture;

// Function Declarations
void initialize();
void processInput();
void update();
void render();
void renderObject(const Texture2D* texture, const Vector2* position, const Vector2* scale);
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);
Ball returnClosestBall(vector<Ball>& balls);

class Ball {
public:
    explicit Ball(const Vector2 position = ORIGIN, const Vector2& scale = BASE_SIZE,
        const Direction& xDir = RIGHT, const Direction& yDir = UP, Texture2D* texture = &gDittoBallTexture) :
                            position(position), scale(scale), xDir(xDir), yDir(yDir), texture(texture) {}
    Vector2& getPosition() { return position; }
    // Vector2& getMovement() { return movement; }
    Vector2& getScale() { return scale; }
    const Direction& getXDirection() const {return xDir;}
    const Direction& getYDirection() const {return yDir;}

    void updateX(Direction dir) {xDir = dir;}
    void updateY(Direction dir) {yDir = dir;}

    void updatePosition(float x, float y) {
        position.x += x;
        position.y += y;
    }
    void setPosition(float x, float y) {
        position.x = x;
        position.y = y;
    }

    void draw() const {
        renderObject(texture, &position, &scale);
    }
private:
    Vector2 position;
    Vector2 scale;
    Direction xDir;
    Direction yDir;
    Texture2D* texture;
};

Ball returnClosestBall(vector<Ball>& balls) {
    Ball closestBall;
    for (Ball& curBall : balls) {
        // if (closestBall.getXDirection() == RIGHT && balls[i].getXDirection() == RIGHT) {
        if (curBall.getPosition().x > closestBall.getPosition().x) {
            if (curBall.getXDirection() == RIGHT){
                closestBall = curBall;
            }
        }
    }
    return closestBall;
}

vector<Ball> balls;

// Function Definitions
bool winCondition(int score) {
    return score == 5;
}
void recalculateBalls() {
    if (numBalls > 1) numBalls--;
  //  if (numBalls == 1 && (winCondition(player1Score) || winCondition(player2Score))) numBalls = 0;
}
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB) {
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}
void processInput()
{
    gDittoPaddle1Movement = {0.0f, 0.0f};
    gDittoPaddle2Movement = {0.0f, 0.0f};
    if (IsKeyDown(KEY_W) && gDittoPaddle1Position.y > 50.0f) gDittoPaddle1Movement.y = -1.0 ;
    else if (IsKeyDown(KEY_S) && gDittoPaddle1Position.y < SCREEN_HEIGHT-50.0f) gDittoPaddle1Movement.y = 1.0;

    if (mode == DOUBLE) {
        if (IsKeyDown(KEY_UP) && gDittoPaddle2Position.y > 50.0f) gDittoPaddle2Movement.y = -1.0;
        else if (IsKeyDown(KEY_DOWN) && gDittoPaddle2Position.y < SCREEN_HEIGHT-50.0f) gDittoPaddle2Movement.y = 1.0;
    }
    //if press t, switch between player modes
    if (IsKeyPressed(KEY_T)) {
        if (mode == SINGLE) mode = DOUBLE;
        else if (mode == DOUBLE) {
            mode = SINGLE;
          //  paddle2Dir = yDir;
        }
    }

    if (IsKeyPressed(KEY_ONE)) numBalls = 1;
    if (IsKeyPressed(KEY_TWO)) numBalls = 2;
    if (IsKeyPressed(KEY_THREE)) numBalls = 3;

   // exit game
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void renderObject(const Texture2D* texture, const Vector2* position,
                  const Vector2* scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture,
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}


void initialize() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ditto Pong Game");

    gDittoBallTexture = LoadTexture(DITTOBALL);
    gDittoPaddle1Texture = LoadTexture(DITTOPADDLE);
    gDittoPaddle2Texture = LoadTexture(DITTOPADDLE);


}

void update() {

    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

   gDittoPaddle1Position = {
        gDittoPaddle1Position.x + gDittoPaddle1Movement.x,
        gDittoPaddle1Position.y + gDittoPaddle1Movement.y * PADDLE_SPEED * deltaTime
    };

    if (mode == DOUBLE) {
        if (gDittoPaddle2Position.y > 0.0f && gDittoPaddle2Position.y < SCREEN_HEIGHT) {
            gDittoPaddle2Position = {
                gDittoPaddle2Position.x + gDittoPaddle2Movement.x,
                gDittoPaddle2Position.y + gDittoPaddle2Movement.y * PADDLE_SPEED * deltaTime
            };
        }

    }else if (mode == SINGLE) {
        Ball closestBall = returnClosestBall(balls);
        if (gDittoPaddle2Position.y > 50.0f && paddle2Dir == UP){
            if (closestBall.getPosition().x > gDittoPaddle2Position.x - 500.0f) gDittoPaddle2Position.y -= PADDLE_SPEED * deltaTime;
            if (closestBall.getPosition().x > gDittoPaddle2Position.x - 500.0f) {
                if (closestBall.getYDirection() == UP && gDittoPaddle2Position.y < closestBall.getPosition().y) paddle2Dir = DOWN;
                else if (closestBall.getYDirection() == DOWN && gDittoPaddle2Position.y > closestBall.getPosition().y) paddle2Dir = UP;
                else paddle2Dir = closestBall.getYDirection();
            }
        }else if (gDittoPaddle2Position.y < SCREEN_HEIGHT - 50.0 && paddle2Dir == DOWN) {
            if (closestBall.getPosition().x > gDittoPaddle2Position.x - 500.0f) gDittoPaddle2Position.y += PADDLE_SPEED * deltaTime;
            if (closestBall.getPosition().x > gDittoPaddle2Position.x - 500.0f) {
                if (closestBall.getYDirection() == UP
                    && gDittoPaddle2Position.y < closestBall.getPosition().y) paddle2Dir = DOWN;
                else if (closestBall.getYDirection() == DOWN && gDittoPaddle2Position.y > closestBall.getPosition().y) paddle2Dir = UP;
                else paddle2Dir = closestBall.getYDirection();
            }
        }else if (gDittoPaddle2Position.y > SCREEN_HEIGHT - 50.0) {
            gDittoPaddle2Position.y -= PADDLE_SPEED * deltaTime;
            paddle2Dir = UP;
        }else if (gDittoPaddle2Position.y < 50.0f) {
            gDittoPaddle2Position.y += PADDLE_SPEED * deltaTime;
            paddle2Dir = DOWN;
        }
    }

    // Ball closestBall = returnClosestBall(balls);
    // if (gDittoPaddle2Position.y > 50.0f && paddle2Dir == UP){
    //     gDittoPaddle2Position.y -= PADDLE_SPEED * deltaTime;
    //     if (gBall1Position.x > gDittoPaddle2Position.x - 400.0f) {
    //         if (yDir == UP && gDittoPaddle2Position.y < gBall1Position.y) paddle2Dir = DOWN;
    //         else if (yDir == DOWN && gDittoPaddle2Position.y > gBall1Position.y) paddle2Dir = UP;
    //         else paddle2Dir = yDir;
    //     }
    // }else if (gDittoPaddle2Position.y < SCREEN_HEIGHT - 50.0 && paddle2Dir == DOWN) {
    //     gDittoPaddle2Position.y += PADDLE_SPEED * deltaTime;
    //     if (gBall1Position.x > gDittoPaddle2Position.x - 400.0f) {
    //         if (yDir == UP && gDittoPaddle2Position.y < gBall1Position.y) paddle2Dir = DOWN;
    //         else if (yDir == DOWN && gDittoPaddle2Position.y > gBall1Position.y) paddle2Dir = UP;
    //         else paddle2Dir = yDir;
    //     }
    // }else if (gDittoPaddle2Position.y > SCREEN_HEIGHT - 50.0) {
    //     gDittoPaddle2Position.y -= PADDLE_SPEED * deltaTime;
    //     paddle2Dir = UP;
    // }else if (gDittoPaddle2Position.y < 50.0f) {
    //     gDittoPaddle2Position.y += PADDLE_SPEED * deltaTime;
    //     paddle2Dir = DOWN;
    // }
    // if (isColliding(&gDittoPaddle1Position, &gDittoPaddle1Scale, &gBall1Position, &gBall1Scale)) {
    //     xDir = RIGHT;
    //     if (gDittoPaddle1Movement.y > 0.0f) yDir = DOWN;
    //     else yDir = UP;
    // }
    //
    // if (isColliding(&gDittoPaddle2Position, &gDittoPaddle2Scale, &gBall1Position, &gBall1Scale)) {
    //     xDir = LEFT;
    //     if (gDittoPaddle2Movement.y > 0.0f) yDir = DOWN;
    //     else yDir = UP;
    // }

    while (balls.size() < numBalls) {
        if (balls.empty()) balls.emplace_back(ORIGIN, BASE_SIZE, RIGHT, UP, &gDittoBallTexture);
        else if (balls.size() == 1) balls.emplace_back(ORIGINOFFSET1, BASE_SIZE, RIGHT, UP, &gDittoBallTexture);
        else if (balls.size() == 2) balls.emplace_back(ORIGINOFFSET2, BASE_SIZE, RIGHT, DOWN, &gDittoBallTexture);
    }

    while (balls.size() > numBalls) {
        balls.pop_back();
    }
    for (Ball& curBall : balls) {
        if (isColliding(&gDittoPaddle1Position, &gDittoPaddle1Scale,
                    &curBall.getPosition(), &curBall.getScale())) {
            curBall.updateX(RIGHT);
            if (gDittoPaddle1Movement.y > 0.0f) curBall.updateY(DOWN);
            else curBall.updateY(UP);
        }

        if (isColliding(&gDittoPaddle2Position, &gDittoPaddle2Scale,
            &curBall.getPosition(), &curBall.getScale())) {
            curBall.updateX(LEFT);
            if (gDittoPaddle2Movement.y > 0.0f) curBall.updateY(DOWN);
            else curBall.updateY(UP);
        }

        if (curBall.getPosition().x < SCREEN_WIDTH - 100.0f && curBall.getXDirection() == RIGHT) {
            curBall.updatePosition(BALL_SPEED * deltaTime, 0.0f);
        }else if (curBall.getPosition().x > 100.0f && curBall.getXDirection() == LEFT) {
            curBall.updatePosition(-BALL_SPEED * deltaTime, 0.0f);
        }else if (curBall.getPosition().x > SCREEN_WIDTH - 100.0f) {
            curBall.setPosition(ORIGIN.x, ORIGIN.y);
            player1Score++;
            recalculateBalls();
        }else if (curBall.getPosition().x < 100.0f) {
            curBall.setPosition(ORIGIN.x, ORIGIN.y);
            player2Score++;
            recalculateBalls();
        }

        if (curBall.getPosition().y > 100.0f && curBall.getYDirection() == UP) {
            curBall.updatePosition(0.0f, -BALL_SPEED * deltaTime);
        }else if (curBall.getPosition().y < SCREEN_HEIGHT - 100.0f && curBall.getYDirection() == DOWN) {
            curBall.updatePosition(0.0f, BALL_SPEED * deltaTime);
        }else if (curBall.getPosition().y < 100.0f) {
            curBall.updatePosition(0.0f, -BALL_SPEED * deltaTime);
            curBall.updateY(DOWN);
        }else if (curBall.getPosition().y >  SCREEN_HEIGHT - 100.0f) {
            curBall.updatePosition(0.0f, BALL_SPEED * deltaTime);
            curBall.updateY(UP);
        }
    }
    // if(gBall1Position.x < SCREEN_WIDTH - 100.0f && xDir == RIGHT){
    //     gBall1Position.x += BALL_SPEED * deltaTime;
    // }else if (gBall1Position.x > 100.0f && xDir == LEFT) {
    //     gBall1Position.x -= BALL_SPEED * deltaTime;
    // }else if (gBall1Position.x > SCREEN_WIDTH - 100.0f) {
    //     gBall1Position = ORIGIN;
    //     player1Score ++;
    // }else if (gBall1Position.x < 100.0f) {
    //     gBall1Position = ORIGIN;
    //     player2Score ++;
    // }

    // if (gBall1Position.y > 100.0f && yDir == UP) {
    //     gBall1Position.y -= BALL_SPEED * deltaTime;
    // }else if (gBall1Position.y < SCREEN_HEIGHT - 100.0f && yDir == DOWN) {
    //     gBall1Position.y += BALL_SPEED * deltaTime;
    // }else if (gBall1Position.y < 100.0f) {
    //     gBall1Position.y -= BALL_SPEED * deltaTime;
    //     yDir = DOWN;
    // }else if (gBall1Position.y >  SCREEN_HEIGHT - 100.0f) {
    //     gBall1Position.y += BALL_SPEED * deltaTime;
    //     yDir = UP;
    // }


    // }else if (mode == SINGLE) {
    //     float ball1DistanceFromPaddle2 = gBall1Position.x - gDittoPaddle2Position.x;
    //     float ball2DistanceFromPaddle2 = gBall2Position.x - gDittoPaddle2Position.x;
    //     float ball3DistanceFromPaddle2 = gBall3Position.x - gDittoPaddle2Position.x;
    //     if (ball1DistanceFromPaddle2 < 100.0f || ball2DistanceFromPaddle2 < 100.0f || ball3DistanceFromPaddle2 < 100.0f) {
    //         if (gBall1Position.y > gDittoPaddle2Position.y) gDittoPaddle2Movement.y = -0.1f;
    //         else gDittoPaddle2Movement.y = 0.1f;
    //     }
    //
    // }

}
void render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText(TextFormat("%d", player1Score), 500, 100, 50, BLACK);
    DrawText(TextFormat("%d", player2Score), 900, 100, 50, BLACK);

    renderObject(&gDittoPaddle1Texture, &gDittoPaddle1Position, &gDittoPaddle1Scale);
    renderObject(&gDittoPaddle2Texture, &gDittoPaddle2Position, &gDittoPaddle2Scale);

    if (winCondition(player1Score)) DrawText("Player 1 Wins!", ORIGIN.x - 300.0f, ORIGIN.y, 100, BLACK);
    else if (winCondition(player2Score)) DrawText("Player 2 Wins!", ORIGIN.x - 300.0f, ORIGIN.y, 100, BLACK);
    else if (!winCondition(player1Score) && !winCondition(player2Score)) {
        for (const Ball& ball : balls){
            ball.draw();
        }
    }


    EndDrawing();

}
void shutdown() {
    balls.clear();
    UnloadTexture(gDittoBallTexture);
    UnloadTexture(gDittoPaddle1Texture);
    UnloadTexture(gDittoPaddle2Texture);
    CloseWindow();
}
int main() {
    initialize();

    while (gAppStatus == RUNNING) {
        processInput();
        if (!winCondition(player1Score) && !winCondition(player2Score)) update();
        render();
    }

    shutdown();
    return 0;
}