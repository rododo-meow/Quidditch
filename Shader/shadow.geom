#version 400

const float dis = 20;
layout( triangles ) in;
layout( triangle_strip, max_vertices = 12 ) out;
varying mat4 projmat[];
varying vec4 lightn[];
uniform bool isFlag;

void main()
{
    vec3 norm = cross(vec3(gl_in[1].gl_Position - gl_in[0].gl_Position), vec3(gl_in[2].gl_Position - gl_in[1].gl_Position));

    if (!isFlag && dot(vec3(-lightn[0]), norm) > 0)
        return;

    if (isFlag && norm[2] < 0) {
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position + lightn[2] * dis;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position + lightn[1] * dis;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + lightn[0] * dis;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        EndPrimitive();

        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + lightn[0] * dis;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position + lightn[2] * dis;
        EmitVertex();
        EndPrimitive();
    } else {
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position + lightn[1] * dis;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position + lightn[2] * dis;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + lightn[0] * dis;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        EndPrimitive();

        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + lightn[0] * dis;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position + lightn[1] * dis;
        EmitVertex();
        EndPrimitive();
    }
}
