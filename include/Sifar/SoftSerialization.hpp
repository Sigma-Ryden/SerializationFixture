#ifndef SIFAR_SOFT_SERIALIZATION_HPP
#define SIFAR_SOFT_SERIALIZATION_HPP

#include <Sifar/ApplyFunctor.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

namespace sifar
{

template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>() and
                    meta::is_serializable_pointer<T>())>
void soft(WriteArchive& archive, T& pointer)
{
    auto is_mapped = static_cast<bool>(pointer);
    archive & is_mapped;

    if (is_mapped) archive & pointer; // call of stricted pointer serialization
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>() and
                    meta::is_serializable_pointer<T>())>
void soft(ReadArchive& archive, T& pointer)
{
    if (pointer != nullptr)
        throw "the read pointer must be initialized to nullptr.";

    auto is_mapped = false;
    archive & is_mapped;

    if (is_mapped) archive & pointer;
}

namespace apply
{

template <typename T>
class SoftFunctor : public ApplyFunctor
{
private:
    T& parameter_;

public:
    SoftFunctor(T& parameter) noexcept : parameter_(parameter) {}

    template <typename Archive, SIREQUIRE(meta::is_archive<Archive>())>
    void operator() (Archive& archive)
    {
        soft(archive, parameter_);
    }
};

} // namespace apply

template <typename T>
apply::SoftFunctor<T> soft(T& parameter) { return { parameter }; }

} // namespace sifar

#endif // SIFAR_SOFT_SERIALIZATION_HPP
