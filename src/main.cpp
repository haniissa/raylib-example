
//Milestone 3a: a real render pass + graphics pipeline drawing a colored
// quad (vertex colors, no texture yet). This replace the raw
// vkCmdClearColorImage hack from milestone 2 -- the render pass now owns
// the layout trnsitions via its attachment description + subpass
// dependency, so we no longer hand-write barriers for the swapchain
// image. Texture sampling (descriptor sets, image loading) is the next
// milestone on top of this.
//
// Usage:
// 		psx_maze_shooter               -> picks the default GPU (vk-bootstrap
// 										  perfers a discriter GPU if present)
// 		psx_maze_shooter --gpu "RTX"   -> picks the first device whose name
// 										  contains "RTX"
//
// Architecture: GLFW (featched directly, one single copy) owns the window
// and input polling, with GLFW_NO_API so it never opens a GL context.
// IMPORTANT: vulkan.h must be included before glfw3.h, or GLFW won't
// declare glfwCreateWindowSurface (it's gated behind VK_VERSION_1_0
// already being defined).


#include <cstddef>
#include <cstring>
#include <ios>
#define GLFW_INCLUDE_NONE
#include <cstdint>
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
#include <fstream>


using namespace fmt;

//Window console apps can close before you read the last line of
//output. Call this before any exit() so nothing gets lost.
static void pauseBeforeExit(){
	fmt::print("Press Enter to close...\n");
	std::cin.get();
}

struct Vertex {
	float pos[2];
	float color[3];
};
//A quad made of two triangles, each corner a different color so it's
//obvious the vertex buffer (not just a hardcoded shader color) is what's
// driving the picture.

static const std::vector<Vertex> kVertices = {
	{{-0.6F, -0.6F}, {1.0F, 0.2F, 0.2F}}, // bottom-left: red
	{{0.6F, -0.6F}, {0.2F, 1.0F, 0.2F}},  // bottom-right: green
	{{0.6F, 0.6F}, {0.2F, 0.4F, 1.0F}},  //top-right: blue
	{{-0.6F, 0.6F}, {1.0F, 1.0F, 1.0F}}, //top-left: white
};
static const std::vector<uint16_t> kIndices = {0,1,2,2,3,0};

struct VulkanCore{
	vkb::Instance instance;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	vkb::PhysicalDevice physicalDevice;
	vkb::Device device;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	uint32_t graphicsQueueFamily = 0;
	vkb::Swapchain swapchain;
	std::vector<VkImageView> swapchainImagesViews;

	VkRenderPass renderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> framebuffers;

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline pipline = VK_NULL_HANDLE;

	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
	VkBuffer indexBuffer= VK_NULL_HANDLE;
	VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

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
		// .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build();
	if(!scRet){
		fmt::print(stderr, "Failed to create swapchain: {}\n", scRet.error().message());
		pauseBeforeExit();
		std::exit(1);
	}
	core.swapchain = scRet.value();
	auto viewRet = core.swapchain.get_image_views();
	if(!viewRet){
		fmt::print(stderr, "Failed to get swapchain image views: {}\n",
			viewRet.error().message());
		pauseBeforeExit();
		std::exit(1);
	}
	core.swapchainImagesViews = viewRet.value();
	return core;
}

static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,VkMemoryPropertyFlags properties){
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);
		for(uint32_t i{}; i < memProps.memoryTypeCount; ++i){
			if((typeFilter & (1 << i)) &&
				(memProps.memoryTypes[i].propertyFlags & properties) == properties){
					return i;
				}
		}
		fmt::print(stderr, "Failed to find a suitable memory type.\n");
		pauseBeforeExit();
		std::exit(1);
}

//Simple host--visible buffer -- fine for a handful of static vertice,
// not how we'll upload real mesh/texture data later (that wants a
// device-local buffer + staging copy for performance).
static void createHostVisibleBuffer(VulkanCore& core, VkDeviceSize size, VkBufferUsageFlags usage,
	VkBuffer& outBuffer, VkDeviceMemory& outMemory,
	const void* data){
		VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if(vkCreateBuffer(core.device.device, &bufferInfo, nullptr, &outBuffer) != VK_SUCCESS){
			fmt::print(stderr, "Failed to create buffer.\n");
			pauseBeforeExit();
			std::exit(1);
		}
		VkMemoryRequirements memReq;
		vkGetBufferMemoryRequirements(core.device.device, outBuffer, &memReq);

		VkMemoryAllocateInfo allocInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
		allocInfo.allocationSize = memReq.size;
		allocInfo.memoryTypeIndex =
			findMemoryType(core.physicalDevice.physical_device,  memReq.memoryTypeBits,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if(vkAllocateMemory(core.device.device, &allocInfo, nullptr, &outMemory) != VK_SUCCESS){
			fmt::print(stderr, "Failed to allocate buffer memory.\n");
			pauseBeforeExit();
			std::exit(1);
		}


		vkBindBufferMemory(core.device.device, outBuffer, outMemory, 0);

		void* mapped = nullptr;
		vkMapMemory(core.device.device, outMemory, 0,  size, 0, &mapped);
		std::memcpy(mapped, data, static_cast<size_t>(size));
		vkUnmapMemory(core.device.device, outMemory);
}


static void createGeometryBuffers(VulkanCore& core){
	createHostVisibleBuffer(core, sizeof(Vertex)* kVertices.size(),
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, core.vertexBuffer,
		core.vertexBufferMemory, kVertices.data());
	createHostVisibleBuffer(core, sizeof(uint16_t) * kIndices.size(),
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT, core.indexBuffer,
		core.indexBufferMemory, kIndices.data());
}

static void createRenderPass(VulkanCore& core){
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = core.swapchain.image_format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorRef{};
	colorRef.attachment = 0;
	colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcAccessMask = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo rpInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
	rpInfo.attachmentCount = 1;
	rpInfo.pAttachments = &colorAttachment;
	rpInfo.subpassCount = 1;
	rpInfo.pSubpasses = &subpass;
	rpInfo.dependencyCount = 1;
	rpInfo.pDependencies = &dependency;

	if(vkCreateRenderPass(core.device.device, &rpInfo, nullptr, &core.renderPass) != VK_SUCCESS){
		fmt::print(stderr, "Failed to create render pass.\n");
		pauseBeforeExit();
		std::exit(1);
	}
}

static void createFramebuffer(VulkanCore& core){
	core.framebuffers.resize(core.swapchainImagesViews.size());
	for(size_t i{}; i < core.swapchainImagesViews.size(); ++i){
		VkImageView attachments[] = {core.swapchainImagesViews[i]};
		VkFramebufferCreateInfo fbInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
		fbInfo.renderPass = core.renderPass;
		fbInfo.attachmentCount = 1;
		fbInfo.pAttachments = attachments;
		fbInfo.width = core.swapchain.extent.width;
		fbInfo.height = core.swapchain.extent.height;
		fbInfo.layers = 1;
		if(vkCreateFramebuffer(core.device.device, &fbInfo, nullptr, &core.framebuffers[i]) != VK_SUCCESS){
			fmt::print(stderr,"Failed to create framebuffer {}.\n", i);
			pauseBeforeExit();
			std::exit(1);
		}
	}
}

static std::vector<char> readFile(const std::string& path){
	std::ifstream file(path, std::ios::ate | std::ios::binary);
	if(!file.is_open()){
		fmt::print(stderr, "Failed to open shader file: {}\n", path);
		pauseBeforeExit();
		std::exit(1);
	}
	size_t size = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(size);
	file.seekg(0);
	file.read(buffer.data(), static_cast<std::streamsize>(size));
	return buffer;
}

static VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code){
	VkShaderModuleCreateInfo info{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
	info.codeSize = code.size();
	info.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule module;
	if(vkCreateShaderModule(device, &info, nullptr, &module) != VK_SUCCESS){
		fmt::print(stderr, "Failed to create shader module.\n");
		pauseBeforeExit();
		std::exit(1);
	}
	return module;
}

static void createPipeline(VulkanCore& core){
	#ifndef SHADER_DIR
	#define SHADER_DIR "shaders/"
	#endif

	auto vertCode = readFile(std::string(SHADER_DIR) + "/basic.vert.spv");
	auto fragCode = readFile(std::string(SHADER_DIR) + "/basic.frag.spv");
	VkShaderModule vertModule = createShaderModule(core.device.device, vertCode);
	VkShaderModule fragModule = createShaderModule(core.device.device, fragCode);

	VkPipelineShaderStageCreateInfo vertStage{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
	vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStage.module = vertModule;
	vertStage.pName = "main";

	VkPipelineShaderStageCreateInfo fragStage{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
	fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStage.module = fragModule;
	fragStage.pName = "main";

	VkPipelineShaderStageCreateInfo stages[] ={vertStage, fragStage};

	VkVertexInputBindingDescription  binding{};
	binding.binding = 0;
	binding.stride = sizeof(Vertex);
	binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription attributes[2]{};
	attributes[0].location = 0;
	attributes[0].binding = 0;
	attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributes[0].offset = offsetof(Vertex, pos);
	attributes[1].location = 1;
	attributes[1].binding = 0;
	attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[1].offset = offsetof(Vertex, color);

	VkPipelineVertexInputStateCreateInfo  vertexInput{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
	};
	vertexInput.vertexBindingDescriptionCount = 1;
	vertexInput.pVertexBindingDescriptions = &binding;
	vertexInput.vertexAttributeDescriptionCount  = 2;
	vertexInput.pVertexAttributeDescriptions = attributes;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
	};
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineViewportStateCreateInfo viewportState{
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO
	};
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
	};
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.lineWidth =  1.0F;

	VkPipelineMultisampleStateCreateInfo multisampling{
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO
	};
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState blendAttachment{};
	blendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	blendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlend{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO
	};

	colorBlend.attachmentCount = 1;
	colorBlend.pAttachments = &blendAttachment;

	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo dynamicState{
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO
	};
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	VkPipelineLayoutCreateInfo layoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
	//No descriptor sets yet -- texture sampling is the next milestone.
	if(vkCreatePipelineLayout(core.device.device,
		&layoutInfo, nullptr, &core.pipelineLayout) != VK_SUCCESS){
			fmt::print(stderr, "Failed to create pipline layout.\n");
			pauseBeforeExit();
			std::exit(1);
		}
	VkGraphicsPipelineCreateInfo  pipelineInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = stages ;
	pipelineInfo.pVertexInputState = &vertexInput ;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState =  &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlend;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = core.pipelineLayout;
	pipelineInfo.renderPass = core.renderPass;
	pipelineInfo.subpass = 0;

	if(vkCreateGraphicsPipelines(core.device.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &core.pipline) != VK_SUCCESS){
		fmt::print(stderr, "Failed to create graphics pipline.\n");
		pauseBeforeExit();
		std::exit(1);
	}
	vkDestroyShaderModule(core.device.device, vertModule, nullptr);
	vkDestroyShaderModule(core.device.device, fragModule, nullptr);
}

//Recored once at startup -- the draw doesn't change frame to frame yet.
static void recordCommandBuffers(VulkanCore& core){
		VkCommandPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
		poolInfo.queueFamilyIndex = core.graphicsQueueFamily;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		if(vkCreateCommandPool(core.device.device, &poolInfo, nullptr, &core.commandPool) != VK_SUCCESS){
			fmt::print(stderr, "Failed to create command pool.\n");
			pauseBeforeExit();
			std::exit(1);
		}
		core.commandBuffers.resize(core.framebuffers.size());
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
		VkClearValue clearColor{};
		clearColor.color = {{0.04F, 0.05F, 0.09F, 1.0F}};
		VkExtent2D extent = core.swapchain.extent;
		VkViewport viewport{0.0f, 0.0F, static_cast<float>(extent.width),
			static_cast<float>(extent.height), 0.0f, 1.0F};
		VkRect2D scissor{{0,0}, extent};


		for(size_t i = 0; i < core.commandBuffers.size(); ++i){
			VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};

			vkBeginCommandBuffer(core.commandBuffers[i], &beginInfo);

			VkRenderPassBeginInfo rpBegin{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
			rpBegin.renderPass = core.renderPass;
			rpBegin.framebuffer = core.framebuffers[i];
			rpBegin.renderArea = scissor;
			rpBegin.clearValueCount = 1;
			rpBegin.pClearValues = &clearColor;

			vkCmdBeginRenderPass(core.commandBuffers[i], &rpBegin, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(core.commandBuffers[i],  VK_PIPELINE_BIND_POINT_GRAPHICS, core.pipline);
			vkCmdSetViewport(core.commandBuffers[i], 0, 1, &viewport);
			vkCmdSetScissor(core.commandBuffers[i], 0, 1, &scissor);

			VkBuffer vertexBuffers[] = {core.vertexBuffer};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(core.commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(core.commandBuffers[i], core.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
			vkCmdDrawIndexed(core.commandBuffers[i], static_cast<uint32_t>(kIndices.size()), 1, 0, 0, 0);

			vkCmdEndRenderPass(core.commandBuffers[i]);
			vkEndCommandBuffer(core.commandBuffers[i]);
		}

		VkSemaphoreCreateInfo semInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
		VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO}; // start signlaed so frame 1 doesn't stall
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start signaled so frame 1 doesn't stall
		vkCreateSemaphore(core.device.device, &semInfo, nullptr, &core.imageAvailableSemaphore);
		vkCreateSemaphore(core.device.device, &semInfo, nullptr, &core.renderFinishedSemaphore);
		vkCreateFence(core.device.device, &fenceInfo, nullptr, &core.inFlightFence);
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

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
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

	vkDestroySemaphore(core.device.device, core.imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(core.device.device, core.renderFinishedSemaphore, nullptr);
	vkDestroyFence(core.device.device, core.inFlightFence,nullptr);
	vkDestroyCommandPool(core.device.device, core.commandPool, nullptr);//frees command buffer too

	vkDestroyBuffer(core.device.device, core.vertexBuffer, nullptr);
	vkFreeMemory(core.device.device, core.vertexBufferMemory, nullptr);
	vkDestroyBuffer(core.device.device, core.indexBuffer, nullptr);
	vkFreeMemory(core.device.device, core.indexBufferMemory, nullptr);

	vkDestroyPipeline(core.device.device, core.pipline, nullptr);
	vkDestroyPipelineLayout(core.device.device, core.pipelineLayout, nullptr);
	for(auto fb : core.framebuffers) vkDestroyFramebuffer(core.device.device, fb, nullptr);
	vkDestroyRenderPass(core.device.device, core.renderPass, nullptr);

	core.swapchain.destroy_image_views(core.swapchainImagesViews);
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

	createGeometryBuffers(vk);
	createRenderPass(vk);
	createFramebuffer(vk);
	createPipeline(vk);
	recordCommandBuffers(vk);
	fmt::print("Command Buffers recorded -- enterning render loop.\n");

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		drawFrame(vk);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}
