#version 430 core

in vec2 tex_coord;
layout(location = 0) out vec4 final_color;
uniform sampler2D sample_texture;

void main()
{
    //final_color = vec4(tex_coord.x,tex_coord.y,0,1);
    final_color = texture(sample_texture, tex_coord);
}
