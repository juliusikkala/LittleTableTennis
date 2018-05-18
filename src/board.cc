#include "board.hh"
#include "loaders.hh"
#include <string>

namespace
{

lt::vec3 random_start_direction()
{
    lt::vec2 candidate = lt::vec2(0);

    // Yes I know this algorithm is dumb...
    while(
        fabs(candidate.x) < 0.1f ||
        fabs(candidate.y) < 0.1f
    ) candidate = lt::circularRand(1.0f);

    return lt::normalize(lt::vec3(candidate.x, 0, candidate.y));
}

// Figures out whether an axis-aligned rectangle and a circle intersect. If they
// do, also determines the collision normal.
bool circle_rect_intersection(
    lt::vec2 rect_origin,
    lt::vec2 rect_radius,
    lt::vec2 circle_origin,
    float circle_radius,
    lt::vec2& normal
){
    lt::vec2 offset = circle_origin - rect_origin;
    lt::vec2 dist = lt::abs(offset);

    if(
        dist.x > rect_radius.x + circle_radius ||
        dist.y > rect_radius.y + circle_radius
    ) return false;

    if(dist.x <= rect_radius.x)
    {
        normal = lt::vec2(0, lt::sign(offset.y));
        return true;
    }

    if(dist.y <= rect_radius.y)
    {
        normal = lt::vec2(lt::sign(offset.x), 0);
        return true;
    }

    dist -= rect_radius;

    float corner_dist = lt::dot(dist, dist);
    if(corner_dist <= circle_radius * circle_radius)
    {
        normal = lt::normalize(dist * offset);
        return true;
    }
    return false;
}

// Sets the positions of the score numbers and sets their models.
void init_score_numbers(
    lt::object& board,
    lt::model& zero,
    std::vector<lt::object>& score_numbers,
    lt::vec3 origin,
    lt::vec3 step,
    float scaling
){
    lt::vec3 offset = step * lt::vec3((score_numbers.size()-1)/-2.0f);

    for(lt::object& number: score_numbers)
    {
        number.set_parent(&board);
        number.set_model(&zero);
        number.set_position(origin + offset);
        number.set_scaling(lt::vec3(scaling));
        number.rotate(90, lt::vec3(1,0,0));
        number.rotate(180, lt::vec3(0,1,0));
        offset += step;
    }
}

void update_score_numbers(
    unsigned score,
    lt::model* numbers,
    std::vector<lt::object>& score_numbers
){
    for(unsigned i = 0; i < score_numbers.size(); ++i)
    {
        score_numbers[i].set_model(numbers + (score % 10));
        score /= 10;
    }
}

}

board::board(
    lt::window& win,
    lt::resource_pool& pool,
    basic_pipeline& pipeline,
    const std::string& data_path,
    const std::string& board_path,
    const std::string& counter_path
):  ball_velocity(7.0f),
    ball_dir(random_start_direction()),
    sun_shadow(
        &pipeline.get_msm(),
        win,
        lt::uvec2(1024, 1024),
        4,
        4,
        lt::vec3(0,0.7f,0),
        lt::vec2(23.f, 5.0f),
        lt::vec2(-11.5f, 11.0f),
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
    std::vector<lt::material*> counter_materials = {
        pool.get_material_mutable("BlueGlow"),
        pool.get_material_mutable("RedGlow")
    };

    // glTF2.0 limits emissiveFactor to 0.0-1.0, but we want some brighter
    // lights.
    for(lt::material* mat: counter_materials) mat->emission_factor *= 5.0f;

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

            lt::model& result_model = p.numbers[j];

            // Copy original model and set materials
            result_model = *base_model;
            for(auto& vgroup: result_model)
                vgroup.mat = counter_materials[i];
        }

        // Add counter numbers
        p.score_numbers.resize(2);
        for(lt::object& number: p.score_numbers) scene.add_object(&number);
        init_score_numbers(
            *game_board,
            p.numbers[0],
            p.score_numbers,
            lt::vec3(6.f - i*12.f, 2.2, 5.5),
            lt::vec3(2, 0, 0),
            0.4f
        );

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
    constexpr float top_edge = 5.0f;
    constexpr float bottom_edge = -5.0f;
    constexpr float left_edge = 10.0f;
    constexpr float right_edge = -10.0f;
    constexpr lt::vec2 paddle_radius = lt::vec2(0.3f, 1.0f);
    constexpr float ball_radius = 0.3f;

    // Update paddles
    for(auto& player: players)
    {
        lt::vec3 pos = player.paddle->get_position();
        pos += lt::vec3(0,0,10*dt*player.paddle_dir);
        pos.z = lt::clamp(
            pos.z,
            bottom_edge + paddle_radius.y,
            top_edge - paddle_radius.y
        );
        player.paddle->set_position(pos);
    }

    // Update ball
    lt::vec3 ball_pos = ball->get_position();
    ball_pos += dt * ball_velocity * ball_dir;

    // Adjust ball direction so that the game doesn't become too slow
    if(fabs(ball_dir.x) < 0.4f) ball_dir.x *= 1.01f;

    // Check for collisions with board edges
    if(
        (ball_pos.z - ball_radius < bottom_edge && ball_dir.z < 0) ||
        (ball_pos.z + ball_radius > top_edge && ball_dir.z > 0)
    ){
        ball_dir.z = -ball_dir.z;
    }

    // Check for collisions with paddles
    for(auto& player: players)
    {
        lt::vec3 pos = player.paddle->get_position();
        lt::vec2 incident = lt::vec2(ball_dir.x, ball_dir.z);
        lt::vec2 collision_normal(0);
        if(
            circle_rect_intersection(
                lt::vec2(pos.x, pos.z),
                paddle_radius,
                lt::vec2(ball_pos.x, ball_pos.z),
                ball_radius,
                collision_normal
            ) &&
            lt::dot(
                collision_normal,
                lt::vec2(ball_dir.x, ball_dir.z)
            ) < 0
        ){
            lt::vec2 reflected_dir = lt::reflect(incident, collision_normal);
            ball_dir.x = reflected_dir.x;
            ball_dir.z = reflected_dir.y;
            ball_velocity += 0.1f;
            break;
        }
    }

    // Check for a goal
    bool reset_ball = false;
    if(ball_pos.x > left_edge)
    {
        reset_ball = true;
        update_score_numbers(
            ++players[1].score,
            players[1].numbers,
            players[1].score_numbers
        );
    }

    if(ball_pos.x < right_edge)
    {
        reset_ball = true;
        update_score_numbers(
            ++players[0].score,
            players[0].numbers,
            players[0].score_numbers
        );
    }

    if(reset_ball)
    {
        ball_pos.x = 0;
        ball_pos.z = 0;
        ball_velocity = 7.0f;
        ball_dir = random_start_direction();
    }

    // Move ball and rotate it along the rolling axis
    ball->set_position(ball_pos);
    ball->rotate(
        dt * 90.0f * ball_velocity,
        lt::cross(lt::vec3(0,1,0), ball_dir)
    );
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
