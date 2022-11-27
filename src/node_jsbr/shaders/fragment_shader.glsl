#version 300 es
//include "test.glsl"
precision highp float;

in vec4 vColor;

out vec4 fragmentColor;

void main() {
  fragmentColor = vColor;
}
