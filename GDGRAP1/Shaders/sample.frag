#version 330 core

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 viewPos;

// Point light uniforms
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;
uniform float pointLightConstant;
uniform float pointLightLinear;
uniform float pointLightQuadratic;

void main() {
    // Ambient
    float ambientStrength = 0.1; // Increased ambient strength
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse for directional light
    vec3 norm = normalize(normCoord);
    float diff = max(dot(norm, normalize(-lightDir)), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular for directional light
    float dirSpecularStrength = 0.5; // Increased specular strength for directional light
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(normalize(lightDir), norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = dirSpecularStrength * spec * lightColor;

    // Point light calculations
    vec3 pointLightToOrigin = normalize(vec3(0.0, 0.0, 0.0) - pointLightPos);
    vec3 pointLightToFrag = normalize(pointLightPos - fragPos);

    float pointDiff = max(dot(norm, pointLightToFrag), 0.0);
    vec3 pointDiffuse = pointDiff * pointLightColor;

    vec3 pointReflectDir = reflect(-pointLightToFrag, norm);
    float pointSpecularStrength = 0.5; // Increased specular strength for point light
    float pointSpec = pow(max(dot(viewDir, pointReflectDir), 0.0), 32);
    vec3 pointSpecular = pointSpecularStrength * pointSpec * pointLightColor;

    float distance = length(pointLightPos - fragPos);
    float attenuation = 1.0 / (pointLightConstant + pointLightLinear * distance + pointLightQuadratic * (distance * distance));

    pointDiffuse *= attenuation;
    pointSpecular *= attenuation;

    vec3 result = (ambient + diffuse + specular + pointDiffuse + pointSpecular) * texture(texture1, texCoord).rgb;
    FragColor = vec4(result, 1.0);
}
