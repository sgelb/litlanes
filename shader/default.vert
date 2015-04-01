#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normalIn;
layout (location = 2) in vec3 colorIn;

out vec3 fragmentPosition;
out vec3 normal;
out vec3 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0f);
  // fragmentPosition is a vertex in world space
  fragmentPosition = vec3(model * vec4(position, 1.0f));
  fragColor = colorIn;
  normal = normalIn;
}

