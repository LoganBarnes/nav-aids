#pragma once

// standard
#include <list>

namespace ltb::gui
{

/// \brief A class that generates incremental unique ids.
template < typename BaseIdType >
    requires std::is_integral_v< BaseIdType >
class IncrementalIdGenerator
{
public:
    template < typename IdType >
    auto generate_id( ) -> IdType;

    template < typename IdType >
    auto reuse_id( IdType id ) -> void;

private:
    BaseIdType              previous_id_  = BaseIdType{ 0 };
    std::list< BaseIdType > returned_ids_ = { };
};

template < typename BaseIdType >
    requires std::is_integral_v< BaseIdType >
template < typename IdType >
auto IncrementalIdGenerator< BaseIdType >::generate_id( ) -> IdType
{
    if ( !returned_ids_.empty( ) )
    {
        auto id = returned_ids_.front( );
        returned_ids_.pop_front( );
        return IdType{ id };
    }
    else
    {
        ++previous_id_;
        return IdType{ previous_id_ };
    }
}

template < typename BaseIdType >
    requires std::is_integral_v< BaseIdType >
template < typename IdType >
auto IncrementalIdGenerator< BaseIdType >::reuse_id( IdType const id ) -> void
{
    returned_ids_.push_back( id.id( ) );
}

} // namespace ltb::gui
