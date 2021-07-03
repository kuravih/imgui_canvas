#version 130

in vec2 position;
in vec3 color;
in vec2 texcoord;

out vec3 Color;
out vec2 Texcoord;

void main() {
  Color = color;
  Texcoord = texcoord;
  gl_Position = vec4(position, 0.0, 1.0);
}