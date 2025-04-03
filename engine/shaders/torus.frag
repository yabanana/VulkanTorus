#version 450
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

void main() {
    // Impostiamo luce e camera (in world space)
    vec3 lightPos = vec3(0.0, 10.0, 10.0);
    vec3 viewPos  = vec3(0.0, 0.0, 10.0);

    // Normalizzazione delle variabili
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir  = normalize(viewPos - fragPos);

    // Componente diffusiva: massimo tra il dot product e 0
    float diff = max(dot(norm, lightDir), 0.0);

    // Componente speculare (Phong)
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    // Calcolo del termine Fresnel (Schlick)
    float fresnel = pow(1.0 - max(dot(viewDir, norm), 0.0), 5.0);
    fresnel = mix(0.04, 1.0, fresnel); // 0.04 è il valore tipico per materiali non metallici

    // Componenti di luce: ambientale, diffusiva e speculare
    vec3 ambient = vec3(0.1);
    vec3 diffuse = vec3(0.7) * diff;
    vec3 specular = vec3(1.0) * spec * fresnel;

    vec3 color = ambient + diffuse + specular;

    // Correzione gamma
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, 1.0);
}
