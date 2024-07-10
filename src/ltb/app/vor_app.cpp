#include "ltb/app/vor_app.hpp"

namespace ltb::app
{
namespace
{

constexpr auto                  speed_of_light_Mm_per_s = 299.7924580; // Megameters per second
[[maybe_unused]] constexpr auto speed_of_sound_mps      = 343.0;

auto constexpr base_frequency_range_mhz = math::Range< double >{ .min = 108.0F, .max = 117.95F };
[[maybe_unused]] auto constexpr base_frequency_wavelength_range_m = math::Range< double >{
    .min = speed_of_light_Mm_per_s / ( base_frequency_range_mhz.max ),
    .max = speed_of_light_Mm_per_s / ( base_frequency_range_mhz.min )
};

auto wavelength_from_freq( double wave_speed, float frequency ) -> float
{
    return static_cast< float >( wave_speed / static_cast< double >( frequency ) );
}

auto update_frequency( float const wavelength, std::vector< float >& values ) -> void
{
    auto const values_count = 100UZ;

    auto const spacing = wavelength / static_cast< float >( values_count + 1UZ );
    utils::ignore( spacing );

    values.resize( values_count );
    for ( auto i = 0U; i < values_count; ++i )
    {
        auto const angle = glm::two_pi< double >( ) * static_cast< double >( i )
                         / static_cast< double >( values_count );
        values[ i ] = static_cast< float >( std::sin( angle ) );
    }
}

} // namespace

VorApp::VorApp( window::Window& window, gui::ImguiSetup& imgui_setup )
    : window_( window )
    , imgui_setup_( imgui_setup )
{
}

auto VorApp::initialize( ) -> utils::Result< VorApp* >
{
    LTB_CHECK( window_.initialize( {
        .title        = "VHF Omnidirectional Range",
        .initial_size = glm::ivec2{ 1280, 720 },
    } ) );
    LTB_CHECK( imgui_setup_.initialize( ) );
    LTB_CHECK( ogl_loader_.initialize( ) );

    // Enable ImGui drag and dock window functionality.
    ImGui::GetIO( ).ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    glClearColor( 0.5F, 0.5F, 0.5F, 1.0F );
    return this;
}

auto VorApp::run( ) -> utils::Result< void >
{
    while ( !window_.should_close( ) )
    {
        window_.poll_events( );

        auto const viewport_size = glm::ivec2( glm::vec2( ImGui::GetMainViewport( )->Size ) );

        glViewport( 0, 0, viewport_size.x, viewport_size.y );
        glClear( GL_COLOR_BUFFER_BIT );

        render_gui( );

        window_.swap_buffers( );
    }

    return utils::success( );
}

auto VorApp::render_gui( ) -> void
{
    imgui_setup_.new_frame( );

    ImGui::DockSpaceOverViewport( );

    if ( ImGui::Begin( "Base Frequency (MHz)" ) )
    {
        ImGui::SliderFloat( "##base_frequency", &base_frequency_mhz_, 108.0F, 117.95F, "%.1f" );
        base_frequency_wavelength_m_
            = wavelength_from_freq( speed_of_light_Mm_per_s, base_frequency_mhz_ );

        update_frequency( base_frequency_wavelength_m_, base_frequency_values_ );

        ImGui::PlotLines(
            "##base_frequency",
            base_frequency_values_.data( ),
            base_frequency_values_.size( ),
            0,
            nullptr,
            -1.0F,
            +1.0F,
            ImVec2( 0, 80 )
        );
    }
    ImGui::End( );

    imgui_setup_.render( );
}

} // namespace ltb::app
