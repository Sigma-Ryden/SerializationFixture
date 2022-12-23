#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Siraf/Core.hpp> // ReadArchive, WriteArchive

#include <Siraf/Support/string.hpp>

using siraf::iarchive;
using siraf::oarchive;

using siraf::base;

using namespace siraf::library;

class Human
{
    SERIALIZABLE(Human)

protected:
    std::string name_;
    int age_;

public:
    Human(const std::string& name, int age)
        : name_(name), age_(age)
    {
    }

    const std::string& name() const noexcept { return name_; }
    int age() const noexcept { return age_; }
};

SAVE_LOAD_SERIALIZABLE(Human)
{
    archive & self.name_
            & self.age_;
}

class Boy : public Human
{
    SERIALIZABLE(Boy)

private:
    int force_;

public:
    Boy() : Human("unnamed", 0), force_(0)
    {
    }

    Boy(const std::string& name, int age, int force)
        : Human(name, age), force_(force)
    {
    }

    int force() const noexcept { return force_; }
};

// Macro same as SAVE_LOAD_SERIALIZABLE
SERIALIZATION(Boy)
{
    archive & base<Human>(self)
            & self.force_;
}

void test_object_serialization()
{
    {
        std::ofstream file("test_object_serialization.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = oarchive(file);

        Boy obj("Tom", 21, 9);

        std::string hi = "Hello!";
        char bye[] = "Good Bye!";

        ar & obj;
        ar & hi;
        ar & bye;

        file.close();

        std::cout << "Serialization done.\n";
    }
    {
        std::ifstream file("test_object_serialization.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = iarchive(file);

        Boy obj;

        std::string hi;
        char bye[10];

        ar & obj;
        ar & hi;
        ar & bye;

        file.close();

        std::cout << "Deserialization done.\n";
        //
        std::cout << obj.name() << ' ' << obj.age() << ' ' << obj.force()
                  << ' ' << hi << ' ' << bye << '\n';
        //
    }
}

int main()
{
    test_object_serialization();

    return 0;
}
