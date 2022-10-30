#ifndef SIFAR_META_MACRO_HPP
#define SIFAR_META_MACRO_HPP

#define SIREQUIRE(...) ::sifar::meta::require<(bool)(__VA_ARGS__)> = 0

#define SIWHEN(...) ::sifar::meta::when<(bool)(__VA_ARGS__)>

#define SIWITH(...) std::is_same<T, __VA_ARGS__>::value

#endif // SIFAR_META_MACRO_HPP
