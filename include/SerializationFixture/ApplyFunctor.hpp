#ifndef SF_APPLY_FUNCTOR_HPP
#define SF_APPLY_FUNCTOR_HPP

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Detail/Meta.hpp>
#include <SerializationFixture/Detail/MetaMacro.hpp>

namespace sf
{

namespace apply
{

struct apply_functor_t {};

} // namespace apply

namespace meta
{

template <typename ApplyFunctorType> struct is_apply_functor : std::is_base_of<apply::apply_functor_t, ApplyFunctorType> {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZABLE_DECLARATION(::sf::meta::is_apply_functor<typename std::decay<S>::type>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(saveload, apply_functor, ::sf::meta::is_apply_functor<typename std::decay<S>::type>::value)
    SERIALIZATION
    (
        apply_functor(archive);
    )
SERIALIZABLE_INIT()

#endif // SF_APPLY_FUNCTOR_HPP
