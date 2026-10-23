#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <wayland-client.h>
#include <shaderc/shaderc.hpp>

#include "Types.h"
#include "IGraphicsAPI.h"

struct VulkanShader
{
    VkShaderModule vertexModule = VK_NULL_HANDLE;
    VkShaderModule fragmentModule = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
};

struct VulkanBuffer
{
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

struct VulkanUniform
{
    float model[16];
    float view[16];
    float projection[16];
};

struct VulkanTexture
{
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;
};

struct VulkanLight
{
    int32_t type;
    alignas(16) float color[3];
    float range;
    float intensity;
    alignas(16) float position[3];
    alignas(16) float direction[3];
    alignas(16) float attenuation[3];
};

struct BufferData
{
    const void* data;
    VkDeviceSize size;
    VkBufferUsageFlags usage;
};

struct AttributeData
{
    const float* data;
    size_t count;
    size_t fallbackCount;
};

struct MemoryRequest
{
    VkMemoryRequirements requirements;
    VkMemoryPropertyFlags properties;
};

struct ImageViewInfo
{
    VkImage image;
    VkFormat format;
    VkImageAspectFlags aspect;
};

struct ImageAllocation
{
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

struct ImageTransition
{
    VkCommandBuffer commandBuffer;
    VkImage image;
    VkImageLayout from;
    VkImageLayout to;
    VkAccessFlags srcAccess;
    VkAccessFlags dstAccess;
    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;
};

static constexpr uint32_t kMaxLights = 20; // must match MAX_LIGHTS in the lit shader

struct VulkanLightBuffer
{
    uint32_t numLights;
    VulkanLight lights[kMaxLights];
};

class Vulkan : public IGraphicsAPI
{
    public:
    void Initialize(void* windowHandle) override;
    void UnInitialize() override;
    void Resize(uint32_t width, uint32_t height) override;
    void OnSurfaceLost() override;
    void OnSurfaceRecreated(void* windowHandle) override;
    void BeginFrame(FrameUniform cmd) override;
    void DrawObject(ObjectUniform cmd) override;
    void EndFrame() override;
    void DestroyShader(uniqueID id) override;
    void DestroyTexture(uniqueID id) override;
    uniqueID CreateShader(const ShaderUpload data) override;
    uniqueID CreateTexture(const TextureUpload data) override;

    private:
    void CreateSwapchain();
    void RecreateSwapchain();
    void DestroySwapchainResources();
    bool HasSwapchainExtension(VkPhysicalDevice device);
    void CreateSwapchainImageViews();
    void CreateDepthResources();
    void CreateRenderPass();
    void CreateFramebuffers();
    void SubmitFrame();
    void PresentFrame();
    void CheckVk(VkResult result);
    void TransitionImage(const ImageTransition& transition);
    void DestroyTextureObjects(VulkanTexture& texture);
    void DestroyBuffer(VulkanBuffer& buffer);
    bool CreateGraphicsPipeline(VulkanShader& shader);
    void DestroyShaderObjects(VulkanShader& shader);
    VkDeviceMemory AllocateMemory(const MemoryRequest& request);
    VulkanBuffer CreateBuffer(const BufferData& source);
    VulkanBuffer UploadBuffer(const BufferData& source);
    VkBuffer UploadFrameBuffer(const BufferData& source);
    VkBuffer UploadAttribute(const AttributeData& attribute);
    ImageAllocation CreateImage(const VkImageCreateInfo& info);
    VkImageView CreateImageView(const ImageViewInfo& info);
    uint32_t FindGraphicsFamily(VkPhysicalDevice device);
    uint32_t FindPresentFamily(VkPhysicalDevice device);
    std::vector<uint32_t> CompileShader(const std::string& code, shaderc_shader_kind kind);
    VkShaderModule CreateShaderModule(const std::vector<uint32_t>& code);

    private:
    bool m_initialized = false;
    bool m_recreateRequested = false;
    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    uint32_t m_graphicsFamily = UINT32_MAX;
    uint32_t m_presentFamily = UINT32_MAX;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkFormat m_swapchainFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D m_extent = { 960, 540 };
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    std::vector<VkFramebuffer> m_framebuffers;
    VkImage m_depthImage = VK_NULL_HANDLE;
    VkDeviceMemory m_depthMemory = VK_NULL_HANDLE;
    VkImageView m_depthView = VK_NULL_HANDLE;
    VkFormat m_depthFormat = VK_FORMAT_D32_SFLOAT;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
    VkSemaphore m_imageAvailable = VK_NULL_HANDLE;
    VkSemaphore m_renderFinished = VK_NULL_HANDLE;
    VkFence m_inFlight = VK_NULL_HANDLE;
    uint32_t m_imageIndex = 0;
    bool m_frameStarted = false;
    VulkanBuffer m_lightBuffer{};
    void* m_lightMapped = nullptr;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    FrameUniform m_currentFrame{};
    std::vector<VulkanBuffer> m_frameBuffers;
    uniqueID m_nextShaderID = 0;
    uniqueID m_nextTextureID = 0;
    std::unordered_map<uniqueID, VulkanShader> m_shaderMap;
    std::unordered_map<uniqueID, VulkanTexture> m_textureMap;
};