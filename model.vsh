#version 330 core
layout (location = 0) in vec3 aPos;       // position
layout (location = 1) in vec3 aNormal;    // normal
layout (location = 2) in vec2 aTexCoords; // texture coordinates
layout (location = 3) in vec3 aTangent;   // tangent
layout (location = 4) in vec3 aBitangent; // bitangent

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    // использование матрицы нормалей, которая убирает компоненты сдвига и исправляет имкажение от неравномерного искажения
    Normal = mat3(transpose(inverse(model))) * aNormal;
    // Normal = aNormal;
}
