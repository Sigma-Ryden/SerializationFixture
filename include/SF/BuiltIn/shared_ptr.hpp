#ifndef SF_BUILT_IN_SHARED_PTR_HPP
#define SF_BUILT_IN_SHARED_PTR_HPP

#include <memory> // shared_ptr

#include <SF/Core/Serialization.hpp>

#include <SF/DataTrack.hpp>

#include <SF/Detail/Meta.hpp> // is_std_shared_ptr

CONDITIONAL_SERIALIZATION(saveload, shared_ptr, ::sf::meta::is_std_shared_ptr<T>::value)
{
    ::sf::tracking::track(archive, shared_ptr);
}

#endif // SF_BUILT_IN_SHARED_PTR_HPP
