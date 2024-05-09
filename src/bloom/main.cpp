#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>

#include <tool/shader.h>
#include <tool/camera.h>
#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>
#include <tool/gui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 读取obj文件要用的
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(char const *path, bool gammaCorrection);
unsigned int loadCubemap(vector<std::string> faces);

void renderQuad();
void renderCube();
// 函数声明
void renderDiamond();


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

float deltaTime = 0.0f; // 当前帧与上一帧之间的时间差
float lastTime = 0.0f;	// 上一帧的时间

float lastX = SCR_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// bloom 设置
bool bloomKeyPressed = false;
bool bloom = true;
float exposure = 1.0;

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

	// 一个钻石形状💎
	float diamondVertices[] = {
    // Bottom pyramid
    // 顶点位置             纹理坐标
    0.0f, -0.5f, 0.0f,     0.5f, 1.0f, // Top
    -0.5f, -0.5f, 0.5f,    0.0f, 0.0f, // Bottom-left
    0.5f, -0.5f, 0.5f,    1.0f, 0.0f, // Bottom-right

    0.0f, -0.5f, 0.0f,     0.5f, 1.0f, // Top
    0.5f, -0.5f, 0.5f,   1.0f, 0.0f, // Bottom-right
    0.5f, -0.5f, -0.5f,    1.0f, 1.0f, // Bottom-back

    0.0f, -0.5f, 0.0f,     0.5f, 1.0f, // Top
    0.5f, -0.5f, -0.5f,   1.0f, 0.0f, // Bottom-back
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // Bottom-left

    0.0f, -0.5f, 0.0f,     0.5f, 1.0f, // Top
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // Bottom-left
    -0.5f, -0.5f, 0.5f,     0.0f, 1.0f, // Bottom-front

    // Top pyramid
    0.0f, 0.5f, 0.0f,      0.5f, 1.0f,  // Top
    -0.5f, 0.5f, 0.5f,   0.0f, 0.0f,  // Bottom-left
    0.5f, 0.5f, 0.5f,      1.0f, 0.0f,  // Bottom-right

    0.0f, 0.5f, 0.0f,     0.5f, 1.0f,  // Top
    0.5f, 0.5f, 0.5f,      1.0f, 0.0f,  // Bottom-right
    0.5f, 0.5f, -0.5f,     1.0f, 1.0f,  // Bottom-back

    0.0f, 0.5f, 0.0f,      0.5f, 1.0f,  // Top
    0.5f, 0.5f, -0.5f,     1.0f, 0.0f,  // Bottom-back
    -0.5f, 0.5f, -0.5f,    0.0f, 0.0f,  // Bottom-left

    0.0f, 0.5f, 0.0f,      0.5f, 1.0f,  // Top
    -0.5f, 0.5f, -0.5f,    0.0f, 0.0f,  // Bottom-left
    -0.5f, 0.5f, 0.5f,     0.0f, 1.0f   // Bottom-front
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

	//diamond VBO AND VAO
	unsigned int diamondVAO, diamondVBO;
	glGenVertexArrays(1, &diamondVAO);
	glGenBuffers(1, &diamondVBO);
	glBindVertexArray(diamondVAO);
	glBindBuffer(GL_ARRAY_BUFFER, diamondVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(diamondVertices), &diamondVertices, GL_STATIC_DRAW);

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
	//加载钻石形状的纹理
	unsigned int diamondTexture = loadTexture("./static/texture/subskybox/6.jpg", false);
	vector<std::string> faces
		{
			"./static/texture/skybox/right.jpg",
			"./static/texture/skybox/left.jpg",
			"./static/texture/skybox/top.jpg",
			"./static/texture/skybox/bottom.jpg",
			"./static/texture/skybox/front.jpg",
			"./static/texture/skybox/back.jpg"};
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
	unsigned int woodMap = loadTexture("./static/texture/awesomeface.png", true);
	unsigned int containerMap = loadTexture("./static/texture/container2.png", true);

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
	// colors
	vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
	lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
	lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));

	// shader 配置信息
	// ---------------
	shader.use();
	shader.setInt("diffuseTexture", 0);

	shaderBlur.use();
	shaderBlur.setInt("image", 0);

	shaderFinal.use();
	shaderFinal.setInt("scene", 0);
	shaderFinal.setInt("bloomBlur", 1);

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


		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodMap);

		// 设置灯光参数值
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			shader.setVec3("lights[" + to_string(i) + "].Position", lightPositions[i]);
			shader.setVec3("lights[" + to_string(i) + "].Color", lightColors[i]);
		}
		shader.setVec3("viewPos", camera.Position);
		// 创建一个大的立方体作为地板
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
		shader.setMat4("model", model);
		renderCube();

		// 创建多个立方体作为物体
		glBindTexture(GL_TEXTURE_2D, containerMap);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		renderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		renderCube();

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

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		renderCube();

		// 创建一个正八面体
		glBindTexture(GL_TEXTURE_2D, diamondTexture);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		renderDiamond();

		// 最后将所有光源显示为明亮的立方体
		shaderLight.use();
		shaderLight.setMat4("projection", projection);
		shaderLight.setMat4("view", view);
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(lightPositions[i]));
			model = glm::scale(model, glm::vec3(0.25f));
			shaderLight.setMat4("model", model);
			shaderLight.setVec3("lightColor", lightColors[i]);
			renderCube();
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
unsigned int diamondVAO = 0;
unsigned int diamondVBO = 0;
unsigned int diamondEBO = 0;
// 渲染钻石形状💎
// void renderDiamond()
// {
//     // Initialize (if necessary)
//     if (diamondVAO == 0)
//     {
//         float octahedronVertices[] = {
//             // Front face
//             -0.5f, 0.0f, 0.5f,   0.0f, 1.0f, 0.0f,   0.25f, 0.5f,
//             0.5f, 0.0f, 0.5f,    0.0f, 1.0f, 0.0f,   0.5f,  0.5f,
//             0.0f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   0.375f, 1.0f,
//             // Right face
//             0.5f, 0.0f, 0.5f,    1.0f, 0.0f, 0.0f,   0.5f,  0.5f,
//             0.5f, 0.0f, -0.5f,   1.0f, 0.0f, 0.0f,   0.75f, 0.5f,
//             0.0f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   0.625f, 1.0f,
//             // Back face
//             0.5f, 0.0f, -0.5f,   0.0f, 0.0f, -1.0f,  0.75f, 0.5f,
//             -0.5f, 0.0f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f,  0.5f,
//             0.0f, 0.5f, 0.0f,    0.0f, 0.0f, -1.0f,  0.875f, 1.0f,
//             // Left face
//             -0.5f, 0.0f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f,  0.5f,
//             -0.5f, 0.0f, 0.5f,   -1.0f, 0.0f, 0.0f,  0.25f, 0.5f,
//             0.0f, 0.5f, 0.0f,    -1.0f, 0.0f, 0.0f,  0.125f, 1.0f,

// 			// Front face
//            -0.5f, 0.0f, 0.5f,    0.0f, -1.0f, 0.0f,   0.25f, 0.5f,  // Bottom left corner
//            0.5f, 0.0f, 0.5f,     0.0f, -1.0f, 0.0f,   0.5f,  0.5f,  // Bottom right corner
//            0.0f, -0.5f, 0.0f,    0.0f, -1.0f, 0.0f,   0.375f, 0.0f, // Bottom point

//            // Right face
//     		0.5f, 0.0f, 0.5f,     -1.0f, 0.0f, 0.0f,   0.5f,  0.5f,  // Bottom right corner
//     		0.5f, 0.0f, -0.5f,    -1.0f, 0.0f, 0.0f,   0.75f, 0.5f,  // Bottom left corner
//     		0.0f, -0.5f, 0.0f,    -1.0f, 0.0f, 0.0f,   0.625f, 0.0f, // Bottom point

//    		 // Back face
//    			 0.5f, 0.0f, -0.5f,    0.0f, -1.0f, 0.0f,   0.75f, 0.5f,  // Bottom left corner
//     		-0.5f, 0.0f, -0.5f,   0.0f, -1.0f, 0.0f,   1.0f,  0.5f,  // Bottom right corner
//     		0.0f, -0.5f, 0.0f,    0.0f, -1.0f, 0.0f,   0.875f, 0.0f, // Bottom point

//     		// Left face
//    			 -0.5f, 0.0f, -0.5f,   1.0f, 0.0f, 0.0f,    0.0f,  0.5f,  // Bottom left corner
//     		-0.5f, 0.0f, 0.5f,    1.0f, 0.0f, 0.0f,    0.25f, 0.5f,  // Bottom right corner
//     		0.0f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.125f, 0.0f, // Bottom point
//         };

//         glGenVertexArrays(1, &diamondVAO);
//         glGenBuffers(1, &diamondVBO);
//         // Fill buffer
//         glBindBuffer(GL_ARRAY_BUFFER, diamondVBO);
//         glBufferData(GL_ARRAY_BUFFER, sizeof(octahedronVertices), octahedronVertices, GL_STATIC_DRAW);
//         // Link vertex attributes
//         glBindVertexArray(diamondVAO);
//         glEnableVertexAttribArray(0);
//         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0); // Position
//         glEnableVertexAttribArray(1);
//         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float))); // Normal
//         glEnableVertexAttribArray(2);
//         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float))); // Texture coordinates
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//         glBindVertexArray(0);
//     }
//     // Render Octahedron
//     glBindVertexArray(diamondVAO);
//     glDrawArrays(GL_TRIANGLES, 0, 24); // 6 faces, 3 vertices per face
//     glBindVertexArray(0);
// }

void renderDiamond()
{
    // Initialize (if necessary)
    if (diamondVAO == 0)
    {
        // Define vertices with shared positions
        float octahedronVertices[] = {
            // Positions            // Normals            // Texture coords
            // Front face
            -0.5f, 0.0f, 0.5f,     0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // 0
            0.5f, 0.0f, 0.5f,      0.0f, 1.0f, 0.0f,    1.0f, 0.0f,  // 1
            0.0f, 0.5f, 0.0f,      0.0f, 1.0f, 0.0f,    0.5f, 1.0f,  // 2

            // Right face
            0.5f, 0.0f, 0.5f,      1.0f, 0.0f, 0.0f,    0.0f, 0.0f,  // 3
            0.5f, 0.0f, -0.5f,     1.0f, 0.0f, 0.0f,    1.0f, 0.0f,  // 4
            0.0f, 0.5f, 0.0f,      1.0f, 0.0f, 0.0f,    0.5f, 1.0f,  // 5

            // Back face
            0.5f, 0.0f, -0.5f,     0.0f, 0.0f, -1.0f,   0.0f, 0.0f,  // 6
            -0.5f, 0.0f, -0.5f,    0.0f, 0.0f, -1.0f,   1.0f, 0.0f,  // 7
            0.0f, 0.5f, 0.0f,      0.0f, 0.0f, -1.0f,   0.5f, 1.0f,  // 8

            // Left face
            -0.5f, 0.0f, -0.5f,    -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // 9
            -0.5f, 0.0f, 0.5f,     -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // 10
            0.0f, 0.5f, 0.0f,      -1.0f, 0.0f, 0.0f,   0.5f, 1.0f,  // 11

            // Bottom face
            -0.5f, 0.0f, 0.5f,     0.0f, -1.0f, 0.0f,   0.0f, 0.0f,  // 12
            0.5f, 0.0f, 0.5f,      0.0f, -1.0f, 0.0f,   1.0f, 0.0f,  // 13
            0.0f, -0.5f, 0.0f,     0.0f, -1.0f, 0.0f,   0.5f, 1.0f,  // 14

            // Right face
            0.5f, 0.0f, 0.5f,      -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // 15
            0.5f, 0.0f, -0.5f,     -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // 16
            0.0f, -0.5f, 0.0f,     -1.0f, 0.0f, 0.0f,   0.5f, 1.0f,  // 17

            // Back face
            0.5f, 0.0f, -0.5f,     0.0f, -1.0f, 0.0f,   0.0f, 0.0f,  // 18
            -0.5f, 0.0f, -0.5f,    0.0f, -1.0f, 0.0f,   1.0f, 0.0f,  // 19
            0.0f, -0.5f, 0.0f,     0.0f, -1.0f, 0.0f,   0.5f, 1.0f,  // 20

            // Left face
            -0.5f, 0.0f, -0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,  // 21
            -0.5f, 0.0f, 0.5f,     1.0f, 0.0f, 0.0f,    1.0f, 0.0f,  // 22
            0.0f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,    0.5f, 1.0f   // 23
        };

        unsigned int indices[] = {
            0, 1, 2,    // Front face
            3, 4, 5,    // Right face
            6, 7, 8,    // Back face
            9, 10, 11,  // Left face
            12, 13, 14, // Bottom face
            15, 16, 17, // Right face
            18, 19, 20, // Back face
            21, 22, 23  // Left face
        };

        // Generate buffers
        glGenVertexArrays(1, &diamondVAO);
        glGenBuffers(1, &diamondVBO);
        glGenBuffers(1, &diamondEBO);

        // Bind VAO
        glBindVertexArray(diamondVAO);

        // Bind and fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, diamondVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(octahedronVertices), octahedronVertices, GL_STATIC_DRAW);

        // Bind and fill element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, diamondEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coords
        glEnableVertexAttribArray(2);

        // Unbind VAO
        glBindVertexArray(0);
    }

    // Render Octahedron
    glBindVertexArray(diamondVAO);
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0); // 8 faces, 3 vertices per face
    glBindVertexArray(0);
}
unsigned int starVAO = 0;
unsigned int starVBO = 0;
unsigned int starEBO = 0;
void renderStar()
{
    // Initialize (if necessary)
    if (starVAO == 0)
    {
        // Define vertices for star
        float starVertices[] = {
            // Positions             // Normals            // Texture coords
            // Bottom pyramid
            0.0f, -0.5f, 0.0f,       0.0f, -1.0f, 0.0f,   0.5f, 0.0f, // 0
            -0.45f, -0.3f, 0.0f,     0.0f, -1.0f, 0.0f,   0.0f, 0.5f, // 1
            0.45f, -0.3f, 0.0f,      0.0f, -1.0f, 0.0f,   1.0f, 0.5f, // 2
            -0.3f, 0.45f, 0.0f,      0.0f, -1.0f, 0.0f,   0.25f, 1.0f, // 3
            0.3f, 0.45f, 0.0f,       0.0f, -1.0f, 0.0f,   0.75f, 1.0f, // 4
            // Top pyramid
            0.0f, 0.5f, 0.25f,       0.0f, 1.0f, 0.0f,    0.5f, 0.0f, // 5
            -0.45f, 0.3f, 0.25f,     0.0f, 1.0f, 0.0f,    0.0f, 0.5f, // 6
            0.45f, 0.3f, 0.25f,      0.0f, 1.0f, 0.0f,    1.0f, 0.5f, // 7
            -0.3f, -0.45f, 0.25f,    0.0f, 1.0f, 0.0f,    0.25f, 1.0f, // 8
            0.3f, -0.45f, 0.25f,     0.0f, 1.0f, 0.0f,    0.75f, 1.0f, // 9
        };

        unsigned int starIndices[] = {
            // Bottom pyramid
            0, 1, 2,    // Triangle 1
            0, 2, 3,    // Triangle 2
            0, 3, 4,    // Triangle 3
            // Top pyramid
            5, 6, 7,    // Triangle 4
            5, 8, 9,    // Triangle 5
            // Connecting sides
            1, 6, 2,    // Triangle 6
            2, 7, 3,    // Triangle 7
            3, 8, 4,    // Triangle 8
            4, 9, 0,    // Triangle 9
            0, 5, 1     // Triangle 10
        };

        // Generate buffers
        glGenVertexArrays(1, &starVAO);
        glGenBuffers(1, &starVBO);
        glGenBuffers(1, &starEBO);

        // Bind VAO
        glBindVertexArray(starVAO);

        // Bind and fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, starVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(starVertices), starVertices, GL_STATIC_DRAW);

        // Bind and fill element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, starEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(starIndices), starIndices, GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coords
        glEnableVertexAttribArray(2);

        // Unbind VAO
        glBindVertexArray(0);
    }

    // Render Star
    glBindVertexArray(starVAO);
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0); // 10 triangles, 3 vertices per triangle
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
		if (exposure > 0.0f)
			exposure -= 0.001f;
		else
			exposure = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		exposure += 0.001f;
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

	//stbi_set_flip_vertically_on_load(true);
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
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
									 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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