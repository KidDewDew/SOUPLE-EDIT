#ifndef SOUPLE_AUX_H
#define SOUPLE_AUX_H

#include <type_traits>

template<class T>
concept like_pointer = requires(T t) {
    t.operator->();
} || std::is_pointer_v<T>;

template<class T>
struct ToPointerLike {
    using type_noref = std::remove_reference_t<T>;
    using type = std::conditional_t<like_pointer<T>,T,type_noref*>;
};

template<class T>
using ToPointerLike_t = ToPointerLike<T>::type;

template<class T>
    requires like_pointer<T>
std::add_lvalue_reference_t<std::remove_reference_t<T>> toPointerLike(T& t) {
    return t;
}

template<class T>
    requires (!like_pointer<T>)
auto toPointerLike(T& t) {
    return &t;
}

#define MEMBER(member) \
views::transform([](auto& _){ return toPointerLike(_)->member; })

#endif // SOUPLE_AUX_H
