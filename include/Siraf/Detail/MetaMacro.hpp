#ifndef SIRAF_DETAIL_META_MACRO_HPP
#define SIRAF_DETAIL_META_MACRO_HPP

#define SIREQUIRE(...) ::siraf::meta::require<(bool)(__VA_ARGS__)> = 0

#define SIWHEN(...) ::siraf::meta::when<(bool)(__VA_ARGS__)>

#define SIWITH(...) std::is_same<T, __VA_ARGS__>::value

#endif // SIRAF_DETAIL_META_MACRO_HPP
