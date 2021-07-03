#version 130

uniform sampler2D image;

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

void main() {
  vec4 image_color = texture(image, Texcoord);
  outColor = vec4(image_color.r,image_color.r,image_color.r,1.0);
}