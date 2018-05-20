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
#include "pipeline.hh"

pipeline::pipeline(
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
    bloom(postprocess.input(0), pool, &lighting, 2.0f, 15, 0.5f, 1),
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
    }),

    dt(win, pool),
    texture_pipeline({&msm, &dt})
{
    ssrt.set_ray_offset(0.1f);
    ssrt.use_fallback_cubemap(true);
}

void pipeline::set_scene(lt::render_scene* scene)
{
    skybox.set_scene(scene);
    msm.set_scene(scene);
    gp.set_scene(scene);
    lp.set_scene(scene);
    sao.set_scene(scene);
    ssrt.set_scene(scene);
    vg.set_scene(scene);
}

lt::pipeline& pipeline::get_pipeline()
{
    return deferred_pipeline;
}

lt::pipeline& pipeline::get_visualizer_pipeline()
{
    return visualizer_pipeline;
}

lt::method::shadow_msm& pipeline::get_msm()
{
    return msm;
}

lt::pipeline& pipeline::get_texture_pipeline()
{
    return texture_pipeline;
}

void pipeline::set_texture(lt::texture& tex)
{
    dt.set_texture(&tex);
}
