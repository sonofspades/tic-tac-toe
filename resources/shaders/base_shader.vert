#version 460

layout (location = 0) in vec3 in_position;

layout (binding = 0, std140) uniform u_transform
{
    mat4 model;
};

layout (binding = 1, std140) uniform u_camera
{
    mat4 view;
    mat4 proj;
};

void main()
{
    gl_Position = proj * view * model * vec4(in_position, 1.0);
}