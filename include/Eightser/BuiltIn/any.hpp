#ifndef EIGHTSER_BUILT_IN_ANY_HPP
#define EIGHTSER_BUILT_IN_ANY_HPP

#if !defined(EIGHTSER_ANY_SUPPORT_DISABLE)

#include <cstdint> // uint64_t

#include <any> // any

#include <Eightser/Core/Hash.hpp>
#include <Eightser/Core/Serialization.hpp>

#include <Eightser/Dynamic/AnyRegistry.hpp>

// please, use 'eightser::serializable' for type any registry before std::any serialization
SERIALIZABLE_DECLARATION(std::any)
SERIALIZABLE_DECLARATION_INIT()

#endif // if

#endif // EIGHTSER_BUILT_IN_ANY_HPP
