#version 330 core

in vec3 vertexPos;
in vec3 vertexNormal;
in vec3 vertexColor;
in vec2 vertexTexCoords;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 uniformColor;
uniform sampler2D uniformTexture;

void main() {

    // FragColor = vec4(vertexColor, 1.0);
    // return;

    vec3 norm = normalize(vertexNormal);
    vec3 lightDir = normalize(lightPos - vertexPos);
    //float diff = max(dot(norm, vec3(0.0, 0.0, 1.0)), 0.0);
    float diff = 0.7 * max(dot(norm, lightDir), 0.0);
    float ambient = 0.2;

    vec3 cameraDir = normalize(cameraPos - vertexPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = 1.0 * pow(max(dot(cameraDir, reflectDir), 0.0), 256);

    //vec3 base_color = vec3(0.8, 0.4, 0.1);
    vec3 base_color = vertexColor;
    FragColor = (ambient + diff + spec) * vec4(base_color, 1.0);//texture(uniformTexture, vertexTexCoords);

};