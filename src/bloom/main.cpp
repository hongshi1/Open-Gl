#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>

#include <tool/shader.h>
#include <tool/camera.h>
#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>
#include <tool/gui.h>
#include <tool/model.h>

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
//模型加载
#include "Model.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(char const *path, bool gammaCorrection);
unsigned int loadCubemap(vector<std::string> faces);
void renderCarpet(Shader& magicCarpetShader, GLuint& woodMap, glm::mat4 projection, glm::mat4 view, float currentTime) ;

void renderQuad();
void renderCube();
void renderBook();
void setupVertices();
//绘制形状
void renderSphereByPointCloud();
void renderBunnyByPointCloud();
void renderFlowerByPointCloud(float);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

//初始化obj
// ImportedModel hyperCar("./static/model/hyperCar/lamborghini-aventador-pbribl.obj");
// 创建 SphereRenderer 对象
SphereRenderer sphereRenderer;
DiamondRenderer diamondRenderer;
// 创建圆锥体渲染器
// ConeRenderer cone(36, 0.5f, 1.0f);
// 创建平面花朵渲染器
FlowerRenderer flower(36, 0.5f, 1.0f);
// 创建正十二面体渲染器
IcosahedronRenderer icosahedronRenderer; 

float deltaTime = 0.0f; // 当前帧与上一帧之间的时间差
float lastTime = 0.0f;	// 上一帧的时间

// 在渲染循环中更新球体的旋转
float angle = 0.0f; // 初始旋转角度
float rotationSpeed = 50.0f; // 旋转速度

float lastX = SCR_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// bloom 设置
bool bloomKeyPressed = false;
bool bloom = true;
float exposure = 1.0;
void renderWater(Shader &waterShader, unsigned int normalTexture, unsigned int noiseTexture, unsigned int cubeMapTexture, float deltaTime, glm::vec3 lightPos, glm::vec3 viewPos) ;

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
	//glEnable(GL_FRAMEBUFFER_SRGB);

	// 编译shader
	// ----------
	Shader shader("./src/bloom/shader/bloom_vert.glsl", "./src/bloom/shader/bloom_frag.glsl");
	Shader shaderLight("./src/bloom/shader/bloom_light_vert.glsl", "./src/bloom/shader/bloom_light_frag.glsl");
	Shader shaderBlur("./src/bloom/shader/bloom_blur_vert.glsl", "./src/bloom/shader/bloom_blur_frag.glsl");
	Shader shaderFinal("./src/bloom/shader/bloom_final_vert.glsl", "./src/bloom/shader/bloom_final_frag.glsl");
	Shader skyboxShader("./src/bloom/shader/skybox_vert.glsl", "./src/bloom/shader/skybox_frag.glsl");
	Shader waterShader("./src/bloom/shader/water_vert.glsl", "./src/bloom/shader/water_frag.glsl");
	Shader magicCarpetShader("./src/bloom/shader/magic_carpet_vert.glsl", "./src/bloom/shader/magic_carpet_frag.glsl");
	//针对点云的shader
	Shader pointCloudShader("./src/bloom/shader/model_sphere_vert.glsl", "./src/bloom/shader/model_sphere_frag.glsl");

	Shader bookShader("./src/bloom/shader/book_vert.glsl", "./src/bloom/shader/book_frag.glsl");

	//lyy
	Model skull("./static/model/skull/skull.obj");
	Model wolf("./static/model/wolf/Wolf_One_obj.obj");
	Model planet("./static/model/planet/planet.obj");
	Model duck("./static/model/duck/duck.obj");
	Model pedestal("./static/model/3D_scifi_pedestal/tech_pedestal.obj");	Shader modelShader(" vertex_shader.glsl ", " fragment_shader.glsl");

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
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f,		// top-right
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f,		// top-right
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
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,		// top-left
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	// top-right
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,		// top-left
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
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,		// bottom-right
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,	// top-right
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,		// bottom-right
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f		// bottom-left
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

			// 绘制星星形状
	float scale = 0.1f; // 缩放因子
    float spacing = 0.3f; // 间隔

    glm::vec3 cubeGroupVertices[] = {
        glm::vec3(-spacing, -spacing, -spacing),
        glm::vec3(spacing, -spacing, -spacing),
        glm::vec3(-spacing, spacing, -spacing),
        glm::vec3(spacing, spacing, -spacing),
        glm::vec3(-spacing, -spacing, spacing),
        glm::vec3(spacing, -spacing, spacing),
        glm::vec3(-spacing, spacing, spacing),
        glm::vec3(spacing, spacing, spacing)
    };
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
    glm::vec3(-spacing * 1.1, 0.0f, 0.0f),  // 左侧中心
    glm::vec3(spacing * 1.1, 0.0f, 0.0f),   // 右侧中心
    glm::vec3(0.0f, -spacing * 1.1, 0.0f),  // 下面中心
    glm::vec3(0.0f, spacing * 1.1, 0.0f),   // 上面中心
    // 棱心
    glm::vec3(-spacing * 1.1, 0.0f, -spacing * 1.1), // 前左棱心
    glm::vec3(-spacing * 1.1, 0.0f, spacing * 1.1),  // 后左棱心
    glm::vec3(spacing * 1.1, 0.0f, -spacing * 1.1),  // 前右棱心
    glm::vec3(spacing * 1.1, 0.0f, spacing * 1.1),   // 后右棱心
    glm::vec3(0.0f, -spacing * 1.1, -spacing * 1.1), // 前下棱心
    glm::vec3(0.0f, -spacing * 1.1, spacing * 1.1),  // 后下棱心
    glm::vec3(0.0f, spacing * 1.1, -spacing * 1.1),  // 前上棱心
    glm::vec3(0.0f, spacing * 1.1, spacing * 1.1),   // 后上棱心
    glm::vec3(-spacing * 1.1, -spacing * 1.1, 0.0f), // 左下棱心
    glm::vec3(-spacing * 1.1, spacing * 1.1, 0.0f),  // 左上棱心
    glm::vec3(spacing * 1.1, -spacing * 1.1, 0.0f),  // 右下棱心
    glm::vec3(spacing * 1.1, spacing * 1.1, 0.0f)    // 右上棱心
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

	//加载钻石形状的纹理
	// unsigned int diamondTexture = loadTexture("./static/texture/subskybox/eight.png", false);
	//加载球体的纹理
	unsigned int sphereTexture = loadTexture("./static/texture/subskybox/sphere8.jpg", false);
	//正八面体的纹理
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
	vector<std::string> faces
		{
			"./static/texture/skyboxq/ny.png",
			"./static/texture/skyboxq/nx.png",
			"./static/texture/skyboxq/nz.png",
			"./static/texture/skyboxq/px.png",
			"./static/texture/skyboxq/py.png",
			"./static/texture/skyboxq/pz.png",};
	unsigned int cubemapTexture = loadCubemap(faces);


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
	unsigned int woodMap = loadTexture("./static/images/b.jpg", false);
	unsigned int containerMap = loadTexture("./static/texture/container2.png", true);
	//lyy
	//book
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
	lightPositions.push_back(glm::vec3(6.0f, 0.0f, 0.0));
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

	//lyy
	bookShader.use();
	bookShader.setInt("texture1", 0);
	bookShader.setInt("normalMap1", 1);
	bookShader.setInt("texture2", 2);
	bookShader.setInt("normalMap2", 3);
	bookShader.setInt("texture3", 4);
	bookShader.setInt("normalMap3", 5);
	bookShader.setInt("texture4", 6);

	// 渲染循环
	// --------
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
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
		glm::vec3 lightPos(1.2f, 2.0f, 1.0f); 
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
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		renderCube();

		// model = glm::mat4(1.0f);
		// model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		// model = glm::scale(model, glm::vec3(0.5f));
		// shader.setMat4("model", model);
		// renderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shader.setMat4("model", model);
		renderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
		model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(1.25));
		shader.setMat4("model", model);
		renderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
		model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shader.setMat4("model", model);
		renderCube();

		// model = glm::mat4(1.0f);
		// model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
		// model = glm::scale(model, glm::vec3(0.5f));
		// shader.setMat4("model", model);
		// renderCube();

		//book
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
		model = glm::translate(model, glm::vec3(0.5f, 0.0f, 3.0f));  // 设置书本位置
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

		bookShader.use();
		bookShader.setMat4("model", model);
		bookShader.setMat4("view", view);
		bookShader.setMat4("projection", projection);
		renderBook();

		shader.use();
		//skull
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -0.5f, -0.5f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));	// it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		skull.Draw(shader);

		//planet
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 3.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
		planet.Draw(shader);

		//wolf
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -0.5f, 2.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setMat4("model", model);
		wolf.Draw(shader);

		//duck
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, -0.5f, 4.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		shader.setMat4("model", model);
		duck.Draw(shader);

		//pedestal
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, 2.5f, 2.0f)); 
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		shader.setMat4("model", model);
		pedestal.Draw(shader);


		// 设置透明度
		float transparency = 0.8; // 设置透明度为50%		
		// 使用着色器程序
		pointCloudShader.use();
		// 设置uniform变量
		pointCloudShader.setVec3("lightPos", lightPositions[1]); // 设置光源位置
		pointCloudShader.setVec3("viewPos", camera.Position); // 设置视点位置
		pointCloudShader.setVec3("lightColor", lightColors[1]); // 设置光源颜色
		pointCloudShader.setVec3("rimLightColor", glm::vec3(0.2, 0.6, 1.0)); // 设置泛光颜色
		pointCloudShader.setFloat("rimIntensity", 0.7); // 设置泛光强度
		pointCloudShader.setFloat("transparency", transparency); // 设置透明度
		//绑定纹理
		glBindTexture(GL_TEXTURE_2D, sphereTexture);
		
		// 设置纹理参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		pointCloudShader.setMat4("projection", projection);
		pointCloudShader.setMat4("view", view);
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(1.0f));
		pointCloudShader.setMat4("model", model);

		// 启用混合功能
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// 渲染球体
    	sphereRenderer.renderSphere();
		
		// 关闭混合功能
		glDisable(GL_BLEND);

		// 根据点云绘制球体		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0));
		// 绕 y 轴旋转
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(1.0f));
		pointCloudShader.setMat4("model", model);
		renderSphereByPointCloud();
		// 绘制花朵点云
		// icosahedronRenderer.renderIcosahedron();
			
		// 更新旋转角度
    	angle += rotationSpeed * deltaTime; // deltaTime是上一帧到当前帧的时间间隔


	//绑定纹理
	glBindTexture(GL_TEXTURE_2D, diamondTexture);
    // 绘制内部八个正八面体
for (int i = 0; i < 8; ++i) {

    model = glm::mat4(1.0f);
    position = cubeGroupVertices[i] * spacing;

    model = glm::translate(model, position); // 设置位置
    model = glm::scale(model, glm::vec3(scale)); // 设置缩放
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f)); 
    pointCloudShader.setMat4("model", model); // 设置模型矩阵
    diamondRenderer.renderDiamond();// 绘制正八面体

    // 绘制外部大立方体
    for (int j = 0; j < 26; ++j) {
       	model = glm::mat4(1.0f);
    	position = largeCubeVertices[j];

        model = glm::translate(model, position); // 设置位置
        model = glm::scale(model, glm::vec3(scale * 2)); // 设置缩放
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)); 

        pointCloudShader.setMat4("model", model); // 设置模型矩阵
        diamondRenderer.renderDiamond();// 绘制正八面体 
    }
}

		//hyperCar
		// setupVertices();

		// 最后将所有光源显示为明亮的立方体
		shaderLight.use();
		shaderLight.setMat4("projection", projection);
		shaderLight.setMat4("view", view);
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{	
			if(i == lightPositions.size()-1){
				model = glm::mat4(1.0f);
    			model = glm::translate(model, glm::vec3(lightPositions[i]));
    			model = glm::scale(model, glm::vec3(0.25f));
    			shaderLight.setMat4("model", model);
    			shaderLight.setVec3("lightColor", lightColors[i]); // 设置光源颜色
    			//sphereRenderer.renderSphere();
				// 绘制圆锥体
        		flower.render();
			}else{
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

		//cout << "bloom: " << (bloom ? "on" : "off") << " | exposure: " << exposure << endl;

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
				1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,		// top-right
				1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,	// bottom-right
				1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,		// top-right
				-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,	// top-left
				// front face
				-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
				1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,	// bottom-right
				1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,		// top-right
				1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,		// top-right
				-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,	// top-left
				-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-right
				-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top-left
				-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
				-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-right
				// right face
				1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-left
				1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
				1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top-right
				1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
				1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-left
				1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// bottom-left
				// bottom face
				-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
				1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,	// top-left
				1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,		// bottom-left
				1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,		// bottom-left
				-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
				-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
				// top face
				-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
				1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,		// bottom-right
				1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,	// top-right
				1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,		// bottom-right
				-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
				-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f		// bottom-left
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
void renderWater(Shader &waterShader, unsigned int normalTexture, unsigned int noiseTexture, unsigned int cubeMapTexture, float deltaTime, glm::vec3 lightPos, glm::vec3 viewPos) {
    if (waterVAO == 0) {
        float waterVertices[] = {
            -10.0f, 0.0f, -10.0f,  0.0f, 0.0f,
            10.0f, 0.0f, -10.0f,  1.0f, 0.0f,
            -10.0f, 0.0f,  10.0f,  0.0f, 1.0f,
            10.0f, 0.0f, -10.0f,  1.0f, 0.0f,
            10.0f, 0.0f,  10.0f,  1.0f, 1.0f,
            -10.0f, 0.0f,  10.0f,  0.0f, 1.0f
        };

        glGenVertexArrays(1, &waterVAO);
        glGenBuffers(1, &waterVBO);
        glBindVertexArray(waterVAO);
        glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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

void renderFlowerByPointCloud(float openness)
{
    static bool initialized = false;
    static unsigned int lilyVAO, lilyVBO;
    const int numPetals = 6; // 百合花瓣数量
    const int pointsPerPetal = 50; // 每个花瓣的点数

    if (!initialized)
    {
        // 生成百合花点云
        std::vector<glm::vec3> points;
        for (int i = 0; i < numPetals; ++i)
        {
            float angle = i * (2.0f * glm::pi<float>()) / numPetals;
            for (int j = 0; j <= pointsPerPetal; ++j)
            {
                float phi = (static_cast<float>(j) / pointsPerPetal) * glm::pi<float>();
                float r = 1.0f + openness * sin(2 * phi); // 开合效果，调整半径
                float x = r * cos(angle) * sin(phi);
                float y = r * sin(angle) * sin(phi);
                float z = r * cos(phi);

                // 将花瓣旋转使花心朝上
                float theta = glm::pi<float>() / 2.0f;
                float newX = x * cos(theta) - z * sin(theta);
                float newZ = x * sin(theta) + z * cos(theta);

                points.push_back(glm::vec3(newX, y, newZ));
            }
        }

        // 生成并绑定缓冲区对象
        glGenVertexArrays(1, &lilyVAO);
        glGenBuffers(1, &lilyVBO);

        // 设置点的大小
        glPointSize(5.0f); // 调整点的大小

        // 绑定顶点数组对象
        glBindVertexArray(lilyVAO);

        // 绑定并填充顶点缓冲区
        glBindBuffer(GL_ARRAY_BUFFER, lilyVBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

        // 设置顶点属性指针
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0); // 位置
        glEnableVertexAttribArray(0);

        // 解绑顶点数组对象
        glBindVertexArray(0);

        initialized = true;
    }

    // 渲染百合花点云
    glBindVertexArray(lilyVAO);
    glDrawArrays(GL_POINTS, 0, numPetals * (pointsPerPetal + 1));
    glBindVertexArray(0);
}



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

		// Set point size
    	glPointSize(5.0f); // Adjust the size as needed

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

//lyy
unsigned int bookVAO = 0;
unsigned int bookVBO = 0;
void renderBook()
{
    // initialize (if necessary)
    if (bookVAO == 0)
    {
        float vertices[] = {
            // front face (face ID 1)
            -4.5f, -6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,    // bottom-left
            4.5f, -6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,     // bottom-right
            4.5f, 6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,      // top-right
            4.5f, 6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,      // top-right
            -4.5f, 6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,     // top-left
            -4.5f, -6.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,    // bottom-left
            // back face (face ID 0)
            -4.5f, -6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
            4.5f, -6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,   // bottom-right
            4.5f, 6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f,    // top-right
            4.5f, 6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f,    // top-right
            -4.5f, 6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // top-left
            -4.5f, -6.0f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
            // top face (face ID 5)
            -4.5f, 6.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 5.0f,    // top-left
            4.5f, 6.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 5.0f,      // top-right
            4.5f, 6.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 5.0f,     // bottom-right
            4.5f, 6.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 5.0f,     // bottom-right
            -4.5f, 6.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 5.0f,    // bottom-left
            -4.5f, 6.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 5.0f,      // top-left
            // bottom face (face ID 4)
            -4.5f, -6.0f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 4.0f,  // bottom-left
            4.5f, -6.0f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 4.0f,   // bottom-right
            4.5f, -6.0f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 4.0f,    // top-right
            4.5f, -6.0f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 4.0f,    // top-right
            -4.5f, -6.0f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 4.0f,   // top-left
            -4.5f, -6.0f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 4.0f,  // bottom-left
            // left face (face ID 2)
            -4.5f, 6.0f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f,    // top-right
            -4.5f, 6.0f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 2.0f,   // top-left
            -4.5f, -6.0f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f,  // bottom-left
            -4.5f, -6.0f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f,  // bottom-left
            -4.5f, -6.0f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f,   // bottom-right
            -4.5f, 6.0f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f,    // top-right
            // right face (face ID 3)
            4.5f, 6.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 3.0f,      // top-left
            4.5f, -6.0f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 3.0f,    // bottom-right
            4.5f, 6.0f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 3.0f,     // top-right
            4.5f, -6.0f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 3.0f,    // bottom-right
            4.5f, 6.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 3.0f,      // top-left
            4.5f, -6.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f      // bottom-left
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

// 输入监听
void processInput(GLFWwindow *window)
{
	 static bool ctrlPressed = false;  // 用于跟踪 CONTROL 键的状态
    static bool isCtrlToggleOn = false;  // 用于控制摄像头移动模式切换
	// 退出窗口
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}


	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && !ctrlPressed)
    {
        isCtrlToggleOn = !isCtrlToggleOn;  // 切换状态
        camera.isHorizontalMode = isCtrlToggleOn;  // 应用状态到摄像机
        ctrlPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
    {
        ctrlPressed = false;
    }

    // 检查是否按下 SHIFT 键
    
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.MovementSpeed = 3.0f;
    }
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
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
		
		if (exposure > 0.0f){
			exposure -= 0.01f;
		}
			
		else
			exposure = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		exposure += 0.01f;
	}

	// 将鼠标指针从窗口中施放
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
	// 加载模型
// void setupVertices(void)
// {
// 	vector<glm::vec3> vert = hyperCar.getVertices();
// 	vector<glm::vec2> text = hyperCar.getTextureCoords();
// 	vector<glm::vec3> norm = hyperCar.getNormals();

// 	vector<float> pValues;
// 	vector<float> tValues;
// 	vector<float> nValues;

// 	for (int i = 0; i < hyperCar.getNumVertices(); i++)
// 	{
// 		pValues.push_back(vert[i].x);
// 		pValues.push_back(vert[i].y);
// 		pValues.push_back(vert[i].z);

// 		tValues.push_back(text[i].s);
// 		tValues.push_back(text[i].t);

// 		nValues.push_back(norm[i].x);
// 		nValues.push_back(norm[i].y);
// 		nValues.push_back(norm[i].z);
// 	}

// 	glGenVertexArrays(numVAOs, vao);
// 	glBindVertexArray(vao[0]);

// 	glGenBuffers(numVBOs, vbo);
// 	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
// 	glBufferData(GL_ARRAY_BUFFER, pValues.size() * sizeof(float), &(pValues[0]), GL_STATIC_DRAW);

// 	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
// 	glBufferData(GL_ARRAY_BUFFER, tValues.size() * sizeof(float), &(tValues[0]), GL_STATIC_DRAW);

// 	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
// 	glBufferData(GL_ARRAY_BUFFER, nValues.size() * sizeof(float), &(nValues[0]), GL_STATIC_DRAW);
	
// }


void renderCarpet(Shader& magicCarpetShader, GLuint& woodMap, glm::mat4 projection, glm::mat4 view, float currentTime) {
    static GLuint VAO = 0, VBO = 0, EBO = 0;
    if (VAO == 0) {
        float magicCarpetVertices[] = {
            // positions        // texture Coords
            -6.25f, 0.0f, 2.5f,  0.0f, 0.0f,   // 左上
             6.25f, 0.0f, 2.5f,  1.0f, 0.0f,   // 右上
             6.25f, 0.0f, -2.5f, 1.0f, 1.0f,   // 右下
            -6.25f, 0.0f, -2.5f, 0.0f, 1.0f    // 左下
        };

        unsigned int indices[] = {
            0, 1, 2,   // 第一个三角形
            0, 2, 3    // 第二个三角形
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(magicCarpetVertices), magicCarpetVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0);
    }

    magicCarpetShader.use();
    magicCarpetShader.setMat4("projection", projection);
    magicCarpetShader.setMat4("view", view);
    magicCarpetShader.setFloat("time", currentTime);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f)); // 根据指定位置调整
    model = glm::scale(model, glm::vec3(7.0f, 0.5f, 4.0f));    // 确保魔毯尺寸正确
    magicCarpetShader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodMap);
    magicCarpetShader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

