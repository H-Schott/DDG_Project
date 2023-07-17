#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec3 aBary;
layout (location = 4) in vec2 aTexCoords;

out vec3 vertexPos;
out vec3 vertexNormal;
out vec3 vertexColor;
out vec2 vertexTexCoords;
out vec3 vertexBary;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 min_vertex;
uniform vec3 max_vertex;

vec3 center = 0.5 * (min_vertex + max_vertex);
vec3 half_diag = max_vertex - center;
float scale = 1. / max(max(half_diag.x, half_diag.y), half_diag.z);

void main() {

   vec3 new_aPos = aPos - center;
   new_aPos *= scale;
   gl_Position = projection * view * model * vec4(new_aPos.x, new_aPos.y, new_aPos.z, 1.0);
   vertexPos = vec3(model * vec4(new_aPos, 1.0));
   //vertexNormal = vec3(model * vec4(aNormal, 0.0));
   vertexNormal = mat3(transpose(inverse(model))) * aNormal;
   vertexColor = aColor;//vec4(0.5 + vec3(new_aPos.x, new_aPos.y, new_aPos.z), 1.0);
   vertexTexCoords = aTexCoords;
   vertexBary = aBary;

};