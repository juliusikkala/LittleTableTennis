#include "game.hh"
#include <iostream>

game::game()
:   win({"LittlePong", {1280, 720}, true, true, false}),
    pool(
        win,
        {
            "data/shaders/",
            "subprojects/Littleton/data/shaders/"
        },
        {"data/shaders/bin"}
    )
{
    win.set_framerate_limit(200);
    win.grab_mouse();
    std::cout << "GPU Vendor: " << win.get_vendor_name() << std::endl
        << "Renderer:   " << win.get_renderer() << std::endl;
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
    return true;
}

void game::render()
{
    win.present();
}
