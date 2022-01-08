#version 300 es
// -*-Mode: C++;-*-

layout (location=0) in vec4 vertexPosition;
layout (location=1) in vec4 color;
layout (location=2) in vec3 normal;

layout (std140) uniform mvp_matrix {
    mat4 model;
    mat4 projection;
};

// uniform mat4 model;
// uniform mat4 projection;

uniform bool enable_lighting;

out vec4 vColor;

void main() {
  vColor = color;
  gl_Position = projection * model * vec4(vertexPosition.xyz, 1.0);
  // gl_Position = vertexPosition + vec4(0, 0.5, 0, 0);;
  //  gl_Position = projection * model * vertexPosition;
  // gl_Position = projection * model * vec4(vertexPosition, 1.0);
  // gl_Position = vec4(vertexPosition, 1.0);
}
