#ifndef SF_BUILT_IN_SHARED_PTR_HPP
#define SF_BUILT_IN_SHARED_PTR_HPP

#include <memory> // shared_ptr

#include <SF/Core/Serialization.hpp>

#include <SF/DataTrack.hpp>

TEMPLATE_SERIALIZATION(saveload, shared_ptr, template <typename ElementType>, std::shared_ptr<ElementType>)
{
    ::sf::tracking::track(archive, shared_ptr);
}

#endif // SF_BUILT_IN_SHARED_PTR_HPP
