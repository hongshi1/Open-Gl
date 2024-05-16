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
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Additional rim light (glow)
    vec3 rimLightDir = normalize(viewPos - FragPos);
    float rim = max(dot(norm, rimLightDir), 0.0);
    vec3 glow = rim * rimIntensity * rimLightColor; // 使用泛光强度参数

    // Get texture color
    vec4 texColor = texture(texture_diffuse, TexCoords);

    // Final color with transparency
    vec3 result = (ambient + diffuse + specular + glow) * texColor.rgb;
    FragColor = vec4(result, texColor.a * transparency);
}
