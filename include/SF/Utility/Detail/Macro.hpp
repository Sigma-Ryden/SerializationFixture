#ifndef SF_UTILITY_DETAIL_MACRO_HPP
#define SF_UTILITY_DETAIL_MACRO_HPP

#define SF_BITPACK_N(...) SF_CONCAT(SF_BITPACK_, SF_VA_ARGS_SIZE(__VA_ARGS__))(__VA_ARGS__)
#define SF_BITPACK_BODY(archive, ...) archive); SF_BITPACK_N(__VA_ARGS__) }
#define SF_BITPACK_IMPLEMENTATION(...) { auto xxbitpack = ::sf::bitpack<__VA_ARGS__>( SF_BITPACK_BODY

#define SF_BITFIELD(field_and_bits) SF_FIRST_ARGUMENT field_and_bits = xxbitpack field_and_bits;

#define SF_BITPACK_1(field_and_bits) SF_BITFIELD(field_and_bits)
#define SF_BITPACK_2(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_1(__VA_ARGS__)
#define SF_BITPACK_3(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_2(__VA_ARGS__)
#define SF_BITPACK_4(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_3(__VA_ARGS__)
#define SF_BITPACK_5(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_4(__VA_ARGS__)
#define SF_BITPACK_6(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_5(__VA_ARGS__)
#define SF_BITPACK_7(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_6(__VA_ARGS__)
#define SF_BITPACK_8(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_7(__VA_ARGS__)

#define SF_BITPACK_9(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_8(__VA_ARGS__)
#define SF_BITPACK_10(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_9(__VA_ARGS__)
#define SF_BITPACK_11(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_10(__VA_ARGS__)
#define SF_BITPACK_12(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_11(__VA_ARGS__)
#define SF_BITPACK_13(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_12(__VA_ARGS__)
#define SF_BITPACK_14(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_13(__VA_ARGS__)
#define SF_BITPACK_15(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_14(__VA_ARGS__)
#define SF_BITPACK_16(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_15(__VA_ARGS__)

#define SF_BITPACK_17(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_16(__VA_ARGS__)
#define SF_BITPACK_18(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_17(__VA_ARGS__)
#define SF_BITPACK_19(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_18(__VA_ARGS__)
#define SF_BITPACK_20(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_19(__VA_ARGS__)
#define SF_BITPACK_21(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_20(__VA_ARGS__)
#define SF_BITPACK_22(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_21(__VA_ARGS__)
#define SF_BITPACK_23(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_22(__VA_ARGS__)
#define SF_BITPACK_24(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_23(__VA_ARGS__)

#define SF_BITPACK_25(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_24(__VA_ARGS__)
#define SF_BITPACK_26(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_25(__VA_ARGS__)
#define SF_BITPACK_27(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_26(__VA_ARGS__)
#define SF_BITPACK_28(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_27(__VA_ARGS__)
#define SF_BITPACK_29(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_28(__VA_ARGS__)
#define SF_BITPACK_30(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_29(__VA_ARGS__)
#define SF_BITPACK_31(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_30(__VA_ARGS__)
#define SF_BITPACK_32(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_31(__VA_ARGS__)
// and etc.

#endif // SF_UTILITY_DETAIL_MACRO_HPP
