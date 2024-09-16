#ifndef SF_BUILT_IN_ATOMIC_HPP
#define SF_BUILT_IN_ATOMIC_HPP

#include <atomic> // atomic

#include <SerializationFixture/Core/Serialization.hpp>

TEMPLATE_SERIALIZATION(save, atomic, template <typename ValueType>, std::atomic<ValueType>)
{
    auto object = atomic.load();
    archive & object;
}

TEMPLATE_SERIALIZATION(load, atomic, template <typename ValueType>, std::atomic<ValueType>)
{
    ValueType value{};
    archive & value;

    atomic.store(value);
}

#endif // SF_BUILT_IN_ATOMIC_HPP
