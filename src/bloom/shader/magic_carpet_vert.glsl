#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main()
{
    // 增加波动的复杂性和速度
    float wave = sin(time + aPos.x) * 0.3;  // 主波动
    float smallWave = sin(time * 2.0 + aPos.x * 2.0) * 0.1; // 快速小波动增加细节

    vec3 pos = aPos;
    pos.y += wave + smallWave; // 将波动应用到顶点的y坐标

    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
