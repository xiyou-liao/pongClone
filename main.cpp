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
constexpr Vector2 ORIGINOFFSET2 = {SCREEN_WIDTH / 2 - 5.0f, SCREEN_HEIGHT / 2 + 7.0f};
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
constexpr char BGM[] = "assets/Maple_Leaf_Rag.mp3";

// Global Variables
AppStatus gAppStatus = RUNNING;
Vector2 gScale = BASE_SIZE;
Vector2 gPositionDitto = ORIGIN;
float gPreviousTicks = 0.0f;
float gAngle = 0.0f;

PlayerMode mode = DOUBLE;
Direction paddle2Dir = UP;

int numBalls = 1;
int player1Score = 0;
int player2Score = 0;

Vector2 gDittoPaddle1Position = LEFT_START,
        gDittoPaddle1Movement = {0.0f, 0.0f},
        gDittoPaddle1Scale = {75.0f, 200.0f},

        gDittoPaddle2Position = RIGHT_START,
        gDittoPaddle2Movement = {0.0f, 0.0f},
        gDittoPaddle2Scale = {75.0f, 200.0f},

        gPlayer1ScoresPosition = {600.0f, 100.0f },
        gPlayer2ScoresPosition = {1050.0f, 100.0f},

        gBackgroundPosition = {0.0f, 0.0f},
        gPlayerWinScale = {500.0f, 300.0f};

Texture2D gDittoPaddle1Texture;
Texture2D gDittoPaddle2Texture;
Texture2D gDittoBallTexture;
Texture2D gScoresTexture;
Texture2D gPlayer1WinTexture;
Texture2D gPlayer2WinTexture;
Texture2D gBackgroundTexture;
Music gBGM;

// Function Declarations
void initialize();
void processInput();
void update();
void render();
void renderObject(const Texture2D* texture, const Vector2* position, const Vector2* scale);
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);
Ball returnClosestBall(const vector<Ball>& ballsInPlay);

class Ball {
public:
    explicit Ball(const Vector2 position = ORIGIN, const Vector2& scale = BASE_SIZE,
        const Direction& xDir = RIGHT, const Direction& yDir = UP, Texture2D* texture = &gDittoBallTexture) :
                            position(position), scale(scale), xDir(xDir), yDir(yDir), texture(texture) {}

    const Vector2& getPosition() const { return position; }
    const Vector2& getScale() const { return scale; }
    const Direction& getXDirection() const {return xDir;}
    const Direction& getYDirection() const {return yDir;}

    void updateX(const Direction& dir) {xDir = dir;}
    void updateY(const Direction& dir) {yDir = dir;}
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

vector<Ball> balls; //vector holding all the balls currently in play

// Function Definitions
Ball returnClosestBall(const vector<Ball>& ballsInPlay) {
    Ball closestBall;
    for (const Ball& curBall : ballsInPlay) {
        if (curBall.getPosition().x > closestBall.getPosition().x) {
            if (curBall.getXDirection() == RIGHT){
                closestBall = curBall;
            }
        }
    }
    return closestBall;
}

bool winCondition(int score) {
    return score == 5;
}
void recalculateBalls() {
    if (numBalls > 1) numBalls--;
}

//checks to see if two objects are colliding using box-to-box collision detection
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB) {
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

void processInput() {
    gDittoPaddle1Movement = {0.0f, 0.0f};
    gDittoPaddle2Movement = {0.0f, 0.0f};
    //takes user input to update the movement of the paddles
    if (IsKeyDown(KEY_W) && gDittoPaddle1Position.y > 50.0f) gDittoPaddle1Movement.y = -1.0 ;
    else if (IsKeyDown(KEY_S) && gDittoPaddle1Position.y < SCREEN_HEIGHT-50.0f) gDittoPaddle1Movement.y = 1.0;

    if (mode == DOUBLE) {
        if (IsKeyDown(KEY_UP) && gDittoPaddle2Position.y > 50.0f) gDittoPaddle2Movement.y = -1.0;
        else if (IsKeyDown(KEY_DOWN) && gDittoPaddle2Position.y < SCREEN_HEIGHT-50.0f) gDittoPaddle2Movement.y = 1.0;
    }
    //if press t, switch between player modes
    if (IsKeyPressed(KEY_T)) {
        if (mode == SINGLE) mode = DOUBLE;
        else if (mode == DOUBLE) mode = SINGLE;
    }

    //toggle the number of balls in play
    if (IsKeyPressed(KEY_ONE)) numBalls = 1;
    if (IsKeyPressed(KEY_TWO)) numBalls = 2;
    if (IsKeyPressed(KEY_THREE)) numBalls = 3;

    //reset the game
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

void renderObject(const Texture2D* texture, const Vector2* position, const Vector2* scale) {
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture->width), static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        position->x, position->y,
        static_cast<float>(scale->x), static_cast<float>(scale->y)
    };

    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f, static_cast<float>(scale->y) / 2.0f
    };

    DrawTexturePro(*texture, textureArea, destinationArea, originOffset, gAngle, WHITE);
}


void initialize() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ditto Pong Game");
    InitAudioDevice();

    gDittoBallTexture = LoadTexture(DITTOBALL);
    gDittoPaddle1Texture = LoadTexture(DITTOPADDLE1);
    gDittoPaddle2Texture = LoadTexture(DITTOPADDLE2);
    gScoresTexture = LoadTexture(SCORES);
    gPlayer1WinTexture = LoadTexture(PLAYER1WIN);
    gPlayer2WinTexture = LoadTexture(PLAYER2WIN);
    gBackgroundTexture = LoadTexture(BACKGROUND);

    gBGM = LoadMusicStream(BGM);
    SetMusicVolume(gBGM, 0.33f);
    PlayMusicStream(gBGM);
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

    if (mode == DOUBLE) { //allows player to control right side paddle in multiplayer mode
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

        //once the closest ball is within 500 units from the computer-controlled paddle, paddle starts moving
        //if the closest ball is above the paddle, paddle go up
        //if the closest ball is below the paddle, paddle go down;
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

    //adds the correct number of balls into the vector containing balls in play
    //adds each ball in a slightly offset position and direction to prevent all the balls from stacking in one group
    while (balls.size() < numBalls) {
        if (balls.empty()) balls.emplace_back(ORIGIN, BASE_SIZE, RIGHT, UP, &gDittoBallTexture);
        else if (balls.size() == 1) balls.emplace_back(ORIGINOFFSET1, BASE_SIZE, RIGHT, UP, &gDittoBallTexture);
        else if (balls.size() == 2) balls.emplace_back(ORIGINOFFSET2, BASE_SIZE, RIGHT, DOWN, &gDittoBallTexture);
    }

    //gets rid of balls that should not be in play
    while (balls.size() > numBalls) {
        balls.pop_back();
    }

    //handles collisions for each ball
    //if a ball collides with a paddle:
        //the ball will head in the opposite x direction that it came from
        //the ball will go in the same y direction as the paddle that hit it was going
    //if the ball goes out of bounds on either side, the corresponding side's player will
    //score a point and the ball will disappear from the game play
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

    UpdateMusicStream(gBGM);
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
    if (player1Score == 0) player1ScoreX = 0.0f;
    else if(player1Score == 1) player1ScoreX = scoreWidth;
    else if(player1Score == 2) player1ScoreX = scoreWidth * 2.0f;
    else if(player1Score == 3) player1ScoreX = scoreWidth * 3.0f;
    else if(player1Score == 4) player1ScoreX = scoreWidth * 4.0f;
    else if(player1Score == 5) player1ScoreX = scoreWidth * 5.0f;

    if (player2Score == 0) player2ScoreX = 0.0f;
    else if(player2Score == 1) player2ScoreX = scoreWidth;
    else if(player2Score == 2) player2ScoreX = scoreWidth * 2.0f;
    else if(player2Score == 3) player2ScoreX = scoreWidth * 3.0f;
    else if(player2Score == 4) player2ScoreX = scoreWidth * 4.0f;
    else if(player2Score == 5) player2ScoreX = scoreWidth * 5.0f;

    Vector2 objectOrigin = {gScale.x / 2.0f, gScale.y / 2.0f};

    Rectangle textureAreaPlayer1Score = {
        player1ScoreX, player1ScoreY,

        scoreWidth, static_cast<float>(gScoresTexture.height)
    };

    Rectangle destinationAreaPlayer1Score = {
        gPlayer1ScoresPosition.x, gPlayer1ScoresPosition.y,
        gScale.x, gScale.y
    };

    DrawTexturePro(gScoresTexture, textureAreaPlayer1Score,
        destinationAreaPlayer1Score, objectOrigin, gAngle, WHITE);


    Rectangle textureAreaPlayer2Score = {
        player2ScoreX, player2ScoreY,

        scoreWidth, static_cast<float>(gScoresTexture.height)
    };

    Rectangle destinationAreaPlayer2Score = {
        gPlayer2ScoresPosition.x, gPlayer2ScoresPosition.y,

        gScale.x, gScale.y
    };

    DrawTexturePro(gScoresTexture, textureAreaPlayer2Score,
        destinationAreaPlayer2Score, objectOrigin, gAngle, WHITE);

    //draw each ball in play
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
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gPlayer1WinTexture);
    UnloadTexture(gPlayer2WinTexture);
    UnloadMusicStream(gBGM);
    CloseAudioDevice();
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