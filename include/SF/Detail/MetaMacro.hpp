#ifndef SF_DETAIL_META_MACRO_HPP
#define SF_DETAIL_META_MACRO_HPP

#define SIREQUIRE(...) ::sf::meta::require<(bool)(__VA_ARGS__)> = 0
#define SIWHEN(...) ::sf::meta::when<(bool)(__VA_ARGS__)>
#define SIWITH(...) std::is_same<T, __VA_ARGS__>::value
#define SIVOID(...) ::sf::meta::to_void<decltype(__VA_ARGS__)>

#endif // SF_DETAIL_META_MACRO_HPP
