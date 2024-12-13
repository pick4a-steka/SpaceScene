#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1; // Текстура объекта

void main()
{
    // Итоговый цвет пикселя
    FragColor = texture(texture_diffuse1, TexCoords);
}
