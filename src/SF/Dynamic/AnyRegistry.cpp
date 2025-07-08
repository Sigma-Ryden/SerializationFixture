#include <SF/Dynamic/AnyRegistry.hpp>

#if !defined(SF_ANY_SUPPORT_DISABLE)

namespace sf
{

namespace dynamic
{

void any_registry_t::save(ioarchive_t& archive, std::any& any, std::uint64_t hash)
{
    all.at(hash).save(archive, any);
}

void any_registry_t::load(ioarchive_t& archive, std::any& any, std::uint64_t hash)
{
    all.at(hash).load(archive, any);
}

any_registry_t any_registry;

} // namespace dynamic

} // namespace sf

#endif // if
