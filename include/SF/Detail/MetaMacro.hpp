#ifndef SF_DETAIL_META_MACRO_HPP
#define SF_DETAIL_META_MACRO_HPP

#define SF_REQUIRE(...) ::sf::meta::require<(bool)(__VA_ARGS__)> = 0
#define SF_WHEN(...) ::sf::meta::when<(bool)(__VA_ARGS__)>
#define SF_VOID(...) ::sf::meta::to_void<decltype(__VA_ARGS__)>

#endif // SF_DETAIL_META_MACRO_HPP
