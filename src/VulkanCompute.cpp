#include "VulkanCompute.h"
#include <VkBootstrap.h>
#include <cstdint>
#include <fmt/base.h>
#include <cstring>
#include <cstdio>
#include <vector>
#include <vulkan/vulkan_core.h>

void VulkanCompute::Init(){
	vkb::InstanceBuilder instBuilder;
	auto instRet = instBuilder
		.set_app_name("CubeArena - Vulkan Compute")
		.set_headless(true)
		.require_api_version(false)
		.build();
	if(!instRet){
		fmt::println("[Vulkan] Instance creation failed: {}", instRet.error().message());
		return;
	}
	instance_vkb = instRet.value();
	instance     = instance_vkb.instance;
	debugMessenger = instance_vkb.debug_messenger;

	vkb::PhysicalDeviceSelector selector(instance_vkb);
	auto physRet = selector
		.prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
		// .require_dedicated_compute_queue() // optional but recommended
		.select();
	if(!physRet){
		fmt::println("[Vulkan] No suitable GPU: {}", physRet.error().message());
		return;
	}
	physicalDevice = physRet.value().physical_device; // raw VkPhysicalDevice

	vkb::DeviceBuilder devBuilder(physRet.value());
	auto devRet = devBuilder.build();
	if(!devRet){
		fmt::println("[Vulkan] Device creation failed: {}",devRet.error().message());
		return;
	}
	vkb::Device vkbDevice = devRet.value();
	device  = vkbDevice.device;
	queue   = vkbDevice.get_queue(vkb::QueueType::compute).value();
	queueFamily  = vkbDevice.get_queue_index(vkb::QueueType::compute).value();
}

void VulkanCompute::Dispatch(float time){
	if(!enabled) return;

	VkCommandBufferBeginInfo beginCI{};
	beginCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(cmdBuf, &beginCI);

	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE,  pipeline);


	//push time as specialization constant or uniform
	// For simplicity we skip descriptor writes - pass time via push constant
	uint32_t pushData[2];
	memcpy(pushData, &time, sizeof(float));
	uint32_t frame = (uint32_t)(time * 60.0F);
	pushData[1] = frame;
	vkCmdPushConstants(cmdBuf, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pushData), pushData);

	vkCmdDispatch(cmdBuf, 16, 1, 1);

	vkEndCommandBuffer(cmdBuf);

	VkSubmitInfo submit{};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmdBuf;
	vkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);
}

void VulkanCompute::Cleanup(){
	if(!enabled)return;
	vkDestroyCommandPool(device, cmdPool, nullptr);
	vkDestroyPipeline(device,  pipeline, nullptr);
	vkDestroyPipelineLayout(device,  pipelineLayout, nullptr);
	vkDestroyShaderModule(device, shaderModule, nullptr);
	vkDestroyDescriptorSetLayout(device, descLayout, nullptr);
	vkDestroyDevice(device, nullptr);
	vkb::destroy_debug_utils_messenger(instance, debugMessenger);
	vkDestroyInstance(instance, nullptr);
	fmt::println("[Vulkan] Cleanup complete");
}
