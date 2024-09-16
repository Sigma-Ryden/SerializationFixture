#ifndef SF_BUILT_IN_UNIQUE_PTR_HPP
#define SF_BUILT_IN_UNIQUE_PTR_HPP

#include <memory> // unique_ptr

#include <SerializationFixture/Core/Memory.hpp>
#include <SerializationFixture/Core/Serialization.hpp>

TEMPLATE_SERIALIZATION(save, unique_ptr,
    (template <typename ElementType, typename DeleterType>), std::unique_ptr<ElementType, DeleterType>)
{
    auto data = unique_ptr.get();
    archive & data;
}

TEMPLATE_SERIALIZATION(load, unique_ptr,
    (template <typename ElementType, typename DeleterType>), std::unique_ptr<ElementType, DeleterType>)
{
    ElementType* data = nullptr;
    archive & data;

    unique_ptr.reset(data);
}

#endif // SF_BUILT_IN_UNIQUE_PTR_HPP
