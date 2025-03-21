#version 330 core

uniform sampler2D tex0;

out vec4 FragColor;

in vec2 texCoord;

void main(){
	//FragColor = vec4(0.7f, 0.0f, 0.0f, 1.0f);

	FragColor = texture(tex0, texCoord);

}