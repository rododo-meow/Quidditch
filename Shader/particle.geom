#version 400
layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform float time;
varying vec4 color[];
varying vec2 outSize[];
varying vec2 outLife[];
out vec2 TexCoord;
out vec4 outColor;

void main() {
  if (time > outLife[0][1] || time < outLife[0][0])
    return;
  gl_Position = vec4(gl_in[0].gl_Position[0] + outSize[0][0] / 2, gl_in[0].gl_Position[1] - outSize[0][1] / 2, 0, 1);
  TexCoord = vec2(1, 0);
  outColor = color[0];
  EmitVertex();
  gl_Position = vec4(gl_in[0].gl_Position[0] + outSize[0][0] / 2, gl_in[0].gl_Position[1] + outSize[0][1] / 2, 0, 1);
  TexCoord = vec2(1, 1);
  outColor = color[0];
  EmitVertex();
  gl_Position = vec4(gl_in[0].gl_Position[0] - outSize[0][0] / 2, gl_in[0].gl_Position[1] - outSize[0][1] / 2, 0, 1);
  TexCoord = vec2(0, 0);
  outColor = color[0];
  EmitVertex();
  gl_Position = vec4(gl_in[0].gl_Position[0] - outSize[0][0] / 2, gl_in[0].gl_Position[1] + outSize[0][1] / 2, 0, 1);
  TexCoord = vec2(0, 1);
  outColor = color[0];
  EmitVertex();
}
