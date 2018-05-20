/*
The MIT License (MIT)

Copyright (c) 2018 Julius Ikkala

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "game.hh"
#include <iostream>
#include <cstdint>

game::game()
:   win({"LittleTableTennis", {1280, 720}, true, true, false}),
    pool(
        win,
        {
            "data/shaders/",
            "subprojects/Littleton/data/shaders/"
        } //, {"data/shaders/bin"}
    ),
    pl(win, pool, win.get_size()),
    board1(
        win,
        pool,
        pl,
        "data",
        "data/models/board1.glb",
        "data/models/counter.glb"
    )
{
    pl.set_scene(board1.get_scene());
    win.set_framerate_limit(200);
    win.grab_mouse();
    std::cout << "GPU Vendor: " << win.get_vendor_name() << std::endl
        << "Renderer:   " << win.get_renderer() << std::endl;

    // Load needed resources and reset frametime
    render();

    // Find connected game controllers
    for(int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        if(SDL_IsGameController(i))
        {
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            if(controller) controllers.push_back(controller);
        }
    }
}

game::~game()
{
    for(SDL_GameController* controller: controllers)
        SDL_GameControllerClose(controller);
}

bool game::update()
{
    SDL_Event e;

    while(win.poll(e))
    {
        switch(e.type)
        {
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:
            if(e.key.keysym.sym == SDLK_ESCAPE) return false;
            break;
        default:
            break;
        };
    }

    float dt = win.get_delta();

    // Controller controls
    if(controllers.size() >= 1)
    {
        int axes[2] = {0};
        // Only a single controller, so use it for both paddles
        if(controllers.size() == 1)
        {
            axes[0] = SDL_GameControllerGetAxis(
                controllers[0],
                SDL_CONTROLLER_AXIS_LEFTY
            );
            axes[1] = SDL_GameControllerGetAxis(
                controllers[0],
                SDL_CONTROLLER_AXIS_RIGHTY
            );
        }
        // Two or more controllers, use the first two.
        else
        {
            axes[0] = SDL_GameControllerGetAxis(
                controllers[0],
                SDL_CONTROLLER_AXIS_LEFTY
            );
            axes[1] = SDL_GameControllerGetAxis(
                controllers[1],
                SDL_CONTROLLER_AXIS_LEFTY
            );
        }
        board1.set_paddle_pos(0, -(float)axes[0] / INT16_MAX);
        board1.set_paddle_pos(1, -(float)axes[1] / INT16_MAX);
    }
    else
    {
        // Keyboard controls
        const uint8_t* state = SDL_GetKeyboardState(NULL);
        int dirs[2] = {0};
        if(state[SDL_SCANCODE_W]) dirs[0]++;
        if(state[SDL_SCANCODE_S]) dirs[0]--;
        if(state[SDL_SCANCODE_I]) dirs[1]++;
        if(state[SDL_SCANCODE_K]) dirs[1]--;
        board1.set_paddle_pos(
            0,
            board1.get_paddle_pos(0) + 2.0f * dt * dirs[0]
        );
        board1.set_paddle_pos(
            1,
            board1.get_paddle_pos(1) + 2.0f * dt * dirs[1]
        );
    }

    board1.update(dt);
    return true;
}

void game::render()
{
    pl.get_pipeline().execute();
    win.present();
}
