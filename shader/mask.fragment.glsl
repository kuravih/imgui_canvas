#version 130

uniform sampler2D image;
uniform sampler2D mask;

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

void main() {
  vec4 image_color = texture(image, Texcoord);
  vec4 mask_color = 255*texture(mask, Texcoord);
  outColor = vec4(image_color.r+(1.0-mask_color.r),image_color.r,image_color.r,1.0);
}