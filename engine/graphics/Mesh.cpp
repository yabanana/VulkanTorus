#include "Mesh.h"
#include "BufferHelper.h" // Assicurati che questo file esista e implementi createBuffer e copyBuffer
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include <cstring>

Mesh::Mesh()
    : vertexBuffer(VK_NULL_HANDLE), vertexBufferMemory(VK_NULL_HANDLE),
      indexBuffer(VK_NULL_HANDLE), indexBufferMemory(VK_NULL_HANDLE)
{
}

Mesh::~Mesh()
{
    // La distruzione dei buffer dovrebbe essere gestita durante il cleanup dell'applicazione,
    // oppure qui se la Mesh possiede il device (ma per semplicità lasceremo la gestione in TorusApp::cleanup()).
}

void Mesh::generateTorus(float TORUS_R1, float TORUS_R2, uint32_t TORUS_SEGMENTS, uint32_t TUBE_SEGMENTS)
{
    vertices.clear();
    indices.clear();
    for (uint32_t i = 0; i < TORUS_SEGMENTS; i++) {
        float theta = 2.0f * glm::pi<float>() * i / TORUS_SEGMENTS;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);
        for (uint32_t j = 0; j < TUBE_SEGMENTS; j++) {
            float phi = 2.0f * glm::pi<float>() * j / TUBE_SEGMENTS;
            float cosPhi = cos(phi);
            float sinPhi = sin(phi);
            Vertex vertex;
            float x = (TORUS_R2 + TORUS_R1 * cosPhi) * cosTheta;
            float y = (TORUS_R2 + TORUS_R1 * cosPhi) * sinTheta;
            float z = TORUS_R1 * sinPhi;
            vertex.pos = glm::vec3(x, y, z);
            vertex.normal = glm::normalize(glm::vec3(cosPhi * cosTheta, cosPhi * sinTheta, sinPhi));
            vertices.push_back(vertex);
        }
    }
    for (uint32_t i = 0; i < TORUS_SEGMENTS; i++) {
        for (uint32_t j = 0; j < TUBE_SEGMENTS; j++) {
            uint32_t current = i * TUBE_SEGMENTS + j;
            uint32_t next = ((i + 1) % TORUS_SEGMENTS) * TUBE_SEGMENTS + j;
            uint32_t currentNext = i * TUBE_SEGMENTS + ((j + 1) % TUBE_SEGMENTS);
            uint32_t nextNext = ((i + 1) % TORUS_SEGMENTS) * TUBE_SEGMENTS + ((j + 1) % TUBE_SEGMENTS);
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(currentNext);
            indices.push_back(next);
            indices.push_back(nextNext);
            indices.push_back(currentNext);
        }
    }
}

void Mesh::createBuffers(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, VkCommandPool commandPool)
{
    // --- Creazione del Vertex Buffer ---
    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    BufferHelper::createBuffer(device, physicalDevice, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    BufferHelper::createBuffer(device, physicalDevice, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertexBuffer, vertexBufferMemory);

    BufferHelper::copyBuffer(device, graphicsQueue, commandPool, stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    // --- Creazione dell'Index Buffer ---
    bufferSize = sizeof(uint32_t) * indices.size();

    BufferHelper::createBuffer(device, physicalDevice, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    BufferHelper::createBuffer(device, physicalDevice, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        indexBuffer, indexBufferMemory);

    BufferHelper::copyBuffer(device, graphicsQueue, commandPool, stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}
