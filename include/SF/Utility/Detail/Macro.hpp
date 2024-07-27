#ifndef SF_UTILITY_DETAIL_MACRO_HPP
#define SF_UTILITY_DETAIL_MACRO_HPP

#define _BITPACK_N(...) SF_CONCAT(_BITPACK_, SF___VA_ARGS___SIZE(__VA_ARGS__))(__VA_ARGS__)
#define _BITPACK_BODY(archive, ...) archive); _BITPACK_N(__VA_ARGS__) }
#define _BITPACK_IMPLEMENTATION(...) { auto xxbitpack = ::sf::bitpack<__VA_ARGS__>( _BITPACK_BODY

#define _BITFIELD(field_and_bits) SF_FIRST_ARGUMENT field_and_bits = xxbitpack field_and_bits;

#define _BITPACK_1(field_and_bits) _BITFIELD(field_and_bits)
#define _BITPACK_2(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_1(__VA_ARGS__)
#define _BITPACK_3(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_2(__VA_ARGS__)
#define _BITPACK_4(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_3(__VA_ARGS__)
#define _BITPACK_5(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_4(__VA_ARGS__)
#define _BITPACK_6(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_5(__VA_ARGS__)
#define _BITPACK_7(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_6(__VA_ARGS__)
#define _BITPACK_8(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_7(__VA_ARGS__)

#define _BITPACK_9(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_8(__VA_ARGS__)
#define _BITPACK_10(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_9(__VA_ARGS__)
#define _BITPACK_11(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_10(__VA_ARGS__)
#define _BITPACK_12(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_11(__VA_ARGS__)
#define _BITPACK_13(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_12(__VA_ARGS__)
#define _BITPACK_14(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_13(__VA_ARGS__)
#define _BITPACK_15(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_14(__VA_ARGS__)
#define _BITPACK_16(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_15(__VA_ARGS__)

#define _BITPACK_17(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_16(__VA_ARGS__)
#define _BITPACK_18(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_17(__VA_ARGS__)
#define _BITPACK_19(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_18(__VA_ARGS__)
#define _BITPACK_20(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_19(__VA_ARGS__)
#define _BITPACK_21(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_20(__VA_ARGS__)
#define _BITPACK_22(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_21(__VA_ARGS__)
#define _BITPACK_23(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_22(__VA_ARGS__)
#define _BITPACK_24(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_23(__VA_ARGS__)

#define _BITPACK_25(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_24(__VA_ARGS__)
#define _BITPACK_26(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_25(__VA_ARGS__)
#define _BITPACK_27(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_26(__VA_ARGS__)
#define _BITPACK_28(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_27(__VA_ARGS__)
#define _BITPACK_29(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_28(__VA_ARGS__)
#define _BITPACK_30(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_29(__VA_ARGS__)
#define _BITPACK_31(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_30(__VA_ARGS__)
#define _BITPACK_32(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_31(__VA_ARGS__)
// and etc.

#endif // SF_UTILITY_DETAIL_MACRO_HPP
