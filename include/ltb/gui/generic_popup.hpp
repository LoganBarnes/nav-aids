#pragma once

// standard
#include <functional>
#include <list>
#include <string>

namespace ltb::gui
{

class GenericPopup
{
public:
    using ContentCallback
        = std::function< bool( ) >; ///< Returns true if the popup should be closed.

    explicit GenericPopup( std::string label );
    virtual ~GenericPopup( ) = default;

    /// \brief Display an ImGui window with the content from
    ///        the next available callback (if one exists).
    auto display_next( ) -> void;

    /// \brief Set the contents of the popup.
    auto to_display( ContentCallback content_callback ) -> void;

    /// \brief Returns true if there are callbacks needing to be displayed.
    [[nodiscard]] auto should_be_open( ) const -> bool;

private:
    std::string                  imgui_label_;
    std::list< ContentCallback > content_callbacks_;
};

} // namespace ltb::gui
