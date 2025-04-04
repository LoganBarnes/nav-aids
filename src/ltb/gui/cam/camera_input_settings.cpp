#include "ltb/gui/cam/camera_input_settings.hpp"

// project
// #include "ltb/gui/imgui_utils.hpp"
// #include "ltb/utils/json_utils.hpp"

namespace ltb::gui::cam
{
//
// auto configure_gui( CameraInputSettings* settings ) -> bool
// {
//     auto something_changed = false;
//
//     auto single_mouse_radio_button
//         = [ &something_changed ]( auto const& label, auto button, auto button_type ) {
//               if ( ImGui::RadioButton( label, button == button_type ) )
//               {
//                   button            = button_type;
//                   something_changed = true;
//               }
//               return button;
//           };
//
//     auto mouse_radio_buttons = [ &single_mouse_radio_button ]( auto const& label, auto button ) {
//         ImGui::TextUnformatted( label );
//         ImGui::SameLine( );
//         button = single_mouse_radio_button( "Left", button, ImGuiMouseButton_Left );
//         ImGui::SameLine( );
//         button = single_mouse_radio_button( "Middle", button, ImGuiMouseButton_Middle );
//         ImGui::SameLine( );
//         button = single_mouse_radio_button( "Right", button, ImGuiMouseButton_Right );
//         ImGui::SameLine( );
//         button = single_mouse_radio_button( "None", button, ImGuiMouseButton_COUNT );
//         return button;
//     };
//
//     auto single_modifier_radio_button
//         = [ &something_changed ]( auto const& label, auto flags, auto flag ) {
//               if ( ImGui::RadioButton( label, flags & flag ) )
//               {
//                   flags             = flags ^ flag;
//                   something_changed = true;
//               }
//               return flags;
//           };
//
//     auto modifiers_radio_buttons
//         = [ &single_modifier_radio_button ]( auto const& label, auto flags ) {
//               ImGui::TextUnformatted( label );
//               ImGui::SameLine( );
//               flags = single_modifier_radio_button( "Ctrl", flags, ImGuiModFlags_Ctrl );
//               ImGui::SameLine( );
//               flags = single_modifier_radio_button( "Shift ", flags, ImGuiModFlags_Shift );
//               ImGui::SameLine( );
//               flags = single_modifier_radio_button( "Super", flags, ImGuiModFlags_Super );
//               ImGui::SameLine( );
//               flags = single_modifier_radio_button( "Alt ", flags, ImGuiModFlags_Alt );
//               return flags;
//           };
//
//     ImGui::TextUnformatted( "Mouse Buttons" );
//     {
//         auto indent             = gui::ScopedIndent{ };
//         auto id                 = gui::ScopedID{ "selection" };
//         settings->select_button = mouse_radio_buttons( "Selection: ", settings->select_button );
//         settings->select_modifiers
//             = modifiers_radio_buttons( "         + ", settings->select_modifiers );
//     }
//     {
//         auto indent             = gui::ScopedIndent{ };
//         auto id                 = gui::ScopedID{ "panning" };
//         settings->pan_button    = mouse_radio_buttons( "Panning:   ", settings->pan_button );
//         settings->pan_modifiers = modifiers_radio_buttons( "         + ", settings->pan_modifiers );
//     }
//     {
//         auto indent             = gui::ScopedIndent{ };
//         auto id                 = gui::ScopedID{ "rotation" };
//         settings->rotate_button = mouse_radio_buttons( "Rotation:  ", settings->rotate_button );
//         settings->rotate_modifiers
//             = modifiers_radio_buttons( "         + ", settings->rotate_modifiers );
//     }
//
//     return something_changed;
// }

// auto to_json( nlohmann::json& json, CameraInputSettings const& settings ) -> void
// {
//     auto& camera = json[ "camera" ];
//
//     camera[ "select_button" ]    = settings.select_button;
//     camera[ "select_modifiers" ] = settings.select_modifiers;
//     camera[ "pan_button" ]       = settings.pan_button;
//     camera[ "pan_modifiers" ]    = settings.pan_modifiers;
//     camera[ "rotate_button" ]    = settings.rotate_button;
//     camera[ "rotate_modifiers" ] = settings.rotate_modifiers;
// }
//
// auto from_json( nlohmann::json const& json, CameraInputSettings& settings ) -> void
// {
//     if ( json.contains( "camera" ) )
//     {
//         auto const& camera = json.at( "camera" );
//
//         utils::assign_if_present( camera, "select_button", settings.select_button );
//         utils::assign_if_present( camera, "select_modifiers", settings.select_modifiers );
//         utils::assign_if_present( camera, "pan_button", settings.pan_button );
//         utils::assign_if_present( camera, "pan_modifiers", settings.pan_modifiers );
//         utils::assign_if_present( camera, "rotate_button", settings.rotate_button );
//         utils::assign_if_present( camera, "rotate_modifiers", settings.rotate_modifiers );
//     }
// }

} // namespace ltb::gui::cam
