#pragma once
#include "Common.h"
namespace VulkanCookbook {
	class vkapp {
	 public:
	 private:
		struct QueueInfo {
			uint32_t		   FamilyIndex;
			std::vector<float> Priorities;
		};
		static bool LoadFunctionExportedFromVulkanLoaderLibrary(LIBRARY_TYPE const & vulkan_library);
		static bool ConnectWithVulkanLoaderLibrary(LIBRARY_TYPE& vulkan_library);
		static bool LoadGlobalLevelFunctions();
		static bool CheckAvailableInstanceExtensions(std::vector<VkExtensionProperties>& availableExtensions);
		static bool CreateVulkanInstance(std::vector<char const *> const& desiredExtensions, const char* const applicationName,
			VkInstance& instance);
		static bool LoadInstanceLevelFunctions(VkInstance, const std::vector<const char*>&);
		static bool EnumerateAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice>&);
		static bool checkAvailableDeviceExtensions(VkPhysicalDevice, std::vector<VkExtensionProperties>&);
		static void getFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice, VkPhysicalDeviceFeatures&,
															 VkPhysicalDeviceProperties&);
		static bool checkAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice, std::vector<VkQueueFamilyProperties>&);
		static bool selectIndexOfQueueFamilyWithDesiredCapabilities(VkPhysicalDevice, VkQueueFlags, uint32_t&);
		static bool createLogicalDevice(VkPhysicalDevice, std::vector<QueueInfo>, const std::vector<const char*>&, VkPhysicalDeviceFeatures*,
										VkDevice);
		static bool loadDeviceLevelFunctions(VkDevice, const std::vector<const char*>&);
		static void getDeviceQueue(VkDevice, uint32_t, uint32_t, VkQueue&);
		static bool createLogicalDeviceWithGeometryShadersAndGraphicsAndComputeQueues(VkInstance, VkDevice&, VkQueue&, VkQueue&);
		static void destroyLogicalDevice(VkDevice&);
		static void destroyInstance(VkInstance&);
		static void releaseVulkanLoaderLibrary(LIBRARY_TYPE&);
		static bool createVulkanInstanceWithExtensionsEnabled(std::vector<const char*>&, const char* const, VkInstance&);
		static bool createPresentationSurface(VkInstance, WindowParameters, VkSurfaceKHR&);
		static bool selectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice, VkSurfaceKHR, uint32_t&);
		static bool createLogicalDeviceWithWsiExtensionsEnables(VkPhysicalDevice, std::vector<QueueInfo>,
																std::vector<const char*>&, VkPhysicalDeviceFeatures*, VkDevice&);
		static bool	selectDesiredPresentationMode(VkPhysicalDevice, VkSurfaceKHR, VkPresentModeKHR, VkPresentModeKHR&);
		static bool getCapabilitiesOfPresentationSurface(VkPhysicalDevice, VkSurfaceKHR, VkSurfaceCapabilitiesKHR&);
		static bool selectNumberOfSwapchainImages(const VkSurfaceCapabilitiesKHR&, uint32_t&);
		static bool chooseSizeOfSwapchainImages(const VkSurfaceCapabilitiesKHR&, VkExtent2D&);
		static bool selectDesiredUsageScenariosOfSwapchainImages(const VkSurfaceCapabilitiesKHR&, VkImageUsageFlags, VkImageUsageFlags);
	};
}