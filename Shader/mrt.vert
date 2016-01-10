uniform bool isFlag;
in vec3 Normal;
in vec3 TexCoord;
varying vec4 ambientColor;
varying vec4 fullLightColor;
varying vec4 spotColor;
varying vec2 texCoord;

void main() {
    vec3 normal, lightdir;
    float NdotL;

    ambientColor = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    lightdir = normalize(vec3(gl_LightSource[0].position));
    if (isFlag) {
        normal = normalize(gl_NormalMatrix * gl_Normal);
        vec4 projnormal = gl_ProjectionMatrix * vec4(normal, 1) - gl_ProjectionMatrix * vec4(0, 0, 0, 1);
        if (projnormal[3] > 0.0)
            normal = -normal;
        NdotL = max(dot(normal, lightdir), 0.0);
        fullLightColor = ambientColor + NdotL * (gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse);
        spotColor = gl_FrontLightProduct[1].ambient + NdotL * (gl_FrontMaterial.diffuse * gl_LightSource[1].diffuse);
    } else {
        normal = normalize(gl_NormalMatrix * Normal);
        NdotL = max(dot(normal, lightdir), 0.0);
        fullLightColor = ambientColor + NdotL * (gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse);
        spotColor = gl_FrontLightProduct[1].ambient + NdotL * (gl_FrontMaterial.diffuse * gl_LightSource[1].diffuse);
    }

    if (isFlag)
        texCoord = gl_MultiTexCoord0.st;
    else
        texCoord = vec2(TexCoord);
    gl_Position = ftransform();
}
