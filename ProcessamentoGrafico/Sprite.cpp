#include "Sprite.h"
#include <iostream>

// Construtor da classe Sprite
Sprite::Sprite(GLuint texture, unsigned int tWidth, unsigned int tHeight, float zDepth, int shaderProgram, glm::mat4 projection, glm::mat4 transform) {
    // Inicializa as vari�veis de proje��o, transforma��o, textura e shader
    modelProjection = projection;
    modelTransform = transform;
    Texture = texture;
    Shader = shaderProgram;

    // Define os v�rtices do sprite (um quadril�tero 2D)
    const int verticesSize = 48;
    GLfloat modelVertices[verticesSize] = {
        // Positions          // Colors           // texture map
           0.0f,     0.0f, zDepth,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f, // bottom left
         tWidth,     0.0f, zDepth,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f, // bottom right
         tWidth,  tHeight, zDepth,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
         tWidth,  tHeight, zDepth,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
           0.0f,  tHeight, zDepth,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, // top left
           0.0f,     0.0f, zDepth,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f  // bottom left
    };

    // Gera��o e configura��o dos buffers de v�rtices
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Habilita a mistura (blending) para transpar�ncia
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Envia os dados dos v�rtices para o buffer
    glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(GLfloat), modelVertices, GL_STATIC_DRAW);

    // Configura os ponteiros de atributos de v�rtice
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// Fun��o para desenhar o sprite
void Sprite::Draw() {
    glUseProgram(Shader);

    // Define as matrizes de proje��o e transforma��o nos shaders
    glUniformMatrix4fv(glGetUniformLocation(Shader, "proj"), 1, GL_FALSE, glm::value_ptr(modelProjection));
    glUniformMatrix4fv(glGetUniformLocation(Shader, "matrix"), 1, GL_FALSE, glm::value_ptr(modelTransform));

    glBindVertexArray(VAO);

    // Ativa e associa a textura
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glUniform1i(glGetUniformLocation(Shader, "basic_texture"), 0);

    // Desenha o sprite
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

// Fun��o para transladar o sprite
void Sprite::Translate(double x, double y) {
    modelTransform = glm::translate(glm::mat4(1), glm::vec3(x, y, 0));
}

// Fun��o para fazer a rolagem lateral do sprite
void Sprite::SideScroll(double x) {
    modelTransform = glm::translate(modelTransform, glm::vec3(x, 0, 0));
}

// Fun��o para definir a matriz de proje��o do sprite
void Sprite::SetProjection(glm::mat4 projection) {
    this->modelProjection = projection;
}

// Fun��o para obter a posi��o X atual do sprite
float Sprite::GetX() {
    return modelTransform[3][0];
}
