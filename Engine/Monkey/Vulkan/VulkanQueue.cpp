#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "VulkanFence.h"
#include "VulkanCommandBuffer.h"

VulkanQueue::VulkanQueue(VulkanDevice* device, uint32 familyIndex)
    : m_Queue(VK_NULL_HANDLE)
    , m_FamilyIndex(familyIndex)
    , m_Device(device)
    , m_SubmitCounter(0)
{
    vkGetDeviceQueue(m_Device->GetInstanceHandle(), m_FamilyIndex, 0, &m_Queue);
}

VulkanQueue::~VulkanQueue()
{
    
}

void VulkanQueue::Submit(VulkanCmdBuffer* cmdBuffer, uint32 numSignalSemaphores = 0, VkSemaphore* signalSemaphores = nullptr)
{
    VulkanFence* fence = cmdBuffer->GetFence();
    const VkCommandBuffer cmdBuffers[] = { cmdBuffer->GetHandle() };
    
    VkSubmitInfo submitInfo;
    ZeroVulkanStruct(submitInfo, VK_STRUCTURE_TYPE_SUBMIT_INFO);
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = cmdBuffers;
    submitInfo.signalSemaphoreCount = numSignalSemaphores;
    submitInfo.pSignalSemaphores    = signalSemaphores;
    
    std::vector<VkSemaphore> waitSemaphores;
    if (cmdBuffer->GetWaitSemaphores.size() > 0)
    {
        waitSemaphores.resize(cmdBuffer->GetWaitSemaphores().size());
        for (int32 i = 0; i < cmdBuffer->GetWaitSemaphores().size(); ++i)
        {
            VulkanSemaphore* semaphore = cmdBuffer->GetWaitSemaphores()[i];
            waitSemaphores.push_back(semaphore->GetHandle());
        }
        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitSemaphores    = waitSemaphores.data();
        submitInfo.pWaitDstStageMask  = cmdBuffer->GetWaitFlags().data();
    }
    {
        SCOPE_CYCLE_COUNTER(STAT_VulkanQueueSubmit);
        VERIFYVULKANRESULT(VulkanRHI::vkQueueSubmit(Queue, 1, &SubmitInfo, Fence->GetHandle()));
    }
    
    vkQueueSubmit(m_Queue, 1, &submitInfo, fence->GetHandle());
    
    cmdBuffer->SetState(VulkanCmdBuffer::State::Submitted);
    cmdBuffer->MarkSemaphoresAsSubmitted();
    cmdBuffer->RefreshSubmittedFenceCounter();
    
    m_Device->GetFenceManager().WaitForFence(cmdBuffer->GetFence(), 200 * 1000 * 1000);
    cmdBuffer->GetOwner()->RefreshFenceStatus(cmdBuffer);
}
