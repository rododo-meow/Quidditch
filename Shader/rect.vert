in vec2 TexCoord;
varying vec2 texCoord;

void main() {
    texCoord = TexCoord;
    gl_Position = gl_Vertex;
}
