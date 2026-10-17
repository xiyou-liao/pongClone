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
enum NUMBER { ZERO, ONE, TWO, THREE, FOUR, FIVE };
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
constexpr Vector2 BACKGROUND_SIZE = {static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)};

constexpr char DITTOBALL[] = "assets/dittoBall.png";
constexpr char DITTOPADDLE1[] = "assets/dittoPaddle1.png";
constexpr char DITTOPADDLE2[] = "assets/dittoPaddle2.png";
constexpr char PLAYER1WIN[] = "assets/player1Win.png";
constexpr char PLAYER2WIN[] = "assets/player2Win.png";
constexpr char SCORES[] = "assets/scores.png";
constexpr char BACKGROUND[] = "assets/dittoBackground.png";


// Global Variables
AppStatus gAppStatus = RUNNING;
Vector2 gScale = BASE_SIZE;
Vector2 gPositionDitto = ORIGIN;
float gPreviousTicks = 0.0f;

float gAngle = 0.0f;

PlayerMode mode = DOUBLE;
Direction paddle2Dir = UP;
NUMBER player1 = ZERO;
NUMBER player2 = ZERO;

int numBalls = 1;


Vector2 gDittoPaddle1Position = LEFT_START,
        gDittoPaddle1Movement = {0.0f, 0.0f},
        gDittoPaddle1Scale = {75.0f, 200.0f},

        gDittoPaddle2Position = RIGHT_START,
        gDittoPaddle2Movement = {0.0f, 0.0f},
        gDittoPaddle2Scale = {75.0f, 200.0f},

        gPlayer1ScoresPosition = {600.0f, 100.0f },
        gPlayer2ScoresPosition = {1000.0f, 100.0f},

        gBackgroundPosition = {0.0f, 0.0f},
        gPlayerWinScale = {500.0f, 300.0f};

int player1Score = 0;
int player2Score = 0;

Texture2D gDittoPaddle1Texture;
Texture2D gDittoPaddle2Texture;
Texture2D gDittoBallTexture;
Texture2D gScoresTexture;
Texture2D gPlayer1WinTexture;
Texture2D gPlayer2WinTexture;
Texture2D gBackgroundTexture;

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

    if (IsKeyPressed(KEY_R)) {
        player1Score = 0;
        player2Score = 0;
        numBalls = 1;
        gDittoPaddle1Position = LEFT_START;
        gDittoPaddle2Position = RIGHT_START;
        mode = DOUBLE;
    }
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
    gDittoPaddle1Texture = LoadTexture(DITTOPADDLE1);
    gDittoPaddle2Texture = LoadTexture(DITTOPADDLE2);
    gScoresTexture = LoadTexture(SCORES);
    gPlayer1WinTexture = LoadTexture(PLAYER1WIN);
    gPlayer2WinTexture = LoadTexture(PLAYER2WIN);
    gBackgroundTexture = LoadTexture(BACKGROUND);

}

void update() {

    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    if (gDittoPaddle1Position.y > 100.0f && gDittoPaddle1Position.y < SCREEN_HEIGHT - 100.0f) {
        gDittoPaddle1Position = {
            gDittoPaddle1Position.x + gDittoPaddle1Movement.x,
            gDittoPaddle1Position.y + gDittoPaddle1Movement.y * PADDLE_SPEED * deltaTime
        };
    }
    else if (gDittoPaddle1Position.y < 100.0f) gDittoPaddle1Position.y += 0.2f;
    else if (gDittoPaddle1Position.y > SCREEN_HEIGHT - 100.0f) gDittoPaddle1Position.y -= 0.2f;


    if (mode == DOUBLE) {
        if (gDittoPaddle2Position.y > 100.0f  && gDittoPaddle2Position.y < SCREEN_HEIGHT - 100.0f) {
            gDittoPaddle2Position = {
                gDittoPaddle2Position.x + gDittoPaddle2Movement.x,
                gDittoPaddle2Position.y + gDittoPaddle2Movement.y * PADDLE_SPEED * deltaTime
            };
        }
        else if (gDittoPaddle2Position.y < 100.0f) gDittoPaddle2Position.y += 0.2f;
        else if (gDittoPaddle2Position.y > SCREEN_HEIGHT - 100.0f) gDittoPaddle2Position.y -= 0.2f;

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

    if (player1Score == 0) player1 = ZERO;
    else if (player1Score == 1) player1 = ONE;
    else if (player1Score == 2) player1 = TWO;
    else if (player1Score == 3) player1 = THREE;
    else if (player1Score == 4) player1 = FOUR;
    else if (player1Score == 5) player1 = FIVE;

    if (player2Score == 0) player2 = ZERO;
    else if (player2Score == 1) player2 = ONE;
    else if (player2Score == 2) player2 = TWO;
    else if (player2Score == 3) player2 = THREE;
    else if (player2Score == 4) player2 = FOUR;
    else if (player2Score == 5) player2 = FIVE;

}
void render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    renderObject(&gBackgroundTexture, &ORIGIN, &BACKGROUND_SIZE);
    renderObject(&gDittoPaddle1Texture, &gDittoPaddle1Position, &gDittoPaddle1Scale);
    renderObject(&gDittoPaddle2Texture, &gDittoPaddle2Position, &gDittoPaddle2Scale);
    if (winCondition(player1Score)) {
        renderObject(&gPlayer1WinTexture, &ORIGIN, &gPlayerWinScale);
    }
    if (winCondition(player2Score)) {
        renderObject(&gPlayer2WinTexture, &ORIGIN, &gPlayerWinScale);
    }

    float player1ScoreX = 0.0f, player1ScoreY = 0.0f,
          player2ScoreX = 0.0f, player2ScoreY = 0.0f,
           scoreWidth = static_cast<float>(gScoresTexture.width) / 6.0f;
    if (player1 == ZERO) player1ScoreX = 0.0f;
    else if(player1 == ONE) player1ScoreX = scoreWidth;
    else if(player1 == TWO) player1ScoreX = scoreWidth * 2.0f;
    else if(player1 == THREE) player1ScoreX = scoreWidth * 3.0f;
    else if(player1 == FOUR) player1ScoreX = scoreWidth * 4.0f;
    else if(player1 == FIVE) player1ScoreX = scoreWidth * 5.0f;

    if (player2 == ZERO) player2ScoreX = 0.0f;
    else if(player2 == ONE) player2ScoreX = scoreWidth;
    else if(player2 == TWO) player2ScoreX = scoreWidth * 2.0f;
    else if(player2 == THREE) player2ScoreX = scoreWidth * 3.0f;
    else if(player2 == FOUR) player2ScoreX = scoreWidth * 4.0f;
    else if(player2 == FIVE) player2ScoreX = scoreWidth * 5.0f;

    Vector2 objectOrigin = {gScale.x / 2.0f, gScale.y / 2.0f};

    Rectangle textureAreaPlayer1Score = {
        player1ScoreX, player1ScoreY,

        scoreWidth,
        static_cast<float>(gScoresTexture.height)
    };

    Rectangle destinationAreaPlayer1Score = {
        gPlayer1ScoresPosition.x, gPlayer1ScoresPosition.y,
        gScale.x, gScale.y
    };

    DrawTexturePro(gScoresTexture, textureAreaPlayer1Score,
        destinationAreaPlayer1Score, objectOrigin, gAngle, WHITE);


    Rectangle textureAreaPlayer2Score = {
        player2ScoreX, player2ScoreY,

        scoreWidth,
        static_cast<float>(gScoresTexture.height)
    };

    Rectangle destinationAreaPlayer2Score = {
        gPlayer2ScoresPosition.x, gPlayer2ScoresPosition.y,

        gScale.x, gScale.y
    };

    DrawTexturePro(gScoresTexture, textureAreaPlayer2Score,
        destinationAreaPlayer2Score, objectOrigin, gAngle, WHITE);

    if (!winCondition(player1Score) && !winCondition(player2Score)) {
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
    UnloadTexture(gScoresTexture);
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