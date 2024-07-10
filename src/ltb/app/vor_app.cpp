#include "ltb/app/vor_app.hpp"

// standard
#include <algorithm>
#include <ranges>

namespace ltb::app
{
namespace
{

constexpr auto speed_of_light_Mm_per_s = 299.7924580; // Megameters per second
constexpr auto speed_of_sound_mps      = 343.0;

auto constexpr vor_frequency_range_mhz = math::Range< float64 >{ .min = 108.0F, .max = 117.95F };
auto constexpr vor_wavelength_range_m  = math::Range< float64 >{
     .min = speed_of_light_Mm_per_s / ( vor_frequency_range_mhz.max ),
     .max = speed_of_light_Mm_per_s / ( vor_frequency_range_mhz.min )
};

constexpr auto reference_audio_frequency_hz = 9960.0F;
constexpr auto variable_audio_frequency_hz  = 480.0F;
constexpr auto reference_audio_wavelength_m = speed_of_sound_mps / reference_audio_frequency_hz;
constexpr auto variable_audio_wavelength_m  = speed_of_sound_mps / variable_audio_frequency_hz;

constexpr auto audio_frequency_range_hz = math::Range< float64 >{
    .min = reference_audio_frequency_hz - variable_audio_frequency_hz,
    .max = reference_audio_frequency_hz + variable_audio_frequency_hz
};
constexpr auto audio_wavelength_range_m = math::Range< float64 >{
    .min = speed_of_sound_mps / audio_frequency_range_hz.max,
    .max = speed_of_sound_mps / audio_frequency_range_hz.min
};

constexpr auto num_points = 5'000UZ;

auto constexpr wavelength_x_axis_distance = vor_wavelength_range_m.max * 5.0;
auto const step = wavelength_x_axis_distance / static_cast< float64 >( num_points );

template < typename F >
auto update_frequency_values(
    std::vector< float64 > const& x_values,
    F const&                      f,
    std::vector< float64 >&       y_values
) -> void
{
    y_values.resize( x_values.size( ) );

    auto const size = x_values.size( );
    for ( auto i = 0U; i < size; ++i )
    {
        auto const x  = x_values[ i ];
        y_values[ i ] = f( x ) * 0.5;
    }
}

struct StandardFrequencyFunctor
{
    float64 const& base_frequency_wavelength_m_;

    auto operator( )( float64 const x ) const -> float64
    {
        auto const angle = ( x / base_frequency_wavelength_m_ ) * glm::two_pi< float64 >( );
        return std::sin( angle );
    }
};

struct AudioFrequencyFunctor
{
    math::Range< float64 > const& audio_wavelength_range_m_;
    float64 const                 audio_range_size_ = math::dimensions( audio_wavelength_range_m_ );

    auto operator( )( float64 const x ) const -> float64
    {
        constexpr auto thirty_hz_wavelength_m = speed_of_sound_mps / 30.0;

        auto const variable_angle   = ( x / thirty_hz_wavelength_m ) * glm::two_pi< float64 >( );
        auto const wavelength_scale = ( std::sin( variable_angle ) * 0.5 ) + 0.5;
        auto const wavelength_m
            = audio_wavelength_range_m_.min + ( wavelength_scale * audio_range_size_ );

        auto const base_angle = ( x / wavelength_m ) * glm::two_pi< float64 >( );
        return std::sin( base_angle );
    }
};

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

    if ( implot_context_
         = std::shared_ptr< ImPlotContext >( ImPlot::CreateContext( ), ImPlot::DestroyContext );
         nullptr == implot_context_ )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Failed to create ImPlot context" );
    }

    // Enable ImGui drag and dock window functionality.
    ImGui::GetIO( ).ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    glClearColor( 0.5F, 0.5F, 0.5F, 1.0F );

    wavelength_x_values_.resize( num_points );
    for ( auto i = 0U; i < num_points; ++i )
    {
        wavelength_x_values_[ i ] = static_cast< float64 >( i ) * step;
    }

    update_frequencies( );
    return this;
}

auto VorApp::run( ) -> utils::Result< void >
{
    while ( !window_.should_close( ) )
    {
        window_.poll_events( );

        auto const viewport_size = ImGui::GetMainViewport( )->Size;

        glViewport(
            0,
            0,
            static_cast< int32 >( viewport_size.x ),
            static_cast< int32 >( viewport_size.y )
        );
        glClear( GL_COLOR_BUFFER_BIT );

        render_gui( );

        window_.swap_buffers( );
    }

    return utils::success( );
}

auto VorApp::render_gui( ) -> void
{
    imgui_setup_.new_frame( );

    utils::ignore( ImGui::DockSpaceOverViewport( ) );

    if ( ImGui::Begin( "Base Frequency (MHz)" ) )
    {
        if ( ImGui::SliderScalar(
                 "##base_frequency",
                 ImGuiDataType_Double,
                 &base_frequency_mhz_,
                 &vor_frequency_range_mhz.min,
                 &vor_frequency_range_mhz.max,
                 "%.2f"
             ) )
        {
            update_frequencies( );
        }

        if ( ImPlot::BeginPlot( "Fundamental Frequency" ) )
        {
            ImPlot::SetupAxes( "Length (m)", "Amplitude" );
            ImPlot::SetupAxesLimits( 0.0F, wavelength_x_axis_distance, -1.0F, +1.0F );

            ImPlot::PlotLine(
                "f(x)",
                wavelength_x_values_.data( ),
                base_radio_wave_y_values_.data( ),
                static_cast< int32 >( num_points )
            );
            ImPlot::EndPlot( );
        }

        if ( ImPlot::BeginPlot( "Audio Frequency (9960 ±480 Hz)" ) )
        {
            ImPlot::SetupAxes( "Length (m)", "Amplitude" );
            ImPlot::SetupAxesLimits( 0.0F, wavelength_x_axis_distance, -1.0F, +1.0F );

            ImPlot::PlotLine(
                "f(x)",
                wavelength_x_values_.data( ),
                reference_audio_wave_y_values_.data( ),
                static_cast< int32 >( num_points )
            );
            ImPlot::EndPlot( );
        }

        if ( ImPlot::BeginPlot( "Composite Output" ) )
        {
            ImPlot::SetupAxes( "Length (m)", "Amplitude" );
            ImPlot::SetupAxesLimits( 0.0F, wavelength_x_axis_distance, -1.0F, +1.0F );

            ImPlot::PlotLine(
                "f(x)",
                wavelength_x_values_.data( ),
                composite_radio_wave_y_values_.data( ),
                static_cast< int32 >( num_points )
            );
            ImPlot::EndPlot( );
        }
    }
    ImGui::End( );

    ImPlot::ShowDemoWindow( );

    imgui_setup_.render( );
}

auto VorApp::update_frequencies( ) -> void
{
    base_frequency_wavelength_m_ = speed_of_light_Mm_per_s / base_frequency_mhz_;

    update_frequency_values(
        wavelength_x_values_,
        StandardFrequencyFunctor{ base_frequency_wavelength_m_ },
        base_radio_wave_y_values_
    );

    update_frequency_values(
        wavelength_x_values_,
        AudioFrequencyFunctor{ audio_wavelength_range_m },
        reference_audio_wave_y_values_
    );

    composite_radio_wave_y_values_.resize( wavelength_x_values_.size( ) );
    utils::ignore(
        // Ignore the returned iterator
        std::ranges::transform(
            base_radio_wave_y_values_,
            reference_audio_wave_y_values_,
            composite_radio_wave_y_values_.begin( ),
            std::plus< float64 >( )
        )
    );
}

} // namespace ltb::app
