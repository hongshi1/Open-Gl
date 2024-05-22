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
    // 更复杂的波动模式
    float wave = sin(time * 0.5 + aPos.x * 3.14) * 0.75; // 长波
    float smallWave = sin(time * 3.0 + aPos.x * 6.28) * 0.5; // 短快波

    vec3 pos = aPos;
    pos.y += (pos.x > 0 ? wave : -wave) + smallWave; // 根据x坐标的正负决定波动方向

    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
