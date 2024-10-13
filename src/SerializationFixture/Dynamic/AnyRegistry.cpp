#include <SerializationFixture/Dynamic/AnyRegistry.hpp>

namespace sf
{

namespace dynamic
{

void any_registry_t::save(core::ioarchive_t& archive, std::any& any, let::u64 hash)
{
    all.at(hash).save(archive, any);
}

void any_registry_t::load(core::ioarchive_t& archive, std::any& any, let::u64 hash)
{
    all.at(hash).load(archive, any);
}

any_registry_t any_registry;

} // namespace dynamic

} // namespace sf
