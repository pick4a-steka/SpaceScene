#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1; // Текстура объекта
uniform vec3 ambient_light_color; // цвет фонового света
uniform float ambient_intensity; // интенсивность фонового света

void main()
{
    // Получаем цвет объекта из текстуры
    vec4 object_color = texture(texture_diffuse1, TexCoords);

    // Расчитываем фоновое освещение
    vec3 ambient = ambient_light_color * ambient_intensity * object_color.rgb;

    // Итоговый цвет пикселя
    FragColor = vec4(ambient, object_color.a);
}
