// Pipeline.h
#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class Pipeline {
public:
    Pipeline(VkDevice device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout,
             VkExtent2D extent);
    ~Pipeline();

    void create();
    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

private:
    VkDevice device;
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkExtent2D extent;

    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;

    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);
};

#endif // PIPELINE_H
