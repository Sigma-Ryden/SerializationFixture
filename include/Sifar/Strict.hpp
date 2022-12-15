#ifndef SIFAR_STRICT_HPP
#define SIFAR_STRICT_HPP

#include <Sifar/ApplyFunctor.hpp>

#include <Sifar/Memory/Memory.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

namespace sifar
{

template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>() and
                    meta::is_pod_pointer<T>())>
void strict(WriteArchive& archive, T& pod_pointer)
{
    if (pod_pointer == nullptr)
        throw "the write pointer must be allocated.";

    archive & (*pod_pointer);
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>() and
                    meta::is_pod_pointer<T>())>
void strict(ReadArchive& archive, T& pod_pointer)
{
    using item_type = typename memory::ptr_trait<T>::item;

    if (pod_pointer != nullptr)
        throw "the read pointer must be initialized to nullptr.";

    memory::allocate<item_type>(pod_pointer);

    archive & (*pod_pointer);
}

template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>() and
                    meta::is_pointer_to_polymorphic<T>())>
void strict(WriteArchive& archive, T& pointer)
{
    auto& registry = archive.registry();

    auto id = registry.save_key(archive, pointer);
    registry.save(archive, pointer, id);
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>() and
                    meta::is_pointer_to_polymorphic<T>())>
void strict(ReadArchive& archive, T& pointer)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.load(archive, pointer, id);
}

namespace detail
{

template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>() and
                    meta::is_serializable_pointer<T>() )>
bool is_refer(WriteArchive& archive, T& pointer)
{
    auto info = static_cast<bool>(pointer);
    archive & info;

    return info;
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>() and
                    meta::is_serializable_pointer<T>())>
bool is_refer(ReadArchive& archive, T& pointer)
{
#ifdef SIFAR_DEBUG
    if (pointer != nullptr)
        throw "the read pointer must be initialized to nullptr.";
#endif // SIFAR_DEBUG

    auto info = false;
    archive & info;

    return info;
}

} // namespace detail

namespace apply
{

template <typename T>
class StrictFunctor : public ApplyFunctor
{
private:
    T& parameter_;

public:
    StrictFunctor(T& parameter) noexcept : parameter_(parameter) {}

    template <typename Archive, SIREQUIRE(meta::is_archive<Archive>())>
    void operator() (Archive& archive)
    {
        ::sifar::strict(archive, parameter_);
    }
};

} // namespace apply

template <typename T>
apply::StrictFunctor<T> strict(T& parameter) { return { parameter }; }

} // namespace sifar

#endif // SIFAR_STRICT_HPP
