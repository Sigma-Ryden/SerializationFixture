#ifndef SF_BUILT_IN_UNIQUE_PTR_HPP
#define SF_BUILT_IN_UNIQUE_PTR_HPP

#include <memory> // unique_ptr

#include <SF/Core/Memory.hpp>
#include <SF/Core/Serialization.hpp>

TEMPLATE_SERIALIZATION(save, unique_ptr,
    (template <typename ElementType, typename Deleter>), std::unique_ptr<ElementType, Deleter>)
{
    auto data = unique_ptr.get();
    archive & data;
}

TEMPLATE_SERIALIZATION(load, unique_ptr,
    (template <typename ElementType, typename Deleter>), std::unique_ptr<ElementType, Deleter>)
{
    ElementType* data = nullptr;
    archive & data;

    unique_ptr.reset(data);
}

#endif // SF_BUILT_IN_UNIQUE_PTR_HPP
