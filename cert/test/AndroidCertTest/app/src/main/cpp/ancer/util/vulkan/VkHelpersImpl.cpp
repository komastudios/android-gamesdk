/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "VkHelpersImpl.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
#pragma ide diagnostic ignored "readability-magic-numbers"
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>

#include <android_native_app_glue.h>
#include <shaderc/shaderc.hpp>
#include <sys/time.h>

#include <ancer/util/Log.hpp>

using namespace ancer;
using namespace ancer::vulkan;


namespace {
    Log::Tag TAG{"VkHelpersImpl"};

    // Keeps ANativeWindow and asset manager instances.
    static android_app *AndroidApplication = nullptr;
}

void vulkan::ExtractVersion(uint32_t version, uint32_t &major, uint32_t &minor,
                    uint32_t &patch) {
    major = version >> 22;
    minor = (version >> 12) & 0x3ff;
    patch = version & 0xfff;
}

std::string vulkan::GetBaseDataDir() {
    return "";
}

namespace {
    std::string GetFileName(const std::string &s) {
        // TODO(tmillican@google.com): Duplicated code, should probably use
        //  std::filesystem anyway.
        static constexpr char sep = '/';
        if (size_t i = s.rfind(sep, s.length()) ; i != std::string::npos) {
            return s.substr(i + 1, s.length() - i);
        } else {
            return "";
        }
    }
}

std::string vulkan::GetDataDir(const std::string &filename) {
    std::string basedir = GetBaseDataDir();
    // get the base filename
    std::string fname = GetFileName(filename);

    // get the prefix of the base filename, i.e. the part before the dash
    std::stringstream stream(fname);
    std::string prefix;
    getline(stream, prefix, '-');
    std::string ddir = basedir + prefix;
    return ddir;
}

bool vulkan::MemoryTypeFromProperties(VulkanInfo &info, uint32_t typeBits,
                                      VkFlags requirementsMask, uint32_t *typeIndex) {
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < info.memoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((info.memoryProperties.memoryTypes[i].propertyFlags &
                 requirementsMask) == requirementsMask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

void vulkan::SetImageLayout(VulkanInfo &demo, VkImage image,
                            VkImageAspectFlags aspectMask,
                            VkImageLayout oldImageLayout,
                            VkImageLayout newImageLayout,
                            VkPipelineStageFlags srcStages,
                            VkPipelineStageFlags destStages) {
    /* DEPENDS on info.cmd and info.queue initialized */

    assert(demo.cmd != VK_NULL_HANDLE);
    assert(demo.graphicsQueue != VK_NULL_HANDLE);

    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext = nullptr;
    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = 0;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;

    switch (oldImageLayout) {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.srcAccessMask =
                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        default:
            break;
    }

    switch (newImageLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask =
                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask =
                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        default:
            break;
    }

    vkCmdPipelineBarrier(demo.cmd, srcStages, destStages, 0, 0, nullptr, 0,
                         nullptr,
                         1, &imageMemoryBarrier);
}

bool vulkan::ReadPpm(char const *const filename, int &width, int &height,
                     uint64_t rowPitch, unsigned char *dataPtr) {
    // PPM format expected from http://netpbm.sourceforge.net/doc/ppm.html
    //  1. magic number
    //  2. whitespace
    //  3. width
    //  4. whitespace
    //  5. height
    //  6. whitespace
    //  7. max color value
    //  8. whitespace
    //  7. data

    // Comments are not supported, but are detected and we kick out
    // Only 8 bits per channel is supported
    // If dataPtr is nullptr, only width and height are returned

    // Read in values from the PPM file as characters to check for comments
    char magicStr[3] = {};
    char heightStr[6] = {};
    char widthStr[6] = {};
    char formatStr[6] = {};

    FILE *fPtr = AndroidFopen(filename, "rb");
    if (!fPtr) {
        printf("Bad filename in ReadPpm: %s\n", filename);
        return false;
    }

    // Read the four values from file, accounting with any and all whitepace
    fscanf(fPtr, "%s %s %s %s ", magicStr, widthStr, heightStr, formatStr);

    // Kick out if comments present
    if (magicStr[0] == '#' || widthStr[0] == '#' || heightStr[0] == '#' ||
        formatStr[0] == '#') {
        printf("Unhandled comment in PPM file\n");
        return false;
    }

    // Only one magic value is valid
    if (strncmp(magicStr, "P6", sizeof(magicStr)) != 0) {
        printf("Unhandled PPM magic number: %s\n", magicStr);
        return false;
    }

    width = atoi(widthStr); // NOLINT(cert-err34-c)
    height = atoi(heightStr); // NOLINT(cert-err34-c)

    // Ensure we got something sane for width/height
    static constexpr int saneDimension = 32768;  //??
    if (width <= 0 || width > saneDimension) {
        printf("Width seems wrong.  Update ReadPpm if not: %u\n", width);
        return false;
    }
    if (height <= 0 || height > saneDimension) {
        printf("Height seems wrong.  Update ReadPpm if not: %u\n", height);
        return false;
    }

    if (dataPtr == nullptr) {
        // If no destination pointer, caller only wanted dimensions
        return true;
    }

    // Now read the data
    for (int y = 0; y < height; ++y) {
        unsigned char *rowPtr = dataPtr;
        for (int x = 0; x < width; ++x) {
            fread(rowPtr, 3, 1, fPtr);
            rowPtr[3] = 255; /* Alpha of 1 */
            rowPtr += 4;
        }
        dataPtr += rowPitch;
    }
    fclose(fPtr);

    return true;
}

void vulkan::InitGlslang() {}

void vulkan::FinalizeGlslang() {}

// Android specific helper functions for shaderc.
namespace {
    struct ShaderTypeMapping {
        VkShaderStageFlagBits vkShaderType;
        shaderc_shader_kind shadercType;
    };
    constexpr ShaderTypeMapping shaderMapTable[] = {
            {VK_SHADER_STAGE_VERTEX_BIT,   shaderc_glsl_vertex_shader},
            {VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
                                           shaderc_glsl_tess_control_shader},
            {VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
                                           shaderc_glsl_tess_evaluation_shader},
            {VK_SHADER_STAGE_GEOMETRY_BIT, shaderc_glsl_geometry_shader},
            {VK_SHADER_STAGE_FRAGMENT_BIT, shaderc_glsl_fragment_shader},
            {VK_SHADER_STAGE_COMPUTE_BIT,  shaderc_glsl_compute_shader},
    };

    shaderc_shader_kind MapShadercType(VkShaderStageFlagBits vkShader) {
        for (auto shader : shaderMapTable) {
            if (shader.vkShaderType == vkShader) {
                return shader.shadercType;
            }
        }
        assert(false);
        return shaderc_glsl_infer_from_source;
    }
}

//
// Compile a given string containing GLSL into SPV for use by VK
// Return value of false means an error was encountered.
//
bool vulkan::GLSLtoSPV(const VkShaderStageFlagBits shaderType, const char *pShader,
                       std::vector<unsigned int> &spirv) {
    shaderc::Compiler compiler;
    shaderc::SpvCompilationResult module =
            compiler.CompileGlslToSpv(pShader, strlen(pShader),
                                      MapShadercType(shaderType), "shader");
    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        Log::E(TAG, "Error: Id=%d, Msg=%s", module.GetCompilationStatus(),
               module.GetErrorMessage().c_str());
        return false;
    }
    spirv.assign(module.cbegin(), module.cend());
    return true;
}

void vulkan::WaitSeconds(int seconds) {
    sleep(static_cast<unsigned int>(seconds));
}

timestamp_t vulkan::GetMilliseconds() {
    timeval now{};
    gettimeofday(&now, nullptr);
    return (now.tv_usec / 1000) + (timestamp_t) now.tv_sec;
}

void vulkan::PrintUUID(uint8_t *pipelineCacheUUID) {
    for (int j = 0; j < VK_UUID_SIZE; ++j) {
        std::cout << std::setw(2) << (uint32_t) pipelineCacheUUID[j];
        if (j == 3 || j == 5 || j == 7 || j == 9) {
            std::cout << '-';
        }
    }
}

namespace {
    bool optionMatch(const char *option, char *optionLine) {
        return strncmp(option, optionLine, strlen(option)) == 0;
    }
}

void vulkan::ProcessCommandLineArgs(VulkanInfo &info, int argc, char **argv) {
    for (int i = 1, n = 1; i < argc; ++i) {
        if (optionMatch("--save-images", argv[i])) {
            info.saveImages = true;
        } else if (optionMatch("--help", argv[i]) || optionMatch("-h", argv[i])) {
            printf("\nOther options:\n");
            printf(
                    "\t--save-images\n"
                    "\t\tSave tests images as ppm files in current working "
                    "directory.\n");
            exit(0);
        } else {
            printf("\nUnrecognized option: %s\n", argv[i]);
            printf("\nUse --help or -h for option list.\n");
            exit(0);
        }

        /*
         * Since the above "consume" inputs, update argv
         * so that it contains the trimmed list of args for glutInit
         */

        argv[n++] = argv[i];
    }
}

void vulkan::WritePpm(VulkanInfo &info, const char *basename) {
    VkResult res; // Bleh, C-style result checking.

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = info.format;
    imageCreateInfo.extent.width = static_cast<uint32_t>(info.width);
    imageCreateInfo.extent.height = static_cast<uint32_t>(info.height);
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = kNumSamples;
    imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.flags = 0;

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = nullptr;
    memAlloc.allocationSize = 0;
    memAlloc.memoryTypeIndex = 0;

    VkImage mappableImage;
    VkDeviceMemory mappableMemory;

    /* Create a mappable image */
    res = vkCreateImage(info.device, &imageCreateInfo, nullptr,
                        &mappableImage);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(info.device, mappableImage, &memReqs);

    memAlloc.allocationSize = memReqs.size;

    /* Find the memory type that is host mappable */
    bool pass = MemoryTypeFromProperties(info, memReqs.memoryTypeBits,
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                         &memAlloc.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");

    /* allocate memory */
    res = vkAllocateMemory(info.device, &memAlloc, nullptr, &(mappableMemory));
    assert(res == VK_SUCCESS);

    /* bind memory */
    res = vkBindImageMemory(info.device, mappableImage, mappableMemory, 0);
    assert(res == VK_SUCCESS);

    VkCommandBufferBeginInfo cmdBufInfo = {};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;
    cmdBufInfo.flags = 0;
    cmdBufInfo.pInheritanceInfo = nullptr;

    vkBeginCommandBuffer(info.cmd, &cmdBufInfo);
    SetImageLayout(info, mappableImage, VK_IMAGE_ASPECT_COLOR_BIT,
                   VK_IMAGE_LAYOUT_UNDEFINED,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                   VK_PIPELINE_STAGE_TRANSFER_BIT);

    SetImageLayout(info, info.buffers[info.currentBuffer].image,
                   VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                   VK_PIPELINE_STAGE_TRANSFER_BIT);

    VkImageCopy copyRegion;
    copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.srcSubresource.mipLevel = 0;
    copyRegion.srcSubresource.baseArrayLayer = 0;
    copyRegion.srcSubresource.layerCount = 1;
    copyRegion.srcOffset.x = 0;
    copyRegion.srcOffset.y = 0;
    copyRegion.srcOffset.z = 0;
    copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.dstSubresource.mipLevel = 0;
    copyRegion.dstSubresource.baseArrayLayer = 0;
    copyRegion.dstSubresource.layerCount = 1;
    copyRegion.dstOffset.x = 0;
    copyRegion.dstOffset.y = 0;
    copyRegion.dstOffset.z = 0;
    copyRegion.extent.width = static_cast<uint32_t>(info.width);
    copyRegion.extent.height = static_cast<uint32_t>(info.height);
    copyRegion.extent.depth = 1;

    /* Put the copy command into the command buffer */
    vkCmdCopyImage(info.cmd, info.buffers[info.currentBuffer].image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, mappableImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

    SetImageLayout(info, mappableImage, VK_IMAGE_ASPECT_COLOR_BIT,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   VK_IMAGE_LAYOUT_GENERAL,
                   VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT);

    res = vkEndCommandBuffer(info.cmd);
    assert(res == VK_SUCCESS);
    const VkCommandBuffer cmdBufs[] = {info.cmd};
    VkFenceCreateInfo fenceInfo;
    VkFence cmdFence;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;
    vkCreateFence(info.device, &fenceInfo, nullptr, &cmdFence);

    VkSubmitInfo submitInfo[1] = {};
    submitInfo[0].pNext = nullptr;
    submitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo[0].waitSemaphoreCount = 0;
    submitInfo[0].pWaitSemaphores = nullptr;
    submitInfo[0].pWaitDstStageMask = nullptr;
    submitInfo[0].commandBufferCount = 1;
    submitInfo[0].pCommandBuffers = cmdBufs;
    submitInfo[0].signalSemaphoreCount = 0;
    submitInfo[0].pSignalSemaphores = nullptr;

    /* Queue the command buffer for execution */
    res = vkQueueSubmit(info.graphicsQueue, 1, submitInfo, cmdFence);
    assert(res == VK_SUCCESS);

    /* Make sure command buffer is finished before mapping */
    do {
        res = vkWaitForFences(info.device, 1, &cmdFence, VK_TRUE,
                              kFenceTimeout);
    } while (res == VK_TIMEOUT);
    assert(res == VK_SUCCESS);

    vkDestroyFence(info.device, cmdFence, nullptr);

    std::string filename{basename};
    filename.append(".ppm");

    VkImageSubresource subres = {};
    subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subres.mipLevel = 0;
    subres.arrayLayer = 0;
    VkSubresourceLayout srLayout;
    vkGetImageSubresourceLayout(info.device, mappableImage, &subres,
                                &srLayout);

    char *ptr;
    res = vkMapMemory(info.device, mappableMemory, 0, memReqs.size, 0,
                      (void **) &ptr);
    assert(res == VK_SUCCESS);

    ptr += srLayout.offset;
    std::ofstream file(filename, std::ios::binary);

    file << "P6\n";
    file << info.width << " ";
    file << info.height << "\n";
    file << 255 << "\n";

    for (int y = 0; y < info.height; ++y) {
        const int *row = (const int *) ptr;
        int swapped;

        if (info.format == VK_FORMAT_B8G8R8A8_UNORM ||
            info.format == VK_FORMAT_B8G8R8A8_SRGB) {
            for (int x = 0; x < info.width; ++x) {
                swapped = (*row & 0xff00ff00) | (*row & 0x000000ff) << 16 |
                          (*row & 0x00ff0000) >> 16;
                file.write((char *) &swapped, 3);
                row++;
            }
        } else if (info.format == VK_FORMAT_R8G8B8A8_UNORM) {
            for (int x = 0; x < info.width; ++x) {
                file.write((char *) row, 3);
                row++;
            }
        } else {
            printf("Unrecognized image format - will not write image files");
            break;
        }

        ptr += srLayout.rowPitch;
    }

    file.close();
    vkUnmapMemory(info.device, mappableMemory);
    vkDestroyImage(info.device, mappableImage, nullptr);
    vkFreeMemory(info.device, mappableMemory, nullptr);
}

std::string vulkan::GetFileDirectory() {
    assert(AndroidApplication != nullptr);
    return AndroidApplication->activity->externalDataPath;
}

//
// Android specific helper functions.
//

ANativeWindow *vulkan::AndroidGetApplicationWindow() {
    assert(AndroidApplication != nullptr);
    return AndroidApplication->window;
}

namespace {
    bool AndroidFillShaderMap(const char *path,
                              std::unordered_map<std::string,
                                                 std::string> *mapShaders) {
        assert(AndroidApplication != nullptr);
        auto directory = AAssetManager_openDir(
                AndroidApplication->activity->assetManager, path);

        const char *name = nullptr;
        while (true) {
            name = AAssetDir_getNextFileName(directory);
            if (name == nullptr) {
                break;
            }

            std::string fileName = name;
            if (fileName.find(".frag") != std::string::npos ||
                fileName.find(".vert") != std::string::npos) {
                // Add path to the filename.
                auto pathName = std::string(path);
                pathName += "/";
                pathName += fileName;
                std::string shader;
                if (!AndroidLoadFile(pathName.c_str(), &shader)) {
                    continue;
                }
                // Remove \n to make the lookup more robust.
                while (true) {
                    auto retPos = shader.find('\n');
                    if (retPos == std::string::npos) {
                        break;
                    }
                    shader.erase(retPos, 1);
                }

                auto pos = pathName.find_last_of('.');
                if (pos == std::string::npos) {
                    // Invalid file nmae.
                    continue;
                }
                // Generate filename for SPIRV binary.
                std::string spirvName = pathName.replace(pos, 1, "-") + ".spirv";
                // Store the SPIRV file name with GLSL contents as a key.
                // The file contents can be long, but as we are using unordered map,
                // it wouldn't take much storage space.
                // Put the file into the map.
                (*mapShaders)[shader] = spirvName;
            }
        };

        AAssetDir_close(directory);
        return true;
    }
}

bool vulkan::AndroidLoadFile(const char *filePath, std::string *data) {
    assert(AndroidApplication != nullptr);
    AAsset *file = AAssetManager_open(
            AndroidApplication->activity->assetManager, filePath,
            AASSET_MODE_BUFFER);
    auto fileLength = static_cast<size_t>(AAsset_getLength(file));
    Log::I(TAG, "Loaded file:%s size:%zu", filePath, fileLength);
    if (fileLength == 0) {
        return false;
    }
    data->resize(fileLength);
    AAsset_read(file, &(*data)[0], fileLength);
    return true;
}

void vulkan::AndroidGetWindowSize(int32_t *width, int32_t *height) {
    // On Android, retrieve the window size from the native window.
    assert(AndroidApplication != nullptr);
    assert(width && height);
    *width = ANativeWindow_getWidth(AndroidApplication->window);
    *height = ANativeWindow_getHeight(AndroidApplication->window);
}

// Android fopen stub described at
// http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer/#comment-1850768990
namespace {
    int AndroidRead(void *cookie, char *buf, int size) {
        return AAsset_read((AAsset *) cookie, buf, static_cast<size_t>(size));
    }

    int AndroidWrite(void *, const char *, int) {
        return EACCES;  // can't provide write access to the apk
    }

    fpos_t AndroidSeek(void *cookie, fpos_t offset, int whence) {
        return AAsset_seek((AAsset *) cookie, offset, whence);
    }

    int AndroidClose(void *cookie) {
        AAsset_close((AAsset *) cookie);
        return 0;
    }
}

FILE *vulkan::AndroidFopen(const char *fname, const char *mode) {
    if (mode[0] == 'w') {
        return nullptr;
    }

    assert(AndroidApplication != nullptr);
    AAsset *asset = AAssetManager_open(
            AndroidApplication->activity->assetManager, fname, 0);
    if (!asset) {
        return nullptr;
    }

    return funopen(asset, AndroidRead, AndroidWrite, AndroidSeek, AndroidClose);
}

#pragma clang diagnostic pop