#ifndef GAME_HH
#define GAME_HH
#include "resource_pool.hh"
#include "window.hh"
#include "board.hh"
#include "basic_pipeline.hh"

class game
{
public:
    game();

    // Updates the game state. Returns false when the game should close.
    bool update();

    // Called for every frame, responsible for actual rendering.
    void render();

private:
    lt::window win;
    lt::resource_pool pool;

    board board1;
    basic_pipeline pipeline;
};
#endif
