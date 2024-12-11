//
// Created by alex on 12/12/24.
//

#ifndef IDESCRIPTORSET_H
#define IDESCRIPTORSET_H

namespace Bcg::Graphics {
    struct DescriptorSetLayoutBinding {
        uint32_t binding;
        DescriptorType type;
        ShaderStageFlags stageFlags;
        uint32_t count;
    };

    struct DescriptorSetLayoutDesc {
        std::vector<DescriptorSetLayoutBinding> bindings;
    };

    class IDescriptorSetLayout {
    public:
        virtual ~IDescriptorSetLayout() = default;
    };

    struct DescriptorSetDesc {
        IDescriptorSetLayout* layout;
    };

    class IDescriptorSet {
    public:
        virtual ~IDescriptorSet() = default;

        virtual void updateBindings(const std::vector<DescriptorBinding>& bindings) = 0;
    };

    class IPipelineLayout {
    public:
        virtual ~IPipelineLayout() = default;
    };

}

#endif //IDESCRIPTORSET_H
