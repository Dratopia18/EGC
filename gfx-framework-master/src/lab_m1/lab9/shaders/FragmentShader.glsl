#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;
// TODO(student): Declare various other uniforms
uniform bool use_two_colors;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.
    vec4 color_1 = texture2D(texture_1, texcoord);
    if (use_two_colors) {
        vec4 color_2 = texture2D(texture_2, texcoord);
        out_color = mix(color_1, color_2, 0.5f);
    } else {
        out_color = color_1;
    }
    if (out_color.a < 0.5) {
        discard;
    }
}
