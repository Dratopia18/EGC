#version 330

in float height;
in vec3 frag_normal;
in vec3 frag_position;

layout(location = 0) out vec4 out_color;

void main()
{
    vec3 grassColor = vec3(0.2, 0.5, 0.1);
    vec3 rockColor = vec3(0.45, 0.25, 0.1);
    
    vec3 baseColor;
    baseColor = mix(grassColor, rockColor, (height - 0.4) * 1.67);
    
    vec3 light_dir = normalize(vec3(1, 1, 1));
    vec3 normal = normalize(frag_normal);
    
    float ambient = 0.3;
    
    float diffuse = max(dot(normal, light_dir), 0.0);
    
    float light = ambient + diffuse * 0.7;
    
    vec3 final_color = baseColor * light;
    
    out_color = vec4(final_color, 1.0);
}
