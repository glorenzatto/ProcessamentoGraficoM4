#version 410
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vTexture;
uniform mat4 proj;
uniform mat4 matrix;
out vec2 text_map;
out vec3 color;
void main() {
    color = vColor;
    text_map = vTexture;
    gl_Position = proj * matrix * vec4(vPosition, 1.0);
}
