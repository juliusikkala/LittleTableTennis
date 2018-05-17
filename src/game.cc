#include "game.hh"
#include <iostream>

game::game()
:   win({"LittleTableTennis", {1280, 720}, true, true, false}),
    pool(
        win,
        {
            "data/shaders/",
            "subprojects/Littleton/data/shaders/"
        } //, {"data/shaders/bin"}
    ),
    pipeline(win, pool, win.get_size()),
    board1(
        win,
        pool,
        pipeline,
        "data",
        "data/models/board1.glb",
        "data/models/counter.glb"
    )
{
    pipeline.set_scene(board1.get_scene());
    win.set_framerate_limit(200);
    win.grab_mouse();
    std::cout << "GPU Vendor: " << win.get_vendor_name() << std::endl
        << "Renderer:   " << win.get_renderer() << std::endl;

    // Load needed resources and reset frametime
    render();
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

    const uint8_t* state = SDL_GetKeyboardState(NULL);
    int dirs[2] = {0};
    if(state[SDL_SCANCODE_W]) dirs[0]++;
    if(state[SDL_SCANCODE_S]) dirs[0]--;
    if(state[SDL_SCANCODE_I]) dirs[1]++;
    if(state[SDL_SCANCODE_K]) dirs[1]--;

    board1.set_paddle_dir(0, dirs[0]);
    board1.set_paddle_dir(1, dirs[1]);

    board1.update(win.get_delta());
    return true;
}

void game::render()
{
    pipeline.get_pipeline().execute();
    win.present();
}
