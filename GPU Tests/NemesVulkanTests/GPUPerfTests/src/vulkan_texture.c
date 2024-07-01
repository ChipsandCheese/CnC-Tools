// Copyright (c) 2021 - 2022, Nemes <nemes@nemez.net>
// SPDX-License-Identifier: MIT
// 
// MIT License
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software andassociated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, andto permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice andthis permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "vulkan_helper.h"
#include "logger.h"
#include "vulkan_memory.h"
#include "vulkan_shader.h"
#include "vulkan_compute_pipeline.h"
#include "vulkan_command_buffer.h"
#include "vulkan_texture.h"

#ifdef VULKAN_TEXTURE_TRACE
#define TRACE_TEXTURE(format, ...)   TRACE("[TEXTURE] " format, __VA_ARGS__)
#else
#define TRACE_TEXTURE(format, ...)
#endif

static test_status _VulkanTextureTransitionLayout(vulkan_texture *texture_handle, VkImageLayout layout);

test_status VulkanTexturePrepareForCopy(vulkan_texture *texture_handle) {
    return _VulkanTextureTransitionLayout(texture_handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
}

test_status VulkanTexturePrepareForRender(vulkan_texture *texture_handle) {
    return _VulkanTextureTransitionLayout(texture_handle, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

static test_status _VulkanTextureTransitionLayout(vulkan_texture *texture_handle, VkImageLayout layout) {
    vulkan_command_buffer_singlerun command_buffer;

    test_status status = VulkanCommandBufferStartSingle(texture_handle->memory_pool->device, &command_buffer);
    TEST_RETFAIL(status);

    status = VulkanCommandBufferTransitionImageLayout(&(command_buffer.sequence), texture_handle, layout);
    if (!TEST_SUCCESS(status)) {
        VulkanCommandBufferAbortSingle(&command_buffer);
        return status;
    }
    status = VulkanCommandBufferFinishSingle(&command_buffer, VULKAN_COMMAND_SEQUENCE_WAIT_INFINITE);
    TEST_RETFAIL(status);
    texture_handle->current_layout = layout;
    return status;
}