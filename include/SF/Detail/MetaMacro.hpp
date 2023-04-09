#ifndef SF_DETAIL_META_MACRO_HPP
#define SF_DETAIL_META_MACRO_HPP

#define SFREQUIRE(...) ::sf::meta::require<(bool)(__VA_ARGS__)> = 0
#define SFWHEN(...) ::sf::meta::when<(bool)(__VA_ARGS__)>
#define SFVOID(...) ::sf::meta::to_void<decltype(__VA_ARGS__)>

#endif // SF_DETAIL_META_MACRO_HPP
