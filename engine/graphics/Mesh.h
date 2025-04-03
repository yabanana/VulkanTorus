#ifndef MESH_H
#define MESH_H

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    // Genera la geometria del toroide
    void generateTorus(float TORUS_R1, float TORUS_R2, uint32_t TORUS_SEGMENTS, uint32_t TUBE_SEGMENTS);

    // Crea i buffer GPU (vertex e index) usando i dati CPU generati
    // Richiede il device, il physical device, la graphics queue e un command pool per il copy
    void createBuffers(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool);

    // Getters per i buffer GPU
    VkBuffer getVertexBuffer() const { return vertexBuffer; }
    VkBuffer getIndexBuffer() const { return indexBuffer; }

    // Getter per il vettore degli indici (usato per il conteggio nella draw call)
    const std::vector<uint32_t>& getIndices() const { return indices; }

private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Buffer e memoria per i dati GPU
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

#endif // MESH_H
