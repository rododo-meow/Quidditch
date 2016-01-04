#version 400

uniform sampler2D texID;
in vec2 texCoord;
layout(location = 0) out vec4 color;

void main() {
    color = texture(texID, texCoord);
}
