#ifndef SERIALIZATION_REGISTRY_HPP
#define SERIALIZATION_REGISTRY_HPP

namespace serialization
{

// will be repair
template <typename To, typename From>
To& base(From& derived)
{
    return static_cast<To&>(derived);
}

} // namespace serialization

#endif // SERIALIZATION_REGISTRY_HPP
