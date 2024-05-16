#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SOIL2/soil2.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include <string>
#include <stack>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <array>
#include <random>

#define DEG2RAD(theta) ((theta * 2.0 * 3.14159) / 360.0)

class Utils {
private:
    static std::string readShaderFile(const char *filePath);
    static void printShaderLog(GLuint shader);
    static void printProgramLog(int prog);
    static GLuint prepareShader(int shaderTYPE, const char *shaderPath);
    static int finalizeShaderProgram(GLuint sprogram);

public:
    Utils();
    static void displayComputeShaderLimits();
    static bool checkOpenGLError();
    static GLuint createShaderProgram(const char *cp);
    static GLuint createShaderProgram(const char *vp, const char *fp);
    static GLuint createShaderProgram(const char *vp, const char *gp, const char *fp);
    static GLuint createShaderProgram(const char *vp, const char *tCS, const char* tES, const char *fp);
    static GLuint createShaderProgram(const char *vp, const char *tCS, const char* tES, char *gp, const char *fp);
    static GLuint loadTexture(const char *texImagePath);
    static GLuint loadCubeMap(const char *mapDir);

    static float* goldAmbient();
    static float* goldDiffuse();
    static float* goldSpecular();
    static float goldShininess();

    static float* silverAmbient();
    static float* silverDiffuse();
    static float* silverSpecular();
    static float silverShininess();

    static float* bronzeAmbient();
    static float* bronzeDiffuse();
    static float* bronzeSpecular();
    static float bronzeShininess();
};

