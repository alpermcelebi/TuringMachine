
#pragma once
#include "typelist.h"

template<typename ...Ts>
struct Size<List<Ts...>> {
    static constexpr auto value = sizeof...(Ts);
};

// @brief base case -- Get the 0th element of a list
template<typename T, typename... Ts>
struct At<0, List<T, Ts...>> {
    using type = T;
};

// @brief inductive case -- Get the Nth element of a list
template<int index, typename T, typename... Ts>
struct At<index, List<T, Ts...>> {
    static_assert(index > 0, "index cannot be negative");
    using type = typename At<index - 1, List<Ts...>>::type;
};

struct NotImpl;
#define NOT_IMPL NotImpl
//haskell gibi, empty case
template<typename Q, int index, typename List>
struct Find {
    static constexpr int value = -1;
};

// recursive case: non-empty
template<typename Q, int index, typename T, typename... Ts>
struct Find<Q, index, List<T, Ts...>> {
    static constexpr int value = Find<Q, index + 1, List<Ts...>>::value;
};

// Matched type bulundu: Q=Q
template<typename Q, int index, typename... Ts>
struct Find<Q, index, List<Q, Ts...>> {
    static constexpr int value = index;
};

//replace q 0 (x:xs) = q:xs
//replace q i (x:xs) = x:find q (i-1) xs
// Base case: Replace the 0th element of a list
template<typename Q, typename T, typename... Ts>
struct Replace<Q, 0, List<T, Ts...>> {
    using type = List<Q, Ts...>;
};

// Inductive case: Replace the Nth element of a list
template<typename Q, int index, typename T, typename... Ts>
struct Replace<Q, index, List<T, Ts...>> {
    static_assert(index >= 0, "index cannot be negative");
    using type = typename Prepend<T, typename Replace<Q, index - 1, List<Ts...>>::type>::type;
};

// Specialization for empty list or index out of bounds case
template<typename Q, int index>
struct Replace<Q, index, List<>> {
    using type = List<>;
};

// non-empty list
template<typename NewItem, typename... Ts>
struct Append<NewItem, List<Ts...>> {
    using type = List<Ts..., NewItem>;
};

// empty list
template<typename NewItem>
struct Append<NewItem, List<>> {
    using type = List<NewItem>;
};






//nonempty
template<typename NewItem, typename... Ts>
struct Prepend<NewItem, List<Ts...>> {
    // @TODO: Implement
    using type = List<NewItem, Ts...>;
};
//empty
template<typename NewItem>
struct Prepend<NewItem, List<>> {
    // @TODO: Implement
    using type = List<NewItem>;
};


// non-empty list
template<template<typename> typename F, typename Ret, typename T, typename... Ts>
struct Map<F, Ret, List<T, Ts...>> {
    using type = List<typename F<T>::type, typename F<Ts>::type...>;
};

// empty list
template<template<typename> typename F, typename Ret>
struct Map<F, Ret, List<>> {
    using type = List<>;
};








// Conditional template to select a type based on a boolean value
template<bool Cond, typename Then, typename Else>
struct Conditional;

template<typename Then, typename Else>
struct Conditional<true, Then, Else> {
    using type = Then;
};

template<typename Then, typename Else>
struct Conditional<false, Then, Else> {
    using type = Else;
};


// Base case: empty list
template<template<typename> typename F, typename Ret>
struct Filter<F, Ret, List<>> {
    using type = Ret;
};

// Recursive case: non-empty list
template<template<typename> typename F, typename Ret, typename T, typename... Ts>
struct Filter<F, Ret, List<T, Ts...>> {
private:
    // Check if the current element satisfies the predicate
    static constexpr bool passs = F<T>::value;

    // Recursively filter the rest of the list
    using filtered_tail = typename Filter<F, Ret, List<Ts...>>::type;

    // If the current element satisfies the predicate, prepend it to the result
    using new_ret = typename Conditional<passs, typename Prepend<T, filtered_tail>::type, filtered_tail>::type;

public:
    using type = new_ret;
};