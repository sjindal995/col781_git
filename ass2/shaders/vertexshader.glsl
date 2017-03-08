#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
//layout(location = 1) in vec3 vertexColor;
//out vec3 fragmentColor;
layout(location = 1) in vec2 vertexUV;
out vec2 UV;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){
	// Output position of the vertex, in clip space : MVP * position
  	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);	
	//gl_Position.xyz = vertexPosition_modelspace;
	//gl_Position.w = 1.0;	

	//fragmentColor = vertexColor;
	UV = vertexUV;
}	
