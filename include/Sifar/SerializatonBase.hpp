#ifndef SIFAR_SERIALIZATON_BASE_HPP
#define SIFAR_SERIALIZATON_BASE_HPP

namespace sifar
{

namespace core
{

class ArchiveBase;

} // namespace core

template <class InStream, class StreamWrapper, class Registry>
class ReadArchive;

template <class OutStream, class StreamWrapper, class Registry>
class WriteArchive;

} // namespace sifar

#endif // SIFAR_SERIALIZATON_BASE_HPP
