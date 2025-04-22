#include "ltb/gui/generic_popup.hpp"

// project
#include "ltb/gui/imgui.hpp"

namespace ltb::gui
{

GenericPopup::GenericPopup( std::string label )
    : imgui_label_( std::move( label ) )
{
}

auto GenericPopup::display_next( ) -> void
{
    ImGui::PushID( imgui_label_.c_str( ) );

    if ( should_be_open( ) )
    {
        ImGui::OpenPopup( imgui_label_.c_str( ) );
    }

    constexpr auto flags = ImGuiWindowFlags_AlwaysAutoResize;
    if ( ImGui::BeginPopupModal( imgui_label_.c_str( ), nullptr, flags ) )
    {
        ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 2, 2 ) );

        if ( content_callbacks_.front( )( ) )
        {
            content_callbacks_.pop_front( );
            ImGui::CloseCurrentPopup( );
        }

        ImGui::PopStyleVar( );
        ImGui::EndPopup( );
    }

    ImGui::PopID( );
}

auto GenericPopup::to_display( ContentCallback content_callback ) -> void
{
    content_callbacks_.push_back( std::move( content_callback ) );
}

auto GenericPopup::should_be_open( ) const -> bool
{
    return !content_callbacks_.empty( );
}

} // namespace ltb::gui
