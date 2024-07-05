#version 410
in vec2 text_map;
in vec3 color;
uniform sampler2D basic_texture;
out vec4 frag_color;
void main() {
    vec4 textel = texture(basic_texture, text_map);
    if(textel.a < 0.01) { discard; }
    frag_color = textel;
}
