#version 330 core
 
uniform sampler2D T_Water_N;
uniform sampler2D T_Perlin_Noise_M;
uniform samplerCube cubeMap;
uniform vec3 cameraPos;
uniform vec3 LightLocation;
uniform float totalTime;
 
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_tangent;
varying vec3 worldPos;
 
vec2 CalTexcoord(vec2 uv, vec2 scale, vec2 panner)
{
    return uv * scale + panner;
}
 
vec3 UnpackNormal(vec3 normal)
{
    vec3 N = normalize(v_normal);
    vec3 T = normalize(v_tangent - N * v_tangent * N);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T,B,N);
    return normalize(TBN * normal);
}
 
vec3 BlendAngleCorrectedNormals(vec3 baseNormal, vec3 additionalNormal)
{
    baseNormal.b += 1;
    additionalNormal *= vec3(-1, -1, 1);
    vec3 normal =  dot(baseNormal, additionalNormal) * baseNormal - baseNormal.b * additionalNormal;
    return normalize(normal);
}
 
vec3 GetBaseNormal()
{
    vec2 texcoord = CalTexcoord(v_texcoord,vec2(0.05, 0.08),totalTime * vec2(-0.03, -0.02));
    vec3 normal = vec3(texture2D(T_Water_N, texcoord));
    normal = normalize(2 * normal - 1);
    return normal;
}
 
vec3 GetAdditionNormal()
{
    float time = sin(worldPos.x / 150 + 0.4 * totalTime);
    vec2 texcoord = CalTexcoord(v_texcoord,vec2(0.18, 0.15),time * vec2(-0.06,-0.04));
    vec3 normal = vec3(texture2D(T_Water_N, texcoord));
    normal = normalize(2 * normal - 1);
    return normal;
}
 
 
float GetNoiseAlpha()
{
    vec2 texcoord = 0.05 * v_texcoord;
    vec4 noiseTex = texture2D(T_Perlin_Noise_M, texcoord);
    float alpha = noiseTex.r;
    return alpha * 0.3;
}
 
vec3 GetSmallWaveNormal(float noise)
{
    vec2 texcoord = CalTexcoord(v_texcoord,vec2(0.75, 0.75),totalTime * vec2(-0.07, -0.07));
    vec3 normal = vec3(texture2D(T_Water_N, texcoord));
    normal = (1 - noise) * normal + noise * vec3(0, 0, 1);
    normal = normalize(2 * normal - 1);
    return normal;
}
 
void main()
{
    vec3 ViewDir = normalize(cameraPos - worldPos);
    vec3 lightDir = normalize(LightLocation - worldPos);
 
    float noise = GetNoiseAlpha();
    vec3 baseNormal = GetBaseNormal();
    vec3 addNormal = GetAdditionNormal();
    vec3 largeWaveNormal = BlendAngleCorrectedNormals(baseNormal, addNormal);
    vec3 smallWaveNormal = GetSmallWaveNormal(noise);
    vec3 normal = BlendAngleCorrectedNormals(largeWaveNormal, smallWaveNormal);
    normal = UnpackNormal(normal);
 
    float diffuse = 0.7 * clamp(dot(normal, lightDir), 0, 1);
    float ambient = 0.1;
    vec3 reflectDir = normalize(reflect(-lightDir,normal));
    float specular = pow(clamp(dot(reflectDir,ViewDir),0,1),2.0);
 
    vec3 R = reflect(-ViewDir,normal);
    vec4 reflectedColor = textureCube(cubeMap, R);
 
    vec3 T = refract(-ViewDir, normal, 0.9);
    vec4 refractedColor = textureCube(cubeMap, T);
 
    float fresnel = 0.4 + 0.6 * pow(1.0 - dot(ViewDir, normal), 6.0);
 
    gl_FragColor = (mix(refractedColor, reflectedColor, fresnel) );
    gl_FragColor = gl_FragColor *( specular + diffuse + ambient);
}