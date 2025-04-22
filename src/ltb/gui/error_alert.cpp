#include "ltb/gui/error_alert.hpp"

// project
#include "ltb/gui/imgui.hpp"

// external
#include <spdlog/spdlog.h>

namespace ltb::gui
{
namespace
{

// Yellow
constexpr auto warning_color = ImVec4{ 1.0F, 1.0F, 0.0F, 1.0F };

// Red
constexpr auto error_color = ImVec4{ 1.0F, 0.0F, 0.0F, 1.0F };

} // namespace

ErrorAlert::ErrorAlert( std::string const& unique_label, LogToConsole log_errors_to_console )
    : popup_( "Error Popup###" + unique_label )
    , log_errors_to_console_( log_errors_to_console )
{
}

auto ErrorAlert::display_next_error( ) -> void
{
    popup_.display_next( );
}

auto ErrorAlert::to_display( utils::Error error ) -> void
{
    if ( log_errors_to_console_ == LogToConsole::Yes )
    {
        if ( error.severity( ) == utils::Error::Severity::Warning )
        {
            spdlog::warn( error.debug_error_message( ) );
        }
        else
        {
            spdlog::error( error.debug_error_message( ) );
        }
    }

    popup_.to_display( [ err = std::move( error ) ] {
        if ( err.severity( ) == utils::Error::Severity::Warning )
        {
            ImGui::TextColored( warning_color, "WARNING: %s", err.error_message( ).c_str( ) );
        }
        else
        {
            ImGui::TextColored( error_color, "ERROR: %s", err.error_message( ).c_str( ) );
        }

        if ( ImGui::Button( "Continue" ) )
        {
            return true;
        }
        return false;
    } );
}

auto ErrorAlert::should_be_open( ) const -> bool
{
    return popup_.should_be_open( );
}

} // namespace ltb::gui
