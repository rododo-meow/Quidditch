varying mat4 projmat;
varying vec4 lightn;

void main(void)
{
    gl_Position = ftransform();
    if (gl_LightSource[0].position[3] == 0)
      lightn = -gl_ProjectionMatrix * gl_LightSource[0].position;
    else
      lightn = gl_Position - gl_ProjectionMatrix * gl_LightSource[0].position;
}
