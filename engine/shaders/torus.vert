#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec3 fragPos;

void main() {
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    fragPos = worldPos.xyz;
    // Trasforma la normale correttamente (in world space)
    fragNormal = mat3(transpose(inverse(ubo.model))) * inNormal;
    gl_Position = ubo.proj * ubo.view * worldPos;
}
