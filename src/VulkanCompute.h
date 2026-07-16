#pragma once
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>

class VulkanCompute{
	public:
		bool enabled = false;

		void Init();
		void Dispatch(float time);
		void Cleanup();
	private:
		VkInstance    instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		vkb::Instance instance_vkb{};
		VkPhysicalDevice   physicalDevice = VK_NULL_HANDLE;
		VkDevice  device   = VK_NULL_HANDLE;
		VkQueue   queue  = VK_NULL_HANDLE;
		uint32_t     queueFamily  = 0;

		VkDescriptorSetLayout descLayout = VK_NULL_HANDLE;
		VkDescriptorSet  descPool = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipeline    pipeline = VK_NULL_HANDLE;
		VkShaderModule shaderModule = VK_NULL_HANDLE;
		VkCommandPool  cmdPool  = VK_NULL_HANDLE;
		VkCommandBuffer  cmdBuf   = VK_NULL_HANDLE;
};
