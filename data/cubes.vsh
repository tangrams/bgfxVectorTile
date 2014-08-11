attribute vec4 a_color0;
attribute vec3 a_position;

varying vec4 v_color0;

uniform mat4 u_modelViewProj;

void main()
{
	gl_Position = u_modelViewProj * vec4(a_position, 1.0);
	v_color0 = a_color0;
}
