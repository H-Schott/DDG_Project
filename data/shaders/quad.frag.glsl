#version 330 core

in vec2 vertexTexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

uniform mat3 processing = mat3(0., 0., 0.,
                               0., 1., 0.,
                               0., 0., 0.);
uniform bool applyProcessing = false;

void main() {
    vec3 col = texture(screenTexture, vertexTexCoords).rgb;
    FragColor = vec4(col, 1.0);
};