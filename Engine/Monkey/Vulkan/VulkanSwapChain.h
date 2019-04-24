#pragma once

#include "VulkanPlatform.h"
#include "Common/Common.h"

#include <memory>
#include <vector>

class VulkanQueue;
class VulkanFence;
class VulkanDevice;

class VulkanSwapChain
{
public:

	enum class SwapStatus
	{
		Healthy     = 0,
		OutOfDate   = -1,
		SurfaceLost = -2,
	};

	VulkanSwapChain(VkInstance instance, std::shared_ptr<VulkanDevice> device, PixelFormat& outPixelFormat, uint32 width, uint32 height, uint32* outDesiredNumBackBuffers, std::vector<VkImage>& outImages, int8 lockToVsync);

	virtual ~VulkanSwapChain();

	SwapStatus Present(std::shared_ptr<VulkanQueue> gfxQueue, std::shared_ptr<VulkanQueue> presentQueue, VkSemaphore* complete);

	int32 AcquireImageIndex(VkSemaphore* outSemaphore);

	FORCEINLINE int8 DoesLockToVsync() 
	{ 
		return m_LockToVsync;
	}

	FORCEINLINE VkSwapchainKHR GetInstanceHandle()
	{
		return m_SwapChain;
	}
    
    FORCEINLINE int32 GetWidth() const
    {
        return m_SwapChainInfo.imageExtent.width;
    }

    FORCEINLINE int32 GetHeight() const
    {
        return m_SwapChainInfo.imageExtent.height;
    }
    
    FORCEINLINE int32 GetBackBufferCount() const
    {
        return m_SwapChainInfo.minImageCount;
    }
    
    const VkSwapchainCreateInfoKHR& GetInfo() const
    {
        return m_SwapChainInfo;
    }

protected:
	friend class VulkanViewport;
	friend class VulkanQueue;
    
protected:
	VkInstance						m_Instance;
	VkSwapchainKHR					m_SwapChain;
	VkSurfaceKHR					m_Surface;
	VkSwapchainCreateInfoKHR		m_SwapChainInfo;
	std::shared_ptr<VulkanDevice>	m_Device;
	std::vector<VkSemaphore>		m_ImageAcquiredSemaphore;

	int32							m_CurrentImageIndex;
	int32							m_SemaphoreIndex;
	uint32							m_NumPresentCalls;
	uint32							m_NumAcquireCalls;
	int8							m_LockToVsync;
	uint32							m_PresentID;
};
