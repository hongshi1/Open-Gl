#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 rimLightColor; // 泛光颜色
uniform float rimIntensity; // 泛光强度

uniform float transparency; // 透明度

uniform sampler2D texture_diffuse; // 纹理

void main()
{
    // Ambient
    vec3 ambient = vec3(0.96, 0.76, 0.28); // 金色的环境光颜色

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.96, 0.76, 0.28); // 金色的光源颜色

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * vec3(0.96, 0.76, 0.28); // 金色的光源颜色

    // Rim light (glow)
    vec3 rimLightDir = normalize(viewPos - FragPos);
    float rim = max(dot(norm, rimLightDir), 0.0);
    vec3 glow = rim * rimIntensity * vec3(0.96, 0.76, 0.28); // 金色的泛光颜色

    // Get texture color
    vec4 texColor = texture(texture_diffuse, TexCoords);

    // Glossiness
    float glossiness = 0.7; // 设置光泽度
    vec3 specularFinal = mix(specular, vec3(1.0), glossiness);

    // Water ripple effect
    float rippleScale = 0.05; // 设置水纹效果的大小
    vec2 texCoordOffset = vec2(
        sin(FragPos.y * rippleScale),
        cos(FragPos.x * rippleScale)
    );
    vec2 distortedTexCoords = TexCoords + texCoordOffset * 0.1; // 调整水纹效果的幅度和频率

    // Final color with transparency and water ripple effect
    vec3 result = (ambient + diffuse + specularFinal + glow) * texture(texture_diffuse, distortedTexCoords).rgb;
    FragColor = vec4(result, texColor.a * transparency);
}
