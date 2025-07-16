#ifndef EIGHTSER_DETAIL_META_MACRO_HPP
#define EIGHTSER_DETAIL_META_MACRO_HPP

#define EIGHTSER_REQUIRES(...) typename std::enable_if<(bool)(__VA_ARGS__), int>::type = 0

#endif // EIGHTSER_DETAIL_META_MACRO_HPP
