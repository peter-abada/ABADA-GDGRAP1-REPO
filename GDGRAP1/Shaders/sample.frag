#version 330 core

uniform sampler2D tex0;

out vec4 FragColor;

in vec2 texCoord;

in vec3 normCoord;

in vec3 fragPos;

uniform vec3 lightPos;

uniform vec3 lightColor;

void main(){
	//FragColor = vec4(0.7f, 0.0f, 0.0f, 1.0f);

	vec3 normal = normalize(normCoord);

	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 diffuse = diff * lightColor;

	FragColor = vec4(diffuse, 1.0) * texture(tex0, texCoord);

}