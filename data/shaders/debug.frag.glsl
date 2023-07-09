#version 330 core

in vec3 vertexPos;
in vec3 vertexNormal;
in vec3 vertexColor;
in vec2 vertexTexCoords;

out vec4 FragColor;

void main() {

    FragColor = vec4(vertexColor, 1.0);
};