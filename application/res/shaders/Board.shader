#shader vertex
#version 410 core

layout(location = 0) in vec2 position;

out vec2 v_Pos;

uniform mat4 u_MVP;

void main() {
    v_Pos = position;
    gl_Position = u_MVP * vec4(position, 0.0, 1.0);
};

#shader fragment
#version 410 core

in vec2 v_Pos;
out vec4 color;

uniform float u_TileWidth;
uniform float u_TileHeight;

void main() {
    int tileX = int(v_Pos.x / u_TileWidth);
    int tileY = int(v_Pos.y / u_TileHeight);

    bool isWhite = (tileX + tileY) % 2 == 0;

    vec3 white = vec3(1.0, 1.0, 1.0);
    vec3 black = vec3(0.1, 0.1, 0.1);

    color = vec4(isWhite ? white : black, 1.0);
};
