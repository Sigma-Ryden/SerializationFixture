#ifndef SERIALIZATION_ACCESS_HPP
#define SERIALIZATION_ACCESS_HPP

#include <Serialization/Detail/Meta.hpp>

#include <Serialization/Detail/MacroScope.hpp>

#define SERIALIZATION_ARCHIVE_ACCESS(...) friend class serialization::Access;

namespace serialization
{

class Access
{
private:
    // Special type for has_function_tpl_helper meta
    struct dummy_type;

    SERIALIZATION_HAS_FUNCTION_TPL_HELPER(save);
    SERIALIZATION_HAS_FUNCTION_TPL_HELPER(load);

public:
    template <class T>
    static constexpr bool is_save_load_class() noexcept
    {
        return has_save<T>::value and has_load<T>::value;
    }

public:
    template <class Archive, class T,
              meta::require<is_save_load_class<T>()> = 0>
    static void save(Archive& archive, T& object)
    {
        object.save(archive);
    }

    template <class Archive, class T,
              meta::require<is_save_load_class<T>()> = 0>
    static void load(Archive& archive, T& object)
    {
        object.load(archive);
    }
};

} // namespace serialization

#include <Serialization/Detail/MacroUnscope.hpp>

#endif // SERIALIZATION_ACCESS_HPP
