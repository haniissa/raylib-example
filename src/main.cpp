//Milestone 1: open a window with raylib, bring up a Vulkan instance,
// physical device, logical device, and swapchain through vk-bootstrapk,
// and prove device seleciton works from the command line.
//
// Usage:
// 		psx_maze_shooter          ->    picks the default GPU (vk-bootstrap)
// 		psx_maze_shooter --gpu "RTX"   ->  picks the first device whose name
//												contains "RTX"

// Architechture: raylib owns the window and input polling. rahl's OpenGl
// context exists underneath (IntWindow create it) but is left
// completely idle -- we never call BeginDrawing/EndDrawing or any
// raylib Draw* function. All rendering is hand-written Vulkan.




#include <cstdint>
#define GLFW_INCLUDE_NONE
#include <cstdio>
#include <fmt/base.h>
#include <vulkan/vulkan_core.h>
#include <raylib.h>
#include <raymath.h>

#include <fmt/core.h> // base give print and println core.h give a  format
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>

#include <cstdlib>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>


using namespace fmt;

//Window console apps can close before you read the last line of
//output. Call this before any exit() so nothing gets lost.
static void pauseBeforeExit(){
	fmt::print("Press Enter to close...\n");
	std::cin.get();
}


struct VulkanCore{
	vkb::Instance instance;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	vkb::PhysicalDevice physicalDevice;
	vkb::Device device;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	uint32_t graphicsQueueFamily = 0;
	vkb::Swapchain swapchain;
	std::vector<VkImage> swapchainImages;

	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> commandBuffers; // one per swapchain iamge

	VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
	VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
	VkFence inFlightFence = VK_NULL_HANDLE;
};

// Enumerates every Vulkan-capable GPU on the system and logs it. If
// preferrdName is non-empty, picks the first device whose name contains
// that substring; otherwise falls back to vk-bootstrap's default
// heuristic (prefers a discrete GPU)
static vkb::PhysicalDevice pickPhysicalDevice(vkb::Instance& instance,
	VkSurfaceKHR surface, const std::string& preferrdName){
		vkb::PhysicalDeviceSelector selector {instance};
		selector.set_surface(surface).set_minimum_version(1, 2);

		auto devicesRet = selector.select_devices();
		if(!devicesRet){
			fmt::print(stderr, "Failed to enumerate Vulkan device: {}\n", devicesRet.error().message());
			pauseBeforeExit();
			std::exit(1);
		}

		fmt::print("Available Vulkan device:\n");
		for(auto& candidate : devicesRet.value())
			fmt::print("    -{}\n", candidate.name);

		if(!preferrdName.empty()){
			for(auto& candidate : devicesRet.value()){
				if(candidate.name.find(preferrdName) != std::string::npos){
					fmt::print("Selected by --gup match: {}\n", candidate.name);
					return candidate;
				}
			}
			fmt::print("No device matched \"{}\" --falling back to default selection\n", preferrdName);
		}
		auto defaultRet = selector.select();
		if(!defaultRet){
			fmt::print(stderr, "Default device selection failed: {}\n", defaultRet.error().message());
			pauseBeforeExit();
			std::exit(1);
		}
		fmt::print("Selected (default): {}\n", defaultRet.value().name);
		return defaultRet.value();
	}
static VulkanCore initVulkan(GLFWwindow* window, const std::string& preferredGpu){
	VulkanCore core;

	vkb::InstanceBuilder builder;
	auto instRet = builder.set_app_name("PSX Maze Shooter")
		.request_validation_layers()
		.use_default_debug_messenger()
		.require_api_version(1,2,0)
		.build();

	if(!instRet){
		fmt::print(stderr, "Failed to create Vulkan instance: {}\n", instRet.error().message());
		pauseBeforeExit();
		std::exit(1);
	}
	core.instance = instRet.value();

	//Diagnostic: prove the handle is actually valid befor ewe hand it to GLFW.
	fmt::print("Vulkan instance handle: {}\n",
		static_cast<void*>(core.instance.instance));
	if(core.instance.instance == VK_NULL_HANDLE){
		fmt::print(stderr, "vk-boostrap reported success bu the instance handle is null."
			"This mean the vulkan loader itself is the problem, not our code---"
			"most likey an incomplete/missing Vulkan SDK install.\n");
		pauseBeforeExit();
		std::exit(1);
	}
	if(glfwCreateWindowSurface(core.instance, window, nullptr, &core.surface) != VK_SUCCESS){
		fmt::print(stderr, "Failed to create Vulkan surface from the GLFW window\n");
		pauseBeforeExit();
		std::exit(1);
	}

	core.physicalDevice = pickPhysicalDevice(core.instance, core.surface, preferredGpu);
	vkb::DeviceBuilder deviceBulder{core.physicalDevice};
	auto devRet = deviceBulder.build();
	if(!devRet){
		fmt::print(stderr, "Failed to create Vulkan device: {}\n", devRet.error().message());
		pauseBeforeExit();
		std::exit(1);
	}
	core.device = devRet.value();
	core.graphicsQueue = core.device.get_queue(vkb::QueueType::graphics).value();
	core.graphicsQueueFamily = core.device.get_queue_index(vkb::QueueType::graphics).value();

	//Note: SwapchainBuilder's convenience method names have shifted
	//slightly across vk-bootstrap release -- if this doesn't compile,
	// check VkBootstrap.h in your fetched version for the exact name.
	vkb::SwapchainBuilder swapchainBuilder{ core.device };
	auto scRet = swapchainBuilder.use_default_format_selection()
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.build();
	if(!scRet){
		fmt::print(stderr, "Failed to create swapchain: {}\n", scRet.error().message());
		pauseBeforeExit();
		std::exit(1);
	}
	core.swapchain = scRet.value();
	core.swapchainImages = core.swapchain.get_images().value();
	return core;
}

//Records one command buffer per swapchain image: transition to a
// clear-fiendly layout, clear to a color, transition to present layout.
// Recorded once at startup since the clear color never changes -- no
// per-frame re-recording needed for this milestone.
static void recordClearCommandBuffers(VulkanCore& core){
		VkCommandPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
		poolInfo.queueFamilyIndex = core.graphicsQueueFamily;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		if(vkCreateCommandPool(core.device.device, &poolInfo, nullptr, &core.commandPool) != VK_SUCCESS){
			fmt::print(stderr, "Failed to create command pool.\n");
			pauseBeforeExit();
			std::exit(1);
		}
		core.commandBuffers.resize(core.swapchainImages.size());
		VkCommandBufferAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
		allocInfo.commandPool = core.commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(core.commandBuffers.size());
		if(vkAllocateCommandBuffers(core.device.device, &allocInfo, core.commandBuffers.data()) != VK_SUCCESS){
			fmt::print(stderr, "Failed to allocate command buffers.\n");
			pauseBeforeExit();
			std::exit(1);
		}
		//dark, slightely desaturated blue -- placeholder "PSX menu" color,
		// easy to swap for a real render pass clear value later
		VkClearColorValue clearColor{{0.04F, 0.05F, 0.09F, 1.0F}};
		VkImageSubresourceRange range{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

		for(size_t i = 0; i < core.commandBuffers.size(); ++i){
			VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
			vkBeginCommandBuffer(core.commandBuffers[i], &beginInfo);

			VkImageMemoryBarrier toClear{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
			toClear.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			toClear.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			toClear.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			toClear.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			toClear.image = core.swapchainImages[i];
			toClear.subresourceRange = range;
			toClear.srcAccessMask = 0;
			toClear.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			vkCmdPipelineBarrier(core.commandBuffers[i],
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				    VK_PIPELINE_STAGE_TRANSFER_BIT,
						0,
						0,nullptr,
						0,nullptr,
						1, &toClear);
			vkCmdClearColorImage(core.commandBuffers[i], core.swapchainImages[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				&clearColor, 1, &range);

			VkImageMemoryBarrier toPresent = toClear;
			toPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			toPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			toPresent.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			toPresent.dstAccessMask = 0;
			vkCmdPipelineBarrier(core.commandBuffers[i],
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0,
				nullptr, 0,nullptr, 1, &toPresent);


			vkEndCommandBuffer(core.commandBuffers[i]);
			VkSemaphoreCreateInfo semInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
			VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO}; // start signlaed so frame 1 doesn't stall
			vkCreateSemaphore(core.device.device, &semInfo, nullptr, &core.imageAvailableSemaphore);
			vkCreateFence(core.device.device, &fenceInfo, nullptr, &core.inFlightFence);
		}
}

static void drawFrame(VulkanCore& core){
	vkWaitForFences(core.device.device,
		1, &core.inFlightFence, VK_TRUE, UINT64_MAX);

	uint32_t imageIndex{};
	VkResult acquireResult =
		vkAcquireNextImageKHR(core.device.device, core.swapchain.swapchain, UINT64_MAX, core.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	if(acquireResult == VK_ERROR_OUT_OF_DATE_KHR){
		//Window was resized -- swapchain recreation is a later milestone
		return;
	}
	vkResetFences(core.device.device, 1, &core.inFlightFence);

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
	submitInfo.waitSemaphoreCount  = 1;
	submitInfo.pWaitSemaphores = &core.imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = &waitStage;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &core.commandBuffers[imageIndex] ;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &core.renderFinishedSemaphore;
	vkQueueSubmit(core.graphicsQueue, 1, &submitInfo, core.inFlightFence);

	VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &core.renderFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &core.swapchain.swapchain;
	presentInfo.pImageIndices = &imageIndex;
	vkQueuePresentKHR(core.graphicsQueue, &presentInfo);
}

static void cleanup(VulkanCore& core, GLFWwindow* window){
	vkDeviceWaitIdle(core.device.device);

	vkDestroySemaphore(core.device.device, core.renderFinishedSemaphore, nullptr);
	vkDestroyFence(core.device.device, core.inFlightFence,nullptr);
	vkDestroyCommandPool(core.device.device, core.commandPool, nullptr);//frees command buffer too

	vkb::destroy_swapchain(core.swapchain);
	vkb::destroy_device(core.device);
	vkb::destroy_surface(core.instance, core.surface);
	vkb::destroy_instance(core.instance);

	glfwDestroyWindow(window);
	glfwTerminate();
}




int main(int argc, char** argv){
	std::string preferrdGpu;
	for(int i = 1; i < argc; ++i){
		std::string arg = argv[i];
		if(arg == "--gpu" and i + 1 < argc){
			preferrdGpu = argv[++i];
		}
	}
	if(!glfwInit()){
		fmt::print(stderr, "glfwInit() failed. \n");
		pauseBeforeExit();
		std::exit(1);
	}
	//No OpenGl context at all -- we're going stright to Vulkan
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "PSX Maze Shooter", nullptr, nullptr);
	if(!window){
		fmt::print(stderr, "glfwCreateWindow() failed.\n");
		pauseBeforeExit();
		glfwTerminate();
		std::exit(1);
	}
	VulkanCore vk = initVulkan(window, preferrdGpu);
	fmt::print("Vulkan device ready: {}\n", vk.physicalDevice.name);
	fmt::print("Swapchain image: {}\n", vk.swapchain.image_count);

	recordClearCommandBuffers(vk);
	fmt::print("Command Buffers recorded -- enterning render loop.\n");

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}
	glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}
