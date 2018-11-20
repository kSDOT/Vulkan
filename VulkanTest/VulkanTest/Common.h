#pragma once

#ifdef _WIN32
#include <Windows.h>
#elif defined __linux
#include <dlfcn.h>
#endif

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <cstring>
#include <thread>
#include <cmath>
#include <functional>
#include <memory>
#include "VulkanFunctions.h"
namespace VulkanCookbook {
#ifdef _WIN32
#define LIBRARY_TYPE HMODULE
#elif defined __linux
#define LIBRARY_TYPE void*
#endif

	struct WindowParameters {
		#ifdef VK_USE_PLATFORM_WIN32_KHR
				HINSTANCE		  HInstance;
				HWND			  HWnd;
		#elif defined VK_USE_PLATFORM_XLIB_KHR
				Display*		  Dpy;
				Window			  Window;
		#elif defined VK_USE_PLATFORM_XCB_KHR
				xcb_connection_t* Connection;
				xcb_window_t	  Window;
		#endif
	};


	bool IsExtensionSupported(const std::vector<VkExtensionProperties>&,
							  const char* const);
}