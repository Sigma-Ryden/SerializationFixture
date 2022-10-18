#ifndef SIFAR_META_MACRO_HPP
#define SIFAR_META_MACRO_HPP

#define SIREQUIRE(...) ::sifar::meta::require<(bool)(__VA_ARGS__)> = 0

#define SIWHEN(...) ::sifar::meta::when<(bool)(__VA_ARGS__)>

#endif // SIFAR_META_MACRO_HPP
