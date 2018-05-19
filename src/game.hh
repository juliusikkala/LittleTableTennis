#ifndef GAME_HH
#define GAME_HH
#include "littleton/resource_pool.hh"
#include "littleton/window.hh"
#include "board.hh"
#include "pipeline.hh"

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

    pipeline pl;
    board board1;
};
#endif
