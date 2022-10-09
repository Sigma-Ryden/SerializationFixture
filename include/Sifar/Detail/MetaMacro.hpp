#ifndef SIFAR_META_MACRO_HPP
#define SIFAR_META_MACRO_HPP

#define SIFAR_REQUIRE(...)                                                                              \
    ::sifar::meta::require<(bool)(__VA_ARGS__)> = 0

#define SIFAR_WHEN(...)                                                                                 \
    ::sifar::meta::when<(bool)(__VA_ARGS__)>

#define SIFAR_POW2(n) (std::size_t(1) << (n))

#endif // SIFAR_META_MACRO_HPP
