#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;

out vec3 vertexPos;
out vec3 vertexNormal;
out vec3 vertexColor;
out vec2 vertexTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {

   vec3 new_aPos = aPos;
   gl_Position = projection * view * model * vec4(new_aPos.x, new_aPos.y, new_aPos.z, 1.0);
   vertexPos = vec3(model * vec4(new_aPos, 1.0));
   vertexNormal = mat3(transpose(inverse(model))) * aNormal;
   vertexColor = aColor;
   vertexTexCoords = aTexCoords;

};