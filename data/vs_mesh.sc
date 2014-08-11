$input a_position, a_normal, a_color0
$output v_pos, v_view, v_normal, v_color0

/*
 * Copyright 2011-2014 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "../include/common/common.sh"

void main()
{

	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	v_pos = gl_Position.xyz;
	v_view = mul(u_modelView, vec4(v_pos, 1.0) ).xyz;

	v_normal = mul(u_modelView, vec4(a_normal, 0.0) ).xyz;

	v_color0 = a_color0;
}
