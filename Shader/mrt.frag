#version 400

uniform sampler2D texID;
varying vec4 ambientColor;
varying vec4 fullLightColor;
varying vec4 spotColor;
varying vec2 texCoord;
layout(location = 0) out vec4 outFullColor;
layout(location = 1) out vec4 outAmbientColor;
layout(location = 2) out vec4 outSpotColor;

void main() {
    vec4 color = texture(texID, texCoord);
    outAmbientColor = ambientColor * color;
    outFullColor = fullLightColor * color;
    outSpotColor = spotColor * color;
}
