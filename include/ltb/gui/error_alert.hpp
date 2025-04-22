#pragma once

// project
#include "ltb/gui/generic_popup.hpp"
#include "ltb/utils/error.hpp"

namespace ltb::gui
{

enum class LogToConsole
{
    No,
    Yes
};

class ErrorAlert
{
public:
    ErrorAlert( std::string const& unique_label, LogToConsole log_errors_to_console );

    /// \brief Display an ImGui window popup with the oldest error message (if one exists).
    auto display_next_error( ) -> void;

    /// \brief Set the error to be displayed by the popup.
    auto to_display( utils::Error error ) -> void;

    /// \brief Returns true if there are errors needing to be displayed.
    [[nodiscard]] auto should_be_open( ) const -> bool;

private:
    GenericPopup popup_;
    LogToConsole log_errors_to_console_;
};

} // namespace ltb::gui
