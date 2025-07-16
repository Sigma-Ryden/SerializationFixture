#include <Eightser/Dynamic/AnyRegistry.hpp>

#if !defined(EIGHTSER_ANY_SUPPORT_DISABLE)

namespace eightser
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

} // namespace eightser

#endif // if
