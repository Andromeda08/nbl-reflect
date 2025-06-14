#pragma once

#include <optional>
#include <string>
#include <vector>
#include <spirv_reflect.h>
#include <vulkan/vulkan.hpp>

namespace nbl
{
    struct ShaderReflectionDescriptorSet
    {
        uint32_t                                    set      = 0;
        std::vector<vk::DescriptorSetLayoutBinding> bindings = {};
    };

    struct ShaderReflectionVertexInput
    {
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
    };

    struct ShaderReflectionData
    {
        std::string                                 shaderName          = "Unknown Shader";
        std::string                                 sourceFile          = "Unknown Shader File";
        std::string                                 entryPoint          = "Unknown Shader Entry Point";
        vk::ShaderStageFlagBits                     shaderStage         = vk::ShaderStageFlagBits::eVertex;
        std::vector<ShaderReflectionDescriptorSet>  descriptorSets      = {};
        std::vector<vk::PushConstantRange>          pushConstants       = {};
        std::optional<ShaderReflectionVertexInput>  vertexInput         = std::nullopt;
        std::vector<char>                           shaderCode          = {};

        auto getShaderModuleCreateInfo()  const -> vk::ShaderModuleCreateInfo;
        auto getPipelineStageCreateInfo() const -> vk::PipelineShaderStageCreateInfo;
    };

    struct PipelineReflectionData
    {
        std::vector<ShaderReflectionDescriptorSet>  descriptorSets  = {};
        std::vector<vk::PushConstantRange>          pushConstants   = {};
        std::vector<ShaderReflectionData>           shaderData      = {};
    };

    class ShaderReflection
    {
    public:
        static ShaderReflectionData reflectShader(const std::string& filePath);

        static PipelineReflectionData reflectPipelineShaders(const std::vector<std::string>& filePaths);

    private:
        // ==============================
        // Resolve Shader data types
        // ==============================

        /**
         * @note When constructing a pipeline from multiple shaders the shader stages need to be correctly merged across descriptors.
         * @return List of Descriptor Sets found in the specified Shader.
         */
        static auto resolveDescriptorSets(const SpvReflectShaderModule& shaderModule) -> std::vector<ShaderReflectionDescriptorSet>;

        /**
         * @note When constructing a pipeline from multiple shaders the shader stages need to be correctly merged across push constants.
         * @return List of Push Constants found in the specified Shader.
         */
        static auto resolvePushConstants (const SpvReflectShaderModule& shaderModule) -> std::vector<vk::PushConstantRange>;

        /**
         * @note This method does NOT resolve binding descriptions and must be manually set.
         * @return List of VertexInputAttributeDescriptions found in the specified Shader.
         */
        static auto resolveVertexInput   (const SpvReflectShaderModule& shaderModule) -> ShaderReflectionVertexInput;

        // ==============================
        // Conversion Methods
        // ==============================
        static auto convertFormat        (SpvReflectFormat              format        ) -> vk::Format;
        static auto convertDescriptorType(SpvReflectDescriptorType      descriptorType) -> vk::DescriptorType;
        static auto convertShaderStage   (SpvReflectShaderStageFlagBits shaderStage   ) -> vk::ShaderStageFlagBits;

        // ==============================
        // Utilities
        // ==============================
        static auto getShaderNameFromFilePath(const std::string& filePath) -> std::string;
        static auto loadShaderBytes          (const std::string& filePath) -> std::vector<char>;
    };
}