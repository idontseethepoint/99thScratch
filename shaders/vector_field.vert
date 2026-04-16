#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 instanceVector;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 dxi;
uniform ivec3 nxi;

out vec3 FragPos;
out vec3 Normal;


void main()
{
    float L = length(instanceVector);
    if (L < 0.001)
    {
        FragPos = vec3(0,0,0);
        Normal = normal;
        gl_Position = vec4(0, 0, 0, 1);
        return;
    }
    else
    {
        float x = dxi.x * (gl_InstanceID % nxi.x);
        float y = dxi.y * (gl_InstanceID / nxi.x % nxi.y);
        float z = dxi.z * (gl_InstanceID / (nxi.x * nxi.y) % nxi.z);
        vec3 v = normalize(instanceVector);
        vec3 axis = normalize(cross(instanceVector, vec3(1, 0, 0)));
        float cosTheta = v.x;
        float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
        vec3 w = L * (pos * cosTheta + cross(axis, pos) * sinTheta + axis * dot(axis, pos) * (1.0 - cosTheta));
        vec3 offset = w + vec3(x, y, z);

        FragPos = offset;
        Normal = normal;
        gl_Position = projection * view * model * vec4(offset, 1.0);
    }
}