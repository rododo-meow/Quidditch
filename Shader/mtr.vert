varying vec4 ambientColor;
varying vec4 fullLightColor;
varying vec2 texCoord;

void main() {
    vec3 normal, lightdir;
    float NdotL;

    ambientColor = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    normal = normalize(gl_NormalMatrix * gl_Normal);
    lightdir = normalize(vec3(gl_LightSource[0].position));
    NdotL = max(dot(normal, lightdir), 0.0);

    fullLightColor = ambientColor + NdotL * (gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse);

    texCoord = gl_MultiTexCoord0.st;
    gl_Position = ftransform();
}
