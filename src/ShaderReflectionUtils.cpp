#include "reflect/ShaderReflection.hpp"

#include <fstream>

namespace nbl
{
    auto ShaderReflection::convertFormat(const SpvReflectFormat format) -> vk::Format
    {
        switch (format)
        {
            case SPV_REFLECT_FORMAT_UNDEFINED:              return vk::Format::eUndefined;
            case SPV_REFLECT_FORMAT_R16_UINT:               return vk::Format::eR16Uint;
            case SPV_REFLECT_FORMAT_R16_SINT:               return vk::Format::eR16Sint;
            case SPV_REFLECT_FORMAT_R16_SFLOAT:             return vk::Format::eR16Sfloat;
            case SPV_REFLECT_FORMAT_R16G16_UINT:            return vk::Format::eR16G16Uint;
            case SPV_REFLECT_FORMAT_R16G16_SINT:            return vk::Format::eR16G16Sint;
            case SPV_REFLECT_FORMAT_R16G16_SFLOAT:          return vk::Format::eR16G16Sfloat;
            case SPV_REFLECT_FORMAT_R16G16B16_UINT:         return vk::Format::eR16G16B16Uint;
            case SPV_REFLECT_FORMAT_R16G16B16_SINT:         return vk::Format::eR16G16B16Sint;
            case SPV_REFLECT_FORMAT_R16G16B16_SFLOAT:       return vk::Format::eR16G16B16Sfloat;
            case SPV_REFLECT_FORMAT_R16G16B16A16_UINT:      return vk::Format::eR16G16B16A16Uint;
            case SPV_REFLECT_FORMAT_R16G16B16A16_SINT:      return vk::Format::eR16G16B16A16Sint;
            case SPV_REFLECT_FORMAT_R16G16B16A16_SFLOAT:    return vk::Format::eR16G16B16A16Sfloat;
            case SPV_REFLECT_FORMAT_R32_UINT:               return vk::Format::eR32Uint;
            case SPV_REFLECT_FORMAT_R32_SINT:               return vk::Format::eR32Sint;
            case SPV_REFLECT_FORMAT_R32_SFLOAT:             return vk::Format::eR32Sfloat;
            case SPV_REFLECT_FORMAT_R32G32_UINT:            return vk::Format::eR32G32Uint;
            case SPV_REFLECT_FORMAT_R32G32_SINT:            return vk::Format::eR32G32Sint;
            case SPV_REFLECT_FORMAT_R32G32_SFLOAT:          return vk::Format::eR32G32Sfloat;
            case SPV_REFLECT_FORMAT_R32G32B32_UINT:         return vk::Format::eR32G32B32Uint;
            case SPV_REFLECT_FORMAT_R32G32B32_SINT:         return vk::Format::eR32G32B32Sint;
            case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:       return vk::Format::eR32G32B32Sfloat;
            case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:      return vk::Format::eR32G32B32A32Uint;
            case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:      return vk::Format::eR32G32B32A32Sint;
            case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:    return vk::Format::eR32G32B32A32Sfloat;
            case SPV_REFLECT_FORMAT_R64_UINT:               return vk::Format::eR64Uint;
            case SPV_REFLECT_FORMAT_R64_SINT:               return vk::Format::eR64Sint;
            case SPV_REFLECT_FORMAT_R64_SFLOAT:             return vk::Format::eR64Sfloat;
            case SPV_REFLECT_FORMAT_R64G64_UINT:            return vk::Format::eR64G64Uint;
            case SPV_REFLECT_FORMAT_R64G64_SINT:            return vk::Format::eR64G64Sint;
            case SPV_REFLECT_FORMAT_R64G64_SFLOAT:          return vk::Format::eR64G64Sfloat;
            case SPV_REFLECT_FORMAT_R64G64B64_UINT:         return vk::Format::eR64G64B64Uint;
            case SPV_REFLECT_FORMAT_R64G64B64_SINT:         return vk::Format::eR64G64B64Sint;
            case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:       return vk::Format::eR64G64B64Sfloat;
            case SPV_REFLECT_FORMAT_R64G64B64A64_UINT:      return vk::Format::eR64G64B64A64Uint;
            case SPV_REFLECT_FORMAT_R64G64B64A64_SINT:      return vk::Format::eR64G64B64A64Sint;
            case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:    return vk::Format::eR64G64B64A64Sfloat;
        }

        throw std::runtime_error("Unknown format: " + static_cast<int32_t>(format));
    }

    auto ShaderReflection::convertDescriptorType(const SpvReflectDescriptorType descriptorType) -> vk::DescriptorType
    {
        switch (descriptorType)
        {
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:                       return vk::DescriptorType::eSampler;
            case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:        return vk::DescriptorType::eCombinedImageSampler;
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:                 return vk::DescriptorType::eSampledImage;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:                 return vk::DescriptorType::eStorageImage;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:          return vk::DescriptorType::eUniformTexelBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:          return vk::DescriptorType::eStorageTexelBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:                return vk::DescriptorType::eUniformBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:                return vk::DescriptorType::eStorageBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:        return vk::DescriptorType::eUniformBufferDynamic;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:        return vk::DescriptorType::eStorageBufferDynamic;
            case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:              return vk::DescriptorType::eInputAttachment;
            case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:    return vk::DescriptorType::eAccelerationStructureKHR;
        }

        throw std::runtime_error("Unknown descriptor type: " + static_cast<int32_t>(descriptorType));
    }

    auto ShaderReflection::convertShaderStage(const SpvReflectShaderStageFlagBits shaderStage) -> vk::ShaderStageFlagBits
    {
        switch (shaderStage)
        {
            case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:                   return vk::ShaderStageFlagBits::eVertex;
            case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:     return vk::ShaderStageFlagBits::eTessellationControl;
            case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:  return vk::ShaderStageFlagBits::eTessellationEvaluation;
            case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:                 return vk::ShaderStageFlagBits::eGeometry;
            case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:                 return vk::ShaderStageFlagBits::eFragment;
            case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:                  return vk::ShaderStageFlagBits::eCompute;
            case SPV_REFLECT_SHADER_STAGE_TASK_BIT_EXT:                 return vk::ShaderStageFlagBits::eTaskEXT;
            case SPV_REFLECT_SHADER_STAGE_MESH_BIT_EXT:                 return vk::ShaderStageFlagBits::eMeshEXT;
            case SPV_REFLECT_SHADER_STAGE_RAYGEN_BIT_KHR:               return vk::ShaderStageFlagBits::eRaygenKHR;
            case SPV_REFLECT_SHADER_STAGE_ANY_HIT_BIT_KHR:              return vk::ShaderStageFlagBits::eAnyHitKHR;
            case SPV_REFLECT_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:          return vk::ShaderStageFlagBits::eClosestHitKHR;
            case SPV_REFLECT_SHADER_STAGE_MISS_BIT_KHR:                 return vk::ShaderStageFlagBits::eMissKHR;
            case SPV_REFLECT_SHADER_STAGE_INTERSECTION_BIT_KHR:         return vk::ShaderStageFlagBits::eIntersectionKHR;
            case SPV_REFLECT_SHADER_STAGE_CALLABLE_BIT_KHR:             return vk::ShaderStageFlagBits::eCallableKHR;
        }

        throw std::runtime_error("Unknown shader stage: " + static_cast<int32_t>(shaderStage));
    }

    auto ShaderReflection::getShaderNameFromFilePath(const std::string& filePath) -> std::string
    {
        const size_t lastSlash = filePath.find_last_of('\\');
        const size_t lastDot = filePath.find_last_of('.');
        return {
            std::begin(filePath) + (lastSlash != std::string::npos ? lastSlash : 0),
            std::begin(filePath) + (lastDot != std::string::npos ? lastDot : 0)
        };
    }

    auto ShaderReflection::loadShaderBytes(const std::string& filePath) -> std::vector<char>
    {
        std::ifstream file(filePath.c_str(), std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        const std::streamsize fileSize = file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
}