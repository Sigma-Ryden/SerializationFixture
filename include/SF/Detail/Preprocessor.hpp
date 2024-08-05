#ifndef SF_DETAIL_PREPROCESSOR_HPP
#define SF_DETAIL_PREPROCESSOR_HPP

// concatenation of two macro arguments
#define SF_CONCAT(lhs, rhs) SF_CONCAT_IMPL(lhs, rhs)
#define SF_CONCAT_IMPL(lhs, rhs) SF_CONCAT_IMPL_(lhs, rhs)
#define SF_CONCAT_IMPL_(lhs, rhs) lhs##rhs

// deparen of macro argument
#define SF_DEPAREN(arg) REW_DEPAREN_IMPL(REW_UATE arg)
#define SF_UATE(...) REW_UATE __VA_ARGS__
#define SF_DEPAREN_IMPL(...) REW_DEPAREN_IMPL_(__VA_ARGS__)
#define SF_DEPAREN_IMPL_(...) REW_EVAL ## __VA_ARGS__
#define SF_EVALREW_UATE

// return first argument from two
#define SF_FIRST_ARGUMENT(first, ...) first

#define SF_VA_ARGS_SIZE_IMPL_(                                                                          \
     _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, _11, _12, _13, _14, _15, _16,                     \
    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32,                     \
    _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48,                     \
    _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...) N

#define SF_REVERSE_INTEGER_SEQUENCE                                                                     \
    63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48,                                     \
    47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32,                                     \
    31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,                                     \
    15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define SF_VA_ARGS_SIZE_IMPL(...) SF_VA_ARGS_SIZE_IMPL_(__VA_ARGS__)

// return __VA_ARGS__ arguments count, for empty __VA_ARGS__ return 1
#define SF_VA_ARGS_SIZE(...) SF_VA_ARGS_SIZE_IMPL(__VA_ARGS__, SF_REVERSE_INTEGER_SEQUENCE)

// generate repeat sequence macro(1) macro(2) ... macro(count)
#define SF_REPEAT(macro, count) SF_CONCAT(SF_REPEAT_, count)(macro)

#define SF_REPEAT_0(macro)

#define SF_REPEAT_1(macro) SF_REPEAT_0(macro) macro(1)
#define SF_REPEAT_2(macro) SF_REPEAT_1(macro) macro(2)
#define SF_REPEAT_3(macro) SF_REPEAT_2(macro) macro(3)
#define SF_REPEAT_4(macro) SF_REPEAT_3(macro) macro(4)
#define SF_REPEAT_5(macro) SF_REPEAT_4(macro) macro(5)
#define SF_REPEAT_6(macro) SF_REPEAT_5(macro) macro(6)
#define SF_REPEAT_7(macro) SF_REPEAT_6(macro) macro(7)
#define SF_REPEAT_8(macro) SF_REPEAT_7(macro) macro(8)

#define SF_REPEAT_9(macro) SF_REPEAT_8(macro) macro(9)
#define SF_REPEAT_10(macro) SF_REPEAT_9(macro) macro(10)
#define SF_REPEAT_11(macro) SF_REPEAT_10(macro) macro(11)
#define SF_REPEAT_12(macro) SF_REPEAT_11(macro) macro(12)
#define SF_REPEAT_13(macro) SF_REPEAT_12(macro) macro(13)
#define SF_REPEAT_14(macro) SF_REPEAT_13(macro) macro(14)
#define SF_REPEAT_15(macro) SF_REPEAT_14(macro) macro(15)
#define SF_REPEAT_16(macro) SF_REPEAT_15(macro) macro(16)

#define SF_REPEAT_17(macro) SF_REPEAT_16(macro) macro(17)
#define SF_REPEAT_18(macro) SF_REPEAT_17(macro) macro(18)
#define SF_REPEAT_19(macro) SF_REPEAT_18(macro) macro(19)
#define SF_REPEAT_20(macro) SF_REPEAT_19(macro) macro(20)
#define SF_REPEAT_21(macro) SF_REPEAT_20(macro) macro(21)
#define SF_REPEAT_22(macro) SF_REPEAT_21(macro) macro(22)
#define SF_REPEAT_23(macro) SF_REPEAT_22(macro) macro(23)
#define SF_REPEAT_24(macro) SF_REPEAT_23(macro) macro(24)

#define SF_REPEAT_25(macro) SF_REPEAT_24(macro) macro(25)
#define SF_REPEAT_26(macro) SF_REPEAT_25(macro) macro(26)
#define SF_REPEAT_27(macro) SF_REPEAT_26(macro) macro(27)
#define SF_REPEAT_28(macro) SF_REPEAT_27(macro) macro(28)
#define SF_REPEAT_29(macro) SF_REPEAT_28(macro) macro(29)
#define SF_REPEAT_30(macro) SF_REPEAT_29(macro) macro(30)
#define SF_REPEAT_31(macro) SF_REPEAT_30(macro) macro(31)
#define SF_REPEAT_32(macro) SF_REPEAT_31(macro) macro(32)

#define SF_REPEAT_33(macro) SF_REPEAT_32(macro) macro(33)
#define SF_REPEAT_34(macro) SF_REPEAT_33(macro) macro(34)
#define SF_REPEAT_35(macro) SF_REPEAT_34(macro) macro(35)
#define SF_REPEAT_36(macro) SF_REPEAT_35(macro) macro(36)
#define SF_REPEAT_37(macro) SF_REPEAT_36(macro) macro(37)
#define SF_REPEAT_38(macro) SF_REPEAT_37(macro) macro(38)
#define SF_REPEAT_39(macro) SF_REPEAT_38(macro) macro(39)
#define SF_REPEAT_40(macro) SF_REPEAT_39(macro) macro(40)

#define SF_REPEAT_41(macro) SF_REPEAT_40(macro) macro(41)
#define SF_REPEAT_42(macro) SF_REPEAT_41(macro) macro(42)
#define SF_REPEAT_43(macro) SF_REPEAT_42(macro) macro(43)
#define SF_REPEAT_44(macro) SF_REPEAT_43(macro) macro(44)
#define SF_REPEAT_45(macro) SF_REPEAT_44(macro) macro(45)
#define SF_REPEAT_46(macro) SF_REPEAT_45(macro) macro(46)
#define SF_REPEAT_47(macro) SF_REPEAT_46(macro) macro(47)
#define SF_REPEAT_48(macro) SF_REPEAT_47(macro) macro(48)

#define SF_REPEAT_49(macro) SF_REPEAT_48(macro) macro(49)
#define SF_REPEAT_50(macro) SF_REPEAT_49(macro) macro(50)
#define SF_REPEAT_51(macro) SF_REPEAT_50(macro) macro(51)
#define SF_REPEAT_52(macro) SF_REPEAT_51(macro) macro(52)
#define SF_REPEAT_53(macro) SF_REPEAT_52(macro) macro(53)
#define SF_REPEAT_54(macro) SF_REPEAT_53(macro) macro(54)
#define SF_REPEAT_55(macro) SF_REPEAT_54(macro) macro(55)
#define SF_REPEAT_56(macro) SF_REPEAT_55(macro) macro(56)

#define SF_REPEAT_57(macro) SF_REPEAT_56(macro) macro(57)
#define SF_REPEAT_58(macro) SF_REPEAT_57(macro) macro(58)
#define SF_REPEAT_59(macro) SF_REPEAT_58(macro) macro(59)
#define SF_REPEAT_60(macro) SF_REPEAT_59(macro) macro(60)
#define SF_REPEAT_61(macro) SF_REPEAT_60(macro) macro(61)
#define SF_REPEAT_62(macro) SF_REPEAT_61(macro) macro(62)
#define SF_REPEAT_63(macro) SF_REPEAT_62(macro) macro(63)
#define SF_REPEAT_64(macro) SF_REPEAT_63(macro) macro(64)
// and etc.

// generate placeholder sequence _0, _1, _2, ..., _count
#define SF_PLACEHOLDERS(count) SF_CONCAT(SF_PLACEHOLDER_, count)()

#define SF_PLACEHOLDER_1() _0
#define SF_PLACEHOLDER_2() SF_PLACEHOLDER_1(), _1
#define SF_PLACEHOLDER_3() SF_PLACEHOLDER_2(), _2
#define SF_PLACEHOLDER_4() SF_PLACEHOLDER_3(), _3
#define SF_PLACEHOLDER_5() SF_PLACEHOLDER_4(), _4
#define SF_PLACEHOLDER_6() SF_PLACEHOLDER_5(), _5
#define SF_PLACEHOLDER_7() SF_PLACEHOLDER_6(), _6
#define SF_PLACEHOLDER_8() SF_PLACEHOLDER_7(), _7

#define SF_PLACEHOLDER_9() SF_PLACEHOLDER_8(), _8
#define SF_PLACEHOLDER_10() SF_PLACEHOLDER_9(), _9
#define SF_PLACEHOLDER_11() SF_PLACEHOLDER_10(), _10
#define SF_PLACEHOLDER_12() SF_PLACEHOLDER_11(), _11
#define SF_PLACEHOLDER_13() SF_PLACEHOLDER_12(), _12
#define SF_PLACEHOLDER_14() SF_PLACEHOLDER_13(), _13
#define SF_PLACEHOLDER_15() SF_PLACEHOLDER_14(), _14
#define SF_PLACEHOLDER_16() SF_PLACEHOLDER_15(), _15

#define SF_PLACEHOLDER_17() SF_PLACEHOLDER_16(), _16
#define SF_PLACEHOLDER_18() SF_PLACEHOLDER_17(), _17
#define SF_PLACEHOLDER_19() SF_PLACEHOLDER_18(), _18
#define SF_PLACEHOLDER_20() SF_PLACEHOLDER_19(), _19
#define SF_PLACEHOLDER_21() SF_PLACEHOLDER_20(), _20
#define SF_PLACEHOLDER_22() SF_PLACEHOLDER_21(), _21
#define SF_PLACEHOLDER_23() SF_PLACEHOLDER_22(), _22
#define SF_PLACEHOLDER_24() SF_PLACEHOLDER_23(), _23

#define SF_PLACEHOLDER_25() SF_PLACEHOLDER_24(), _24
#define SF_PLACEHOLDER_26() SF_PLACEHOLDER_25(), _25
#define SF_PLACEHOLDER_27() SF_PLACEHOLDER_26(), _26
#define SF_PLACEHOLDER_28() SF_PLACEHOLDER_27(), _27
#define SF_PLACEHOLDER_29() SF_PLACEHOLDER_28(), _28
#define SF_PLACEHOLDER_30() SF_PLACEHOLDER_29(), _29
#define SF_PLACEHOLDER_31() SF_PLACEHOLDER_30(), _30
#define SF_PLACEHOLDER_32() SF_PLACEHOLDER_31(), _31

#define SF_PLACEHOLDER_33() SF_PLACEHOLDER_32(), _32
#define SF_PLACEHOLDER_34() SF_PLACEHOLDER_33(), _33
#define SF_PLACEHOLDER_35() SF_PLACEHOLDER_34(), _34
#define SF_PLACEHOLDER_36() SF_PLACEHOLDER_35(), _35
#define SF_PLACEHOLDER_37() SF_PLACEHOLDER_36(), _36
#define SF_PLACEHOLDER_38() SF_PLACEHOLDER_37(), _37
#define SF_PLACEHOLDER_39() SF_PLACEHOLDER_38(), _38
#define SF_PLACEHOLDER_40() SF_PLACEHOLDER_39(), _39

#define SF_PLACEHOLDER_41() SF_PLACEHOLDER_40(), _40
#define SF_PLACEHOLDER_42() SF_PLACEHOLDER_41(), _41
#define SF_PLACEHOLDER_43() SF_PLACEHOLDER_42(), _42
#define SF_PLACEHOLDER_44() SF_PLACEHOLDER_43(), _43
#define SF_PLACEHOLDER_45() SF_PLACEHOLDER_44(), _44
#define SF_PLACEHOLDER_46() SF_PLACEHOLDER_45(), _45
#define SF_PLACEHOLDER_47() SF_PLACEHOLDER_46(), _46
#define SF_PLACEHOLDER_48() SF_PLACEHOLDER_47(), _47

#define SF_PLACEHOLDER_49() SF_PLACEHOLDER_48(), _48
#define SF_PLACEHOLDER_50() SF_PLACEHOLDER_49(), _49
#define SF_PLACEHOLDER_51() SF_PLACEHOLDER_50(), _50
#define SF_PLACEHOLDER_52() SF_PLACEHOLDER_51(), _51
#define SF_PLACEHOLDER_53() SF_PLACEHOLDER_52(), _52
#define SF_PLACEHOLDER_54() SF_PLACEHOLDER_53(), _53
#define SF_PLACEHOLDER_55() SF_PLACEHOLDER_54(), _54
#define SF_PLACEHOLDER_56() SF_PLACEHOLDER_55(), _55

#define SF_PLACEHOLDER_57() SF_PLACEHOLDER_56(), _56
#define SF_PLACEHOLDER_58() SF_PLACEHOLDER_57(), _57
#define SF_PLACEHOLDER_59() SF_PLACEHOLDER_58(), _58
#define SF_PLACEHOLDER_60() SF_PLACEHOLDER_59(), _59
#define SF_PLACEHOLDER_61() SF_PLACEHOLDER_60(), _60
#define SF_PLACEHOLDER_62() SF_PLACEHOLDER_61(), _61
#define SF_PLACEHOLDER_63() SF_PLACEHOLDER_62(), _62
#define SF_PLACEHOLDER_64() SF_PLACEHOLDER_63(), _63
// and etc.

#endif // SF_DETAIL_PREPROCESSOR_HPP
