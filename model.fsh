#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1; // Текстура объекта
uniform vec3 ambient_light_color; // цвет фонового света
uniform float ambient_intensity; // интенсивность фонового света
uniform vec3 lightPos; // координаты источника света

void main()
{
    // Получаем цвет объекта из текстуры
    vec4 object_color = texture(texture_diffuse1, TexCoords);

    // Расчитываем фоновое освещение
    vec3 ambient = ambient_light_color * ambient_intensity * object_color.rgb;

    // Нормализуем (приводим к единичному вектору) вектор нормали
    vec3 norm = normalize(Normal);

    // Также нормализуем вектор направления источника света
    vec3 lightDir = normalize(lightPos - FragPos);

    /* Посредством скалярного произведения векторов norm и lightDir
    вычисляется величина воздействия диффузного освещения на текущий фрагмент */
    float diff = max(dot(norm, lightDir), 0.0);

    /* Величину воздействия диффузного освещения умножаем на цвет источника света */
    vec3 diffuse = diff * ambient_light_color;

    // Получаем результирующий цвет выходного фрагмента
    vec3 result = (ambient + diffuse) * object_color.rgb;

    // Итоговый цвет пикселя
    FragColor = vec4(result, object_color.a);
}
