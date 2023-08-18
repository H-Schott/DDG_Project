#version 330 core

in vec2 vertexTexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

uniform mat3 processing = mat3(0., 0., 0.,
                               0., 1., 0.,
                               0., 0., 0.);
uniform bool applyProcessing = false;

const float offset = 1.0 / 1000.0;

void main() {
    
    if (!applyProcessing) {
        vec3 color = texture(screenTexture, vertexTexCoords).rgb;
        FragColor = vec4(color, 1.0);
        return;
    }

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel_x[9] = float[](
        -1./8., 0., 1./8.,
        -2./8., 0., 2./8.,
        -1./8., 0., 1./8.
    );

    float kernel_y[9] = float[](
        -1./8., -2./8., -1./8.,
        0., 0., 0.,
        1./8., 2./8., 1./8.
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, vertexTexCoords.st + offsets[i]));
    }

    vec3 col_x = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col_x += sampleTex[i] * kernel_x[i];
    
    vec3 col_y = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col_y += sampleTex[i] * kernel_y[i];

    vec3 col = sqrt(col_x * col_x + col_y * col_y);
    if (length(col) > 0.01) col = vec3(1.);
    else col = vec3(0.);
    //col = vec3(pow(col.x, 0.5), pow(col.y, 0.5), pow(col.z, 0.5));

    FragColor = vec4(col, 1.0);
    //FragColor = vec4(vertexTexCoords.x, vertexTexCoords.y, 1., 1.);
};