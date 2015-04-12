#version 330 core

out vec4 color;

in vec3 fragmentColor;
in vec3 normal;
in vec3 fragmentPosition;

uniform vec3 lightColor;
uniform vec3 lightPosition;
	
void main() {
  // ambient lighting
  vec3 ambient = 0.3f * lightColor;

  // diffuse lighting
  // normalizing simplifies calculations
  vec3 norm = normalize(normal);
  vec3 lightDirection = normalize(lightPosition - fragmentPosition);
  vec3 diffuse = max(dot(normal, lightDirection), 0.0f) * lightColor;

  // result
  vec3 result = (ambient + diffuse) * fragmentColor;
	color = vec4(result, 1.0f);
}

