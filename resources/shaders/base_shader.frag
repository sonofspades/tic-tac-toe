#version 460

layout (location = 0) out vec4 out_color;

layout (binding = 2, std140) uniform u_material
{
    vec3 albedo;
};

void main()
{
    out_color = vec4(albedo, 1.0);
}