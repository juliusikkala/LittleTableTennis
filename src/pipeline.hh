#ifndef PIPELINE_HH
#define PIPELINE_HH

#include "littleton/pipeline.hh"
#include "littleton/window.hh"
#include "littleton/texture.hh"
#include "littleton/gbuffer.hh"
#include "littleton/doublebuffer.hh"
#include "littleton/method/clear_gbuffer.hh"
#include "littleton/method/skybox.hh"
#include "littleton/method/shadow_msm.hh"
#include "littleton/method/geometry_pass.hh"
#include "littleton/method/lighting_pass.hh"
#include "littleton/method/bloom.hh"
#include "littleton/method/tonemap.hh"
#include "littleton/method/sao.hh"
#include "littleton/method/ssrt.hh"
#include "littleton/method/visualize_gbuffer.hh"
#include "littleton/method/blit_framebuffer.hh"
#include "littleton/method/draw_texture.hh"

/* The pipeline creation is quite convoluted for the moment. It would be nice to
 * have some built-in solutions in Littleton.
 */
class pipeline
{
public:
    pipeline(
        lt::window& win,
        lt::resource_pool& pool,
        lt::uvec2 size,
        lt::render_scene* scene = nullptr
    );

    void set_scene(lt::render_scene* scene);

    lt::pipeline& get_pipeline();
    lt::pipeline& get_visualizer_pipeline();
    lt::pipeline& get_texture_pipeline();

    void set_texture(lt::texture& tex);

    lt::method::shadow_msm& get_msm();
private:
    // Render targets
    lt::texture normal;
    lt::texture color;
    lt::texture material;
    lt::texture lighting;
    lt::texture linear_depth;
    lt::texture depth_stencil;

    lt::gbuffer buf;
    lt::doublebuffer postprocess;

    // Clearing methods
    lt::method::clear_gbuffer clear_buf;
    lt::method::skybox skybox;

    // Shadow mapping methods
    lt::method::shadow_msm msm;

    // Deferred shading methods
    lt::method::geometry_pass gp;
    lt::method::lighting_pass lp;

    // Post-processing methods
    lt::method::sao sao;
    lt::method::ssrt ssrt;
    lt::method::bloom bloom;
    lt::method::tonemap tm;

    // Final blitting method
    lt::method::blit_framebuffer postprocess_to_window;

    lt::pipeline deferred_pipeline;

    // G-Buffer visualizer pipeline
    lt::method::visualize_gbuffer vg;
    lt::method::blit_framebuffer buf_to_window;
    lt::pipeline visualizer_pipeline;

    // Texture pipeline
    lt::method::draw_texture dt;
    lt::pipeline texture_pipeline;
};

#endif
