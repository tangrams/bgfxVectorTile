uniform mat4 u_modelViewProj;
uniform mat4 u_modelView;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec4 a_color0;

varying vec3 v_pos;
varying vec3 v_view;
varying vec3 v_normal;
varying vec4 v_color0;

void main()
{

	gl_Position = u_modelViewProj * vec4(a_position, 1.0);
	v_pos = gl_Position.xyz;
	v_view = ( u_modelView * vec4(v_pos, 1.0) ).xyz;

	v_normal = ( u_modelView * vec4(a_normal, 0.0) ).xyz;

	v_color0 = a_color0;
}
