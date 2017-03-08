#version 330 core

//in vec3 fragmentColor;
in vec2 UV;
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){
  //color = vec3(1,0,0);
   //color = fragmentColor;
    color = texture( myTextureSampler, UV ).rgb;
}
