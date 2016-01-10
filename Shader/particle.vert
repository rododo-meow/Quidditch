uniform float time;
layout(location = 1) in vec4 size;
layout(location = 2) in vec4 beginColor;
layout(location = 3) in vec4 endColor;
layout(location = 4) in vec2 v;
layout(location = 5) in vec2 life;
varying vec4 color;
varying vec2 outSize;
varying vec2 outLife;

void main() {
  color = beginColor + (endColor - beginColor) * (time - life[0]) / (life[1] - life[0]);
  gl_Position = ftransform();
  gl_Position[0] += v[0] * (time - life[0]);
  gl_Position[1] += v[1] * (time - life[0]);
  outLife = life;
  outSize = vec2(size[0], size[1]) + (vec2(size[2], size[3]) - vec2(size[0], size[1])) * (time - life[0]) / (life[1] - life[0]);
}
