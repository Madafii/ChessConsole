#shader vertex
#version 410 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec2 a_Offset;
layout(location = 3) in float a_Layer;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
flat out float v_Layer;

void main() {
    vec2 pos = a_Position + a_Offset;

    v_TexCoord = a_TexCoord;
    v_Layer = a_Layer;

    gl_Position = u_MVP * vec4(pos, 0.0, 1.0);
};

#shader fragment
#version 410 core

uniform sampler2DArray u_Texture;
uniform bool u_Promotion;

in vec2 v_TexCoord;
flat in float v_Layer;

out vec4 color;

void main() {
    if (v_Layer < 0.0) {
        float radius = length(v_TexCoord - 0.5);
        float edge = max(fwidth(radius), 0.001);
        float dotAlpha = 1.0 - smoothstep(0.16 - edge, 0.16 + edge, radius);
        float ringAlpha = 1.0 - smoothstep(0.022 - edge, 0.022 + edge, abs(radius - 0.44));
        color = vec4(0.10, 0.63, 0.49, 0.88 * (v_Layer < -1.5 ? ringAlpha : dotAlpha));
        return;
    }

    color = texture(u_Texture, vec3(v_TexCoord, v_Layer));
    if (u_Promotion) {
        vec2 edge = min(v_TexCoord, 1.0 - v_TexCoord);
        vec3 background = min(edge.x, edge.y) < 0.025 ? vec3(0.18, 0.32, 0.44) : vec3(0.71, 0.82, 0.90);
        color = vec4(mix(background, color.rgb, color.a), 1.0);
    }
};
