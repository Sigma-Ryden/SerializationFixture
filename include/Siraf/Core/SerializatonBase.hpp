#ifndef SIRAF_SERIALIZATON_BASE_HPP
#define SIRAF_SERIALIZATON_BASE_HPP

namespace siraf
{

namespace core
{

class ArchiveBase;

} // namespace core

template <class InStream, class StreamWrapper, class Registry>
class ReadArchive;

template <class OutStream, class StreamWrapper, class Registry>
class WriteArchive;

} // namespace siraf

#endif // SIRAF_SERIALIZATON_BASE_HPP
