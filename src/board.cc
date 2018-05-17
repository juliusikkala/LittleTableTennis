#include "board.hh"
#include "loaders.hh"
#include <string>

board::board(
    lt::window& win,
    lt::resource_pool& pool,
    basic_pipeline& pipeline,
    const std::string& data_path,
    const std::string& board_path,
    const std::string& counter_path
):  sun_shadow(
        &pipeline.get_msm(),
        win,
        lt::uvec2(1024, 1024),
        4,
        2,
        lt::vec3(0),
        lt::vec2(25.0f, 25.0f),
        lt::vec2(-12.0f, 12.0f),
        &sun
    ),
    environment(win, "data/skybox/sky.hdr", false, 2.0f)
{
    /* load_gltf returns all the scene graphs found in the file. Merge them into
     * one.
     */
    auto scenes = lt::load_gltf(pool, board_path, data_path);
    for(const auto& pair: scenes) graph.merge(pair.second);
    graph.add_to_scene(&scene);

    // Add counter objects too
    scenes = lt::load_gltf(pool, counter_path, data_path);
    for(const auto& pair: scenes) graph.merge(pair.second);

    /* Get the objects from the graph. The names of the objects are defined in
     * the .glb file. You can take look at the included .blend to see them too.
     */
    game_board = graph.get_object("Board");
    ball = graph.get_object("Ball");


    // The loader should also add these new materials to the pool.
    std::vector<const lt::material*> counter_materials = {
        pool.get_material("BlueGlow"),
        pool.get_material("RedGlow")
    };

    // Handle player-specific objects
    players.resize(2);
    for(unsigned i = 0; i < players.size(); ++i)
    {
        player& p = players[i];

        /* Generate number objects for each player as an example in object &
         * model handling
         */
        for(unsigned j = 0; j < 10; ++j)
        {
            lt::object* base_number = graph.get_object(
                "Counter_" + std::to_string(j)
            );
            const lt::model* base_model = base_number->get_model();

            lt::model& result_model = p.numbers[j].mod;
            lt::object& result_number = p.numbers[j].obj;

            // Copy original model and set materials
            result_model = *base_model;
            for(auto& vgroup: result_model)
                vgroup.mat = counter_materials[i];

            // Copy original object and set model 
            result_number = *base_number;
            result_number.set_model(&result_model);
        }

        p.paddle = graph.get_object("Paddle" + std::to_string(i+1));
        p.paddle_dir = 0;
        p.score = 0;
    }

    // Setup lighting
    sun.set_color(lt::vec3(3.0, 2.0, 1.0));
    sun.set_direction(lt::normalize(lt::vec3(
        0.8059,
        -0.08716,
        0.5855
    )));
    sun_shadow.set_offset(lt::vec3(-1,0,0));
    scene.add_light(&sun);
    scene.add_shadow(&sun_shadow);
    scene.set_skybox(&environment);
    pipeline.set_texture(sun_shadow.get_moments());
    scene.set_ambient(lt::vec3(0.06f,0.08f,0.1f));

    // Setup camera
    cam.set_parent(game_board);
    cam.perspective(60, win.get_aspect(), 0.1, 40);
    cam.set_position(lt::vec3(0,8,-8));
    cam.lookat(lt::vec3(0), lt::vec3(0,1,0));
    cam.translate(lt::vec3(0,0,-1));

    scene.set_camera(&cam);

    // Position board
    game_board->rotate(10.0f, lt::vec3(0,1,0));
    game_board->rotate_local(-10.0f, lt::vec3(1,0,0));
}

lt::render_scene* board::get_scene()
{
    return &scene;
}

void board::set_paddle_dir(unsigned player_index, int dir)
{
    if(player_index < players.size())
        players[player_index].paddle_dir = dir;
}

void board::update(float dt)
{
    for(auto& player: players)
    {
        glm::vec3 pos = player.paddle->get_position();
        pos += lt::vec3(0,0,5*dt*player.paddle_dir);
        pos.z = lt::clamp(pos.z, -4.0f, 4.0f);
        player.paddle->set_position(pos);
    }
}

bool board::declare_winner(unsigned& winner)
{
    constexpr unsigned WINNING_SCORE = 10;
    for(unsigned i = 0; i < players.size(); ++i)
    {
        if(players[i].score > WINNING_SCORE)
        {
            winner = i;
            return true;
        }
    }

    return false;
}
