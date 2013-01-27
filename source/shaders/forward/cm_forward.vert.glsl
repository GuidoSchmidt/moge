#version 400

/*** Input ********************************************************************/
layout (location=0) in vec3 vertex;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uv;

/*** Output *******************************************************************/
out vec4 vert_Position;
out vec4 vert_Normal;
out vec2 vert_UV;
out vec3 ReflectDir;

/*** Uniforms *****************************************************************/
uniform bool drawSkyBox;

uniform vec3 CameraPosition;

uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;

/*** Main *********************************************************************/
void main(void)
{	
	if(drawSkyBox)
	{
		ReflectDir = vertex;
	}
	else
	{
		vec3 worldPos = vec3(ModelMatrix * vec4(vertex, 1.0f));
		vec3 worldNormal = vec3(ModelMatrix * vec4(normal, 0.0f));
		vec3 worldView = normalize(CameraPosition - worldPos);
		ReflectDir = reflect(-worldView, worldNormal);
	}

	//vert_Position = ModelMatrix * vec4(vertex, 1.0f);
	//vert_Normal = normalize(NormalMatrix * vec4(normal, 0.0f));
	//vert_UV = uv;
	gl_Position = MVPMatrix * vec4(vertex, 1.0f);
}