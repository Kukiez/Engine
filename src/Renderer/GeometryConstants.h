#pragma once


struct GeometryConstants {
    static constexpr auto FULL_SCREEN_QUAD = "FullScreenQuad";

    static constexpr auto FULL_SCREEN_DRAW_COMMAND = DrawArrays{
        .function = GLDrawFunction::ARRAYS,
        .primitive = GLDrawPrimitive::TRIANGLES,
        .first_vertex = 0,
        .vertex_count = 6
    };

    static constexpr auto CUBEMAP = "CubeMap";
};