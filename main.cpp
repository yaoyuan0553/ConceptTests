// Type your code here, or load an example.
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <cstdio>
#include <compare>

using namespace std;

template <typename T>
concept Incrementable = requires(T x) { ++x; x++; };

template <typename T>
concept Decrementable = requires(T x) { --x; x--; };

class Animal {
protected:
    string name_;
    explicit Animal(const char* name) : name_(name) { }
public:
    virtual ~Animal() = default;
    virtual void makeSound() const = 0;
};

/**
 * class who implements this interface are homeothermic animal
 */
class Homeothermic {
protected:
    double bodyTemperature_;
    explicit Homeothermic(double bodyTemp) : bodyTemperature_(bodyTemp) { }
public:
    virtual void setBodyTemperature(double bodyTemp)
    {
        bodyTemperature_ = bodyTemp;
    }

    virtual double getBodyTemperature() const
    {
        printf("temp: %.2f\n", bodyTemperature_);
        return bodyTemperature_;
    }
};

class Reptilia : public Animal {
protected:
    explicit Reptilia(const char* name) : Animal(name) { }
public:
    ~Reptilia() override = default;
};

class Snake : public Reptilia {
public:
    explicit Snake(const char* name) : Reptilia(name) { }
    void makeSound() const override
    {
        printf("hiss\n");
    }
};

/*
 * birds
 */
class Tori : public Animal, public Homeothermic {
protected:
    double flyingSpeed_;
    Tori(const char* name, double bodyTemp, double flightSpeed) :
            Animal(name), Homeothermic(bodyTemp), flyingSpeed_(flightSpeed) { }
public:
    ~Tori() override = default;

    virtual double getFlyingSpeed() const
    {
        printf("%s flight speed: %.2f\n", name_.c_str(), flyingSpeed_);
        return flyingSpeed_;
    }
    virtual void setFlyingSpeed(double flyingSpeed)
    {
        flyingSpeed_ = flyingSpeed;
    }
};

class Owl : public Tori {
private:
    inline static constexpr double TEMP = 38.2;
    inline static constexpr double SPEED = 15.0;
public:
    explicit Owl(const char* name) : Tori(name, TEMP, SPEED) { }
    void makeSound() const override
    {
        printf("hoo-hoo-hoooooo\n");
    }
};

class Mammal : public Animal, public Homeothermic {
protected:
    Mammal(const char* name, double bodyTemp) : Animal(name), Homeothermic(bodyTemp) { }
public:
    ~Mammal() override = default;
};

class Cat : public Mammal {
    inline static constexpr double TEMP = 37.3;
public:
    explicit Cat(const char* name) : Mammal(name, TEMP) { }
    void makeSound() const override
    {
        printf("meow\n");
    }
};

class Dog : public Mammal {
    inline static constexpr double TEMP = 36.8;
public:
    explicit Dog(const char* name) : Mammal(name, TEMP) { }
    void makeSound() const override
    {
        printf("woof\n");
    }
};

class Bat : public Mammal {
    inline static constexpr double TEMP = 40.0;
public:
    explicit Bat(const char* name) : Mammal(name, TEMP) { }
    void makeSound() const override
    {
        printf("**ultra-sound**\n");
    }
};

class Test {
    int x_ = 0;
public:
    explicit Test(int x) : x_(x) { }
    void print() const 
    {
        printf("%d\n", x_);
    }
    //strong_ordering operator<=>(const Test& other) const
    //{
        //return x_ <=> other.x_;
    //}
    auto operator<=>(const Test&) const = default;
};

//template< class Derived, class Base >
//    concept derived_from =
//            std::is_base_of_v<Base, Derived> && std::is_convertible_v<const volatile Derived*, const volatile Base*>;

void makeSound(const Animal& animal)
{
    animal.makeSound();
}

void makeSound(const vector<reference_wrapper<const Animal>>& animals)
{
    for (const Animal& animal : animals) {
        animal.makeSound();
    }
}

//template< class Derived, class Base >
//concept derived_from =
        //std::is_base_of_v<Base, Derived> &&
                //std::is_convertible_v<const volatile Derived*, const volatile Base*>;

template <typename Base, typename... Ts>
concept ChildrenOf = (... && derived_from<std::decay_t<Ts>, Base>);

template <typename... Ts>
concept ChildrenOfAnimal = ChildrenOf<Animal, Ts...>;

// template <typename T0, typename... Ts>
// requires (std::derived_from<std::remove_cvref_t<T0>, Animal> 
//         && ... && std::derived_from<std::remove_cvref_t<Ts>, Animal>)
// template <typename T0, typename... Ts>
//     requires (ChildrenOf<Animal, T0, Ts...>)
template <ChildrenOfAnimal T0, ChildrenOfAnimal... Ts>
struct MakeSoundAllImpl {
    static void makeSound(T0&& animal, Ts&&... animals)
    {
        animal.makeSound();

        if constexpr (sizeof...(animals) == 0)
            return;
        else
            MakeSoundAllImpl<Ts...>::makeSound(forward<Ts>(animals)...);
    }
};

template <typename Base>
struct Maker {
private:
    // requires (std::derived_from<std::remove_cvref_t<T0>, Base> &&...&& std::derived_from<std::remove_cvref_t<Ts>, Base>)
    template <typename T0, typename... Ts>
        requires ChildrenOf<Base, T0, Ts...>
    struct CallFuncNoParam {
        template <typename Func>
        static void call(Func&& f, T0&& obj, Ts&&... objs)
        {
            std::invoke(std::forward<Func>(f), std::forward<T0>(obj));

            if constexpr (sizeof...(objs) == 0)
                return;
            else
                CallFuncNoParam<Ts...>::call(std::forward<Func>(f), std::forward<Ts>(objs)...);
        }
    };
public:
    template <typename Func, typename... Ts>
        // requires(... && std::derived_from<std::remove_cvref_t<Ts>, Base>)
    static void callFuncOfObjs(Func&& f, Ts&&... objs) requires ChildrenOf<Base, Ts...>
    {
        if constexpr (sizeof...(objs) == 0)
            return;
        else
            //CallFuncNoParam<Ts...>::call(std::forward<Func>(f), std::forward<Ts>(objs)...);
            CallFuncNoParam<Ts...>::call(std::forward<Func>(f), std::forward<Ts>(objs)...);
    }
};

void blah()
{
    std::string x;
}

    
//template <typename... Ts>
//    requires (... && derived_from<remove_cvref_t<Ts>, Animal>)
// template <typename... Ts>
//     requires ChildrenOf<Animal, Ts...>
template <ChildrenOfAnimal... Ts>
void makeSoundAll(Ts&&... animals)
{
    if constexpr (sizeof...(animals) == 0)
        return;
    else
        MakeSoundAllImpl<Ts...>::makeSound(std::forward<Ts>(animals)...);
}

template<template<class> typename C, typename E>
void xds(C<E>) {
    C<E> x;
}


void testThis()
{
    Cat doug("doug");
    Dog kat("kat");
    Bat viro("viro");
    Owl owl("owl");
    Snake sni("sni");

//    makeSound(doug);
//    makeSound({cref(doug), cref(kat), cref(viro)});

    makeSoundAll();
    makeSoundAll(doug, kat, viro);

    Maker<Animal>::callFuncOfObjs(&Animal::makeSound, doug, kat, viro, owl, sni);
    Maker<Animal>::callFuncOfObjs(&Animal::makeSound);
    Maker<Homeothermic>::callFuncOfObjs(&Homeothermic::getBodyTemperature, doug, kat, viro, owl);
    Maker<Tori>::callFuncOfObjs(&Tori::getFlyingSpeed, owl);
}

int main()
{
    testThis();

    Test t1{1};
    Test t2{2};

    cout << boolalpha << (t1 == t2) << " " << (t1 < t2) << " " << (t1 > t2) << " " << (t1 != t2) << '\n';

    return 0;
}
