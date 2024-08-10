#include "game.hpp"

Game::Game() : mWindow(nullptr), mIsRunning(true), mTicksToCount(0)
{
    mLeftPaddle.PaddlePos = { 10, (WINDOW_WIDTH / 2.0f) - (PADDLE_HEIGHT / 2.0f) };
    mLeftPaddle.PaddleDir = 0;

    mRightPaddle.PaddlePos = {(WINDOW_WIDTH - 25.0f), (WINDOW_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 2.0f) };
    mRightPaddle.PaddleDir = 0;
}

bool Game::Initialize()
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);

    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow(
        "Game Programming",
        100,
        100,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    return true;
}

void Game::RunLoop()
{
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::Shutdown()
{
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::ProcessInput()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            mIsRunning = false;
            break;

        default:
            break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);

    mLeftPaddle.PaddleDir = 0;
    mRightPaddle.PaddleDir = 0;

    if (state[SDL_SCANCODE_ESCAPE]) {
        mIsRunning = false;
    }

    if (state[SDL_SCANCODE_W]) {
        mLeftPaddle.PaddleDir -= 1;
    }

    if (state[SDL_SCANCODE_S]) {
        mLeftPaddle.PaddleDir += 1;
    }

    if (state[SDL_SCANCODE_I]) {
        mRightPaddle.PaddleDir -= 1;
    }

    if (state[SDL_SCANCODE_K]) {
        mRightPaddle.PaddleDir += 1;
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksToCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksToCount) / 1000.0f;

    if (deltaTime > 0.05f) { deltaTime = 0.005f; }

    if (mLeftPaddle.PaddleDir != 0) {
        mLeftPaddle.PaddlePos.y += mLeftPaddle.PaddleDir * 500.0f * deltaTime;

        if (mLeftPaddle.PaddlePos.y < (thickness + 3)) { 
            mLeftPaddle.PaddlePos.y = thickness + 3; 
        }
        else if (mLeftPaddle.PaddlePos.y > (WINDOW_HEIGHT - PADDLE_HEIGHT - thickness - 3)) {
            mLeftPaddle.PaddlePos.y = WINDOW_HEIGHT - PADDLE_HEIGHT - thickness - 3;
        }
    }

    if (mRightPaddle.PaddleDir != 0) {
        mRightPaddle.PaddlePos.y += mRightPaddle.PaddleDir * 500.0f * deltaTime;

        if (mRightPaddle.PaddlePos.y < (thickness + 3)) {
            mRightPaddle.PaddlePos.y = thickness + 3;
        }
        else if (mRightPaddle.PaddlePos.y > (WINDOW_HEIGHT - PADDLE_HEIGHT - thickness - 3)) {
            mRightPaddle.PaddlePos.y = WINDOW_HEIGHT - PADDLE_HEIGHT - thickness - 3;
        }
    }

    if (mBallPos.y <= thickness && mBallVel.y < 0.0f) {
        mBallVel.y *= -1;
    }

    if (mBallPos.y >= WINDOW_HEIGHT - thickness) {
        mBallVel.y *= -1;
    }

    mBallPos.x += mBallVel.x * deltaTime;
    mBallPos.y += mBallVel.y * deltaTime;

    float diff = 0.0f;

    if (mBallVel.x < 0.0f) {
        diff = abs((int)mBallPos.y - (int)mLeftPaddle.PaddlePos.y);

        if ((diff <= (PADDLE_HEIGHT / 2.0f)) && (mBallPos.x <= 25.0f) && (mBallPos.x >= 20.0f)) {
            mBallVel.x *= -1.0f;
        }
    }
    else if (mBallVel.x > 0.0f) {
        diff = abs((int)mBallPos.y - (int)mRightPaddle.PaddlePos.y);

        if ((diff <= PADDLE_HEIGHT / 2.0f) && (mBallPos.x <= (WINDOW_WIDTH - 20.0f)) && (mBallPos.x >= (WINDOW_WIDTH - 25.0f))) {
            mBallVel.x *= -1.0f;
        }
    }

    mTicksToCount = SDL_GetTicks();
}

void Game::GenerateOutput()
{
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);

    SDL_RenderClear(mRenderer);

    SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
    
    SDL_Rect lPaddle {
        static_cast<int>(mLeftPaddle.PaddlePos.x),
        static_cast<int>(mLeftPaddle.PaddlePos.y),
        thickness,
        PADDLE_HEIGHT
    };

    SDL_Rect rPaddle {
        static_cast<int>(mRightPaddle.PaddlePos.x),
        static_cast<int>(mRightPaddle.PaddlePos.y),
        thickness,
        PADDLE_HEIGHT
    };

    SDL_Rect topWall {
        0, 0, WINDOW_WIDTH, thickness
    };

    SDL_Rect bottomWall {
        0, WINDOW_HEIGHT - thickness, WINDOW_WIDTH, thickness
    };

    SDL_Rect ball {
        static_cast<int>(mBallPos.x - thickness / 2),
        static_cast<int>(mBallPos.y - thickness / 2),
        thickness,
        thickness
    };

    SDL_RenderFillRect(mRenderer, &lPaddle);
    SDL_RenderFillRect(mRenderer, &rPaddle);
    SDL_RenderFillRect(mRenderer, &topWall);
    SDL_RenderFillRect(mRenderer, &bottomWall);
    SDL_RenderFillRect(mRenderer, &ball);
    
    SDL_RenderPresent(mRenderer);
}