#include "Game.h"
#include <iostream>

Game::Game()
{
    window = nullptr;
    renderer = nullptr;
    running = false;
}

Game::~Game()
{
    clean();
}

bool Game::init(const char* title, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        std::cout << "Erro ao iniciar SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!window)
    {
        std::cout << "Erro ao criar janela: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer)
    {
        std::cout << "Erro ao criar renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    running = true;
    return true;
}

void Game::handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            running = false;
        }
    }
}

void Game::update()
{
    // Atualizar jogadores, bola, IA e física
}

void Game::render()
{
    // Fundo verde (campo)
    SDL_SetRenderDrawColor(renderer, 40, 150, 40, 255);
    SDL_RenderClear(renderer);

    // Futuramente desenhar:
    // - Campo
    // - Jogadores
    // - Bola
    // - Interface

    SDL_RenderPresent(renderer);
}

void Game::clean()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

bool Game::isRunning() const
{
    return running;
}
