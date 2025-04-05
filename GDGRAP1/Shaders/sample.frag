#version 330 core

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

in mat3 TBN;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D norm_tex;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 viewPos;

// Point light uniforms
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;
uniform float pointLightConstant;
uniform float pointLightLinear;
uniform float pointLightQuadratic;

// Light intensity uniforms
uniform float dirLightIntensity;
uniform float pointLightIntensity;

void main() {
    // Ambient
    float ambientStrength = 0.1; // Increased ambient strength
    vec3 ambient = ambientStrength * lightColor * dirLightIntensity;

    // Diffuse for directional light
    // vec3 norm = normalize(normCoord);

    vec3 norm = texture(norm_tex, texCoord).rgb;
    norm = normalize(norm * 2.0 - 1.0);
    norm = normalize(TBN * norm);

    float diff = max(dot(norm, normalize(-lightDir)), 0.0);
    vec3 diffuse = diff * lightColor * dirLightIntensity;

    // Point light calculations
    vec3 pointLightToFrag = normalize(pointLightPos - fragPos);

    float pointDiff = max(dot(norm, pointLightToFrag), 0.0);
    vec3 pointDiffuse = pointDiff * pointLightColor * pointLightIntensity;

    float distance = length(pointLightPos - fragPos);
    float attenuation = 1.0 / (pointLightConstant + pointLightLinear * distance + pointLightQuadratic * (distance * distance));

    pointDiffuse *= attenuation;

    vec3 result = (ambient + diffuse + pointDiffuse) * texture(texture1, texCoord).rgb;
    FragColor = vec4(result, 1.0);
}
