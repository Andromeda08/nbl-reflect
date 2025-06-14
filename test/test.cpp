#include <iostream>
#include <ranges>
#include <stdexcept>
#include <vector>
#include <reflect/ShaderReflection.hpp>

int main()
{
    const std::vector<std::string> shaderFiles = { /* Specify Shader Files Here */ };

    try
    {
        const auto data = nbl::ShaderReflection::reflectPipelineShaders(shaderFiles);

        vk::ShaderStageFlags allStages;
        for (const auto& shader : data.shaderData)
        {
            allStages |= shader.shaderStage;
        }
        std::cout << "[Pipeline | " << to_string(allStages) << "]" << std::endl;

        std::cout << "\t-[Shader Stages: " << data.shaderData.size() << "]" << std::endl;
        for (const auto& shader : data.shaderData)
        {
            std::cout << "\t\t-[" << shader.shaderName
                      << " | " << to_string(shader.shaderStage)
                      << " | Source: " << shader.sourceFile
                      << "]" << std::endl;
        }

        for (const auto& descriptorSet : data.descriptorSets)
        {
            std::cout << "\t-[Descriptor Set #" << descriptorSet.set << " | Bindings: " << descriptorSet.bindings.size() << "]" << std::endl;
            for (const auto& binding : descriptorSet.bindings)
            {
                std::cout << "\t\t-[Binding #" << binding.binding
                          << " | " << to_string(binding.descriptorType)
                          << " | Count: " << binding.descriptorCount
                          << " | Stages: " << to_string(binding.stageFlags)
                          << "]" << std::endl;
            }
        }

        for (const auto& [i, pushConstant] : std::views::enumerate(data.pushConstants))
        {
            std::cout << "\t-[Push Constant #" << i
                      << " | Size: " << pushConstant.size
                      << " | Offset: " << pushConstant.offset
                      << " | Stages: " << to_string(pushConstant.stageFlags)
                      << "]" << std::endl;
        }
    }
    catch (std::runtime_error const& err)
    {
        std::cout << err.what() << std::endl;
    }

    return 0;
}