#include "Common.h"
namespace VulkanCookbook {
	bool IsExtensionSupported(const std::vector<VkExtensionProperties>& availableExtensions,
							  const char* const extension) {
		return (std::find_if(availableExtensions.cbegin(), availableExtensions.cend(), 
				[&extension](const VkExtensionProperties& e)
				{return strstr(e.extensionName, extension) != nullptr; }) != availableExtensions.end());
	}
}