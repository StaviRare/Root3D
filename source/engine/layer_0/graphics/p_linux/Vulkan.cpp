#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>
#include <wayland-client.h>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

#include "Vulkan.h"
#include "Log.h"
#include "WaylandHelpers.h"

void Vulkan::Initialize(void* windowHandle)
{
    bool noIssue = true;
    WindowHandle* handle = static_cast<WindowHandle*>(windowHandle);
    bool handleInvalid = !handle || !handle->display || !handle->surface;

    if (handleInvalid)
    {
        ENGINE_ERROR("Vulkan::Initialize failed: invalid display or surface.");
        noIssue = false;
    }

    // Create instance
    if(noIssue){
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Root3D";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Root3D";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        const char* instanceExtensions[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
        };

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = 2;
        instanceInfo.ppEnabledExtensionNames = instanceExtensions;
        VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_instance);

        if (result != VK_SUCCESS)
        {
            ENGINE_ERROR("Failed to create Vulkan instance.");
            noIssue = false;
        }
    }

    // Create surface
    if(noIssue){
        VkWaylandSurfaceCreateInfoKHR surfaceInfo{};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.display = handle->display;
        surfaceInfo.surface = handle->surface;
        VkResult result
            = vkCreateWaylandSurfaceKHR(m_instance, &surfaceInfo, nullptr, &m_surface);

        if (result != VK_SUCCESS)
        {
            ENGINE_ERROR("Failed to create Vulkan Wayland surface.");
            noIssue = false;
        }
    }

    // Pick physical device
    if(noIssue){
        uint32_t deviceCount = 0;
        VkResult result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
        bool noPhysicalDevice = result != VK_SUCCESS || deviceCount == 0;

        if (noPhysicalDevice)
        {
            ENGINE_ERROR("No Vulkan physical devices found.");
            noIssue = false;
        }
        else{
            std::vector<VkPhysicalDevice> devices(deviceCount);
            result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

            if (result != VK_SUCCESS)
            {
                ENGINE_ERROR("Failed to enumerate Vulkan physical devices.");
                noIssue = false;
            }
            else{
                bool candidateFound = false;
                for (VkPhysicalDevice candidate : devices)
                {
                    const uint32_t graphics = FindGraphicsFamily(candidate);
                    const uint32_t present = FindPresentFamily(candidate);
                    candidateFound = graphics != UINT32_MAX
                                     && present != UINT32_MAX
                                     && HasSwapchainExtension(candidate);

                    if (candidateFound)
                    {
                        m_physicalDevice = candidate;
                        m_graphicsFamily = graphics;
                        m_presentFamily = present;
                        break;
                    }
                }

                if(candidateFound == false){
                    ENGINE_ERROR("No Vulkan device supports graphics, presentation and swapchains.");
                    noIssue = false;
                }
            }
        }
    }

    // Create logical device
    if(noIssue){
        float priority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queues;
        std::vector<uint32_t> families = { m_graphicsFamily, m_presentFamily };
        std::sort(families.begin(), families.end());
        families.erase(std::unique(families.begin(), families.end()), families.end());

        for (uint32_t family : families)
        {
            VkDeviceQueueCreateInfo queue{};
            queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue.queueFamilyIndex = family;
            queue.queueCount = 1;
            queue.pQueuePriorities = &priority;
            queues.push_back(queue);
        }

        const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        VkPhysicalDeviceFeatures features{};
        VkDeviceCreateInfo deviceInfo{};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queues.size());
        deviceInfo.pQueueCreateInfos = queues.data();
        deviceInfo.pEnabledFeatures = &features;
        deviceInfo.enabledExtensionCount = 1;
        deviceInfo.ppEnabledExtensionNames = deviceExtensions;
        VkResult result = vkCreateDevice(m_physicalDevice, &deviceInfo, nullptr, &m_device);

        if (result != VK_SUCCESS)
        {
            ENGINE_ERROR("Failed to create Vulkan logical device.");
            noIssue = false;
        }
        else{
            vkGetDeviceQueue(m_device, m_graphicsFamily, 0, &m_graphicsQueue);
            vkGetDeviceQueue(m_device, m_presentFamily, 0, &m_presentQueue);
        }
    }

    // Create command objects
    if(noIssue){
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = m_graphicsFamily;
        VkResult result = vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool);

        if (result != VK_SUCCESS)
        {
            ENGINE_ERROR("Failed to create Vulkan command pool.");
            noIssue = false;
        }
        else{
            VkCommandBufferAllocateInfo commandInfo{};
            commandInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandInfo.commandPool = m_commandPool;
            commandInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandInfo.commandBufferCount = 1;
            result = vkAllocateCommandBuffers(m_device, &commandInfo, &m_commandBuffer);

            if (result != VK_SUCCESS)
            {
                ENGINE_ERROR("Failed to allocate Vulkan command buffer.");
                noIssue = false;
            }
        }
    }

    // Create sync objects
    if(noIssue){
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkResult result1 = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailable);
        VkResult result2 = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinished);
        bool semaphoreCreationFailed = result1 != VK_SUCCESS || result2 != VK_SUCCESS;

        if (semaphoreCreationFailed)
        {
            ENGINE_ERROR("Failed to create Vulkan semaphores.");
            noIssue = false;
        }
        else{
            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            VkResult result3 = vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlight);

            if (result3 != VK_SUCCESS)
            {
                ENGINE_ERROR("Failed to create Vulkan fence.");
                noIssue = false;
            }
        }
    }

    // Create descriptor pool
    if(noIssue){
        VkDescriptorPoolSize poolSizes[2]{};
        poolSizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2000 };
        poolSizes[1] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 };

        VkDescriptorPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        info.maxSets = 1000;
        info.poolSizeCount = 2;
        info.pPoolSizes = poolSizes;
        VkResult result = vkCreateDescriptorPool(m_device, &info, nullptr, &m_descriptorPool);

        if (result != VK_SUCCESS)
        {
            ENGINE_ERROR("Failed to create Vulkan descriptor pool.");
            noIssue = false;
        }
    }

    // Create light buffer
    if(noIssue){
        m_lightBuffer = CreateBuffer({ nullptr, sizeof(VulkanLightBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT });
        VkResult result = vkMapMemory(m_device, m_lightBuffer.memory, 0, sizeof(VulkanLightBuffer), 0, &m_lightMapped);
        bool lightBufferCreationFailed = m_lightBuffer.buffer == VK_NULL_HANDLE || result != VK_SUCCESS;

        if (lightBufferCreationFailed)
        {
            ENGINE_ERROR("Failed to create Vulkan light buffer.");
            noIssue = false;
        }
    }

    // Create swapchain resources
    if(noIssue){
        CreateSwapchain();
        bool noSwapchainResources
            = m_swapchain == VK_NULL_HANDLE || m_renderPass == VK_NULL_HANDLE ||
              m_framebuffers.empty();

        if (noSwapchainResources)
        {
            ENGINE_ERROR("Failed to create Vulkan swapchain resources.");
            noIssue = false;
        }
    }

    if(noIssue){
        m_initialized = true;
        m_frameStarted = false;
        m_recreateRequested = false;
    }
    else{
        UnInitialize();
    }
}

void Vulkan::UnInitialize()
{
    if (m_device != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(m_device);

        for (auto& b : m_frameBuffers)
        {
            DestroyBuffer(b);
        }

        for (auto& entry : m_shaderMap)
        {
            DestroyShaderObjects(entry.second);
        }

        for (auto& entry : m_textureMap)
        {
            DestroyTextureObjects(entry.second);
        }

        DestroyBuffer(m_lightBuffer);
        DestroySwapchainResources();

        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
        vkDestroyFence(m_device, m_inFlight, nullptr);
        vkDestroySemaphore(m_device, m_imageAvailable, nullptr);
        vkDestroySemaphore(m_device, m_renderFinished, nullptr);
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        vkDestroyDevice(m_device, nullptr);
    }

    m_frameBuffers.clear();
    m_shaderMap.clear();
    m_textureMap.clear();

    if (m_instance != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    m_instance = VK_NULL_HANDLE;
    m_surface = VK_NULL_HANDLE;
    m_device = VK_NULL_HANDLE;
    m_physicalDevice = VK_NULL_HANDLE;
    m_graphicsQueue = VK_NULL_HANDLE;
    m_presentQueue = VK_NULL_HANDLE;
    m_graphicsFamily = UINT32_MAX;
    m_presentFamily = UINT32_MAX;
    m_swapchain = VK_NULL_HANDLE;
    m_renderPass = VK_NULL_HANDLE;
    m_descriptorPool = VK_NULL_HANDLE;
    m_inFlight = VK_NULL_HANDLE;
    m_imageAvailable = VK_NULL_HANDLE;
    m_renderFinished = VK_NULL_HANDLE;
    m_commandPool = VK_NULL_HANDLE;
    m_commandBuffer = VK_NULL_HANDLE;
    m_extent = { 800, 600 };
    m_imageIndex = 0;
    m_frameStarted = false;
    m_initialized = false;
    m_recreateRequested = false;
    m_currentFrame = {};
    m_lightMapped = nullptr;
}

void Vulkan::Resize(uint32_t width, uint32_t height)
{
    m_extent = { width, height };
    m_recreateRequested = true;
}

void Vulkan::OnSurfaceLost()
{
    // No need in linux
}

void Vulkan::OnSurfaceRecreated(void* windowHandle)
{
    // No need in linux
}

void Vulkan::BeginFrame(FrameUniform cmd)
{
    m_currentFrame = cmd;

    if (!m_initialized || m_frameStarted ||
        m_extent.width == 0 || m_extent.height == 0)
    {
        return;
    }

    if (m_recreateRequested)
    {
        RecreateSwapchain();
    }

    CheckVk(vkWaitForFences(m_device, 1, &m_inFlight, VK_TRUE, UINT64_MAX));

    // Update light buffer
    VulkanLightBuffer data{};
    const size_t frameLights = sizeof(m_currentFrame.lights) / sizeof(m_currentFrame.lights[0]);
    data.numLights = static_cast<uint32_t>(std::min<size_t>(frameLights, kMaxLights));
    for (uint32_t i = 0; i < data.numLights; ++i)
    {
        VulkanLight light{};
        const LightUniform& source = m_currentFrame.lights[i];
        light.type = static_cast<int32_t>(source.type);
        light.intensity = source.intensity;
        light.range = source.range;
        std::copy(source.color, source.color + 3, light.color);
        std::copy(source.direction, source.direction + 3, light.direction);
        std::copy(source.position, source.position + 3, light.position);
        std::copy(source.attenuation, source.attenuation + 3, light.attenuation);
        data.lights[i] = light;
    }
    std::memcpy(m_lightMapped, &data, sizeof(data));

    // Release prev frame resources
    for (auto& b : m_frameBuffers)
    {
        DestroyBuffer(b);
    }
    m_frameBuffers.clear();
    CheckVk(vkResetDescriptorPool(m_device, m_descriptorPool, 0));

    // Acquire image
    bool imageAquired = true;
    VkResult result = vkAcquireNextImageKHR(
        m_device, m_swapchain, UINT64_MAX,
        m_imageAvailable, VK_NULL_HANDLE, &m_imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        imageAquired = false;
    }

    if (result == VK_SUBOPTIMAL_KHR)
    {
        m_recreateRequested = true;
    }
    else
    {
        CheckVk(result);
    }

    if(imageAquired){
        CheckVk(vkResetFences(m_device, 1, &m_inFlight));
        CheckVk(vkResetCommandBuffer(m_commandBuffer, 0));

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        CheckVk(vkBeginCommandBuffer(m_commandBuffer, &beginInfo));

        // Begin render pass
        VkClearValue clearValues[2]{};
        std::copy(
            m_currentFrame.backgroundColor,
            m_currentFrame.backgroundColor + 4,
            clearValues[0].color.float32);
        clearValues[1].depthStencil = { 1.0f, 0 };
        VkRenderPassBeginInfo info{};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = m_renderPass;
        info.framebuffer = m_framebuffers[m_imageIndex];
        info.renderArea.extent = m_extent;
        info.clearValueCount = 2;
        info.pClearValues = clearValues;
        vkCmdBeginRenderPass(m_commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);

        m_frameStarted = true;
    }
}

void Vulkan::DrawObject(ObjectUniform cmd)
{
    if (m_initialized && m_frameStarted)
    {
        bool cmdValid = true;
        auto shaderIt = m_shaderMap.find(cmd.shaderHandle);
        auto textureIt = m_textureMap.find(cmd.textureHandle);

        if (shaderIt == m_shaderMap.end())
        {
            ENGINE_ERROR("Shader not found for DrawObject.");
            cmdValid = false;
        }

        if (textureIt == m_textureMap.end())
        {
            ENGINE_ERROR("A valid texture is required by this pipeline.");
            cmdValid = false;
        }

        if (cmd.mesh.vertices == nullptr || cmd.mesh.indices == nullptr ||
            cmd.mesh.verticesSize == 0 || cmd.mesh.indicesSize == 0)
        {
            ENGINE_ERROR("Invalid mesh data for DrawObject.");
            cmdValid = false;
        }

        if (cmdValid)
        {
            const VulkanShader& shader = shaderIt->second;
            const VulkanTexture& texture = textureIt->second;
            const auto& mesh = cmd.mesh;

            VulkanUniform uniform{};
            std::memcpy(uniform.model, cmd.modelMatrix, sizeof(uniform.model));
            std::memcpy(uniform.view, m_currentFrame.viewMatrix, sizeof(uniform.view));
            std::memcpy(uniform.projection, m_currentFrame.projectionMatrix, sizeof(uniform.projection));

            const size_t vertexCount = mesh.verticesSize / 3;

            VkBuffer vertexBuffer = UploadFrameBuffer(
                { mesh.vertices, mesh.verticesSize * sizeof(float),
                 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT });

            VkBuffer texCoordBuffer = UploadAttribute(
                { mesh.texCoords, mesh.texCoordsSize, vertexCount * 2 });

            VkBuffer normalBuffer = UploadAttribute(
                { mesh.normals, mesh.normalsSize, vertexCount * 3 });

            VkBuffer indexBuffer = UploadFrameBuffer(
                { mesh.indices, mesh.indicesSize * sizeof(int),
                 VK_BUFFER_USAGE_INDEX_BUFFER_BIT });

            VkBuffer uniformBuffer = UploadFrameBuffer(
                { &uniform, sizeof(uniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT });

            // Allocate descriptor set
            VkDescriptorSetAllocateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            info.descriptorPool = m_descriptorPool;
            info.descriptorSetCount = 1;
            info.pSetLayouts = &shader.descriptorSetLayout;
            VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

            if (vkAllocateDescriptorSets(m_device, &info, &descriptorSet) != VK_SUCCESS)
            {
                descriptorSet = VK_NULL_HANDLE;
            }

            // Final check
            cmdValid =
                vertexBuffer != VK_NULL_HANDLE &&
                texCoordBuffer != VK_NULL_HANDLE &&
                normalBuffer != VK_NULL_HANDLE &&
                indexBuffer != VK_NULL_HANDLE &&
                uniformBuffer != VK_NULL_HANDLE &&
                descriptorSet != VK_NULL_HANDLE;

            if (cmdValid)
            {
                const VkDescriptorBufferInfo matrixInfo{
                    uniformBuffer, 0, sizeof(VulkanUniform)
                };

                const VkDescriptorBufferInfo lightInfo{
                    m_lightBuffer.buffer, 0, sizeof(VulkanLightBuffer)
                };

                const VkDescriptorImageInfo imageInfo{
                    texture.sampler,
                    texture.imageView,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                };

                VkWriteDescriptorSet matrixWrite{};
                matrixWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                matrixWrite.dstSet = descriptorSet;
                matrixWrite.dstBinding = 0;
                matrixWrite.descriptorCount = 1;
                matrixWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                matrixWrite.pBufferInfo = &matrixInfo;

                VkWriteDescriptorSet imageWrite{};
                imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                imageWrite.dstSet = descriptorSet;
                imageWrite.dstBinding = 1;
                imageWrite.descriptorCount = 1;
                imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                imageWrite.pImageInfo = &imageInfo;

                VkWriteDescriptorSet lightWrite{};
                lightWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                lightWrite.dstSet = descriptorSet;
                lightWrite.dstBinding = 2;
                lightWrite.descriptorCount = 1;
                lightWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                lightWrite.pBufferInfo = &lightInfo;

                const VkWriteDescriptorSet writes[3] = {
                    matrixWrite,
                    imageWrite,
                    lightWrite
                };

                vkUpdateDescriptorSets(m_device, 3, writes, 0, nullptr);

                VkViewport viewport{};
                viewport.width = static_cast<float>(m_extent.width);
                viewport.height = static_cast<float>(m_extent.height);
                viewport.maxDepth = 1.0f;

                VkRect2D scissor{};
                scissor.extent = m_extent;

                const VkBuffer vertexBuffers[] = {
                    vertexBuffer,
                    texCoordBuffer,
                    normalBuffer
                };

                const VkDeviceSize offsets[] = { 0, 0, 0 };
                vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
                vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
                vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.pipeline);
                vkCmdBindVertexBuffers(m_commandBuffer, 0, 3, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(m_commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdBindDescriptorSets(m_commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,shader.pipelineLayout,0, 1,
                                        &descriptorSet, 0, nullptr);
                vkCmdDrawIndexed(m_commandBuffer,static_cast<uint32_t>(mesh.indicesSize),1, 0, 0, 0);
            }
            else
            {
                ENGINE_ERROR("Failed to prepare Vulkan draw resources.");
            }
        }
    }
}

void Vulkan::EndFrame()
{
    if (m_initialized && m_frameStarted)
    {
        vkCmdEndRenderPass(m_commandBuffer);
        CheckVk(vkEndCommandBuffer(m_commandBuffer));

        // Submit frame:
        VkPipelineStageFlags waitStage =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_imageAvailable;
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_renderFinished;

        CheckVk(vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlight));

        // Present frame:
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_renderFinished;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_swapchain;
        presentInfo.pImageIndices = &m_imageIndex;

        VkResult result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            m_recreateRequested = true;
        }
        else
        {
            CheckVk(result);
        }

        m_frameStarted = false;
    }
}

void Vulkan::DestroyShader(uniqueID id)
{
    auto it = m_shaderMap.find(id);

    if (it != m_shaderMap.end())
    {
        if (m_device != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(m_device);
            DestroyShaderObjects(it->second);
        }

        m_shaderMap.erase(it);
    }
}

void Vulkan::DestroyTexture(uniqueID id)
{
    auto it = m_textureMap.find(id);

    if (it != m_textureMap.end())
    {
        if (m_device != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(m_device);
            DestroyTextureObjects(it->second);
        }

        m_textureMap.erase(it);
    }
}

uniqueID Vulkan::CreateShader(const ShaderUpload data)
{
    if (m_device == VK_NULL_HANDLE)
    {
        ENGINE_ERROR("Cannot create shader: Vulkan device is not initialized.");
        return 0;
    }

    const std::vector<uint32_t> vertexCode
        = CompileShader(data.vertexCode, shaderc_vertex_shader);
    const std::vector<uint32_t> fragmentCode
        = CompileShader(data.fragmentCode, shaderc_fragment_shader);

    if (vertexCode.empty() || fragmentCode.empty())
    {
        return 0;
    }

    VulkanShader shader{};
    shader.vertexModule = CreateShaderModule(vertexCode);
    shader.fragmentModule = CreateShaderModule(fragmentCode);

    if (shader.vertexModule != VK_NULL_HANDLE &&
        shader.fragmentModule != VK_NULL_HANDLE &&
        CreateGraphicsPipeline(shader))
    {
        uniqueID id = ++m_nextShaderID;
        m_shaderMap.emplace(id, shader);
        return id;
    }

    DestroyShaderObjects(shader);
    return 0;
}

uniqueID Vulkan::CreateTexture(const TextureUpload data)
{
    if (m_device == VK_NULL_HANDLE || data.rawData == nullptr ||
        data.width == 0 || data.height == 0 ||
        m_commandPool == VK_NULL_HANDLE)
    {
        return 0;
    }

    // Create texture image
    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = VK_FORMAT_R8G8B8A8_UNORM;
    info.extent = { data.width, data.height, 1 };
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    const ImageAllocation allocation = CreateImage(info);
    VulkanTexture texture{};
    texture.image = allocation.image;
    texture.memory = allocation.memory;

    // Upload texture data
    bool uploadSucceeded = false;
    const VkDeviceSize imageSize = static_cast<VkDeviceSize>(data.width) * data.height * 4;
    VulkanBuffer staging = UploadBuffer({ data.rawData, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT });

    if (staging.buffer != VK_NULL_HANDLE)
    {
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = m_commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        uploadSucceeded = vkAllocateCommandBuffers(m_device, &allocateInfo, &commandBuffer) == VK_SUCCESS;

        if (uploadSucceeded)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            uploadSucceeded = vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS;

            if (uploadSucceeded)
            {
                TransitionImage({
                    commandBuffer, texture.image,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    0, VK_ACCESS_TRANSFER_WRITE_BIT,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
                });

                VkBufferImageCopy copyRegion{};
                copyRegion.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
                copyRegion.imageExtent = { data.width, data.height, 1 };

                vkCmdCopyBufferToImage(
                    commandBuffer, staging.buffer, texture.image,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion
                    );

                TransitionImage({
                    commandBuffer, texture.image,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
                });

                VkSubmitInfo submitInfo{};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &commandBuffer;

                uploadSucceeded = vkEndCommandBuffer(commandBuffer) == VK_SUCCESS;

                if (uploadSucceeded)
                {
                    uploadSucceeded = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) == VK_SUCCESS;

                    if (uploadSucceeded)
                    {
                        uploadSucceeded = vkQueueWaitIdle(m_graphicsQueue) == VK_SUCCESS;
                    }
                }
            }

            vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
        }


        DestroyBuffer(staging);
    }

    if (texture.image != VK_NULL_HANDLE && uploadSucceeded)
    {
        texture.imageView = CreateImageView(
            { texture.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT });

        // Create texture sampler
        VkSamplerCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.magFilter = VK_FILTER_LINEAR;
        info.minFilter = VK_FILTER_LINEAR;
        info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.maxAnisotropy = 1.0f;
        info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

        VkSampler sampler = VK_NULL_HANDLE;
        VkResult result = vkCreateSampler(m_device, &info, nullptr, &sampler);

        if (result == VK_SUCCESS)
        {
            texture.sampler = sampler;
        }
    }

    if (texture.imageView == VK_NULL_HANDLE || texture.sampler == VK_NULL_HANDLE)
    {
        ENGINE_ERROR("Failed to create Vulkan texture.");
        DestroyTextureObjects(texture);
        return 0;
    }

    uniqueID id = ++m_nextTextureID;
    m_textureMap.emplace(id, texture);
    return id;
}

bool Vulkan::HasSwapchainExtension(VkPhysicalDevice device)
{
    uint32_t count = 0;

    if (vkEnumerateDeviceExtensionProperties(
            device, nullptr, &count, nullptr) != VK_SUCCESS)
    {
        return false;
    }

    std::vector<VkExtensionProperties> extensions(count);

    if (vkEnumerateDeviceExtensionProperties(
            device, nullptr, &count, extensions.data()) != VK_SUCCESS)
    {
        return false;
    }

    return std::any_of(extensions.begin(), extensions.end(),
                       [](const VkExtensionProperties& e)
                       {
                           return std::strcmp(
                                      e.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0;
                       });
}

uint32_t Vulkan::FindGraphicsFamily(VkPhysicalDevice device)
{
    uint32_t returnValue = UINT32_MAX;
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

    for (uint32_t i = 0; i < families.size(); ++i)
    {
        if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            returnValue = i;
            break;
        }
    }

    return returnValue;
}

uint32_t Vulkan::FindPresentFamily(VkPhysicalDevice device)
{
    uint32_t returnValue = UINT32_MAX;
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

    for (uint32_t i = 0; i < count; ++i)
    {
        VkBool32 presentSupport = VK_FALSE;
        CheckVk(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport));

        if (presentSupport)
        {
            returnValue = i;
            break;
        }
    }

    return returnValue;
}

std::vector<uint32_t> Vulkan::CompileShader(const std::string& code, shaderc_shader_kind kind)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);

    const char* name =
        kind == shaderc_vertex_shader ? "vertex.glsl" : "fragment.glsl";

    shaderc::SpvCompilationResult result =
        compiler.CompileGlslToSpv(code, kind, name, options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        ENGINE_ERROR("Shader compilation failed:\n" + result.GetErrorMessage());
        return {};
    }

    return std::vector<uint32_t>(result.cbegin(), result.cend());
}

VkShaderModule Vulkan::CreateShaderModule(const std::vector<uint32_t>& code)
{
    VkShaderModuleCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = code.size() * sizeof(uint32_t);
    info.pCode = code.data();

    VkShaderModule module = VK_NULL_HANDLE;
    VkResult result = vkCreateShaderModule(m_device, &info, nullptr, &module);

    if (result != VK_SUCCESS)
    {
        ENGINE_ERROR("Failed to create Vulkan shader module: " + std::to_string(result));
        return VK_NULL_HANDLE;
    }

    return module;
}

bool Vulkan::CreateGraphicsPipeline(VulkanShader& shader)
{
    bool noIssue = true;
    VkDescriptorSetLayoutBinding bindings[3]{};

    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[2].descriptorCount = 1;
    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorInfo{};
    descriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorInfo.bindingCount = 3;
    descriptorInfo.pBindings = bindings;
    VkResult result
        = vkCreateDescriptorSetLayout(m_device, &descriptorInfo, nullptr, &shader.descriptorSetLayout);

    if (result != VK_SUCCESS)
    {
        ENGINE_ERROR("Failed to create Vulkan descriptor set layout.");
        noIssue = false;
    }

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &shader.descriptorSetLayout;

    if (vkCreatePipelineLayout(
            m_device, &layoutInfo, nullptr, &shader.pipelineLayout) != VK_SUCCESS)
    {
        ENGINE_ERROR("Failed to create Vulkan pipeline layout.");
        noIssue = false;
    }

    if (noIssue)
    {
        VkPipelineShaderStageCreateInfo stages[2]{};

        stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        stages[0].module = shader.vertexModule;
        stages[0].pName = "main";

        stages[1] = stages[0];
        stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        stages[1].module = shader.fragmentModule;

        VkVertexInputBindingDescription vertexBindings[3] = {
            { 0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX },
            { 1, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX },
            { 2, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX }
        };

        VkVertexInputAttributeDescription attributes[3] = {
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
            { 1, 1, VK_FORMAT_R32G32_SFLOAT, 0 },
            { 2, 2, VK_FORMAT_R32G32B32_SFLOAT, 0 }
        };

        VkPipelineVertexInputStateCreateInfo vertexInput{};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.vertexBindingDescriptionCount = 3;
        vertexInput.pVertexBindingDescriptions = vertexBindings;
        vertexInput.vertexAttributeDescriptionCount = 3;
        vertexInput.pVertexAttributeDescriptions = attributes;

        VkPipelineInputAssemblyStateCreateInfo assembly{};
        assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkPipelineViewportStateCreateInfo viewport{};
        viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport.viewportCount = 1;
        viewport.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo raster{};
        raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        raster.polygonMode = VK_POLYGON_MODE_FILL;
        raster.cullMode = VK_CULL_MODE_NONE;
        raster.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        raster.lineWidth = 1.0f;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

        VkPipelineColorBlendAttachmentState blendAttachment{};
        blendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo blending{};
        blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blending.attachmentCount = 1;
        blending.pAttachments = &blendAttachment;

        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicInfo{};
        dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicInfo.dynamicStateCount = 2;
        dynamicInfo.pDynamicStates = dynamicStates;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = stages;
        pipelineInfo.pVertexInputState = &vertexInput;
        pipelineInfo.pInputAssemblyState = &assembly;
        pipelineInfo.pViewportState = &viewport;
        pipelineInfo.pRasterizationState = &raster;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &blending;
        pipelineInfo.pDynamicState = &dynamicInfo;
        pipelineInfo.layout = shader.pipelineLayout;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.subpass = 0;

        VkResult pipelineCreationResult = vkCreateGraphicsPipelines(
            m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &shader.pipeline);

        if (pipelineCreationResult != VK_SUCCESS)
        {
            ENGINE_ERROR("Failed to create Vulkan graphics pipeline.");
            noIssue = false;
        }
    }

    return noIssue;
}

void Vulkan::DestroyShaderObjects(VulkanShader& shader)
{
    vkDestroyPipeline(m_device, shader.pipeline, nullptr);
    vkDestroyPipelineLayout(m_device, shader.pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(m_device, shader.descriptorSetLayout, nullptr);
    vkDestroyShaderModule(m_device, shader.vertexModule, nullptr);
    vkDestroyShaderModule(m_device, shader.fragmentModule, nullptr);

    shader.pipeline = VK_NULL_HANDLE;
    shader.pipelineLayout = VK_NULL_HANDLE;
    shader.descriptorSetLayout = VK_NULL_HANDLE;
    shader.vertexModule = VK_NULL_HANDLE;
    shader.fragmentModule = VK_NULL_HANDLE;
}

void Vulkan::DestroyTextureObjects(VulkanTexture& texture)
{
    vkDestroySampler(m_device, texture.sampler, nullptr);
    vkDestroyImageView(m_device, texture.imageView, nullptr);
    vkDestroyImage(m_device, texture.image, nullptr);
    vkFreeMemory(m_device, texture.memory, nullptr);

    texture.sampler = VK_NULL_HANDLE;
    texture.imageView = VK_NULL_HANDLE;
    texture.image = VK_NULL_HANDLE;
    texture.memory = VK_NULL_HANDLE;
}

void Vulkan::CheckVk(VkResult result)
{
    if (result != VK_SUCCESS)
    {
        ENGINE_ERROR("Vulkan operation failed: " + std::to_string(result));
    }
}

VkDeviceMemory Vulkan::AllocateMemory(const MemoryRequest& request)
{
    VkDeviceMemory returnValue = VK_NULL_HANDLE;
    uint32_t type = UINT32_MAX;
    VkMemoryPropertyFlags properties = request.properties;
    uint32_t typeBits = request.requirements.memoryTypeBits;
    VkPhysicalDeviceMemoryProperties memoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        if ((typeBits & (1u << i)) != 0 &&
            (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            type = i;
            break;
        }
    }

    if (type == UINT32_MAX)
    {
        returnValue = VK_NULL_HANDLE;
    }
    else{
        VkMemoryAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        info.allocationSize = request.requirements.size;
        info.memoryTypeIndex = type;
        VkResult result = vkAllocateMemory(m_device, &info, nullptr, &returnValue);

        if (result != VK_SUCCESS)
        {
            returnValue = VK_NULL_HANDLE;
        }
    }

    return returnValue;
}

VulkanBuffer Vulkan::CreateBuffer(const BufferData& source)
{
    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = source.size;
    info.usage = source.usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VulkanBuffer buffer{};

    if (vkCreateBuffer(m_device, &info, nullptr, &buffer.buffer) != VK_SUCCESS)
    {
        return {};
    }

    VkMemoryRequirements requirements{};
    vkGetBufferMemoryRequirements(m_device, buffer.buffer, &requirements);

    buffer.memory = AllocateMemory(
        { requirements,
         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT });

    if (buffer.memory == VK_NULL_HANDLE ||
        vkBindBufferMemory(m_device, buffer.buffer, buffer.memory, 0) != VK_SUCCESS)
    {
        DestroyBuffer(buffer);
        return {};
    }

    return buffer;
}

void Vulkan::DestroyBuffer(VulkanBuffer& buffer)
{
    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(m_device, buffer.buffer, nullptr);
        vkFreeMemory(m_device, buffer.memory, nullptr);
    }

    buffer.buffer = VK_NULL_HANDLE;
    buffer.memory = VK_NULL_HANDLE;
}

VulkanBuffer Vulkan::UploadBuffer(const BufferData& source)
{
    if (source.data == nullptr || source.size == 0)
    {
        return {};
    }

    VulkanBuffer buffer = CreateBuffer(source);

    if (buffer.buffer == VK_NULL_HANDLE)
    {
        return {};
    }

    void* mapped = nullptr;

    if (vkMapMemory(m_device, buffer.memory, 0, source.size, 0, &mapped) != VK_SUCCESS)
    {
        DestroyBuffer(buffer);
        return {};
    }

    std::memcpy(mapped, source.data, static_cast<size_t>(source.size));
    vkUnmapMemory(m_device, buffer.memory);
    return buffer;
}

VkBuffer Vulkan::UploadFrameBuffer(const BufferData& source)
{
    const VulkanBuffer buffer = UploadBuffer(source);

    if (buffer.buffer == VK_NULL_HANDLE)
    {
        return VK_NULL_HANDLE;
    }

    m_frameBuffers.push_back(buffer);
    return buffer.buffer;
}

VkBuffer Vulkan::UploadAttribute(const AttributeData& attribute)
{
    if (attribute.data != nullptr && attribute.count > 0)
    {
        return UploadFrameBuffer(
            { attribute.data, attribute.count * sizeof(float),
             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT });
    }

    const std::vector<float> zeros(attribute.fallbackCount, 0.0f);
    return UploadFrameBuffer(
        { zeros.data(), zeros.size() * sizeof(float),
         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT });
}

ImageAllocation Vulkan::CreateImage(const VkImageCreateInfo& info)
{
    ImageAllocation result{};

    if (vkCreateImage(m_device, &info, nullptr, &result.image) != VK_SUCCESS)
    {
        return {};
    }

    VkMemoryRequirements requirements{};
    vkGetImageMemoryRequirements(m_device, result.image, &requirements);

    result.memory = AllocateMemory({ requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT });

    if (result.memory == VK_NULL_HANDLE ||
        vkBindImageMemory(m_device, result.image, result.memory, 0) != VK_SUCCESS)
    {
        vkDestroyImage(m_device, result.image, nullptr);
        vkFreeMemory(m_device, result.memory, nullptr);
        return {};
    }

    return result;
}

VkImageView Vulkan::CreateImageView(const ImageViewInfo& info)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = info.image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = info.format;
    viewInfo.subresourceRange = { info.aspect, 0, 1, 0, 1 };

    VkImageView view = VK_NULL_HANDLE;

    if (vkCreateImageView(m_device, &viewInfo, nullptr, &view) != VK_SUCCESS)
    {
        ENGINE_ERROR("Failed to create Vulkan image view.");
        return VK_NULL_HANDLE;
    }

    return view;
}

void Vulkan::TransitionImage(const ImageTransition& transition)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = transition.from;
    barrier.newLayout = transition.to;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = transition.image;
    barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    barrier.srcAccessMask = transition.srcAccess;
    barrier.dstAccessMask = transition.dstAccess;

    vkCmdPipelineBarrier(
        transition.commandBuffer, transition.srcStage, transition.dstStage,
        0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Vulkan::DestroySwapchainResources()
{
    if (m_device != VK_NULL_HANDLE)
    {
        for (VkFramebuffer framebuffer : m_framebuffers)
        {
            vkDestroyFramebuffer(m_device, framebuffer, nullptr);
        }

        for (VkImageView imageView : m_imageViews)
        {
            vkDestroyImageView(m_device, imageView, nullptr);
        }

        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
        vkDestroyImageView(m_device, m_depthView, nullptr);
        vkDestroyImage(m_device, m_depthImage, nullptr);
        vkFreeMemory(m_device, m_depthMemory, nullptr);
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

        m_framebuffers.clear();
        m_imageViews.clear();
        m_images.clear();
        m_renderPass = VK_NULL_HANDLE;
        m_depthView = VK_NULL_HANDLE;
        m_depthImage = VK_NULL_HANDLE;
        m_depthMemory = VK_NULL_HANDLE;
        m_swapchain = VK_NULL_HANDLE;
    }
}

void Vulkan::CreateSwapchainImageViews()
{
    uint32_t count = 0;
    CheckVk(vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, nullptr));

    m_images.resize(count);
    CheckVk(vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, m_images.data()));

    m_imageViews.resize(count);

    for (uint32_t i = 0; i < count; ++i)
    {
        m_imageViews[i] = CreateImageView(
            { m_images[i], m_swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT });
    }
}

void Vulkan::CreateDepthResources()
{
    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = m_depthFormat;
    info.extent = { m_extent.width, m_extent.height, 1 };
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    const ImageAllocation depth = CreateImage(info);

    if (depth.image == VK_NULL_HANDLE)
    {
        ENGINE_ERROR("Failed to create Vulkan depth image.");
        return;
    }

    m_depthImage = depth.image;
    m_depthMemory = depth.memory;
    m_depthView = CreateImageView(
        { m_depthImage, m_depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT });
}

void Vulkan::CreateRenderPass()
{
    VkAttachmentDescription attachments[2]{};

    attachments[0].format = m_swapchainFormat;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachments[1] = attachments[0];
    attachments[1].format = m_depthFormat;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
    VkAttachmentReference depthReference{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorReference;
    subpass.pDepthStencilAttachment = &depthReference;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = dependency.srcStageMask;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 2;
    info.pAttachments = attachments;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;

    CheckVk(vkCreateRenderPass(m_device, &info, nullptr, &m_renderPass));
}

void Vulkan::CreateFramebuffers()
{
    m_framebuffers.resize(m_imageViews.size());

    for (size_t i = 0; i < m_imageViews.size(); ++i)
    {
        VkImageView attachments[2] = { m_imageViews[i], m_depthView };

        VkFramebufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass = m_renderPass;
        info.attachmentCount = 2;
        info.pAttachments = attachments;
        info.width = m_extent.width;
        info.height = m_extent.height;
        info.layers = 1;

        CheckVk(vkCreateFramebuffer(m_device, &info, nullptr, &m_framebuffers[i]));
    }
}


void Vulkan::CreateSwapchain()
{
    bool noIssue = true;
    VkSurfaceCapabilitiesKHR capabilities{};
    VkSurfaceFormatKHR selectedFormat{};

    CheckVk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        m_physicalDevice, m_surface, &capabilities));

    uint32_t formatCount = 0;
    CheckVk(vkGetPhysicalDeviceSurfaceFormatsKHR(
        m_physicalDevice, m_surface, &formatCount, nullptr));

    if (formatCount == 0)
    {
        ENGINE_ERROR("No Vulkan surface formats available.");
        noIssue = false;
    }

    if (noIssue)
    {
        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        CheckVk(vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_physicalDevice, m_surface, &formatCount, formats.data()));

        selectedFormat = formats[0];

        for (const auto& format : formats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                selectedFormat = format;
                break;
            }
        }
    }

    if (noIssue)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            m_extent = capabilities.currentExtent;
        }
        else
        {
            m_extent.width = std::clamp(
                m_extent.width,
                capabilities.minImageExtent.width,
                capabilities.maxImageExtent.width);

            m_extent.height = std::clamp(
                m_extent.height,
                capabilities.minImageExtent.height,
                capabilities.maxImageExtent.height);
        }

        uint32_t imageCount = capabilities.minImageCount + 1;

        if (capabilities.maxImageCount > 0)
        {
            imageCount = std::min(imageCount, capabilities.maxImageCount);
        }

        uint32_t queueFamilies[] = { m_graphicsFamily, m_presentFamily };

        VkSwapchainCreateInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.surface = m_surface;
        info.minImageCount = imageCount;
        info.imageFormat = selectedFormat.format;
        info.imageColorSpace = selectedFormat.colorSpace;
        info.imageExtent = m_extent;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.preTransform = capabilities.currentTransform;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        info.clipped = VK_TRUE;

        if (m_graphicsFamily != m_presentFamily)
        {
            info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            info.queueFamilyIndexCount = 2;
            info.pQueueFamilyIndices = queueFamilies;
        }
        else
        {
            info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        CheckVk(vkCreateSwapchainKHR(
            m_device, &info, nullptr, &m_swapchain));

        m_swapchainFormat = selectedFormat.format;

        if (m_swapchain != VK_NULL_HANDLE)
        {
            CreateSwapchainImageViews();
            CreateDepthResources();
            CreateRenderPass();
            CreateFramebuffers();
        }
    }
}


void Vulkan::RecreateSwapchain()
{
    CheckVk(vkDeviceWaitIdle(m_device));
    DestroySwapchainResources();
    CreateSwapchain();
    m_recreateRequested = false;
}