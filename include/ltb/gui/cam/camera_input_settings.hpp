// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2023 Rotor Technologies, Inc. - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/gui/imgui.hpp"

// external
// #include <nlohmann/json.hpp>

namespace ltb::gui::cam
{

struct CameraInputSettings
{
    ImGuiMouseButton select_button    = ImGuiMouseButton_Left;
    ImGuiKeyChord    select_modifiers = ImGuiMod_None;
    ImGuiMouseButton pan_button       = ImGuiMouseButton_Left;
    ImGuiKeyChord    pan_modifiers    = ImGuiMod_Ctrl;
    ImGuiMouseButton rotate_button    = ImGuiMouseButton_Left;
    ImGuiKeyChord    rotate_modifiers = ImGuiMod_Shift;
};

auto configure_gui( CameraInputSettings* settings ) -> bool;

// auto to_json( nlohmann::json& json, CameraInputSettings const& settings ) -> void;
// auto from_json( nlohmann::json const& json, CameraInputSettings& settings ) -> void;

} // namespace ltb::gui::cam
