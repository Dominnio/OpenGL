#version 330 core
in vec3 vecColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D Texture2;

void main()
{
    color = texture(Texture2, TexCoord);
}