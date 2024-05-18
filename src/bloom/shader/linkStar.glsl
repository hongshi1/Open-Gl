#version 330 core

out vec4 FragColor;

// 物体的固定颜色
uniform vec3 objectColor;  

// 发光效果的参数
uniform vec3 glowColor;      // 发光的颜色
uniform float glowIntensity; // 发光的强度

void main() {
    // 计算发光效果
    vec3 glow = glowIntensity * glowColor; // 根据发光的颜色和强度计算发光效果

    // 将物体的颜色用于发光效果
    vec3 result = objectColor + glow;

    FragColor = vec4(result, 1.0);
}
