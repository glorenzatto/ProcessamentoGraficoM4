#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma once
class Sprite
{
public:
	Sprite(GLuint texture, unsigned int textureWidth, unsigned int textureHeight, float zDepth, int shaderProgram, glm::mat4 projection, glm::mat4 transform);
	void Draw();
	void Translate(double x, double y);
	void SideScroll(double x);
	void SetProjection(glm::mat4 projection);
	float GetX();

private:
	GLuint VAO, VBO, Texture;
	glm::mat4 modelProjection, modelTransform;
	int Shader;

};

