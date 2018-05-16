#include "basic_pipeline.hh"

basic_pipeline::basic_pipeline(
    lt::window& win,
    lt::resource_pool& pool,
    lt::uvec2 size,
    lt::render_scene* scene
):  normal(win, size, GL_RG16_SNORM, GL_UNSIGNED_BYTE),
    color(win, size, GL_RGB8, GL_UNSIGNED_BYTE),
    material(win, size, GL_RGBA8, GL_UNSIGNED_BYTE),
    lighting(win, size, GL_RGB16F, GL_FLOAT),
    linear_depth(win, size, GL_RG16F, GL_FLOAT),
    depth_stencil(win, size, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8),

    buf(
        win,
        size,
        &normal,
        &color,
        &material,
        &lighting,
        &linear_depth,
        &depth_stencil
    ),
    postprocess(win, size, GL_RGB16F),

    clear_buf(buf),
    skybox(buf, pool, scene),

    msm(pool, scene),

    gp(buf, pool, scene, false),
    lp(buf, buf, pool, scene),

    sao(buf, buf, pool, scene, 1.0f, 8, 0.01, 0.5f),
    ssrt(buf, buf, pool, scene),
    bloom(postprocess.input(0), pool, &lighting, 6.0f, 30, 0.1f),
    tm(postprocess.input(1), pool, &postprocess.output(1)),

    postprocess_to_window(
        win,
        postprocess.input(1),
        lt::method::blit_framebuffer::COLOR_ONLY
    ),

    deferred_pipeline({
        &msm,
        &clear_buf,
        &gp,
        &skybox,
        &lp,
        &sao,
        &ssrt,
        &bloom,
        &tm,
        &postprocess_to_window
    }),

    vg(buf, buf, pool, scene),
    buf_to_window(
        win,
        buf,
        lt::method::blit_framebuffer::COLOR_ONLY
    ),
    visualizer_pipeline({
        &clear_buf,
        &gp,
        &skybox,
        &vg,
        &buf_to_window
    })
{
    ssrt.set_thickness(0.5f);
    ssrt.set_ray_offset(0.1f);
    ssrt.use_fallback_cubemap(false);
}

void basic_pipeline::set_scene(lt::render_scene* scene)
{
    skybox.set_scene(scene);
    msm.set_scene(scene);
    gp.set_scene(scene);
    lp.set_scene(scene);
    sao.set_scene(scene);
    ssrt.set_scene(scene);
    vg.set_scene(scene);
}

lt::pipeline& basic_pipeline::get_pipeline()
{
    return deferred_pipeline;
}

lt::pipeline& basic_pipeline::get_visualizer_pipeline()
{
    return visualizer_pipeline;
}
