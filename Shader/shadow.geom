#version 400

const float dis = 20;
layout( triangles ) in;
layout( triangle_strip, max_vertices = 12 ) out;
varying mat4 projmat[];
varying vec4 lightpos[];
uniform bool isFlag;

void main()
{
    vec3 norm = cross(vec3(gl_in[1].gl_Position - gl_in[0].gl_Position), vec3(gl_in[2].gl_Position - gl_in[1].gl_Position));
    vec4 lightn = normalize(projmat[0] * lightpos[0]);

    if (!isFlag && dot(vec3(lightn), norm) > 0)
        return;

    if (isFlag && norm[2] < 0) {
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position - lightn * dis;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position - lightn * dis;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position - lightn * dis;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        EndPrimitive();

        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position - lightn * dis;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position - lightn * dis;
        EmitVertex();
        EndPrimitive();
    } else {
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position - lightn * dis;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position - lightn * dis;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position - lightn * dis;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        EndPrimitive();

        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position - lightn * dis;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position - lightn * dis;
        EmitVertex();
        EndPrimitive();
    }
}
