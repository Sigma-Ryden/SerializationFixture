#ifndef SF_DETAIL_META_MACRO_HPP
#define SF_DETAIL_META_MACRO_HPP

#define SF_REQUIRE(...) typename std::enable_if<(bool)(__VA_ARGS__), int>::type = 0
#define SF_WHEN(...) typename std::enable_if<(bool)(__VA_ARGS__)>::type
#define SF_VOID(...) ::sf::meta::to_void<decltype(__VA_ARGS__)>

#endif // SF_DETAIL_META_MACRO_HPP
