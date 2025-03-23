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

	// Ambient lighting
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	vec3 normal = normalize(normCoord);

	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 diffuse = diff * lightColor;

	vec3 result = (ambient + diffuse) * texture(tex0, texCoord).rgb;
	FragColor = vec4(result, 1.0);

}
