#ifndef EIGHTSER_APPLY_FUNCTOR_HPP
#define EIGHTSER_APPLY_FUNCTOR_HPP

#include <Eightser/Core/Serialization.hpp>

#include <Eightser/Detail/Meta.hpp>
#include <Eightser/Detail/MetaMacro.hpp>

namespace eightser
{

namespace apply
{

struct apply_functor_t {};

} // namespace apply

namespace meta
{

template <typename ApplyFunctorType> struct is_apply_functor : std::is_base_of<apply::apply_functor_t, ApplyFunctorType> {};

} // namespace meta

} // namespace eightser

CONDITIONAL_SERIALIZABLE_DECLARATION(::eightser::meta::is_apply_functor<typename std::decay<S>::type>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(saveload, apply_functor, ::eightser::meta::is_apply_functor<typename std::decay<S>::type>::value)
    SERIALIZATION
    (
        apply_functor(archive);
    )
SERIALIZABLE_INIT()

#endif // EIGHTSER_APPLY_FUNCTOR_HPP
