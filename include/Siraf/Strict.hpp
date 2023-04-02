#ifndef SIRAF_STRICT_HPP
#define SIRAF_STRICT_HPP

#include <Siraf/ApplyFunctor.hpp>

#include <Siraf/Core/Memory.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>() and
                    meta::is_pointer_to_standard_layout<T>())>
void strict(WriteArchive& archive, T& pointer)
{
    if (pointer == nullptr)
        throw "The write pointer must be allocated.";

    archive & (*pointer);
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>() and
                    meta::is_pointer_to_standard_layout<T>())>
void strict(ReadArchive& archive, T& pointer, Memory::void_ptr<T>& cache)
{
    using item_type = typename Memory::ptr_trait<T>::item;

    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";

    Memory::allocate<item_type>(pointer);
    cache = Memory::pure(pointer);

    archive & (*pointer);
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
void strict(ReadArchive& archive, T& pointer, Memory::void_ptr<T>& cache)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.load(archive, pointer, id, cache);
}

// verison without cache using
template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>() and
                    meta::is_serializable_pointer<T>())>
void strict(ReadArchive& archive, T& pointer)
{
    Memory::void_ptr<T> cache = nullptr;
    strict(archive, pointer, cache);
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
#ifdef SIRAF_DEBUG
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SIRAF_DEBUG

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
        ::siraf::strict(archive, parameter_);
    }
};

} // namespace apply

template <typename T>
apply::StrictFunctor<T> strict(T& parameter) { return { parameter }; }

} // namespace siraf

#endif // SIRAF_STRICT_HPP
