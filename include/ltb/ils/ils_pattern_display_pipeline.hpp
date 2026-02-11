// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/vlk/dd/mesh_data.hpp"
#include "ltb/vlk/dd/simple_mesh_2.hpp"
#include "ltb/vlk/objs/vulkan_graphics_pipeline.hpp"

namespace ltb::ils
{
    struct IlsPatternPipelineSettings
    {
        uint32 frame_count = 0U;
        vlk::objs::VulkanBuffer const& camera_ubo;
    };

    enum class IlsPatternForm : uint32
    {
        Combined,
        Sbo,
        Csb,
        Carrier,
        All,
    };

    struct IlsPatternData
    {
        glm::vec2 pos_start = {0.0F, 0.0F};
        glm::vec2 neg_start = {0.0F, 0.0F};
        glm::vec2 end = {1.0F, 0.0F};
        float32 carrier_frequency_hz = 25.0F;

        float32 line_width = 1.0F;
        IlsPatternForm wave_form = IlsPatternForm::Combined;
    };

    class IlsPatternDisplayPipeline
    {
    public:
        IlsPatternDisplayPipeline(
            vlk::objs::VulkanGpu& gpu,
            vlk::objs::VulkanPresentation& presentation
        );

        auto initialize(IlsPatternPipelineSettings const& settings) -> utils::Result<void>;

        [[nodiscard( "Const getter" )]]
        auto is_initialized() const -> bool;

        [[nodiscard( "Const getter" )]]
        auto get_data() const -> IlsPatternData;
        auto set_data(IlsPatternData const& data) -> void;

        auto draw(vlk::objs::FrameInfo const& frame) -> utils::Result<void>;

    private:
        vlk::objs::VulkanGpu& gpu_;
        vlk::objs::VulkanPresentation& presentation_;

        vlk::objs::VulkanGraphicsPipeline pipeline_ = {gpu_, presentation_};

        IlsPatternData wave_data_ = {};

        bool initialized_ = false;
    };
} // namespace ltb::ils
