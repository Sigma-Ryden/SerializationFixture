#ifndef SIFAR_SERIALIZATON_BASE_HPP
#define SIFAR_SERIALIZATON_BASE_HPP

namespace sifar
{

template <class InStream, class Registry, class StreamWrapper>
class ReadArchive;

template <class OutStream, class Registry, class StreamWrapper>
class WriteArchive;

} // namespace sifar

#endif // SIFAR_SERIALIZATON_BASE_HPP
