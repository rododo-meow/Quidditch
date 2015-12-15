varying mat4 projmat;
varying vec4 lightpos;

void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    projmat = gl_ProjectionMatrix;
    lightpos = gl_LightSource[0].position;
}
