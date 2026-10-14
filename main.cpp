#include "raylib.h"
#include <iostream>
#include <math.h>

using namespace std;
// Enums
enum AppStatus { TERMINATED, RUNNING };
enum PlayerMode { SINGLE, DOUBLE };
enum Direction {RIGHT, LEFT, UP, DOWN};
// Global Constants
constexpr int SCREEN_WIDTH  = 1000 * 1.5f,
              SCREEN_HEIGHT = 600 * 1.5f,
              FPS           = 60,
              SIZE          = 100;
float BALL_SPEED = 400.0f;
float PADDLE_SPEED = 250.0f;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 LEFT_START = {100.0f, SCREEN_HEIGHT / 2};
constexpr Vector2 RIGHT_START = {SCREEN_WIDTH - 100.0f, SCREEN_HEIGHT / 2};
constexpr Vector2 BASE_SIZE   = { static_cast<float>(SIZE), static_cast<float>(SIZE) };


constexpr char DITTOBALL[] = "assets/ditto.png";
constexpr char DITTOPADDLE1[] = "assets/ditto.png";
constexpr char DITTOPADDLE2[] = "assets/ditto.png";





//constexpr char DITTO[] = "assets/ditto.png";
// Global Variables
AppStatus gAppStatus = RUNNING;
Vector2 gScale = BASE_SIZE;
Vector2 gPositionDitto = ORIGIN;
float gPreviousTicks = 0.0f;

float gAngle = 0.0f;

PlayerMode mode = DOUBLE;
Direction xDir = RIGHT;
Direction yDir = UP;
Direction paddle2Dir = UP;

Vector2 gDittoPaddle1Position = LEFT_START,
        gDittoPaddle1Movement = {0.0f, 0.0f},
        gDittoPaddle1Scale = BASE_SIZE,

        gDittoPaddle2Position = RIGHT_START,
        gDittoPaddle2Movement = {0.0f, 0.0f},
        gDittoPaddle2Scale = BASE_SIZE,

        gBall1Position = ORIGIN,
        gBall1Movement = {0.0f, 0.0f},
        gBall1Scale = BASE_SIZE,

        gBall2Position = ORIGIN,
      //  gBall2Movement = {0.0f, 0.0f},
        gBall2Scale = BASE_SIZE,

        gBall3Position = ORIGIN,
      //  gBall3Movement = {0.0f, 0.0f},
        gBall3Scale = BASE_SIZE;

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
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions
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
            paddle2Dir = yDir;
        }
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
        if (gDittoPaddle2Position.y > 50.0f && paddle2Dir == UP){
            gDittoPaddle2Position.y -= PADDLE_SPEED * deltaTime;
            if (gBall1Position.x > gDittoPaddle2Position.x - 400.0f) {
                if (yDir == UP && gDittoPaddle2Position.y < gBall1Position.y) paddle2Dir = DOWN;
                else if (yDir == DOWN && gDittoPaddle2Position.y > gBall1Position.y) paddle2Dir = UP;
                else paddle2Dir = yDir;
            }
        }else if (gDittoPaddle2Position.y < SCREEN_HEIGHT - 50.0 && paddle2Dir == DOWN) {
            gDittoPaddle2Position.y += PADDLE_SPEED * deltaTime;
            if (gBall1Position.x > gDittoPaddle2Position.x - 400.0f) {
                if (yDir == UP && gDittoPaddle2Position.y < gBall1Position.y) paddle2Dir = DOWN;
                else if (yDir == DOWN && gDittoPaddle2Position.y > gBall1Position.y) paddle2Dir = UP;
                else paddle2Dir = yDir;
            }
        }else if (gDittoPaddle2Position.y > SCREEN_HEIGHT - 50.0) {
            gDittoPaddle2Position.y -= PADDLE_SPEED * deltaTime;
            paddle2Dir = UP;
        }else if (gDittoPaddle2Position.y < 50.0f) {
            gDittoPaddle2Position.y += PADDLE_SPEED * deltaTime;
            paddle2Dir = DOWN;
        }
    }

    if (isColliding(&gDittoPaddle1Position, &gDittoPaddle1Scale, &gBall1Position, &gBall1Scale)) {
        xDir = RIGHT;
        if (gDittoPaddle1Movement.y > 0.0f) yDir = DOWN;
        else yDir = UP;
    }

    if (isColliding(&gDittoPaddle2Position, &gDittoPaddle2Scale, &gBall1Position, &gBall1Scale)) {
        xDir = LEFT;
        if (gDittoPaddle2Movement.y > 0.0f) yDir = DOWN;
        else yDir = UP;
    }

    if(gBall1Position.x < SCREEN_WIDTH - 100.0f && xDir == RIGHT){
        gBall1Position.x += BALL_SPEED * deltaTime;
    }else if (gBall1Position.x > 100.0f && xDir == LEFT) {
        gBall1Position.x -= BALL_SPEED * deltaTime;
    }else if (gBall1Position.x > SCREEN_WIDTH - 100.0f) {
        gBall1Position = ORIGIN;
        player1Score ++;
    }else if (gBall1Position.x < 100.0f) {
        gBall1Position = ORIGIN;
        player2Score ++;
    }

    if (gBall1Position.y > 100.0f && yDir == UP) {
        gBall1Position.y -= BALL_SPEED * deltaTime;
    }else if (gBall1Position.y < SCREEN_HEIGHT - 100.0f && yDir == DOWN) {
        gBall1Position.y += BALL_SPEED * deltaTime;
    }else if (gBall1Position.y < 100.0f) {
        gBall1Position.y -= BALL_SPEED * deltaTime;
        yDir = DOWN;
    }else if (gBall1Position.y >  SCREEN_HEIGHT - 100.0f) {
        gBall1Position.y += BALL_SPEED * deltaTime;
        yDir = UP;
    }


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
    //makes the object's origin at its origin
    // Vector2 objectOrigin = {gScale.x / 2.0f, gScale.y / 2.0f};
    //
    // //background drawing
    // Rectangle textureAreaDittoBall = {
    //     0.0f, 0.0f,
    //     static_cast<float>(gDittoBallTexture.width),
    //     static_cast<float>(gDittoBallTexture.height),
    // };
    // Rectangle destinationAreaDittoBall = {
    //     ORIGIN.x, ORIGIN.y,
    //     gScale.x, gScale.y
    // };
    //
    // DrawTexturePro(gDittoBallTexture, textureAreaDittoBall,
    //     destinationAreaDittoBall, objectOrigin, gAngle, WHITE);
    //
    // Rectangle textureAreaDittoPaddle1 = {
    //     0.0f, 0.0f,
    //     static_cast<float>(gDittoPaddle1Texture.width),
    //     static_cast<float>(gDittoPaddle1Texture.height)
    // };
    //
    // Rectangle destinationAreaDittoPaddle1 = {gPlayer1Position.x, gPlayer1Position.y,
    //     gScale.x, gScale.y
    //
    // };
    //
    // DrawTexturePro(gDittoPaddle1Texture, textureAreaDittoPaddle1,
    //     destinationAreaDittoPaddle1, objectOrigin, gAngle, WHITE);
    //
    // Rectangle textureAreaDittoPaddle2 = {
    //     0.0f, 0.0f,
    //     static_cast<float>(gDittoPaddle2Texture.width),
    //         static_cast<float>(gDittoPaddle2Texture.height)
    // };
    //
    // Rectangle destinationAreaDittoPaddle2 = {
    //     gPlayer2Position.x, gPlayer2Position.y,
    //     gScale.x, gScale.y
    // };
    //
    // DrawTexturePro(gDittoPaddle2Texture, textureAreaDittoPaddle2,
    //     destinationAreaDittoPaddle2, objectOrigin, gAngle, WHITE);

    renderObject(&gDittoPaddle1Texture, &gDittoPaddle1Position, &gDittoPaddle1Scale);
    renderObject(&gDittoPaddle2Texture, &gDittoPaddle2Position, &gDittoPaddle2Scale);
    renderObject(&gDittoBallTexture, &gBall1Position, &gBall1Scale);
    EndDrawing();

}
void shutdown() {
    CloseWindow();
}
int main() {
    initialize();

    while (gAppStatus == RUNNING) {
        processInput();
        update();
        render();
    }

    shutdown();
    return 0;
}