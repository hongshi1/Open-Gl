#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <map>
#include <tool/shader.h>
#include <tool/camera.h>
#define STB_IMAGE_IMPLEMENTATION
#include <tool/gui.h>
#include <tool/model.h>

#include "Sphere.h"
#include "Config.h"
#include "Renderer.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImportedModel.h"
#include "PLYHandler.h"
// 球体
#include "SphereRenderer.h"
// 正八面体
#include "DiamondRenderer.h"
// 圆锥
#include "ConeRenderer.h"
// 平面花
#include "FlowerRenderer.h"
// 正十二面体
#include "IcosahedronRenderer.h"
// 粒子系统
#include "ParticleSystem.h"
// 圆台
#include "FrustumRenderer.h"
// 云雾系统
#include "CloudRenderer.h"

#include "Sphere.h"
#include "Config.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(char const *path, bool gammaCorrection);
unsigned int loadCubemap(vector<std::string> faces);
void renderCarpet(Shader &magicCarpetShader, GLuint &woodMap, glm::mat4 projection, glm::mat4 view, float currentTime);

void renderQuad();
void renderCube();
void renderDodecahedron();
void renderBook();
void renderIcosahedron();
void setupVertices();

void renderSphereByPointCloud();
// 点云兔子
void renderBunnyByPointCloud();
// 发光的平面花
void renderFlowerByPointCloud(float);
// 组合物体的数组
std::vector<glm::vec3> createVertices(float spacing, float scale, const glm::vec3 &offset);
// 随机生成粒子
std::vector<float> generateParticleData(int numParticles, float cloudSize);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::vector<unsigned int> planetTextures;
std::vector<glm::vec3> starLightPositions;
std::vector<glm::vec3> initialStarLightPositions;

float lightMovementSpeed = glm::radians(3.0f); // 默认移动速度

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
glm::vec3 lightPos(1.2f, 2.0f, 1.0f); // 初始光源位置

// 初始化obj
//  ImportedModel hyperCar("./static/model/hyperCar/lamborghini-aventador-pbribl.obj");
// 创建 mesh球体
SphereRenderer sphereRenderer;
// 创建正八面体
DiamondRenderer diamondRenderer;
// 创建圆锥体渲染器
// ConeRenderer cone(36, 0.5f, 1.0f);
// 创建平面花朵渲染器
FlowerRenderer flower(36, 0.5f, 1.0f);
// 创建正十二面体渲染器
IcosahedronRenderer icosahedronRenderer;
// 创建圆台
int sectors = 36;
float bottomRadius = 1.3f;
float topRadius = 1.0f;
float height = 0.3f * topRadius;
FrustumRenderer frustum(sectors, bottomRadius, topRadius, height);
// 备注：粒子系统的对象在main函数里面创建
bool isFlow = false;

float deltaTime = 0.0f; // 当前帧与上一帧之间的时间差
float lastTime = 0.0f;	// 上一帧的时间

// 在渲染循环中更新球体的旋转
float angle = 0.0f;			 // 初始旋转角度
float rotationSpeed = 50.0f; // 旋转速度

float lastX = SCR_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 行星

// bloom 设置
bool bloomKeyPressed = false;
bool bloom = true;
float exposure = 1.0;
void renderWater(Shader &waterShader, unsigned int normalTexture, unsigned int noiseTexture, unsigned int cubeMapTexture, float deltaTime, glm::vec3 lightPos, glm::vec3 viewPos);

#include <string>
#include <vector>

void loadPlanetTextures()
{
	std::vector<std::string> planetFiles = {
		"./static/texture/solar/earth.png",
		"./static/texture/solar/mars.png",
		"./static/texture/solar/neptune.png", // 海王星
		"./static/texture/solar/venus.png",	  // 金星
		"./static/texture/solar/jupiter.png", // 木星
		"./static/texture/solar/solar.png",	  // 太阳
		"./static/texture/solar/mercury.png", // 水星
		"./static/texture/solar/moon.png",
		"./static/texture/solar/pluto.png"	// 冥王星
		"./static/texture/solar/uranus.png" // 天王星
		"./static/texture/solar/saturn.png"};	//土星

	// for (const auto &file : planetFiles)
	// {
	// 	planetTextures.push_back(loadTexture(file.c_str(), false));
	// }
}

// //初始化各大planet
// void GLInitData()
// {
// 	// 申请纹理对象 ID
// 	glGenTextures(PLANET_COUNT, texture_id);

// 	// 初始化各星球数据     纹理图片路径，           公转速度      自转速度    公转半径    自身半径    中心天体     纹理ID
// 	planet[0] = new Planet("./static/texture/solar/solar.png", 0.0f, 0.1f, 0.0f, 15.0f, NULL, texture_id[0]);
// 	planet[1] = new Planet("./static/texture/solar/mercury.png", 0.20f, 0.4f, 25.0f, 2.5f, planet[0], texture_id[1]);
// 	planet[2] = new Planet("./static/texture/solar/venus.png", 0.15f, 0.5f, 38.0f, 5.0f, planet[0], texture_id[2]);
// 	planet[3] = new Planet("./static/texture/solar/earth.png", 0.12f, 0.4f, 58.8f, 5.5f, planet[0], texture_id[3]);
// 	planet[4] = new Planet("./static/texture/solar/moon.png", -0.20f, 0.6f, 10.0f, 2.0f, planet[3], texture_id[4]);
// 	planet[5] = new Planet("./static/texture/solar/mars.png", 0.08f, 0.7f, 79.0f, 5.2f, planet[0], texture_id[5]);
// 	planet[6] = new Planet("./static/texture/solar/jupiter.png", 0.14f, 0.5f, 120.0f, 11.0f, planet[0], texture_id[6]);
// 	planet[7] = new Planet("./static/texture/solar/saturn.png", 0.09f, 0.4f, 145.0f, 7.5f, planet[0], texture_id[7]);
// 	planet[8] = new Planet("./static/texture/solar/uranus.png", 0.13f, 0.5f, 170.0f, 6.6f, planet[0], texture_id[8]);
// 	planet[9] = new Planet("./static/texture/solar/neptune.png", 0.16f, 0.5f, 195.0f, 6.3f, planet[0], texture_id[9]);
// 	planet[10] = new Planet("./static/texture/solar/pluto.png", 0.11f, 0.5f, 220.0f, 6.0f, planet[0], texture_id[10]);

// 	// 太阳光晕纹理导入
// 	glGenTextures(1, &sun_light);
// 	unsigned int width, height;
// 	std::vector<unsigned char> sun;
// 	unsigned int error = lodepng::decode(sun, width, height, "..\\texture\\sun_light.png");
// 	if (error != 0)
// 	{
// 		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
// 		return;
// 	}
// 	glBindTexture(GL_TEXTURE_2D, sun_light);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// }

int main()
{

	// 初始化GLFW
	glfwInit();

	// 配置GLFW
	// --------
	const char *glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// 创建GLFW窗口对象
	// ----------------
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);

	if (window == NULL)
	{
		cout << "初始化glfw窗口失败!" << endl;
		glfwTerminate();
		return -1;
	}

	// 注测窗口监听
	// ------------
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 初始化GLAD
	// ----------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "初始化GLAD失败!" << endl;
		return -1;
	}

	// 深度测试
	// --------
	glEnable(GL_DEPTH_TEST);

	// 启用gamma校正
	// glEnable(GL_FRAMEBUFFER_SRGB);

	// 加载模型
	// Model hyperCar1("./static/model/hyperCar1/textures/ImageToStl.com_glbfile.obj", true);
	Model hyperCar1("./static/model/hyperCar1/textures/ImageToStl.com_glbfile.obj", true);
	Model hyperCar2("./static/model/hyperCar2/Lamborghini_Veneno_(LP750-4)_Roadster_2014.obj", true);
	Model hyperCar3("./static/model/hyperCar3/Lamborghini Aventador SV.obj", true);
	Model linkStar("./static/model/planet/linked_star.obj");
	Model Mars("./static/model/Mars/Mars 2K.obj");
	Model Jupiter("./static/model/Jupiter/jupiter.obj");
	Model venus("./static/model/Venus/venus.obj");
	Model Neptune("./static/model/Neptune/Neptune.obj");
	Model Uranus("./static/model/Uranus/Uranus.obj");
	Model saturn("./static/model/Saturn/13906_Saturn_v1_l3.obj");

	// 编译shader
	// ----------
	Shader shader("./src/bloom/shader/bloom_vert.glsl", "./src/bloom/shader/bloom_frag.glsl");
	Shader shaderLight("./src/bloom/shader/bloom_light_vert.glsl", "./src/bloom/shader/bloom_light_frag.glsl");
	Shader shaderBlur("./src/bloom/shader/bloom_blur_vert.glsl", "./src/bloom/shader/bloom_blur_frag.glsl");
	Shader shaderFinal("./src/bloom/shader/bloom_final_vert.glsl", "./src/bloom/shader/bloom_final_frag.glsl");
	Shader skyboxShader("./src/bloom/shader/skybox_vert.glsl", "./src/bloom/shader/skybox_frag.glsl");
	Shader waterShader("./src/bloom/shader/water_vert.glsl", "./src/bloom/shader/water_frag.glsl");
	Shader magicCarpetShader("./src/bloom/shader/magic_carpet_vert.glsl", "./src/bloom/shader/magic_carpet_frag.glsl");

	Shader pointCloudShader("./src/bloom/shader/model_sphere_vert.glsl", "./src/bloom/shader/model_sphere_frag.glsl");
	Shader glodenShader("./src/bloom/shader/model_sphere_vert.glsl", "./src/bloom/shader/model_gloden_frag.glsl");
	// 水晶球的shader
	Shader crystalShader("./src/bloom/shader/crystal_vert.glsl", "./src/bloom/shader/crystal_frag.glsl");

	Shader bookShader("./src/bloom/shader/book_vert.glsl", "./src/bloom/shader/book_frag.glsl");

	// 旋转物体
	Shader linkStarShader("./src/bloom/shader/vertex_shader.glsl", "./src/bloom/shader/linkStar.glsl");

	// lyy
	Model skull("./static/model/skull/skull.obj");
	Model wolf("./static/model/wolf/Wolf_One_obj.obj");
	Model planet("./static/model/planet/planet.obj");
	Model duck("./static/model/duck/duck.obj");
	Model pedestal("./static/model/3D_scifi_pedestal/tech_pedestal.obj");
	Model tech_duck("./static/model/3D_scifi_pedestal/tech_pedestal.obj");
	Model tech_wolf("./static/model/3D_scifi_pedestal/tech_pedestal.obj");
	Model tech_skull("./static/model/3D_scifi_pedestal/tech_pedestal.obj");
	Model tech_car1("./static/model/3D_scifi_pedestal/tech_pedestal.obj");
	Model tech_car2("./static/model/3D_scifi_pedestal/tech_pedestal.obj");
	Model tech_car3("./static/model/3D_scifi_pedestal/tech_pedestal.obj");

	// 粒子系统-dcy
	ParticleSystem particleSystem;
	particleSystem.initialize();
	// 效果
	//  云雾对象
	//  创建 CloudRenderer 对象并设置参数
	CloudRenderer cloudRenderer; // 创建一个包含 1000 个粒子的 CloudRenderer 对象
								 // cloudRenderer.setFlowSpeed(1.0f); // 设置粒子的流动速度
								 // cloudRenderer.setFlowDirection(glm::vec3(0.0f, 1.0f, 0.0f)); // 设置粒子的流动方向

	// 发光效果的参数
	glm::vec3 glowColor = glm::vec3(0.0f, 1.0f, 0.0f); // 蓝色发光
	// 顶点数组
	float cubeVertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	 // top-right
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f,	 // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	 // top-right
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,	 // top-left
		// Front face
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,	// bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,	// top-right
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,	// top-right
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,	// top-left
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,	 // top-right
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	 // top-left
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	 // bottom-right
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,	 // top-right
		// Right face
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,	// top-left
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	// top-right
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,	// top-left
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// bottom-left
		// Bottom face
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f,	 // top-left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,	 // bottom-left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,	 // bottom-left
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	 // bottom-right
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
		// Top face
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,	// bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	// top-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,	// bottom-right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f	// bottom-left
	};

	float skyboxVertices[] = {
		// positions
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f};

	// 绘制组合正八面体
	float scale = 0.1f;	  // 缩放因子
	float spacing = 0.3f; // 间隔

	glm::vec3 cubeGroupVertices[] = {
		glm::vec3(-spacing, -spacing, -spacing),
		glm::vec3(spacing, -spacing, -spacing),
		glm::vec3(-spacing, spacing, -spacing),
		glm::vec3(spacing, spacing, -spacing),
		glm::vec3(-spacing, -spacing, spacing),
		glm::vec3(spacing, -spacing, spacing),
		glm::vec3(-spacing, spacing, spacing),
		glm::vec3(spacing, spacing, spacing)};
	// 定义大立方体的顶点数组
	glm::vec3 largeCubeVertices[] = {
		glm::vec3(-spacing * 1.1, -spacing * 1.1, -spacing * 1.1),
		glm::vec3(spacing * 1.1, -spacing * 1.1, -spacing * 1.1),
		glm::vec3(-spacing * 1.1, spacing * 1.1, -spacing * 1.1),
		glm::vec3(spacing * 1.1, spacing * 1.1, -spacing * 1.1),
		glm::vec3(-spacing * 1.1, -spacing * 1.1, spacing * 1.1),
		glm::vec3(spacing * 1.1, -spacing * 1.1, spacing * 1.1),
		glm::vec3(-spacing * 1.1, spacing * 1.1, spacing * 1.1),
		glm::vec3(spacing * 1.1, spacing * 1.1, spacing * 1.1),

		// 面心
		glm::vec3(0.0f, 0.0f, -spacing * 1.1), // 前面中心
		glm::vec3(0.0f, 0.0f, spacing * 1.1),  // 后面中心
		glm::vec3(-spacing * 1.1, 0.0f, 0.0f), // 左侧中心
		glm::vec3(spacing * 1.1, 0.0f, 0.0f),  // 右侧中心
		glm::vec3(0.0f, -spacing * 1.1, 0.0f), // 下面中心
		glm::vec3(0.0f, spacing * 1.1, 0.0f),  // 上面中心
		// 棱心
		glm::vec3(-spacing * 1.1, 0.0f, -spacing * 1.1), // 前左棱心
		glm::vec3(-spacing * 1.1, 0.0f, spacing * 1.1),	 // 后左棱心
		glm::vec3(spacing * 1.1, 0.0f, -spacing * 1.1),	 // 前右棱心
		glm::vec3(spacing * 1.1, 0.0f, spacing * 1.1),	 // 后右棱心
		glm::vec3(0.0f, -spacing * 1.1, -spacing * 1.1), // 前下棱心
		glm::vec3(0.0f, -spacing * 1.1, spacing * 1.1),	 // 后下棱心
		glm::vec3(0.0f, spacing * 1.1, -spacing * 1.1),	 // 前上棱心
		glm::vec3(0.0f, spacing * 1.1, spacing * 1.1),	 // 后上棱心
		glm::vec3(-spacing * 1.1, -spacing * 1.1, 0.0f), // 左下棱心
		glm::vec3(-spacing * 1.1, spacing * 1.1, 0.0f),	 // 左上棱心
		glm::vec3(spacing * 1.1, -spacing * 1.1, 0.0f),	 // 右下棱心
		glm::vec3(spacing * 1.1, spacing * 1.1, 0.0f)	 // 右上棱心
	};

	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glBindVertexArray(0);

	// skyboxVBO quad VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	// 加载纹理

	unsigned int cubeTexture = loadTexture("./static/texture/container.jpg", false);
	unsigned int waterTexture = loadTexture("./static/images/wave.bmp", false);
	// 加载法线贴图
	unsigned int normalTexture = loadTexture("./static/texture/TexturesCom_MuddySand2_2x2_2K_normal.png", false);
	unsigned int noiseTexture = loadTexture("./static/texture/perlin.png", false);

	// 加载钻石形状的纹理
	//  unsigned int diamondTexture = loadTexture("./static/texture/subskybox/eight.png", false);
	// 加载球体的纹理
	unsigned int sphereTexture = loadTexture("./static/texture/subskybox/sphere8.jpg", false);
	// 正八面体的纹理
	unsigned int diamondTexture = loadTexture("./static/texture/subskybox/sphere2.jpg", false);
	// vector<std::string> faces
	// 	{
	// 		"./static/texture/skyboxq/ny.png",
	// 		"./static/texture/skyboxq/nx.png",
	// 		"./static/texture/skyboxq/nz.png",
	// 		"./static/texture/skyboxq/px.png",
	// 		"./static/texture/skyboxq/py.png",
	// 		"./static/texture/skyboxq/pz.png",

	// 		};

	// 备注：这是5-21周二之前有缝隙的一版
	vector<std::string> faces{
		"./static/texture/skyboxq/px.png",
		"./static/texture/skyboxq/nx.png",
		"./static/texture/skyboxq/pz.png",
		"./static/texture/skyboxq/nz.png",
		"./static/texture/skyboxq/ny.png",
		"./static/texture/skyboxq/py.png",
	};
	// 全是白色星星的
	/* vector<std::string> faces{
		"./static/texture/subskybox/right.bmp",
		"./static/texture/subskybox/left.bmp",
		"./static/texture/subskybox/top.bmp",
		"./static/texture/subskybox/bottom.bmp",
		"./static/texture/subskybox/front.bmp",
		"./static/texture/subskybox/back.bmp",
	}; */
	unsigned int cubemapTexture = loadCubemap(faces);

	// unsigned int cubemapTexture = loadCubemap(faces);

	// 创建imgui上下文
	// ---------------
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// 加载贴图
	// --------
	unsigned int woodMap = loadTexture("./static/images/light.png", false);
	unsigned int containerMap = loadTexture("./static/texture/toy_box_normal.png", true);
	// lyy
	// book
	unsigned int texture1 = loadTexture("./static/texture/book/book.jpg", true);
	unsigned int normalMap1 = loadTexture("./static/texture/book/book_normal1.jpg", true);
	unsigned int texture2 = loadTexture("./static/texture/book/bookBottom.png", true);
	unsigned int normalMap2 = loadTexture("./static/texture/book/bookBottom_normal.jpg", true);
	unsigned int texture3 = loadTexture("./static/texture/book/bookLeft.png", true);
	unsigned int normalMap3 = loadTexture("./static/texture/book/bookLeft_normal.jpg", true);
	unsigned int texture4 = loadTexture("./static/texture/book/bookBack.png", true);

	// 配置浮点帧缓冲区
	// ----------------
	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// 创建两个浮点颜色缓冲区，一个用于正常渲染，一个用于亮度阈值读取
	unsigned int colorBuffers[2];
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// 因为模糊滤镜会采样重复的纹理值
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// 将纹理添加到帧缓冲
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}

	{
		// 定义球体
		Sphere mySphere(40);
		std::vector<float> sphereVertices = mySphere.getVertices();
		std::vector<unsigned int> sphereIndices = mySphere.getIndices();
		VertexArray va;
		VertexBuffer vb(&sphereVertices[0], sphereVertices.size() * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);
		va.AddBuffer(layout);

		IndexBuffer ib(&sphereIndices[0], sphereIndices.size()); // �������

		glEnable(GL_DEPTH_TEST);



		vb.Unbind();
		va.Unbind();
		ib.Unbind();

		std::vector<std::string> planetFiles = {
			"./static/texture/solar/earth.png",
			"./static/texture/solar/mars.png",
			"./static/texture/solar/neptune.png", // 海王星
			"./static/texture/solar/venus.png",	  // 金星
			"./static/texture/solar/jupiter.png", // 木星
			"./static/texture/solar/solar.png",	  // 太阳
			"./static/texture/solar/mercury.png", // 水星
			"./static/texture/solar/moon.png",
			"./static/texture/solar/pluto.png"	  // 冥王星
			"./static/texture/solar/uranus.png"	  // 天王星
			"./static/texture/solar/saturn.png"}; // 土星

			
	}

	// 创建并附加到深度缓冲区
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// 告诉OpenGL我们将使用此帧缓冲区的哪些颜色附件进行渲染
	unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, attachments);
	// 检查framebuffer 是否编译成功
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "Framebuffer not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 用于模糊的framebuffer
	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

		// 检查framebuffer 是否编译成功
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "Framebuffer not complete!" << endl;
	}

	// lighting info
	// -------------
	// light positions
	vector<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
	lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
	lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
	lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
	// 点云球的球心
	lightPositions.push_back(glm::vec3(10.0f, 1.0f, 0.0f));

	vector<glm::vec3> starLightColors;

	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

	// 在星光源初始化代码中，增加 initialStarLightPositions 初始化
	for (int i = 0; i < 200; ++i)
	{
		float theta = glm::radians(distribution(generator) * 180.0f);
		float phi = glm::radians(distribution(generator) * 360.0f);
		float radius = 70.0f + distribution(generator) * 50.0f; // Large sphere radius
		float x = radius * sin(theta) * cos(phi);
		float y = radius * sin(theta) * sin(phi);
		float z = radius * cos(theta);

		starLightPositions.push_back(glm::vec3(x, y, z));
		initialStarLightPositions.push_back(glm::vec3(x, y, z));																			  // 初始化初始位置
		starLightColors.push_back(glm::vec3(distribution(generator) + 1.0f, distribution(generator) + 1.0f, distribution(generator) + 1.0f)); // Random bright colors
	}

	// colors
	vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
	lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
	lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));
	// 白光
	lightColors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

	// shader 配置信息
	// ---------------
	shader.use();
	shader.setInt("diffuseTexture", 0);

	shaderBlur.use();
	shaderBlur.setInt("image", 0);

	shaderFinal.use();
	shaderFinal.setInt("scene", 0);
	shaderFinal.setInt("bloomBlur", 1);

	// lyy
	bookShader.use();
	bookShader.setInt("texture1", 0);
	bookShader.setInt("normalMap1", 1);
	bookShader.setInt("texture2", 2);
	bookShader.setInt("normalMap2", 3);
	bookShader.setInt("texture3", 4);
	bookShader.setInt("normalMap3", 5);
	bookShader.setInt("texture4", 6);

	// linkStart
	linkStarShader.use();
	linkStarShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	linkStarShader.setVec3("glowColor", glowColor);
	linkStarShader.setFloat("glowIntensity", 2.0f);
	// 渲染循环
	// --------
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		// 帧时间
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;

		// input
		// -----
		processInput(window);

		// imgui
		// -----
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("view value");
		ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. 将场景渲染到浮点帧缓冲区
		// ---------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 view1 = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 position;

		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		view1 = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		// projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		skyboxShader.setMat4("view", view1);
		skyboxShader.setMat4("projection", projection);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// 导入光源位置和摄像机位置
		glm::vec3 lightPos(15.2f, 2.0f, 1.0f);
		// glm::vec3 lightPos(1.2f, 2.0f, 1.0f);

		renderWater(waterShader, normalTexture, noiseTexture, cubemapTexture, deltaTime, lightPos, camera.Position);

		magicCarpetShader.use();
		magicCarpetShader.setMat4("projection", projection);
		magicCarpetShader.setMat4("view", view);
		magicCarpetShader.setMat4("model", model);
		magicCarpetShader.setFloat("time", glfwGetTime());
		renderCarpet(magicCarpetShader, woodMap, projection, view, glfwGetTime());

		shader.use();
		shader.setMat4("projection", projection);
		shader.setVec3("lightPos", lightPos);
		shader.setMat4("view", view);
		glActiveTexture(GL_TEXTURE0);

		// 设置灯光参数值
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			shader.setVec3("lights[" + to_string(i) + "].Position", lightPositions[i]);
			shader.setVec3("lights[" + to_string(i) + "].Color", lightColors[i]);
		}
		shader.setVec3("viewPos", camera.Position);
		// 创建一个大的立方体作为地板

		// 创建多个立方体作为物体
		glBindTexture(GL_TEXTURE_2D, containerMap);
		model = glm::mat4(0.5f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		
		renderIcosahedron();

		model = glm::mat4(0.5f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		renderIcosahedron();

		model = glm::mat4(0.5f);
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shader.setMat4("model", model);
		renderIcosahedron();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
		model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(1.25));
		shader.setMat4("model", model);
		renderIcosahedron();

		model = glm::mat4(0.5f);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
		model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shader.setMat4("model", model);
		renderIcosahedron();

		crystalShader.use();
		crystalShader.setMat4("model", model);
		crystalShader.setMat4("view", view);
		crystalShader.setMat4("projection", projection);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		sphereRenderer.renderSphere();

		// book
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalMap2);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, normalMap3);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, texture4);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-10.5f, 0.0f, 3.0f)); // 设置书本位置
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

		bookShader.use();
		bookShader.setMat4("model", model);
		bookShader.setMat4("view", view);
		bookShader.setMat4("projection", projection);
		renderBook();

		shader.use();
		// skull
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -0.3f, 3.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f)); // it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		skull.Draw(shader);

		// tech_skull
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -0.5f, 3.0f)); // translate it down so it's at the center of the scene
		// model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f)); // it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		tech_skull.Draw(shader);

		// planet
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 10.5f, 3.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f)); // it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		planet.Draw(shader);

		// wolf
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, -0.3f, 3.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setMat4("model", model);
		wolf.Draw(shader);

		// tech_wolf
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, -0.5f, 3.0f)); // translate it down so it's at the center of the scene
		// model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f)); // it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		tech_wolf.Draw(shader);

		// duck
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, -0.3f, 3.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		shader.setMat4("model", model);
		duck.Draw(shader);

		// tech_duck
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, -0.5f, 3.0f)); // translate it down so it's at the center of the scene
		// model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f)); // it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		tech_duck.Draw(shader);

		// pedestal
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 3.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		model = glm::rotate(model, glm::radians(0.0f), glm::normalize(glm::vec3(6.0, 0.0, 0.0)));
		shader.setMat4("model", model);
		pedestal.Draw(shader);

		// hyper1
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(12.0f, -0.3f, 1.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		model = glm::rotate(model, glm::radians(angle / 10), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		shader.setMat4("model", model);
		hyperCar1.Draw(shader);

		// tech_car1
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(12.0f, -0.5f, 1.0f)); // translate it down so it's at the center of the scene
		// model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f)); // it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		tech_car1.Draw(shader);

		// hyper2
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(10.0f, -0.3f, 1.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		model = glm::rotate(model, glm::radians(180.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		shader.setMat4("model", model);
		hyperCar2.Draw(shader);

		// tech_car2
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(10.0f, -0.5f, 1.0f)); // translate it down so it's at the center of the scene
		// model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f)); // it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		tech_car2.Draw(shader);

		//
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(7.0f, -0.3f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, glm::radians(0.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		shader.setMat4("model", model);
		hyperCar3.Draw(shader);

		// tech_car3
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.0f)); // translate it down so it's at the center of the scene
		// model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f)); // it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		tech_car3.Draw(shader);

		// 旋转物体
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, -0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		model = glm::rotate(model, glm::radians(angle), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		linkStarShader.setMat4("model", model);
		linkStar.Draw(linkStarShader);

		// 火星
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 10.5f, -3.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, glm::radians(angle / 10), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		shader.setMat4("model", model);
		Mars.Draw(shader);

		// 木星
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 10.5f, -3.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(angle / 10), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		shader.setMat4("model", model);
		Jupiter.Draw(shader);

		// 金星
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0f, 10.5f, -3.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(angle / 10), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		shader.setMat4("model", model);
		venus.Draw(shader);

		// 海王星
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(6.0f, 10.5f, -3.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(angle / 10), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		shader.setMat4("model", model);
		Neptune.Draw(shader);

		// 天王星
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(10.0f, 10.5f, -3.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(angle / 10), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		shader.setMat4("model", model);
		Uranus.Draw(shader);

		// 土星
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(14.0f, 10.5f, -3.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(angle / 10), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		shader.setMat4("model", model);
		saturn.Draw(shader);
		// renderPlanet(lightPos, camera.Position);

		// 使用着色器程序
		glodenShader.use();
		// 设置uniform变量
		glodenShader.setVec3("viewPos", camera.Position); // 设置视点位置
		glodenShader.setFloat("rimIntensity", 0.7);		  // 设置泛光强度
		glodenShader.setMat4("projection", projection);
		glodenShader.setMat4("view", view);
		// 设置模型矩阵
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(18.0f, 8.0f, -5.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		glodenShader.setMat4("model", model);
		// 更新粒子位置
		cloudRenderer.update(0.01f, 10, 10, 10);
		// 绘制云彩
		cloudRenderer.render(10, 10, 10);

		// 设置透明度
		float transparency = 0.8; // 设置透明度为50%
		// 使用着色器程序
		pointCloudShader.use();
		// 设置uniform变量
		pointCloudShader.setVec3("viewPos", camera.Position);	 // 设置视点位置
		pointCloudShader.setFloat("rimIntensity", 0.7);			 // 设置泛光强度
		pointCloudShader.setFloat("transparency", transparency); // 设置透明度
		pointCloudShader.setMat4("projection", projection);
		pointCloudShader.setMat4("view", view);
		// 修改光源的颜色和泛光颜色
		pointCloudShader.setVec3("lightPos", lightPositions[1]);
		pointCloudShader.setVec3("lightColor", lightColors[1]);				 // 设置光源颜色
		pointCloudShader.setVec3("rimLightColor", glm::vec3(0.2, 0.6, 1.0)); // 设置泛光颜色

		// 纹理设置--------------------------------------------
		glBindTexture(GL_TEXTURE_2D, sphereTexture);
		// 设置纹理参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// 启用混合功能-透明效果----------------
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// 纹理透明球
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0f, 1.0f, 8.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		pointCloudShader.setMat4("model", model);
		// 渲染球体
		sphereRenderer.renderSphere();

		// 透明圆台--点云球
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, -0.5f, 8.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		pointCloudShader.setMat4("model", model);
		frustum.render();
		// 透明圆台--粒子系统
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, -0.5f, 8.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		pointCloudShader.setMat4("model", model);
		frustum.render();

		// 透明圆台--纹理球(太丑,注释掉)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0f, -0.5f, 8.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		pointCloudShader.setMat4("model", model);
		frustum.render();

		// 关闭混合功能
		glDisable(GL_BLEND);

		// 点云绘制球体
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, 8.0f));
		// 绕 y 轴旋转
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 1.0f)); // 绕 X 轴旋转 90 度
		model = glm::scale(model, glm::vec3(1.5f));
		pointCloudShader.setMat4("model", model);
		// 设置点的大小
		glPointSize(3.0f); // Adjust the size as needed
		renderSphereByPointCloud();

		// 粒子系统绘制球体
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 2.0f, 8.0f));
		// 绕 y 轴旋转
		// model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 1.0f)); // 绕 X 轴旋转 90 度
		model = glm::scale(model, glm::vec3(0.4f));
		particleSystem.update(deltaTime);
		pointCloudShader.setMat4("model", model);
		// 设置点的大小
		glPointSize(1.3f); // Adjust the size as needed
		particleSystem.render();

		// 绘制正二十面体
		// icosahedronRenderer.renderIcosahedron();

		// 更新旋转角度
		angle += rotationSpeed * deltaTime; // deltaTime是上一帧到当前帧的时间间隔

		// 绑定纹理
		glBindTexture(GL_TEXTURE_2D, diamondTexture);
		// 绘制内部八个正八面体
		for (int i = 0; i < 8; ++i)
		{
			model = glm::mat4(1.0f);
			position = cubeGroupVertices[i] * spacing;

			model = glm::translate(model, position);	 // 设置位置
			model = glm::scale(model, glm::vec3(scale)); // 设置缩放
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
			pointCloudShader.setMat4("model", model); // 设置模型矩阵
			diamondRenderer.renderDiamond();		  // 绘制正八面体

			// 绘制外部大立方体
			for (int j = 0; j < 26; ++j)
			{
				model = glm::mat4(1.0f);
				position = largeCubeVertices[j];

				model = glm::translate(model, position);		 // 设置位置
				model = glm::scale(model, glm::vec3(scale * 2)); // 设置缩放
				model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
				pointCloudShader.setMat4("model", model); // 设置模型矩阵
				diamondRenderer.renderDiamond();		  // 绘制正八面体
			}
		}

		// hyperCar
		//  setupVertices();

		// 最后将所有光源显示为明亮的立方体
		shaderLight.use();
		shaderLight.setMat4("projection", projection);
		shaderLight.setMat4("view", view);

		for (unsigned int i = 0; i < starLightPositions.size(); i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, starLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.5f)); // Adjust the size of the spheres
			shaderLight.setMat4("model", model);
			shaderLight.setVec3("lightColor", starLightColors[i]); // Set light color
			sphereRenderer.renderSphere();						   // Render the light as a sphere
		}
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			if (i == lightPositions.size() - 1)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(lightPositions[i]));
				model = glm::scale(model, glm::vec3(0.5f));
				shaderLight.setMat4("model", model);
				shaderLight.setVec3("lightColor", lightColors[i]);

				// 设置光源颜色
				// sphereRenderer.renderSphere();

				//  绘制花朵
				flower.render();
			}
			else
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(lightPositions[i]));
				model = glm::scale(model, glm::vec3(0.25f));
				shaderLight.setMat4("model", model);
				shaderLight.setVec3("lightColor", lightColors[i]); // 设置光源颜色
				renderCube();
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. 通过两次高斯模糊来模糊明亮的片元
		// -----------------------------------
		bool horizontal = true, first_iteration = true;
		unsigned int amount = 10;
		shaderBlur.use();
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			shaderBlur.setInt("horizontal", horizontal);
			// 绑定其他缓冲区，如果是第一次迭代的纹理
			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);
			renderQuad();
			horizontal = !horizontal;
			if (first_iteration)
			{
				first_iteration = false;
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. 现在将浮点颜色缓冲区渲染为2d四边形，并将色调映射HDR颜色渲染为默认帧缓冲区颜色范围
		//-------------------------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderFinal.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		shaderFinal.setInt("bloom", bloom);
		shaderFinal.setFloat("exposure", exposure);
		renderQuad();

		// cout << "bloom: " << (bloom ? "on" : "off") << " | exposure: " << exposure << endl;

		// ImGui
		// -----
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw 交换颜色缓冲区和轮询IO事件
		// ------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 删除/释放资源
	glfwTerminate();
	return 0;
}

unsigned int dodecahedronVAO = 0;
unsigned int dodecahedronVBO = 0;
unsigned int dodecahedronEBO = 0;

void renderDodecahedron()
{
    if (dodecahedronVAO == 0)
    {
        float vertices[] = {
            // Vertex positions       // Normals            // Texture Coords
             0.607f,  0.000f,  0.795f,  0.607f,  0.000f,  0.795f,  0.5f,  1.0f,  // 0
            -0.607f,  0.000f,  0.795f, -0.607f,  0.000f,  0.795f,  0.0f,  1.0f,  // 1
             0.982f,  0.795f,  0.000f,  0.982f,  0.795f,  0.000f,  0.5f,  0.5f,  // 2
            -0.982f,  0.795f,  0.000f, -0.982f,  0.795f,  0.000f,  0.0f,  0.5f,  // 3
             0.000f,  1.295f,  0.795f,  0.000f,  1.295f,  0.795f,  0.5f,  0.0f,  // 4
             0.982f, -0.795f,  0.000f,  0.982f, -0.795f,  0.000f,  1.0f,  0.5f,  // 5
            -0.982f, -0.795f,  0.000f, -0.982f, -0.795f,  0.000f,  0.0f,  0.5f,  // 6
             0.000f, -1.295f,  0.795f,  0.000f, -1.295f,  0.795f,  0.5f,  0.0f,  // 7
             0.607f,  0.000f, -0.795f,  0.607f,  0.000f, -0.795f,  0.5f,  0.0f,  // 8
            -0.607f,  0.000f, -0.795f, -0.607f,  0.000f, -0.795f,  0.0f,  0.0f,  // 9
             0.000f,  1.295f, -0.795f,  0.000f,  1.295f, -0.795f,  1.0f,  1.0f,  // 10
             0.000f, -1.295f, -0.795f,  0.000f, -1.295f, -0.795f,  0.5f,  0.5f   // 11
        };

        unsigned int indices[] = {
            0, 1, 3, 0, 3, 4, 0, 4, 2, 1, 0, 5,
            5, 0, 2, 1, 5, 6, 1, 6, 3, 3, 6, 7,
            3, 7, 4, 4, 7, 8, 4, 8, 2, 2, 8, 5,
            5, 8, 9, 6, 5, 9, 6, 9, 7, 7, 9, 10,
            7, 10, 8, 8, 10, 9, 9, 10, 11, 10, 11, 8
        };

        glGenVertexArrays(1, &dodecahedronVAO);
        glGenBuffers(1, &dodecahedronVBO);
        glGenBuffers(1, &dodecahedronEBO);

        glBindVertexArray(dodecahedronVAO);

        glBindBuffer(GL_ARRAY_BUFFER, dodecahedronVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dodecahedronEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glBindVertexArray(dodecahedronVAO);
    glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,	// bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,	// top-left
			// front face
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	  // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	  // top-right
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right
			// right face
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	  // top-left
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-right
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	  // top-left
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
			// bottom face
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,	// top-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
			-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	  // bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	  // bottom-right
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f	  // bottom-left
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
unsigned int waterVAO = 0;
unsigned int waterVBO = 0;
void renderWater(Shader &waterShader, unsigned int normalTexture, unsigned int noiseTexture, unsigned int cubeMapTexture, float deltaTime, glm::vec3 lightPos, glm::vec3 viewPos)
{
	if (waterVAO == 0)
	{
		float waterVertices[] = {
			-10.0f, 0.0f, -10.0f, 0.0f, 0.0f,
			10.0f, 0.0f, -10.0f, 1.0f, 0.0f,
			-10.0f, 0.0f, 10.0f, 0.0f, 1.0f,
			10.0f, 0.0f, -10.0f, 1.0f, 0.0f,
			10.0f, 0.0f, 10.0f, 1.0f, 1.0f,
			-10.0f, 0.0f, 10.0f, 0.0f, 1.0f};

		glGenVertexArrays(1, &waterVAO);
		glGenBuffers(1, &waterVBO);
		glBindVertexArray(waterVAO);
		glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	}

	waterShader.use();
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));

	waterShader.setMat4("ModelMatrix", model);
	waterShader.setMat4("IT_ModelMatrix", normalMatrix);
	waterShader.setMat4("ViewMatrix", view);
	waterShader.setMat4("ProjectMatrix", projection);
	waterShader.setVec3("cameraPos", camera.Position);
	waterShader.setVec3("LightLocation", lightPos);
	waterShader.setFloat("totalTime", glfwGetTime());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
	waterShader.setInt("T_Water_N", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	waterShader.setInt("cubeMap", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	waterShader.setInt("T_Perlin_Noise_M", 2);

	glBindVertexArray(waterVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
unsigned int icosahedronVAO = 0;
unsigned int icosahedronVBO = 0;
unsigned int icosahedronEBO = 0;

void renderIcosahedron()
{
    if (icosahedronVAO == 0)
    {
        float t = (1.0 + sqrt(5.0)) / 2.0;
        float vertices[] = {
            // positions          // texture coords
            -1,  t,  0,          0.0f, 1.0f,
             1,  t,  0,          1.0f, 1.0f,
            -1, -t,  0,          0.0f, 0.0f,
             1, -t,  0,          1.0f, 0.0f,
             0, -1,  t,          0.5f, 0.0f,
             0,  1,  t,          0.5f, 1.0f,
             0, -1, -t,          0.5f, 0.0f,
             0,  1, -t,          0.5f, 1.0f,
             t,  0, -1,          1.0f, 0.5f,
             t,  0,  1,          1.0f, 0.5f,
            -t,  0, -1,          0.0f, 0.5f,
            -t,  0,  1,          0.0f, 0.5f,
        };

        unsigned int indices[] = {
            0, 11,  5,
            0,  5,  1,
            0,  1,  7,
            0,  7, 10,
            0, 10, 11,
            1,  5,  9,
            5, 11,  4,
            11, 10,  2,
            10,  7,  6,
            7,  1,  8,
            3,  9,  4,
            3,  4,  2,
            3,  2,  6,
            3,  6,  8,
            3,  8,  9,
            4,  9,  5,
            2,  4, 11,
            6,  2, 10,
            8,  6,  7,
            9,  8,  1,
        };

        glGenVertexArrays(1, &icosahedronVAO);
        glGenBuffers(1, &icosahedronVBO);
        glGenBuffers(1, &icosahedronEBO);

        glBindVertexArray(icosahedronVAO);

        glBindBuffer(GL_ARRAY_BUFFER, icosahedronVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, icosahedronEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glBindVertexArray(icosahedronVAO);
    glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


// 绘制点云球体
void renderSphereByPointCloud()
{
	static bool initialized = false;
	static unsigned int sphereVAO, sphereVBO;
	// 水平切割数
	const int sectors = 36;
	// 垂直切割数
	const int stacks = 18;

	if (!initialized)
	{
		std::vector<glm::vec3> points;
		const int sectors = 36;
		const int stacks = 18;

		for (int i = 0; i <= stacks; ++i)
		{
			float phi = glm::pi<float>() * static_cast<float>(i) / static_cast<float>(stacks);
			for (int j = 0; j <= sectors; ++j)
			{
				float theta = 2.0f * glm::pi<float>() * static_cast<float>(j) / static_cast<float>(sectors);
				float x = cos(theta) * sin(phi);
				float y = cos(phi);
				float z = sin(theta) * sin(phi);
				points.push_back(glm::vec3(x, y, z));
			}
		}

		// Generate buffers
		glGenVertexArrays(1, &sphereVAO);
		glGenBuffers(1, &sphereVBO);

		// Bind VAO
		glBindVertexArray(sphereVAO);

		// Bind and fill vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
		glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

		// Set vertex attribute pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0); // Position
		glEnableVertexAttribArray(0);

		// Unbind VAO
		glBindVertexArray(0);

		initialized = true;
	}

	// Render sphere point cloud
	glBindVertexArray(sphereVAO);
	glDrawArrays(GL_POINTS, 0, (sectors + 1) * (stacks + 1));
	glBindVertexArray(0);
}

// 设置顶点数组函数
std::vector<glm::vec3> createVertices(float spacing, float scale, const glm::vec3 &offset)
{
	// 顶点数组
	std::vector<glm::vec3> vertices = {
		// 小立方体的顶点
		glm::vec3(-spacing, -spacing, -spacing),
		glm::vec3(spacing, -spacing, -spacing),
		glm::vec3(-spacing, spacing, -spacing),
		glm::vec3(spacing, spacing, -spacing),
		glm::vec3(-spacing, -spacing, spacing),
		glm::vec3(spacing, -spacing, spacing),
		glm::vec3(-spacing, spacing, spacing),
		glm::vec3(spacing, spacing, spacing),
		// 大立方体的顶点
		glm::vec3(-spacing * 1.1, -spacing * 1.1, -spacing * 1.1),
		glm::vec3(spacing * 1.1, -spacing * 1.1, -spacing * 1.1),
		glm::vec3(-spacing * 1.1, spacing * 1.1, -spacing * 1.1),
		glm::vec3(spacing * 1.1, spacing * 1.1, -spacing * 1.1),
		glm::vec3(-spacing * 1.1, -spacing * 1.1, spacing * 1.1),
		glm::vec3(spacing * 1.1, -spacing * 1.1, spacing * 1.1),
		glm::vec3(-spacing * 1.1, spacing * 1.1, spacing * 1.1),
		glm::vec3(spacing * 1.1, spacing * 1.1, spacing * 1.1),
		// 面心
		glm::vec3(0.0f, 0.0f, -spacing * 1.1), // 前面中心
		glm::vec3(0.0f, 0.0f, spacing * 1.1),  // 后面中心
		glm::vec3(-spacing * 1.1, 0.0f, 0.0f), // 左侧中心
		glm::vec3(spacing * 1.1, 0.0f, 0.0f),  // 右侧中心
		glm::vec3(0.0f, -spacing * 1.1, 0.0f), // 下面中心
		glm::vec3(0.0f, spacing * 1.1, 0.0f),  // 上面中心
		// 棱心
		glm::vec3(-spacing * 1.1, 0.0f, -spacing * 1.1), // 前左棱心
		glm::vec3(-spacing * 1.1, 0.0f, spacing * 1.1),	 // 后左棱心
		glm::vec3(spacing * 1.1, 0.0f, -spacing * 1.1),	 // 前右棱心
		glm::vec3(spacing * 1.1, 0.0f, spacing * 1.1),	 // 后右棱心
		glm::vec3(0.0f, -spacing * 1.1, -spacing * 1.1), // 前下棱心
		glm::vec3(0.0f, -spacing * 1.1, spacing * 1.1),	 // 后下棱心
		glm::vec3(0.0f, spacing * 1.1, -spacing * 1.1),	 // 前上棱心
		glm::vec3(0.0f, spacing * 1.1, spacing * 1.1),	 // 后上棱心
		glm::vec3(-spacing * 1.1, -spacing * 1.1, 0.0f), // 左下棱心
		glm::vec3(-spacing * 1.1, spacing * 1.1, 0.0f),	 // 左上棱心
		glm::vec3(spacing * 1.1, -spacing * 1.1, 0.0f),	 // 右下棱心
		glm::vec3(spacing * 1.1, spacing * 1.1, 0.0f)	 // 右上棱心
	};

	// 应用缩放和偏移量
	for (auto &vertex : vertices)
	{
		vertex = vertex * scale + offset;
	}

	return vertices;
}
std::vector<float> generateParticleData(int numParticles, float cloudSize)
{
	std::vector<float> particleData;
	particleData.reserve(numParticles * 3);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-cloudSize / 2, cloudSize / 2);

	for (int i = 0; i < numParticles; ++i)
	{
		float x = dis(gen);
		float y = dis(gen);
		float z = dis(gen);

		particleData.push_back(x);
		particleData.push_back(y);
		particleData.push_back(z);
	}

	return particleData;
}
// 测试读取ply文件数据用
void renderBunnyByPointCloud()
{
	static bool initialized = false;
	static unsigned int pointCloudVAO, pointCloudVBO;
	std::vector<glm::vec3> points;

	if (!initialized)
	{
		// Read point cloud data from PLY file
		std::string filePath = "./static/model/bunny/bun000.ply";
		points = PLYHandler::readPLY(filePath);

		// Debug: print number of points
		std::cout << "Number of points: " << points.size() << std::endl;

		// Generate buffers
		glGenVertexArrays(1, &pointCloudVAO);
		glGenBuffers(1, &pointCloudVBO);

		// Bind VAO
		glBindVertexArray(pointCloudVAO);

		// Bind and fill vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, pointCloudVBO);
		glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

		// Set vertex attribute pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0); // Position
		glEnableVertexAttribArray(0);

		// Unbind VAO
		glBindVertexArray(0);

		initialized = true;
	}

	// Set point size
	glPointSize(5.0f); // Adjust the size as needed

	// Render point cloud
	glBindVertexArray(pointCloudVAO);
	glDrawArrays(GL_POINTS, 0, 40000);
	glBindVertexArray(0);
}
// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,
			1.0f,
			0.0f,
			0.0f,
			1.0f,
			-1.0f,
			-1.0f,
			0.0f,
			0.0f,
			0.0f,
			1.0f,
			1.0f,
			0.0f,
			1.0f,
			1.0f,
			1.0f,
			-1.0f,
			0.0f,
			1.0f,
			0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

// lyy
unsigned int bookVAO = 0;
unsigned int bookVBO = 0;
void renderBook()
{
	// initialize (if necessary)
	if (bookVAO == 0)
	{
		float vertices[] = {
			// front face (face ID 1)
			-4.5f, -6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			4.5f, -6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,	 // bottom-right
			4.5f, 6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,	 // top-right
			4.5f, 6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,	 // top-right
			-4.5f, 6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,	 // top-left
			-4.5f, -6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			// back face (face ID 0)
			-4.5f, -6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
			4.5f, -6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,	 // bottom-right
			4.5f, 6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f,	 // top-right
			4.5f, 6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f,	 // top-right
			-4.5f, 6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	 // top-left
			-4.5f, -6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
			// top face (face ID 5)
			-4.5f, 6.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 5.0f,	// top-left
			4.5f, 6.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 5.0f,	// top-right
			4.5f, 6.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 5.0f,	// bottom-right
			4.5f, 6.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 5.0f,	// bottom-right
			-4.5f, 6.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 5.0f, // bottom-left
			-4.5f, 6.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 5.0f,	// top-left
			// bottom face (face ID 4)
			-4.5f, -6.0f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 4.0f, // bottom-left
			4.5f, -6.0f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 4.0f,  // bottom-right
			4.5f, -6.0f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 4.0f,	  // top-right
			4.5f, -6.0f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 4.0f,	  // top-right
			-4.5f, -6.0f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 4.0f,  // top-left
			-4.5f, -6.0f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 4.0f, // bottom-left
			// left face (face ID 2)
			-4.5f, 6.0f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f,	  // top-right
			-4.5f, 6.0f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 2.0f,  // top-left
			-4.5f, -6.0f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, // bottom-left
			-4.5f, -6.0f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, // bottom-left
			-4.5f, -6.0f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f,  // bottom-right
			-4.5f, 6.0f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f,	  // top-right
			// right face (face ID 3)
			4.5f, 6.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 3.0f,	// top-left
			4.5f, -6.0f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 3.0f, // bottom-right
			4.5f, 6.0f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 3.0f,	// top-right
			4.5f, -6.0f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 3.0f, // bottom-right
			4.5f, 6.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 3.0f,	// top-left
			4.5f, -6.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f	// bottom-left
		};
		glGenVertexArrays(1, &bookVAO);
		glGenBuffers(1, &bookVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, bookVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(bookVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(8 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render book
	glBindVertexArray(bookVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// 窗口大小变换监听
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	static bool ctrlPressed = false;						  // 用于跟踪 CONTROL 键的状态
	static bool isCtrlToggleOn = false;						  // 用于控制摄像头移动模式切换
	static float rotationAngle = 0.0f;						  // 初始旋转角度
	static float baseStarRotationSpeed = glm::radians(10.0f); // 基本旋转速度
	static float starRotationSpeed = baseStarRotationSpeed;	  // 当前旋转速度

	// 处理旋转逻辑
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		starRotationSpeed = glm::radians(20.0f);		// 加快旋转速度
		rotationAngle += starRotationSpeed * deltaTime; // 向左旋转
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		starRotationSpeed = glm::radians(20.0f);		// 加快旋转速度
		rotationAngle -= starRotationSpeed * deltaTime; // 向右旋转
	}
	else
	{
		starRotationSpeed = baseStarRotationSpeed;		// 恢复基本旋转速度
		rotationAngle += starRotationSpeed * deltaTime; // 基本速度旋转
	}

	// 更新光源位置
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	for (unsigned int i = 0; i < starLightPositions.size(); i++)
	{
		glm::vec4 newPos = rotationMatrix * glm::vec4(initialStarLightPositions[i], 1.0f);
		starLightPositions[i] = glm::vec3(newPos);
	}

	// 退出窗口
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && !ctrlPressed)
	{
		isCtrlToggleOn = !isCtrlToggleOn;		// 切换状态
		camera.gravityEnabled = isCtrlToggleOn; // 应用状态到摄像机
		ctrlPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
	{
		ctrlPressed = false;
	}

	// 检查是否按下 SHIFT 键
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		camera.MovementSpeed = 3.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		camera.MovementSpeed = 1.5f;
	}

	// 相机移动
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.Jump();
	}

	// 响应上下方向键
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(DOWN, deltaTime);
	}

	// 切换bloom
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !bloomKeyPressed)
	{
		bloom = !bloom;
		bloomKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
	{
		bloomKeyPressed = false;
	}

	// 曝光度
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (exposure > 0.0f)
		{
			exposure -= 0.01f;
		}
		else
			exposure = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		exposure += 0.01f;
	}

	// 将鼠标指针从窗口中释放
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	// 切换流星场景
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		std::cout << "Flow" << std::endl;
		isFlow = !isFlow; // 取反并赋值回去
	}
}

unsigned int loadTexture(char const *path, bool gammaCorrection)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum internalFormat;
		GLenum dataFormat;

		if (nrComponents == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrComponents == 3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_SRGB;
			else if (nrChannels == 4)
				format = GL_SRGB_ALPHA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			std::cout << "Cubemap texture loaded at path: " << faces[i] << " (Width: " << width << ", Height: " << height << ", Channels: " << nrChannels << ")" << std::endl;
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

static const int numVAOs = 1;
static const int numVBOs = 3;
GLuint vao[numVAOs] = {0};
GLuint vbo[numVBOs] = {0};
void renderCarpet(Shader &magicCarpetShader, GLuint &woodMap, glm::mat4 projection, glm::mat4 view, float currentTime)
{
	static GLuint VAO = 0, VBO = 0, EBO = 0;
	if (VAO == 0)
	{
		float magicCarpetVertices[] = {
			// positions        // texture Coords
			-6.25f, 0.0f, 2.5f, 0.0f, 0.0f, // 左上
			6.25f, 0.0f, 2.5f, 1.0f, 0.0f,	// 右上
			6.25f, 0.0f, -2.5f, 1.0f, 1.0f, // 右下
			-6.25f, 0.0f, -2.5f, 0.0f, 1.0f // 左下
		};

		unsigned int indices[] = {
			0, 1, 2, // 第一个三角形
			0, 2, 3	 // 第二个三角形
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(magicCarpetVertices), magicCarpetVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// 启用混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	magicCarpetShader.use();
	magicCarpetShader.setMat4("projection", projection);
	magicCarpetShader.setMat4("view", view);
	magicCarpetShader.setFloat("time", currentTime);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f)); // 根据指定位置调整
	model = glm::scale(model, glm::vec3(7.0f, 0.5f, 4.0f));		 // 确保魔毯尺寸正确
	magicCarpetShader.setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodMap);
	magicCarpetShader.setInt("texture1", 0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// 禁用混合
	glDisable(GL_BLEND);
}

unsigned int sphereVAO = 0;
unsigned int indexCount;

void renderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow)
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		std::vector<float> data;
		for (unsigned int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (!uv.empty())
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (!normals.empty())
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void *)(5 * sizeof(float)));
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}
