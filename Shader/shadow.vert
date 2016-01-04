varying mat4 projmat;
varying vec4 lightpos;

void main(void)
{
    gl_Position = ftransform();
    projmat = gl_ProjectionMatrix;
    lightpos = gl_LightSource[0].position;
}
