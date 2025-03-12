#version 330 core

uniform sampler2D tex0;

out vec4 FragColor;

in vec2 texCoord;

in vec3 normCoord;

in vec3 fragPos;

uniform vec3 lightPos;

uniform vec3 lightColor;

uniform float ambientStr;

uniform vec3 ambientColor;

uniform vec3 cameraPos;

uniform float specStr;

uniform float specPhong;

//Brightness parameter
uniform float brightness;

//Point light

void main(){

	//Diffuse
	vec3 normal = normalize(normCoord);

	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 diffuse = diff * lightColor;

	//Ambient
	vec3 ambientCol = ambientColor * ambientStr;

	vec3 viewDir = normalize(cameraPos - fragPos);

	//Specular
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);

	vec3 specColor = spec * specStr * vec3(0, 1, 0);

	//Get distance and attenuation
	float distance = length(lightPos - fragPos);
	//Formula of light intensity via inverse law
	float atten = 1.0 / (distance * distance);

	//Include brightness and attenuation in lighting calculation
	FragColor = vec4((ambientCol + specColor + diffuse) * atten * brightness, 1.0) * texture(tex0, texCoord);

}