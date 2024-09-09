#include "ltb/app/vor_app.hpp"

// external
#include <glm/gtc/constants.hpp>

// standard
#include <algorithm>
#include <ranges>

namespace ltb::app
{
namespace
{

auto constexpr ms_from_s = 1.0e6F;

auto constexpr vor_frequency_range_mhz = math::Range< float64 >{
    .min = 108.0F,
    .max = 117.95F,
};
auto constexpr vor_period_ms = math::Range< float64 >{
    .min = 1.0F / ( vor_frequency_range_mhz.max ),
    .max = 1.0F / ( vor_frequency_range_mhz.min )
};

constexpr auto reference_audio_frequency_hz = 9960.0F;
constexpr auto variable_audio_frequency_hz  = 480.0F;

constexpr auto audio_frequency_range_hz = math::Range< float64 >{
    .min = reference_audio_frequency_hz - variable_audio_frequency_hz,
    .max = reference_audio_frequency_hz + variable_audio_frequency_hz
};
constexpr auto audio_period_range_ms = math::Range< float64 >{
    .min = ms_from_s / audio_frequency_range_hz.max,
    .max = ms_from_s / audio_frequency_range_hz.min
};

[[maybe_unused]] auto constexpr vor_period_max = vor_period_ms.max;
[[maybe_unused]] auto constexpr vor_period_min = vor_period_ms.min;

[[maybe_unused]] auto constexpr audio_period_max = audio_period_range_ms.max;
[[maybe_unused]] auto constexpr audio_period_min = audio_period_range_ms.min;

constexpr auto num_points = 50'000_UZ;

auto constexpr x_axis_time_ms = 100.0F;
auto const step               = x_axis_time_ms / static_cast< float64 >( num_points );

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
        y_values[ i ] = f( x );
    }
}

struct StandardFrequencyFunctor
{
    float64 const& base_frequency_period_s_;

    auto operator( )( float64 const x ) const -> float64
    {
        auto const angle = ( x / base_frequency_period_s_ ) * glm::two_pi< float64 >( );
        return std::sin( angle );
    }
};

struct AudioFrequencyFunctor
{
    math::Range< float64 > const& audio_wavelength_range_m_;
    float64 const                 audio_range_size_ = math::dimensions( audio_wavelength_range_m_ );

    auto operator( )( float64 const x ) const -> float64
    {
        constexpr auto thirty_hz_period_s = 1.0F / 30.0;

        auto const variable_angle = ( x / thirty_hz_period_s ) * glm::two_pi< float64 >( );
        auto const period_scale   = ( std::sin( variable_angle ) * 0.5 ) + 0.5;
        auto const period_s = audio_wavelength_range_m_.min + ( period_scale * audio_range_size_ );

        auto const base_angle = ( x / period_s ) * glm::two_pi< float64 >( );
        return std::sin( base_angle );
    }
};

struct AmplitudeModulation
{
    auto operator( )( float64 const carrier, float64 const amplitude ) const -> float64
    {
        return carrier * ( ( amplitude * 0.5 ) + 0.5 );
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

    time_ms_x_values_.resize( num_points );
    for ( auto i = 0U; i < num_points; ++i )
    {
        time_ms_x_values_[ i ] = static_cast< float64 >( i ) * step;
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

    if ( ImGui::Begin( "Frequencies" ) )
    {
        if ( ImGui::SliderScalar(
                 "##carrier_frequency",
                 ImGuiDataType_Double,
                 &carrier_frequency_mhz_,
                 &vor_frequency_range_mhz.min,
                 &vor_frequency_range_mhz.max,
                 "%.2f"
             ) )
        {
            update_frequencies( );
        }

        constexpr auto rows = 2;
        constexpr auto cols = 1;
        if ( ImPlot::BeginSubplots(
                 "##modulation",
                 rows,
                 cols,
                 ImVec2( -1.0F, -1.0F ),
                 ImPlotSubplotFlags_LinkCols
             ) )
        {

            if ( ImPlot::BeginPlot( "Carrier Frequency (108.0-117.95 MHz)" ) )
            {
                ImPlot::SetupAxes( "Time (ms)", "Amplitude" );
                ImPlot::SetupAxesLimits( 0.0F, x_axis_time_ms, -1.0F, +1.0F );
                // ImPlot::SetupAxesLimits( ImAxis_X1, 0.0F, x_axis_time_ms, ImPlotCond_Once );
                // ImPlot::SetupAxisLimits( ImAxis_Y1, -1.0F, +1.0F, ImPlotCond_Always );

                // ImPlot::SetupAxisLimitsConstraints( ImAxis_Y1, -1.0F, +1.0F );
                // ImPlot::SetupAxisZoomConstraints( ImAxis_Y1, -1.0F, +1.0F );

                ImPlot::PlotLine(
                    "f(x)",
                    time_ms_x_values_.data( ),
                    carrier_wave_y_values_.data( ),
                    static_cast< int32 >( num_points )
                );

                ImPlot::PlotLine(
                    "f(x)",
                    time_ms_x_values_.data( ),
                    reference_audio_wave_y_values_.data( ),
                    static_cast< int32 >( num_points )
                );
                ImPlot::EndPlot( );
            }

            if ( ImPlot::BeginPlot( "Amplitude Modulation" ) )
            {
                ImPlot::SetupAxes( "Time (ms)", "Amplitude" );
                // ImPlot::SetupAxesLimits( ImAxis_X1, 0.0F, x_axis_time_ms, ImPlotCond_Once );
                // ImPlot::SetupAxisLimits( ImAxis_Y1, -1.0F, +1.0F, ImPlotCond_Always );
                ImPlot::SetupAxesLimits( 0.0F, x_axis_time_ms, -1.0F, +1.0F );

                ImPlot::PlotLine(
                    "f(x)",
                    time_ms_x_values_.data( ),
                    composite_radio_wave_y_values_.data( ),
                    static_cast< int32 >( num_points )
                );
                ImPlot::EndPlot( );
            }

            ImPlot::EndSubplots( );
        }
    }
    ImGui::End( );

    ImPlot::ShowDemoWindow( );

    imgui_setup_.render( );
}

auto VorApp::update_frequencies( ) -> void
{
    carrier_frequency_period_ms_ = 1.0F / carrier_frequency_mhz_;

    update_frequency_values(
        time_ms_x_values_,
        StandardFrequencyFunctor{ carrier_frequency_period_ms_ },
        carrier_wave_y_values_
    );

    auto const variable_audio_period_ms = ms_from_s / reference_audio_frequency_hz;

    update_frequency_values(
        time_ms_x_values_,
        StandardFrequencyFunctor{ variable_audio_period_ms },
        reference_audio_wave_y_values_
    );

    composite_radio_wave_y_values_.resize( time_ms_x_values_.size( ) );
    utils::ignore(
        // Ignore the returned iterator
        std::ranges::transform(
            carrier_wave_y_values_,
            reference_audio_wave_y_values_,
            composite_radio_wave_y_values_.begin( ),
            AmplitudeModulation{ }
        )
    );
}

} // namespace ltb::app
