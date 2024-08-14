#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec3 outColor;

layout(push_constant) uniform Push {
  mat4 transform;
  mat4 normalMat;
} push;

const vec3 lightDir = normalize(vec3(1.0, -3.0, -1.0));
const float ambiant = 0.02;

void main() {
  // gl_Position = vec4(push.transform * position + push.offset, 0.0, 1.0);
  gl_Position = push.transform * vec4(position, 1.0);

  //only works in specific conditions

  // mat3 normalMat = transpose(inverse(mat3(push.modelmat)));
  // vec3 normalWS = normalize(normalMat * normal);
  vec3 normalWS = normalize(mat3(push.normalMat) * normal);
  float intensity = ambiant + max(dot(normalWS, lightDir), 0.0);
  outColor = intensity * color;
}