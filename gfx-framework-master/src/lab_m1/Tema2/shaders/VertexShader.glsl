#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out float height;
out vec3 frag_normal;
out vec3 frag_position;


float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

vec3 calculateNormal(vec3 pos, float noise_value) {
    float eps = 0.01;
    float noise_x = noise(vec2(pos.x + eps, pos.z)) - noise(vec2(pos.x - eps, pos.z));
    float noise_z = noise(vec2(pos.x, pos.z + eps)) - noise(vec2(pos.x, pos.z - eps));
    return normalize(vec3(-noise_x, 2.0 * eps, -noise_z));
}

void main()
{
    float frequency = 0.2;  
    float amplitude = 2.0;  
    
    vec2 noisePos = v_position.xz * frequency;
    float noise_value = noise(noisePos);
    
    vec3 pos = v_position;
    pos.y += noise_value * amplitude;
    
    vec3 new_normal = calculateNormal(v_position, noise_value);
    
    height = noise_value;
    frag_position = (Model * vec4(pos, 1.0)).xyz;
    frag_normal = normalize(mat3(Model) * new_normal);
    
    gl_Position = Projection * View * Model * vec4(pos, 1.0);
}
