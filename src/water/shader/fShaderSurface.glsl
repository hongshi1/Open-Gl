#version 430

in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec2 tc;
in vec4 glp;
out vec4 color;

layout (binding=0) uniform sampler2D reflectTex;
layout (binding=1) uniform sampler2D refractTex;
layout (binding=2) uniform sampler3D noiseTex;

struct PositionalLight
{	vec4 ambient;  
	vec4 diffuse;
	vec4 specular;  
	vec3 position;
};

struct Material
{	vec4 ambient;  
	vec4 diffuse;  
	vec4 specular;  
	float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;	 
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
uniform int isAbove;
uniform float depthOffset;

//使用噪声纹理计算法线, 使用当前顶点的三个顶点计算
vec3 estimateWaveNormal(float offset, float mapScale, float hScale)
{
	float h1 = (texture(noiseTex, vec3(((tc.s)    )*mapScale, depthOffset, ((tc.t)+offset)*mapScale))).r * hScale;
	float h2 = (texture(noiseTex, vec3(((tc.s)-offset)*mapScale, depthOffset, ((tc.t)-offset)*mapScale))).r * hScale;
	float h3 = (texture(noiseTex, vec3(((tc.s)+offset)*mapScale, depthOffset, ((tc.t)-offset)*mapScale))).r * hScale;
	vec3 v1 = vec3(0, h1, -1);
	vec3 v2 = vec3(-1, h2, 1);
	vec3 v3 = vec3(1, h3, 1);
	vec3 v4 = v2-v1;
	vec3 v5 = v3-v1;
	vec3 normEst = normalize(cross(v4,v5));
	return normEst;
}

void main(void)
{	
	//雾化颜色
	vec4 fogColor = vec4(0.0, 0.0, 0.2, 1.0);
	float fogStart = 10.0;
	float fogEnd = 300.0;
	float dist = length(varyingVertPos.xyz);
	float fogFactor = clamp(((fogEnd-dist) / (fogEnd-fogStart)), 0.0, 1.0);
	//归一化光照方向、法线方向、视图方向
	vec3 L = normalize(varyingLightDir);
	vec3 V = normalize(-varyingVertPos);
	vec3 N = estimateWaveNormal(.0002, 32.0, 16.0);
	vec3 Nfres = normalize(varyingNormal); // for fresnel effect
	//光线和顶点法线之间的角度
	float cosTheta = dot(L,N);
	//计算光反射矢量:
	vec3 R = normalize(reflect(-L, N));
	//视图方向和反射方向的夹角
	float cosPhi = dot(V,R);
	
	float cosFres = dot(V,Nfres);

	vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
	vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta,0.0);
	vec3 specular = light.specular.xyz * material.specular.xyz * pow(max(cosPhi,0.0), material.shininess);
	
	vec4 mixColor, reflectColor, refractColor, blueColor;
	blueColor = vec4(0.0, 0.25, 1.0, 1.0);

	if (isAbove == 1)
	{	
		float fresnel = acos(cosFres);
		fresnel = pow(clamp(fresnel-0.3, 0.0, 1.0), 3.0);
		//将[-1, 1]转换到纹理采样的[0,1]坐标
		refractColor = texture(refractTex, (vec2(glp.x,glp.y))/(2.0*glp.w)+0.5);
		//绘制反射图，y轴需要反转
		reflectColor = texture(reflectTex, (vec2(glp.x,-glp.y))/(2.0*glp.w)+0.5);
		reflectColor = vec4((reflectColor.xyz * (ambient + diffuse) + 0.75*specular), 1.0);
		color = mix(refractColor, reflectColor, fresnel);
	}
	else
	{	refractColor = texture(refractTex, (vec2(glp.x,glp.y))/(2.0*glp.w)+0.5);
		mixColor = (0.5 * blueColor) + (0.6 * refractColor);
		color = vec4((mixColor.xyz * (ambient + diffuse) + 0.75*specular), 1.0);
	}

	if (isAbove != 1) color = mix(fogColor, color, pow(fogFactor,5));
}
