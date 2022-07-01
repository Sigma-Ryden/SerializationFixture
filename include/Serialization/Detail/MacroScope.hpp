// This file contains all external macros
// You MUST include MacroUnscope.hpp at the end of the file you use
#define SERIALIZATION_HAS_FUNCTION_TPL_HELPER(name)                                                     \
    template <typename C, typename = void>                                                              \
    struct has_##name : std::false_type {};								\
    template <typename C>                                                                               \
    struct has_##name<C, serialization::meta::void_t<decltype(&C::template name<dummy_type>)>>          \
        : std::true_type {}

#define SERIALIZATION_HAS_FUNCTION_HELPER(name)                                                         \
    template <typename C, typename = void>                                                              \
    struct has_##name : std::false_type {};								\
    template <typename C>                                                                               \
    struct has_##name<C, serialization::meta::void_t<decltype(&C::name)>>                               \
        : std::true_type {}
