#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D firstTexture;
uniform sampler2D secondTexture;
uniform float interpolation_const;

void main()
{
    FragColor = mix(texture(firstTexture, TexCoord), texture(secondTexture,TexCoord),interpolation_const);
}