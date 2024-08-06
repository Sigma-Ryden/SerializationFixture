#ifndef SF_BUILT_IN_WEAK_PTR_HPP
#define SF_BUILT_IN_WEAK_PTR_HPP

#include <memory> // weak_ptr

#include <SF/Core/Memory.hpp>
#include <SF/Core/Serialization.hpp>

// serialization of shared_ptr
#include <SF/BuiltIn/shared_ptr.hpp>

TEMPLATE_SERIALIZATION(save, weak_ptr, template <typename ElementType>, std::weak_ptr<ElementType>)
{
    auto sptr = weak_ptr.lock();
    archive & sptr;
}

TEMPLATE_SERIALIZATION(load, weak_ptr, template <typename ElementType>, std::weak_ptr<ElementType>)
{
    std::shared_ptr<ElementType> sptr;
    archive & sptr;

    weak_ptr = sptr;
}

#endif // SF_BUILT_IN_WEAK_PTR_HPP
