#ifndef SF_BUILT_IN_SHARED_PTR_HPP
#define SF_BUILT_IN_SHARED_PTR_HPP

#include <memory> // shared_ptr

#include <SF/Core/Serialization.hpp>

#include <SF/DataTrack.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION(template <typename ElementType>, std::shared_ptr<ElementType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(saveload, shared_ptr, template <typename ElementType>, std::shared_ptr<ElementType>)
    ::sf::track(archive, shared_ptr);
SERIALIZABLE_INIT()

#endif // SF_BUILT_IN_SHARED_PTR_HPP
