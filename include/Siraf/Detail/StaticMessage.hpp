#ifndef SIRAF_DETAIL_STATIC_MESSAGE_HPP
#define SIRAF_DETAIL_STATIC_MESSAGE_HPP

#define STATIC_MESSAGE_UNREGISTERED_TYPE(archive, mode)                                                 \
    "The 'T' is an unregistered type for the '"#archive"'. "                                            \
    "Try overload an operator& to serialize the type 'T' with the macro "                               \
    "'EXTERN_SERIALIZATION("#mode", <parameter>, <condition>)' or "                                     \
    "'EXTERN_CONDITIONAL_SERIALIZATION("#mode", <parameter>, <type>)', "                                \
    "and then register the type 'T' with the macros: "                                                  \
    "'TYPE_REGISTRY(<type>)' or 'CONDITIONAL_TYPE_REGISTRY(<condition>)'."

#endif // SIRAF_DETAIL_STATIC_MESSAGE_HPP
