#version 400

uniform sampler2D texID;
varying vec4 outColor;
varying vec2 TexCoord;
layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = texture(texID, TexCoord) * outColor;
}
