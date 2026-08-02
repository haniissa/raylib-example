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
	vkb::Swapchain swapchain;
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
	return core;
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
