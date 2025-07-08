#ifndef SF_BUILT_IN_ATOMIC_HPP
#define SF_BUILT_IN_ATOMIC_HPP

#include <atomic> // atomic

#include <SF/Core/Serialization.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION(template <typename ValueType>, std::atomic<ValueType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(save, atomic, template <typename ValueType>, std::atomic<ValueType>)
    SERIALIZATION
    (
        auto object = atomic.load();
        archive & object;
    )
SERIALIZABLE_INIT()

TEMPLATE_SERIALIZABLE(load, atomic, template <typename ValueType>, std::atomic<ValueType>)
    SERIALIZATION
    (
        ValueType value{};
        archive & value;

        atomic.store(value);
    )
SERIALIZABLE_INIT()

#endif // SF_BUILT_IN_ATOMIC_HPP
