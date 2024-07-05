#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>

// Bibliotecas OpenGL e GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Biblioteca para carregar texturas
#include "stb_image.h"

// Classe Sprite para gerenciamento de sprites
#include "Sprite.h"

// Definição das dimensões da janela
const GLint WIDTH = 798, HEIGHT = 420;

// Função para carregar texturas
static bool LoadTexture(const char* fileName, GLuint* texture, int* width, int* height) {
    int components;
    int force_channels = 4;
    glEnable(GL_TEXTURE_2D);

    unsigned char* image_data = stbi_load(fileName, width, height, &components, force_channels);

    if (!image_data) {
        std::cerr << "ERROR: could not load " << fileName << std::endl;
        return false;
    }

    glGenTextures(1, texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    GLfloat max_aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);

    stbi_image_free(image_data); // Libera a memória da imagem carregada
    return true;
}

// Função para criar um sprite
Sprite* CreateSprite(const char* textureFileName, const int shaderProgram, const float zDepth) {
    GLuint modelTexture;
    int width, height;

    if (!LoadTexture(textureFileName, &modelTexture, &width, &height)) {
        std::cerr << "Failed to load texture: " << textureFileName << std::endl;
        return nullptr;
    }

    glm::mat4 modelProjection = glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);
    glm::mat4 modelTransform = glm::mat4(1);

    return new Sprite(modelTexture, width, height, zDepth, shaderProgram, modelProjection, modelTransform);
}

// Função para carregar o conteúdo dos arquivos de shader
std::string LoadShaderSource(const char* filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open " << filePath << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
}

// Função para compilar shaders
GLuint CompileShader(const char* shaderCode, GLenum shaderType) {
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shaderId;
}

int main() {
    // Inicialização do GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return EXIT_FAILURE;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Criação da janela
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Gustavo Lorenzatto Cauduro - M4 P.G.", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        return EXIT_FAILURE;
    }

    // Carregar os códigos dos shaders a partir dos arquivos
    std::string vertexShaderSourceCode = LoadShaderSource("Shaders/vertex_shader.glsl");
    std::string fragmentShaderSourceCode = LoadShaderSource("Shaders/fragment_shader.glsl");

    if (vertexShaderSourceCode.empty() || fragmentShaderSourceCode.empty()) {
        std::cerr << "Shader source code is empty." << std::endl;
        return EXIT_FAILURE;
    }

    const char* vertexShaderCode = vertexShaderSourceCode.c_str();
    const char* fragmentShaderCode = fragmentShaderSourceCode.c_str();

    // Compilação dos shaders
    GLuint vertexShaderId = CompileShader(vertexShaderCode, GL_VERTEX_SHADER);
    GLuint fragmentShaderId = CompileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);

    // Criação e linkagem do programa de shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShaderId);
    glAttachShader(shaderProgram, fragmentShaderId);
    glLinkProgram(shaderProgram);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return EXIT_FAILURE;
    }

    // Criação do background
    Sprite* background = CreateSprite("Textures/estadio_futebol.png", shaderProgram, 0.50f);
    if (!background) return EXIT_FAILURE;
    background->Translate(0, 0);

    // Criação dos sprites
    Sprite* cr7 = CreateSprite("Textures/cr7_siuuu.png", shaderProgram, 0.51f);
    if (!cr7) return EXIT_FAILURE;
    cr7->Translate(255, 150);

    Sprite* neymar = CreateSprite("Textures/neymar.png", shaderProgram, 0.52f);
    if (!neymar) return EXIT_FAILURE;
    neymar->Translate(500, 300);

    Sprite* geromel = CreateSprite("Textures/geromel.png", shaderProgram, 0.53f);
    if (!geromel) return EXIT_FAILURE;
    geromel->Translate(100, 150);

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int screenWidth, screenHeight;
        glfwGetWindowSize(window, &screenWidth, &screenHeight);
        glViewport(0, 0, screenWidth, screenHeight);

        // Desenho das camadas
        background->Draw();
        cr7->Draw();
        neymar->Draw();
        geromel->Draw();

        glfwSwapBuffers(window);
    }

    // Limpeza
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return EXIT_SUCCESS;
}
