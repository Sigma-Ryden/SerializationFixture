#ifndef SIRAF_DATA_TRACK_HPP
#define SIRAF_DATA_TRACK_HPP

#include <memory> // addressof

#include <Siraf/DataTrackBase.hpp>

#include <Siraf/NativeSaveLoad.hpp>

#include <Siraf/ApplyFunctor.hpp>

#include <Siraf/Strict.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define _SIRAF_APPLY_FUNCTOR_GENERIC(class_name, function_name)                                         \
    template <typename T>                                                                               \
    class class_name : public ApplyFunctor {                                                            \
    private:                                                                                            \
        T& parameter_;                                                                                  \
    public:                                                                                             \
        class_name(T& parameter) noexcept : parameter_(parameter) {}                                    \
        template <typename Archive, SIREQUIRE(meta::is_archive<Archive>())>                             \
        void operator() (Archive& archive)                                                              \
        { ::siraf::tracking::function_name(archive, parameter_); }                                      \
    };

#define _SIRAF_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC(class_name, function_name)                        \
    template <typename T>                                                                               \
    apply::class_name<T> function_name(T& data) { return { data }; }

namespace siraf
{

namespace tracking
{

template <typename TrackType, class Archive, typename key_type,
          SIREQUIRE(meta::is_archive<Archive>())>
bool is_track(Archive& archive, key_type key)
{
    auto& item = archive.template tracking<TrackType>();
    return item.find(key) != item.end();
}

template <typename TrackType, class Archive, typename key_type,
          SIREQUIRE(meta::is_archive<Archive>())>
bool is_mixed(Archive& archive, key_type key)
{
    using reverse_track_type = typename reverse_trait<TrackType>::trait;

    return is_track<reverse_track_type>(archive, key);
}

template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>()
                    and meta::is_pointer<T>())>
void track(WriteArchive& archive, T& pointer)
{
    using key_type = typename WriteArchive::TrackingKeyType;

#ifndef SIRAF_NULLPTR_DISABLE
    const auto success = detail::is_refer(archive, pointer); // serialize refer info
    if (not success) return;
#endif // SIRAF_NULLPTR_DISABLE

    auto pure = memory::pure(pointer);
    auto key = reinterpret_cast<key_type>(memory::raw(pure));

    auto& is_tracking = archive.tracking()[key];

    if (not is_tracking)
    {
        archive & key;
        strict(archive, pointer); // call the strict serialization of not tracking pointer

        is_tracking = true;
    }
    else
    {
        detail::native_save(archive, pointer, key);
    }
}

template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>()
                    and not meta::is_pointer<T>())>
void track(WriteArchive& archive, T& data)
{
    using key_type = typename WriteArchive::TrackingKeyType;

    auto address = memory::pure(std::addressof(data));
    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.tracking()[key];

    if (is_tracking)
        throw "The write tracking data is already tracked.";

    is_tracking = true;

    archive & key;
    archive & data;
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>()
                    and meta::is_pointer<T>())>
void track(ReadArchive& archive, T& pointer)
{
    using key_type   = typename ReadArchive::TrackingKeyType;
    using track_type = typename tracking::track_trait<T>::trait;

    if (pointer != nullptr)
        throw "The read track pointer must be initialized to nullptr.";

#ifndef SIRAF_NULLPTR_DISABLE
    const auto success = detail::is_refer(archive, pointer); // serialize refer info
    if (not success) return;
#endif // SIRAF_NULLPTR_DISABLE

    key_type key;
    archive & key;

#ifdef SIRAF_DEBUG
    if (is_mixed<track_type>(archive, key))
        throw "Mixed pointer tracking is not allowed.";
#endif // SIRAF_DEBUG

    auto& item = archive.template tracking<track_type>()[key];

    if (item.address == nullptr)
    {
        strict(archive, pointer); // call the strict serialization of not tracking pointer
        item.address = memory::pure(pointer);
    }
    else
    {
        detail::native_load(archive, pointer, item.address);
    }
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>()
                    and not meta::is_pointer<T>())>
void track(ReadArchive& archive, T& data)
{
    using key_type   = typename ReadArchive::TrackingKeyType;
    using track_type = tracking::Raw;

    key_type key;
    archive & key;

    auto& item = archive.template tracking<track_type>()[key];

    if (item.address != nullptr)
        throw  "The read tracking data is already tracked.";

    item.address = memory::pure(std::addressof(data));

    archive & data;
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<T>() and
                    meta::is_serializable_raw_pointer<T>())>
void raw(Archive& archive, T& pointer)
{
#ifndef SIRAF_NULLPTR_DISABLE
    if (detail::is_refer(archive, pointer))
#endif //SIRAF_NULLPTR_DISABLE
    strict(archive, pointer);
}

} // namespace tracking

namespace apply
{

_SIRAF_APPLY_FUNCTOR_GENERIC(TrackFunctor, track)
_SIRAF_APPLY_FUNCTOR_GENERIC(RawFunctor, raw)

} // namespace apply

namespace tracking
{

_SIRAF_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC(TrackFunctor, track)
_SIRAF_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC(RawFunctor, raw)

} // namespace tracking

} // namespace siraf

// clean up
#undef _SIRAF_APPLY_FUNCTOR_GENERIC
#undef _SIRAF_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC

#endif // SIRAF_DATA_TRACK_HPP
