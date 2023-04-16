#ifndef SF_DETAIL_PREPROCESSOR_HPP
#define SF_DETAIL_PREPROCESSOR_HPP

#define _SF_CONCAT_IMPL(a, b) a##b
#define SFCONCAT(a, b) _SF_CONCAT_IMPL(a, b)

// generate repeat sequance macro(1) macro(2) ... macro(count)
#define SFREPEAT(macro, count) SFCONCAT(_REPEAT_, count)(macro)

#define _REPEAT_0(macro)

#define _REPEAT_1(macro) _REPEAT_0(macro) macro(1)
#define _REPEAT_2(macro) _REPEAT_1(macro) macro(2)
#define _REPEAT_3(macro) _REPEAT_2(macro) macro(3)
#define _REPEAT_4(macro) _REPEAT_3(macro) macro(4)
#define _REPEAT_5(macro) _REPEAT_4(macro) macro(5)
#define _REPEAT_6(macro) _REPEAT_5(macro) macro(6)
#define _REPEAT_7(macro) _REPEAT_6(macro) macro(7)
#define _REPEAT_8(macro) _REPEAT_7(macro) macro(8)

#define _REPEAT_9(macro) _REPEAT_8(macro) macro(9)
#define _REPEAT_10(macro) _REPEAT_9(macro) macro(10)
#define _REPEAT_11(macro) _REPEAT_10(macro) macro(11)
#define _REPEAT_12(macro) _REPEAT_11(macro) macro(12)
#define _REPEAT_13(macro) _REPEAT_12(macro) macro(13)
#define _REPEAT_14(macro) _REPEAT_13(macro) macro(14)
#define _REPEAT_15(macro) _REPEAT_14(macro) macro(15)
#define _REPEAT_16(macro) _REPEAT_15(macro) macro(16)

#define _REPEAT_17(macro) _REPEAT_16(macro) macro(17)
#define _REPEAT_18(macro) _REPEAT_17(macro) macro(18)
#define _REPEAT_19(macro) _REPEAT_18(macro) macro(19)
#define _REPEAT_20(macro) _REPEAT_19(macro) macro(20)
#define _REPEAT_21(macro) _REPEAT_20(macro) macro(21)
#define _REPEAT_22(macro) _REPEAT_21(macro) macro(22)
#define _REPEAT_23(macro) _REPEAT_22(macro) macro(23)
#define _REPEAT_24(macro) _REPEAT_23(macro) macro(24)

#define _REPEAT_25(macro) _REPEAT_24(macro) macro(25)
#define _REPEAT_26(macro) _REPEAT_25(macro) macro(26)
#define _REPEAT_27(macro) _REPEAT_26(macro) macro(27)
#define _REPEAT_28(macro) _REPEAT_27(macro) macro(28)
#define _REPEAT_29(macro) _REPEAT_28(macro) macro(29)
#define _REPEAT_30(macro) _REPEAT_29(macro) macro(30)
#define _REPEAT_31(macro) _REPEAT_30(macro) macro(31)
#define _REPEAT_32(macro) _REPEAT_31(macro) macro(32)

#define _REPEAT_33(macro) _REPEAT_32(macro) macro(33)
#define _REPEAT_34(macro) _REPEAT_33(macro) macro(34)
#define _REPEAT_35(macro) _REPEAT_34(macro) macro(35)
#define _REPEAT_36(macro) _REPEAT_35(macro) macro(36)
#define _REPEAT_37(macro) _REPEAT_36(macro) macro(37)
#define _REPEAT_38(macro) _REPEAT_37(macro) macro(38)
#define _REPEAT_39(macro) _REPEAT_38(macro) macro(39)
#define _REPEAT_40(macro) _REPEAT_39(macro) macro(40)

#define _REPEAT_41(macro) _REPEAT_40(macro) macro(41)
#define _REPEAT_42(macro) _REPEAT_41(macro) macro(42)
#define _REPEAT_43(macro) _REPEAT_42(macro) macro(43)
#define _REPEAT_44(macro) _REPEAT_43(macro) macro(44)
#define _REPEAT_45(macro) _REPEAT_44(macro) macro(45)
#define _REPEAT_46(macro) _REPEAT_45(macro) macro(46)
#define _REPEAT_47(macro) _REPEAT_46(macro) macro(47)
#define _REPEAT_48(macro) _REPEAT_47(macro) macro(48)

#define _REPEAT_49(macro) _REPEAT_48(macro) macro(49)
#define _REPEAT_50(macro) _REPEAT_49(macro) macro(50)
#define _REPEAT_51(macro) _REPEAT_50(macro) macro(51)
#define _REPEAT_52(macro) _REPEAT_51(macro) macro(52)
#define _REPEAT_53(macro) _REPEAT_52(macro) macro(53)
#define _REPEAT_54(macro) _REPEAT_53(macro) macro(54)
#define _REPEAT_55(macro) _REPEAT_54(macro) macro(55)
#define _REPEAT_56(macro) _REPEAT_55(macro) macro(56)

#define _REPEAT_57(macro) _REPEAT_56(macro) macro(57)
#define _REPEAT_58(macro) _REPEAT_57(macro) macro(58)
#define _REPEAT_59(macro) _REPEAT_58(macro) macro(59)
#define _REPEAT_60(macro) _REPEAT_59(macro) macro(60)
#define _REPEAT_61(macro) _REPEAT_60(macro) macro(61)
#define _REPEAT_62(macro) _REPEAT_61(macro) macro(62)
#define _REPEAT_63(macro) _REPEAT_62(macro) macro(63)
#define _REPEAT_64(macro) _REPEAT_63(macro) macro(64)
// and etc.

// generate placeholder sequance _0, _1, _2, ..., _count
#define SFPLACEHOLDERS(count) SFCONCAT(_PLACEHOLDER_, count)()

#define _PLACEHOLDER_1() _0
#define _PLACEHOLDER_2() _PLACEHOLDER_1(), _1
#define _PLACEHOLDER_3() _PLACEHOLDER_2(), _2
#define _PLACEHOLDER_4() _PLACEHOLDER_3(), _3
#define _PLACEHOLDER_5() _PLACEHOLDER_4(), _4
#define _PLACEHOLDER_6() _PLACEHOLDER_5(), _5
#define _PLACEHOLDER_7() _PLACEHOLDER_6(), _6
#define _PLACEHOLDER_8() _PLACEHOLDER_7(), _7

#define _PLACEHOLDER_9() _PLACEHOLDER_8(), _8
#define _PLACEHOLDER_10() _PLACEHOLDER_9(), _9
#define _PLACEHOLDER_11() _PLACEHOLDER_10(), _10
#define _PLACEHOLDER_12() _PLACEHOLDER_11(), _11
#define _PLACEHOLDER_13() _PLACEHOLDER_12(), _12
#define _PLACEHOLDER_14() _PLACEHOLDER_13(), _13
#define _PLACEHOLDER_15() _PLACEHOLDER_14(), _14
#define _PLACEHOLDER_16() _PLACEHOLDER_15(), _15

#define _PLACEHOLDER_17() _PLACEHOLDER_16(), _16
#define _PLACEHOLDER_18() _PLACEHOLDER_17(), _17
#define _PLACEHOLDER_19() _PLACEHOLDER_18(), _18
#define _PLACEHOLDER_20() _PLACEHOLDER_19(), _19
#define _PLACEHOLDER_21() _PLACEHOLDER_20(), _20
#define _PLACEHOLDER_22() _PLACEHOLDER_21(), _21
#define _PLACEHOLDER_23() _PLACEHOLDER_22(), _22
#define _PLACEHOLDER_24() _PLACEHOLDER_23(), _23

#define _PLACEHOLDER_25() _PLACEHOLDER_24(), _24
#define _PLACEHOLDER_26() _PLACEHOLDER_25(), _25
#define _PLACEHOLDER_27() _PLACEHOLDER_26(), _26
#define _PLACEHOLDER_28() _PLACEHOLDER_27(), _27
#define _PLACEHOLDER_29() _PLACEHOLDER_28(), _28
#define _PLACEHOLDER_30() _PLACEHOLDER_29(), _29
#define _PLACEHOLDER_31() _PLACEHOLDER_30(), _30
#define _PLACEHOLDER_32() _PLACEHOLDER_31(), _31

#define _PLACEHOLDER_33() _PLACEHOLDER_32(), _32
#define _PLACEHOLDER_34() _PLACEHOLDER_33(), _33
#define _PLACEHOLDER_35() _PLACEHOLDER_34(), _34
#define _PLACEHOLDER_36() _PLACEHOLDER_35(), _35
#define _PLACEHOLDER_37() _PLACEHOLDER_36(), _36
#define _PLACEHOLDER_38() _PLACEHOLDER_37(), _37
#define _PLACEHOLDER_39() _PLACEHOLDER_38(), _38
#define _PLACEHOLDER_40() _PLACEHOLDER_39(), _39

#define _PLACEHOLDER_41() _PLACEHOLDER_40(), _40
#define _PLACEHOLDER_42() _PLACEHOLDER_41(), _41
#define _PLACEHOLDER_43() _PLACEHOLDER_42(), _42
#define _PLACEHOLDER_44() _PLACEHOLDER_43(), _43
#define _PLACEHOLDER_45() _PLACEHOLDER_44(), _44
#define _PLACEHOLDER_46() _PLACEHOLDER_45(), _45
#define _PLACEHOLDER_47() _PLACEHOLDER_46(), _46
#define _PLACEHOLDER_48() _PLACEHOLDER_47(), _47

#define _PLACEHOLDER_49() _PLACEHOLDER_48(), _48
#define _PLACEHOLDER_50() _PLACEHOLDER_49(), _49
#define _PLACEHOLDER_51() _PLACEHOLDER_50(), _50
#define _PLACEHOLDER_52() _PLACEHOLDER_51(), _51
#define _PLACEHOLDER_53() _PLACEHOLDER_52(), _52
#define _PLACEHOLDER_54() _PLACEHOLDER_53(), _53
#define _PLACEHOLDER_55() _PLACEHOLDER_54(), _54
#define _PLACEHOLDER_56() _PLACEHOLDER_55(), _55

#define _PLACEHOLDER_57() _PLACEHOLDER_56(), _56
#define _PLACEHOLDER_58() _PLACEHOLDER_57(), _57
#define _PLACEHOLDER_59() _PLACEHOLDER_58(), _58
#define _PLACEHOLDER_60() _PLACEHOLDER_59(), _59
#define _PLACEHOLDER_61() _PLACEHOLDER_60(), _60
#define _PLACEHOLDER_62() _PLACEHOLDER_61(), _61
#define _PLACEHOLDER_63() _PLACEHOLDER_62(), _62
#define _PLACEHOLDER_64() _PLACEHOLDER_63(), _63
// and etc.

#endif // SF_DETAIL_PREPROCESSOR_HPP