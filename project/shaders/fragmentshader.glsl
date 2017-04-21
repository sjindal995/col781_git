#version 330 core
in vec2 TexCoord;
in vec4 ourColor;

out vec4 color;

// Texture samplers
uniform sampler2D ourTexture1;
//uniform sampler2D ourTexture2;

void main()
{
	// Linearly interpolate between both textures (second texture is only slightly combined)
	if(ourColor == vec4(0.0f,0.0f,0.0f,1.0f)){
		color = texture(ourTexture1, TexCoord);
	}
	else{
		color = ourColor;
	}
}