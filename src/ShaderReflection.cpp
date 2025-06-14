#include "reflect/ShaderReflection.hpp"

#include <ranges>

namespace nbl
{
    auto ShaderReflectionData::getShaderModuleCreateInfo() const -> vk::ShaderModuleCreateInfo
    {
        return vk::ShaderModuleCreateInfo()
            .setPCode(reinterpret_cast<const uint32_t*>(shaderCode.data()))
            .setCodeSize(sizeof(char) * shaderCode.size());
    }

    auto ShaderReflectionData::getPipelineStageCreateInfo() const -> vk::PipelineShaderStageCreateInfo
    {
        return vk::PipelineShaderStageCreateInfo()
            .setStage(shaderStage)
            .setPName(entryPoint.c_str());
    }

    auto ShaderReflection::reflectShader(const std::string& filePath) -> ShaderReflectionData
    {
        const auto shaderBytes = loadShaderBytes(filePath);

        ShaderReflectionData result;
        result.sourceFile = filePath;
        result.shaderName = getShaderNameFromFilePath(filePath);
        result.shaderCode = shaderBytes;

        // Reflection
        SpvReflectShaderModule spvShaderModule;
        SpvReflectResult rflResult = spvReflectCreateShaderModule(shaderBytes.size() * sizeof(char), shaderBytes.data(), &spvShaderModule);
        if (rflResult != SPV_REFLECT_RESULT_SUCCESS)
        {
            throw std::runtime_error("Failed to create reflection shader module.");
        }

        result.entryPoint     = spvShaderModule.entry_point_name;
        result.shaderStage    = convertShaderStage(spvShaderModule.shader_stage);
        result.descriptorSets = resolveDescriptorSets(spvShaderModule);
        result.pushConstants  = resolvePushConstants(spvShaderModule);

        if (result.shaderStage == vk::ShaderStageFlagBits::eVertex)
        {
            result.vertexInput = resolveVertexInput(spvShaderModule);
        }

        spvReflectDestroyShaderModule(&spvShaderModule);

        return result;
    }

    auto ShaderReflection::reflectPipelineShaders(const std::vector<std::string>& filePaths) -> PipelineReflectionData
    {
        PipelineReflectionData result;

        for (const auto& path : filePaths)
        {
            result.shaderData.push_back(
                ShaderReflection::reflectShader(path)
            );
        }

        for (const auto& shaderData : result.shaderData)
        {
            for (const auto& descriptorSet : shaderData.descriptorSets)
            {
                auto set = std::ranges::find_if(result.descriptorSets, [&](const ShaderReflectionDescriptorSet& ds) -> bool {
                    return ds.set == descriptorSet.set;
                });

                if (std::end(result.descriptorSets) == set)
                {
                    result.descriptorSets.push_back(descriptorSet);
                }
                else
                {
                    for (const auto& binding : descriptorSet.bindings)
                    {
                        auto b = std::ranges::find_if(set->bindings, [&](const vk::DescriptorSetLayoutBinding& dlb) -> bool {
                            return dlb.binding == binding.binding;
                        });
                        if (b == std::end(set->bindings))
                        {
                            set->bindings.push_back(binding);
                        }
                    }

                    for (auto&& [resultSet, shaderSet] : std::views::zip(set->bindings, descriptorSet.bindings))
                    {
                        resultSet.stageFlags |= shaderSet.stageFlags;
                    }
                }
            }

            for (const auto& pushConstant : shaderData.pushConstants)
            {
                auto pc = std::ranges::find_if(result.pushConstants, [&](const vk::PushConstantRange& ps) -> bool {
                    return pushConstant.size == ps.size && pushConstant.offset == ps.offset;
                });

                if (std::end(result.pushConstants) == pc)
                {
                    result.pushConstants.push_back(pushConstant);
                }
                else
                {
                    pc->stageFlags |= pushConstant.stageFlags;
                }
            }
        }

        return result;
    }

    auto ShaderReflection::resolveDescriptorSets(const SpvReflectShaderModule& shaderModule) -> std::vector<ShaderReflectionDescriptorSet>
    {
        const uint32_t nDescriptorSets = shaderModule.descriptor_set_count;
        if (nDescriptorSets == 0)
        {
            return {};
        }

        // Get Descriptor Sets
        std::vector<ShaderReflectionDescriptorSet> result(nDescriptorSets);
        for (uint32_t set = 0; set < nDescriptorSets; ++set)
        {
            ShaderReflectionDescriptorSet& descriptorSet = result[set];
            const SpvReflectDescriptorSet spvDescriptorSet = shaderModule.descriptor_sets[set];

            // Get Bindings
            const uint32_t nBindings = spvDescriptorSet.binding_count;
            if (nBindings == 0)
            {
                throw std::runtime_error("No bindings found for descriptor set #" + set);
            }

            std::vector<vk::DescriptorSetLayoutBinding> bindings(nBindings);
            for (uint32_t b = 0; b < nBindings; ++b)
            {
                const SpvReflectDescriptorBinding* spvDescriptorBinding = spvDescriptorSet.bindings[b];
                bindings[b] = vk::DescriptorSetLayoutBinding()
                    .setDescriptorCount(spvDescriptorBinding->count)
                    .setBinding(spvDescriptorBinding->binding)
                    .setDescriptorType(convertDescriptorType(spvDescriptorBinding->descriptor_type))
                    .setStageFlags(convertShaderStage(shaderModule.shader_stage));
            }

            descriptorSet.set      = set;
            descriptorSet.bindings = bindings;
        }

        return result;
    }

    auto ShaderReflection::resolvePushConstants(const SpvReflectShaderModule& shaderModule) -> std::vector<vk::PushConstantRange>
    {
        const uint32_t nPushConstants = shaderModule.push_constant_block_count;
        if (nPushConstants == 0)
        {
            return {};
        }

        std::vector<vk::PushConstantRange> pushConstants(nPushConstants);
        for (uint32_t i = 0; i < nPushConstants; ++i)
        {
            const SpvReflectBlockVariable spvPushConstant = shaderModule.push_constant_blocks[i];
            pushConstants[i] = vk::PushConstantRange()
                .setStageFlags(convertShaderStage(shaderModule.shader_stage))
                .setOffset(spvPushConstant.offset)
                .setSize(spvPushConstant.size);
        }

        return pushConstants;
    }

    auto ShaderReflection::resolveVertexInput(const SpvReflectShaderModule& shaderModule) -> ShaderReflectionVertexInput
    {
        ShaderReflectionVertexInput result;

        // Input Variables
        const uint32_t nInputVars = shaderModule.input_variable_count;
        for (uint32_t i = 0; i < nInputVars; ++i)
        {
            const SpvReflectInterfaceVariable* spvInputVar = shaderModule.input_variables[i];
            const auto attrib = vk::VertexInputAttributeDescription()
                .setLocation(spvInputVar->location)
                .setFormat(convertFormat(spvInputVar->format));
            result.attributeDescriptions.push_back(attrib);
        }

        return result;
    }
}
