#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec4 ourColor;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(position, 1.0f);
	// We swap the y-axis by substracing our coordinates from 1. This is done because most images have the top y-axis inversed with OpenGL's top y-axis.
	// TexCoord = texCoord;
	ourColor = vec4(color,1.0f);
	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}