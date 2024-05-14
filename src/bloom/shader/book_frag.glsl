#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in float FaceID;

uniform sampler2D texture1; // 前面
uniform sampler2D normalMap1;
uniform sampler2D texture2; // 上面和下面，右面
uniform sampler2D normalMap2;
uniform sampler2D texture3; // 左边
uniform sampler2D normalMap3;
uniform sampler2D texture4; // 背面


void main()
{
    vec3 color;
    vec3 normal;

    if (FaceID == 1.0f) // 前面
    {
        color = texture(texture1, TexCoords).rgb;
        normal = texture(normalMap1, TexCoords).rgb;
    }
    else if (FaceID == 2.0f) // 左边
    {
        color = texture(texture3, TexCoords).rgb;
        normal = texture(normalMap3, TexCoords).rgb;
    }
    else if (FaceID == 3.0f || FaceID == 4.0f || FaceID == 5.0f) // 上面和下面、右面
    {
        color = texture(texture2, TexCoords).rgb;
        normal = texture(normalMap2, TexCoords).rgb;
    }
    else 
    {
        color = texture(texture4, TexCoords).rgb;
        normal = vec3(0.5, 0.5, 1.0);
    }

    FragColor = vec4(color, 1.0);
}
