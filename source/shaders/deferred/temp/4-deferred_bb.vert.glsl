#version 400

/*** Input ********************************************************************/
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;

/*** Output *******************************************************************/
out vec2 vert_UV;

/*** Uniforms *****************************************************************/
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

/*** Main *********************************************************************/
void main(void)
{	
	vert_UV = uv;
	
	gl_Position = vec4(vertex, 1.0);
}