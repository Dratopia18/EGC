#version 330

// Input
// TODO(student): Get values from vertex shader
in vec3 frag_color;
in vec3 frag_normal;
in vec3 frag_position;
in vec2 frag_coord;

// Output
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_position;
layout(location = 3) out vec4 out_coord;


void main()
{
    // TODO(student): Write pixel out color
    out_color = vec4(abs(frag_normal), 1.0);

}
