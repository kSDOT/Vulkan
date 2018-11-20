#include "vkapp.h"

namespace VulkanCookbook {
	bool vkapp::LoadFunctionExportedFromVulkanLoaderLibrary(LIBRARY_TYPE const & vulkan_library){
		#if defined _WIN32
		#define LoadFunction GetProcAddress
		#elif defined __linux
		#define LoadFunction dlsym
		#endif
		#define EXPORTED_VULKAN_FUNCTION( name )                              \
		    name = (PFN_##name)LoadFunction( vulkan_library, #name );         \
		    if( name == nullptr ) {                                           \
		      std::cout << "Could not load exported Vulkan function named: "  \
		        << #name << std::endl;                                        \
		      return false;                                                   \
		    }
		#include "ListOfVulkanFunctions.inl"		
		return true;
	}
	bool vkapp::ConnectWithVulkanLoaderLibrary(LIBRARY_TYPE & vulkan_library){
	#if defined _WIN32
		vulkan_library = LoadLibrary(L"vulkan-1.dll");
	#elif defined __linux
		vulkan_library = dlopen("libvulkan.so.1", RLTD_NOW);
	#endif
		if (!vulkan_library) {
			std::cout << "Could not connect with a Vulkan Runtime library." << std::endl;
			return false;
		}
		return true;
	}
	bool vkapp::LoadGlobalLevelFunctions() {
		#define	GLOBAL_LEVEL_VULKAN_FUNCTION( name )								\
		name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr( nullptr, #name));\
		if(!name){																	\
			std::cout << "Could not load global-level function named: "				\
				<< #name << std::endl;   											\
				return false;														\
		}
		#include "ListOfVulkanFunctions.inl"
		return true;
	}
	bool vkapp::CheckAvailableInstanceExtensions(std::vector<VkExtensionProperties>& availableExtensions){
		uint32_t extensionsCount;
		VkResult result = VK_SUCCESS;

		result = vkEnumerateInstanceExtensionProperties( nullptr, &extensionsCount, nullptr);
		if ((result != VK_SUCCESS) || (extensionsCount == 0)) {
			std::cout << "Could not get the number of Instance extensions." << std::endl;
			return false;
		}
		availableExtensions.resize(extensionsCount);
		result = vkEnumerateInstanceExtensionProperties( nullptr, &extensionsCount,
			availableExtensions.data());
		if ((result != VK_SUCCESS) || (extensionsCount == 0)) {
			std::cout << "Could not enumerate Instance extensions." << std::endl;
			return false;
		}
		return true;
	}
	bool vkapp::CreateVulkanInstance(std::vector<char const *> const& desiredExtensions, const char* const applicationName, 
									 VkInstance& instance) {
		std::vector<VkExtensionProperties> availableExtensions;
		if (!CheckAvailableInstanceExtensions(availableExtensions))
			return false;
		for (auto& extension : desiredExtensions) 
			if (!IsExtensionSupported(availableExtensions, extension)) {
				std::cout << "Extension named '" << extension << "' is not supported." << std::endl;
				return false;
			}
		
		VkApplicationInfo applicationInfo = {
			VK_STRUCTURE_TYPE_APPLICATION_INFO,	//sType
			nullptr,							//pNext
			"My app",							//pApplicationName
			VK_MAKE_VERSION(0, 0, 0),			//applicationVersion
			"My engine",						//pEngineName
			VK_MAKE_VERSION(1, 0, 0),			//engineVersion
			VK_MAKE_VERSION(1, 0, 0)			//apiVersion
		};
		VkInstanceCreateInfo instanceCreateInfo = {
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,			//sType
			nullptr,										//pNext
			0,												//flags
			&applicationInfo,								//pApplicationInfo
			0,												//enabledLayerCount
			nullptr,										//ppEnabledLayerNames
			static_cast<uint32_t>(desiredExtensions.size()),//enabledExtensionsCount
			desiredExtensions.size() > 0 ?					//ppEnabledExtensionsNames
			desiredExtensions.data() : nullptr		
		};
		
		if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance)!= VK_SUCCESS) {
			std::cout << "Error creating instance!" << std::endl;
			return false;
		}
		return true;
	}
	bool vkapp::LoadInstanceLevelFunctions(VkInstance instance, const std::vector<const char*>& enabledExtensions) {
		#define INSTANCE_LEVEL_VULKAN_FUNCTION( name )								   \
		name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr( instance , #name ));\
		if(name == nullptr){														   \
				std::cout << "Could not load instance-level Vulkan function named: "   \
						  << #name << std::endl;									   \
				return false;														   \
		}

		#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )        \
		    for( auto & enabled_extension : enabledExtensions ) {                       \
		      if( std::string( enabled_extension ) == std::string( extension ) ) {      \
		        name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );            \
		        if( name == nullptr ) {                                                 \
		          std::cout << "Could not load instance-level Vulkan function named: "  \
		            #name << std::endl;                                                 \
		          return false;                                                         \
		        }                                                                       \
		      }                                                                         \
		    }

		#include "ListOfVulkanFunctions.inl"
		return true;
	}
	bool vkapp::EnumerateAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice>& availableDevices) {
		uint32_t devicesCount = 0;
		VkResult result = VK_NOT_READY;
		if (result = vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);
		result != VK_SUCCESS || devicesCount == 0) {
			std::cout << "Could not get the number of available physical devices." << std::endl;
			return false;
		}
		availableDevices.resize(devicesCount);
		if (result = vkEnumeratePhysicalDevices(instance, &devicesCount, availableDevices.data()); 
		result != VK_SUCCESS || devicesCount == 0){
			std::cout << "Could not enumerate physical devices." << std::endl;
			return false;
		}
		return true;
	}
	bool vkapp::checkAvailableDeviceExtensions(VkPhysicalDevice physicalDevice, 
											   std::vector<VkExtensionProperties>& availableExtensions){
		uint32_t extensionsCount = 0;
		VkResult result = VK_NOT_READY;
		if (result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr);
		result != VK_SUCCESS || extensionsCount == 0) {
			std::cout << "Could not get the number of available device extension properties" << std::endl;
			return false;
		}
		availableExtensions.resize(extensionsCount);
		if (result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, availableExtensions.data());
		result != VK_SUCCESS || extensionsCount == 0) {
			std::cout << "Could not enumerate device extension properties." << std::endl;
			return false;
		}
		return false;
	}
	void vkapp::getFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physicalDevice, 
														 VkPhysicalDeviceFeatures& deviceFeatures, 
														 VkPhysicalDeviceProperties& deviceProperties){
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	}
	bool vkapp::checkAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice physicalDevice, 
															  std::vector<VkQueueFamilyProperties>& queueFamilies){
		uint32_t queueFamiliesCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
		if (queueFamiliesCount == 0) {
			std::cout << "Error getting queue family count." << std::endl;
			return false;
		}
		queueFamilies.resize(queueFamiliesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, queueFamilies.data());
		if (queueFamiliesCount == 0) {
			std::cout << "Error getting queue family count." << std::endl;
			return false;
		}

		return true;
	}
	bool vkapp::selectIndexOfQueueFamilyWithDesiredCapabilities(VkPhysicalDevice physicalDevice, VkQueueFlags desiredFlags,
		uint32_t& queueFamilyIndex) {
		std::vector<VkQueueFamilyProperties> queueFamilies;
		if(!checkAvailableQueueFamiliesAndTheirProperties(physicalDevice, queueFamilies))
			return false;

		for (uint32_t index = 0; index < static_cast<uint32_t>(queueFamilies.size()); ++index)
			if ((queueFamilies[index].queueCount > 0) && (desiredFlags & queueFamilies[index].queueFlags)){
				queueFamilyIndex = index;
				return true;
			}

		return false;
	}

	bool vkapp::createLogicalDevice(VkPhysicalDevice physicalDevice, 
									std::vector<QueueInfo> queueInfos,
									const std::vector<const char*>& desiredExtensions, 
									VkPhysicalDeviceFeatures* desiredFeatures,
									VkDevice logicalDevice) {
		std::vector<VkExtensionProperties> availableExtensions;
		if (!checkAvailableDeviceExtensions(physicalDevice, availableExtensions))
			return false;

		for (auto& extension : desiredExtensions)
			if (!IsExtensionSupported(availableExtensions, extension)) {
				std::cout << "Extension named '" << extension << "' is not supported by a physical device." << std::endl;
				return  false;
			}
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		for (auto& info : queueInfos)
			queueCreateInfos.push_back({
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,   //sType
				nullptr,									  //pNext
				0,											  //flags
				info.FamilyIndex,							  //queueFamilyIndex
				static_cast<uint32_t>(info.Priorities.size()),//queueCount
				info.Priorities.data()						  //pQueuePriorities
			});

		VkDeviceCreateInfo deviceCreateInfo = {
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,		    //sType
			nullptr,										//pNext
			0,												//flags
			static_cast<uint32_t>(queueCreateInfos.size()), //queueCreateInfoCount
			queueCreateInfos.data(),					    //pQueueCreateInfos
			0,												//enabledLayerCount
			nullptr,										//ppEnabledLayerNames
			static_cast<uint32_t>(desiredExtensions.size()),//enabledExtensionCount
			desiredExtensions.data(),						//ppenabledExtensionCount
			desiredFeatures									//pEnabledFeatures
		};
		if (VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);
		result != VK_SUCCESS) {
			std::cout << "Error creating logical device!" << std::endl;
			return false;
		}

		return false;
	}

	bool vkapp::loadDeviceLevelFunctions(VkDevice logicalDevice,
		const std::vector<const char*>& enabledExtensions) {

			#define DEVICE_LEVEL_VULKAN_FUNCTION( name )							    \
			name  = (PFN_##name)vkGetDeviceProcAddr( logicalDevice, #name );			\
			if( name == nullptr){														\
					std::cout << "Could not load device-level Vulkan function named: "	\
							  << #name << std::endl;									\
						return false;													\
			}

			#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )				\
			for(auto& enabledExtensions : enabledExtensions)									\
				if (std::string(enabledExtensions) == std::string(extension)) {					\
					name = (PFN_##name)vkGetDeviceProcAddr( logicalDevice, #name );				\
					if(name == nullptr){														\
							std::cout << "Could not load device-level Vulkan function named: "	\
									  << #name << std::endl;									\
							return false;														\
					}																			\
				}																				
			
		#include "ListOfVulkanFunctions.inl"
		return true;
	}

	void vkapp::getDeviceQueue(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue& queue){
		vkGetDeviceQueue( logicalDevice, queueFamilyIndex, queueIndex, &queue);
	}

	bool vkapp::createLogicalDeviceWithGeometryShadersAndGraphicsAndComputeQueues(VkInstance	instance, 
																				  VkDevice& logicalDevice, 
																				  VkQueue&	graphicsQueue, 
																				  VkQueue&	computeQueue){
		std::vector<VkPhysicalDevice> physicalDevices;
		if (!EnumerateAvailablePhysicalDevices(instance, physicalDevices))
			return false;
		for (auto physicalDevice : physicalDevices) {
			VkPhysicalDeviceFeatures deviceFeatures;
			VkPhysicalDeviceProperties deviceProperties;
			getFeaturesAndPropertiesOfPhysicalDevice(physicalDevice, deviceFeatures, deviceProperties);
			if (deviceFeatures.geometryShader == VK_TRUE) {
				deviceFeatures = {};
				deviceFeatures.geometryShader = VK_TRUE;
			}
			else
				continue;
			uint32_t graphicsQueueFamilyIndex, computeQueueFamilyIndex;
			if (!selectIndexOfQueueFamilyWithDesiredCapabilities(physicalDevice, VK_QUEUE_GRAPHICS_BIT, graphicsQueueFamilyIndex))
				continue;
			if (!selectIndexOfQueueFamilyWithDesiredCapabilities(physicalDevice, VK_QUEUE_COMPUTE_BIT, computeQueueFamilyIndex))
				continue;

			std::vector<QueueInfo> requestedQueues = { {graphicsQueueFamilyIndex, { 1.0f }} };
			if (graphicsQueueFamilyIndex != computeQueueFamilyIndex)
				requestedQueues.push_back({ computeQueueFamilyIndex, { 1.0f } });
			if (!createLogicalDevice(physicalDevice, requestedQueues, {}, &deviceFeatures, logicalDevice))
				continue;
			else{
				if (!loadDeviceLevelFunctions(logicalDevice, {}))
					return false;
				getDeviceQueue(logicalDevice, graphicsQueueFamilyIndex, 0, graphicsQueue);
				getDeviceQueue(logicalDevice, computeQueueFamilyIndex, 0, computeQueue);
				return true;
			}
		}
		return false;
	}
	void vkapp::destroyLogicalDevice(VkDevice& logicalDevice){
		if (logicalDevice) {
			vkDestroyDevice(logicalDevice, nullptr);
			logicalDevice = VK_NULL_HANDLE;
		}
	}

	void vkapp::destroyInstance(VkInstance& instance) {
		if (instance) {
			vkDestroyInstance(instance, nullptr);
			instance = VK_NULL_HANDLE;
		}
	}

	void vkapp::releaseVulkanLoaderLibrary(LIBRARY_TYPE& vulkanLibrary) {
		if (vulkanLibrary) {
			#if defined _WIN32
				FreeLibrary(vulkanLibrary);
			#elif defined __linux 
					dlclose(vulkanLibrary);
			#endif // _WIN32
				vulkanLibrary = nullptr;
		}
	}

	bool vkapp::createVulkanInstanceWithExtensionsEnabled(std::vector<const char*>& desiredExtensions, const char* const applicationName,
														   VkInstance& instance){
		desiredExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
		desiredExtensions.emplace_back(
										#ifdef VK_USE_PLATFORM_WIN32_KHR
											VK_KHR_WIN32_SURFACE_EXTENSION_NAME
										#elif defined VK_USE_PLATFORM_XCB_KHR
											VK_KHR_XCB_SURFACE_EXTENSION_NAME
										#elif defined VK_USE_PLATFORM_XLIB_KHR
											VK_KHR_XLIB_SURFACE_EXTENSION_NAME
										#endif	
									  );

		return CreateVulkanInstance( desiredExtensions, applicationName, instance);
	}

	bool vkapp::createPresentationSurface(VkInstance instance, WindowParameters windowParameters,
		VkSurfaceKHR& presentationSurface) {
		VkResult result;

		#ifdef VK_USE_PLATFORM_WIN32_KHR
			VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
				VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR, //sType
				nullptr,										 //pNext
				0,												 //flags
				windowParameters.HInstance,						 //hinstance
				windowParameters.HWnd							 //hwnd
			};
			result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);
		#elif defined VK_USE_PLATFORM_XLIB_KHR
			VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {
				VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR, //sType
				nullptr,										//pNext
				0,												//flags
				windowParameters.Dpy,							//dpy
				windowParameters.Window							//window
			};
			result = vkCreateXlibSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);
		#elif defined VK_USE_PLATFORM_XCB_KHR
			VkXcbSurfaceCreateInfoKHRf surfaceCreateInfo = {
				VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR //sType
				nullptr,									  //pNext
				0,											  //flags
				windowParameters.Connection,				  //connection
				windowParameters.Window						  //window
		}
			result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);
		#endif 
		if( (VK_SUCCESS != result) ||
		(VK_NULL_HANDLE == presentationSurface)) 
			std::cout << "Could not create presentation surface." << std::endl;
			return false;
		
		return true;
	}

	bool vkapp::selectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice physicalDevice, 
																		VkSurfaceKHR	 presentationSurface, 
																		uint32_t&		 queueFamilyIndex){
		std::vector<VkQueueFamilyProperties> queueFamilies;
		if (!checkAvailableQueueFamiliesAndTheirProperties(physicalDevice, queueFamilies))
			return false;
		for (int index = 0; index < static_cast<int>(queueFamilies.size()); ++index) {
			VkBool32 presentationSupported = VK_FALSE;
			VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, 
																	presentationSurface, &presentationSupported);
			if (result == VK_SUCCESS && presentationSupported == VK_TRUE) {
				queueFamilyIndex = index;
				return true;		
			}
		}
		return false;
	}

	bool vkapp::createLogicalDeviceWithWsiExtensionsEnables(VkPhysicalDevice physicalDevice, 
															std::vector<QueueInfo> queueInfos, 
															std::vector<const char*>& desiredExtensions, 
															VkPhysicalDeviceFeatures* desiredFeatures, 
															VkDevice& logicalDevice){
		desiredExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		return createLogicalDevice(physicalDevice, queueInfos, desiredExtensions, desiredFeatures, logicalDevice);
	}

	bool vkapp::selectDesiredPresentationMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, 
											  VkPresentModeKHR desiredPresentMode, VkPresentModeKHR& presentMode){
		uint32_t presentModesCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentModesCount, nullptr);
		std::vector<VkPresentModeKHR> presentModes(presentModesCount);
		if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface,
		&presentModesCount, presentModes.data()); result != VK_SUCCESS || 
												  presentModesCount == 0) {
			std::cout << "Could not enumerate Present modes " << std::endl;
			return false;
		}
		if (find(presentModes.begin(), presentModes.end(), desiredPresentMode) == presentModes.end()) 
			if (find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_FIFO_KHR) == presentModes.end()) {
				std::cout << "VK_PRESENT_MODE_FIFO_KHR not supported, even why its mandatory!" << std::endl;
				return false;
			}
			else {
				presentMode = VK_PRESENT_MODE_FIFO_KHR;
				return true;
			}
		
		presentMode = desiredPresentMode;
		return true;
	}

	bool vkapp::getCapabilitiesOfPresentationSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, 
													 VkSurfaceCapabilitiesKHR& surfaceCapabilities){
		return (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, presentationSurface, &surfaceCapabilities)
				==VK_SUCCESS);
	}
	bool vkapp::selectNumberOfSwapchainImages(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, uint32_t& numberOfImages){
		numberOfImages = surfaceCapabilities.minImageCount + 1;
		if (uint32_t maxImgCount = surfaceCapabilities.maxImageCount; maxImgCount > 0 && 
			numberOfImages > maxImgCount)
				numberOfImages = maxImgCount;
		return true;
	}

	bool vkapp::chooseSizeOfSwapchainImages(const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
															 VkExtent2D& sizeOfImages){
		if (surfaceCapabilities.currentExtent.width == static_cast<uint32_t>(-1)) {
			
			uint32_t maxWidth = surfaceCapabilities.maxImageExtent.width,
			         maxHeight = surfaceCapabilities.maxImageExtent.height,
			         minWidth = surfaceCapabilities.minImageExtent.width,
			         minHeight = surfaceCapabilities.minImageExtent.height;

			sizeOfImages = {sizeOfImages.width = std::clamp(static_cast<uint32_t>(1920), minWidth, maxWidth),
							sizeOfImages.height = std::clamp(static_cast<uint32_t>(1080), minHeight, maxHeight)};
			//unchecked code;	
		}
		else
			sizeOfImages = surfaceCapabilities.currentExtent;
		return true;
	}

	bool vkapp::selectDesiredUsageScenariosOfSwapchainImages(const VkSurfaceCapabilitiesKHR &, VkImageUsageFlags, VkImageUsageFlags){
		return false;
	}


}