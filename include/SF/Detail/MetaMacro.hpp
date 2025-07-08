#ifndef SF_DETAIL_META_MACRO_HPP
#define SF_DETAIL_META_MACRO_HPP

#define SF_REQUIRES(...) typename std::enable_if<(bool)(__VA_ARGS__), int>::type = 0

#endif // SF_DETAIL_META_MACRO_HPP
