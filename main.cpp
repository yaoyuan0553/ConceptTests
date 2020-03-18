// Type your code here, or load an example.
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <string>
#include <unordered_set>
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

template <typename Func>
concept Function = std::is_function_v<Func>;

template <typename Func>
concept MemberFunction = std::is_member_function_pointer_v<Func>;

template <typename Func>
concept Method = MemberFunction<Func>;

template <typename Func>
concept FunctionOrMethod = Function<Func> || Method<Func>;

template <typename Ret, typename Func, typename... Args>
concept FunctionWithReturnType =
    Function<Func> && same_as<invoke_result_t<Func, Args...>, Ret>;

template <typename Ret, typename Func, typename Type, typename... Args>
concept MethodWithReturnType = 
    Method<Func> && same_as<invoke_result_t<Func, Type, Args...>, Ret>;

template <typename Func, typename Type>
concept MethodNoArgsWithReturnString = MethodWithReturnType<string, Func, Type>;


namespace details
{
    template <typename T, typename = void_t<>>
    struct _has_toString_t : std::false_type { };

    template <typename T>
    struct _has_toString_t<T, void_t<is_same<
        decltype((declval<T>().toString())), 
        std::string
    >>> : std::true_type { };

    template <typename T, typename = void_t<>>
    struct _has_toString_const_t : std::false_type { };

    template <typename T>
    struct _has_toString_const_t<T, void_t<is_same<
        decltype((declval<T const>().toString())), 
        std::string
    >>> : std::true_type { };
}

namespace details
{
    template <
        typename Default, 
        typename AlwaysVoid, 
        template<class...> typename MetaOp, 
        typename... MetaArgs
    >
    struct Detector {
        using type = Default;
        using value_t = false_type;
        static constexpr bool value = value_t::value;
    };

    template <
        typename Default,
        template<class...> typename MetaOp,
        typename... MetaArgs
    >
    struct Detector<Default, void_t<MetaOp<MetaArgs...>>, MetaOp, MetaArgs...> {
        using type = MetaOp<MetaArgs...>;
        using value_t = true_type;
        static constexpr bool value = value_t::value;
    };
}

/**
 * @brief default type of the Default template parameter of Detector 
 * when a given <MetaOp> and <MetaArgs> fail to evaluate to [true_type]
 */
struct Nonesuch {
    Nonesuch() = delete;
    ~Nonesuch() = delete;
    Nonesuch(const Nonesuch&) = delete;
    void operator=(const Nonesuch&) = delete;
};

template <template<class...> typename MetaOp, typename... MetaArgs>
using is_detected = details::Detector<Nonesuch, void, MetaOp, MetaArgs...>;

template <template<class...> typename MetaOp, typename... MetaArgs>
using is_detected_t = typename is_detected<MetaOp, MetaArgs...>::value_t;

template <template<class...> typename MetaOp, typename... MetaArgs>
constexpr bool is_detected_v = is_detected_t<MetaOp, MetaArgs...>::value;

template <template<class...> typename MetaOp, typename... MetaArgs>
using detected_t = typename details::Detector<Nonesuch, void, MetaOp, MetaArgs...>::type;

template <typename Default, template<class...> typename MetaOp, typename... MetaArgs>
using detected_or = details::Detector<Default, void, MetaOp, MetaArgs...>;

template <typename Default, template<class...> typename MetaOp, typename... MetaArgs>
using detected_or_t = typename details::Detector<Default, void, MetaOp, MetaArgs...>::type;

/**
 * detection for T::hashCode() const;
 */
template <typename T>
using hashCode_type = decltype(declval<const T&>().hashCode());

template <typename T>
using has_hashCode = is_detected<hashCode_type, T>;

/**
 * obtains the return type of T::hashCode() const
 */
template <typename T>
using has_hashCode_ret_t = typename has_hashCode<T>::type;

template <typename T>
using has_hashCode_t = is_detected_t<hashCode_type, T>;

template <typename T>
constexpr bool has_hashCode_v = has_hashCode_t<T>::value;

template <typename T>
using has_hashCode_ret_int = std::is_same<int, has_hashCode_ret_t<T>>;

template <typename T>
constexpr bool has_hashCode_ret_int_v = has_hashCode_ret_int<T>::value;

template <typename T>
concept HasHashCode = has_hashCode_ret_int_v<T>;

template <typename T>
enable_if_t<has_hashCode_ret_int_v<T>, void> printObjViaHashCodeSFINAE(T&& t)
{
    cout << t.hashCode() << '\n';
}

/**
 * toString()
 */
template <typename T>
struct has_toString_t : details::_has_toString_t<T> { };

template <typename T>
constexpr bool has_toString_v = has_toString_t<T>::value;

template <typename T>
concept HasToString = requires(const T& t) { t.toString(); };

template <typename T>
concept HasToStringReturnString = 
    MethodWithReturnType<string, decltype(&T::toString), T>;

template <typename T>
struct has_toString_const_t : details::_has_toString_const_t<T> { };

template <typename T>
constexpr bool has_toString_const_v = has_toString_const_t<T>::value;

template <typename T>
concept HasToStringConst = requires(T&& t, string (T::*)() const) { 
    t.toString();
};


template <typename T>
std::enable_if_t<has_toString_v<T>, void> printObjViaToStringSFINAE(const T& t) {
    cout << t.toString() << '\n';
}

template <HasToStringReturnString T>
void printObjViaToStringConcept(const T& t) {
    cout << t.toString() << '\n';
}
// void printObjViaToStringConcept(HasToStringReturnString auto& t) {
    // cout << t.toString() << '\n';
// }

template <typename T>
std::enable_if_t<has_toString_const_v<T>, void> printObjViaToStringConstSFINAE(const T& t) {
    cout << t.toString() << '\n';
}

template <HasToStringConst T>
void printObjViaToStringConstConcept(const T& t) {
    cout << t.toString() << '\n';
}


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
    Maker<Homeothermic>::callFuncOfObjs(&Owl::getFlyingSpeed, owl);
}

struct TypeWithToString {
    int x, y;

    string toString() const { return to_string(x) + ", " + to_string(y); }
};

struct TypeWithoutToString {
    int x, y;
};

struct TypeWithToStringNonConst {
    int x, y;
    string toString() { return "okay"; }
};

struct TypeWithToStringIllformed {
    int x, y;
    int toString() const { return x; }
};

struct TypeWithHashCode {
    int x, y;

    int hashCode() const { return x * y; }
};

struct TypeWithoutHashCode {
    int x, y;
};

void testToString()
{
    TypeWithToString tw{1,2};
    // TypeWithoutToString two{3,4};
    // TypeWithToStringNonConst twnc{5,6};
    // TypeWithToStringIllformed twi{3,4};

    static_assert(FunctionOrMethod<decltype(&TypeWithToString::toString)>, "should be true");
    static_assert(MethodWithReturnType<string, decltype(&TypeWithToString::toString), TypeWithToString>, "should be true");

    printObjViaToStringConcept(tw);
    // printObjViaToStringConcept(two);
    // printObjViaToStringConcept(twnc);
    // printObjViaToStringConcept(twi);

    printObjViaToStringConstSFINAE(tw);
    // printObjViaToStringConstSFINAE(twnc);

    // TypeWithHashCode twh{1,2};
    // TypeWithoutHashCode twoh{3,4};

    // printObjViaHashCodeSFINAE(twh);
    // printObjViaHashCodeSFINAE(twoh);
}

int main()
{
    testThis();

    Test t1{1};
    Test t2{2};

    cout << boolalpha << (t1 == t2) << " " << (t1 < t2) << " " << (t1 > t2) << " " << (t1 != t2) << '\n';

    testToString();

    return 0;
}
