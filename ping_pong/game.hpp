#pragma once

struct Vector2
{
    float x;
    float y;
};

struct Paddle
{
    Vector2 PaddlePos;
    int PaddleDir;
};

class Game
{
public:
    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

private:
    SDL_Window* mWindow;
    bool mIsRunning;

    SDL_Renderer* mRenderer = nullptr;

    const int thickness = 15;

    const float WINDOW_WIDTH = 1024.0f;
    const float WINDOW_HEIGHT = 768.0f;
    const int PADDLE_HEIGHT = 100;

    Paddle mLeftPaddle;
    Paddle mRightPaddle;

    Vector2 mBallPos = { (float)(WINDOW_WIDTH / 2.0f), (float)(WINDOW_HEIGHT / 2.0f) };
    Vector2 mBallVel = {200.0f, -235.0f};
};