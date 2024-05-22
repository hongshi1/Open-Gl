#version 330 core

uniform mat4 ModelMatrix;
uniform mat4 IT_ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
uniform float time;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord;

out vec2 v_texcoord;
out vec3 v_normal;
out vec3 worldPos;

void main()
{
    float waveHeight = 0.1;
    float waveSpeed = 2.0;
    float waveFrequency = 1.0;
    float wave = sin(a_position.x * waveFrequency + time * waveSpeed) * waveHeight + 
                 cos(a_position.z * waveFrequency + time * waveSpeed) * waveHeight;
                 
    vec3 modPosition = a_position + vec3(0.0, wave, 0.0);

    gl_Position = ModelMatrix * vec4(modPosition, 1.0);
    worldPos = vec3(ModelMatrix * vec4(modPosition, 1.0));
    gl_Position = ViewMatrix * gl_Position;
    gl_Position = ProjectMatrix * gl_Position;
    v_texcoord = a_texcoord;

    v_normal = mat3(IT_ModelMatrix) * vec3(0.0, 1.0, 0.0); // Normal pointing upwards
}
