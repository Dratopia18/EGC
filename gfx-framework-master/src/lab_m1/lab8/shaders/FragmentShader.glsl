#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

struct Light {
	vec3 position;
	vec3 direction;
	vec3 color;
	float cutoff_angle;
	float attenuation;
    int type;
};

// TODO(student): Declare any other uniforms
uniform float cutoff_angle = 75.0;
uniform vec3 object_color;
uniform bool is_spotlight;

// Output
layout(location = 0) out vec4 out_color;


vec3 light_contribution(Light light)
{
	vec3 color;
    vec3 L = normalize(light.position - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);
    vec3 R = normalize(reflect(L, normalize(world_normal)));
    vec3 ambient_light = vec3(0.25) * object_color;
    vec3 diffuse_light = material_kd * max(0, dot(normalize(world_normal), L)) * light.color;
    vec3 specular_light = material_ks * pow(max(dot(normalize(world_normal), H), 0.0), material_shininess) * light.color;
    
    float d = distance(light.position, world_position);
    float attenuation = 1 / (1 + 0.14 * d + 0.07 * d * d);
    
    if (is_spotlight) {
        float cut_off = radians(light.cutoff_angle);
        float spot_light = dot(-L, light.direction);
        float spot_light_limit = cos(cut_off);

        float light_att_factor = 0.0f;
        if (spot_light > spot_light_limit) {
            float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
            light_att_factor = pow(linear_att, 2);
        }

        vec3 light_contrib = ambient_light + attenuation * light_att_factor * (diffuse_light + specular_light);
        return light_contrib;
    } else {
        vec3 light_contrib = ambient_light + attenuation * (diffuse_light + specular_light);
        return light_contrib;
    }
}

void main()
{
    Light light;
    light.position = light_position;
    light.direction = light_direction;
    light.color = vec3(1.0);
    light.cutoff_angle = cutoff_angle;
    light.attenuation = 1.0;
    light.type = is_spotlight ? 1 : 0;

    vec3 Light = light_contribution(light);
    // Write pixel out color
    out_color = vec4(Light, 1);
}
