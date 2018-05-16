#ifndef BASIC_PIPELINE_HH
#define BASIC_PIPELINE_HH

#include "pipeline.hh"
#include "window.hh"
#include "texture.hh"
#include "gbuffer.hh"
#include "doublebuffer.hh"
#include "method/clear_gbuffer.hh"
#include "method/skybox.hh"
#include "method/shadow_msm.hh"
#include "method/geometry_pass.hh"
#include "method/lighting_pass.hh"
#include "method/bloom.hh"
#include "method/tonemap.hh"
#include "method/sao.hh"
#include "method/ssrt.hh"
#include "method/visualize_gbuffer.hh"
#include "method/blit_framebuffer.hh"

/* The pipeline creation is quite convoluted for the moment. It would be nice to
 * have some built-in solutions in Littleton.
 */
class basic_pipeline
{
public:
    basic_pipeline(
        lt::window& win,
        lt::resource_pool& pool,
        lt::uvec2 size,
        lt::render_scene* scene = nullptr
    );

    void set_scene(lt::render_scene* scene);

    lt::pipeline& get_pipeline();
    lt::pipeline& get_visualizer_pipeline();
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

};

#endif
