#pragma once
#include "turing-machine_utils.h"

// Utility templates
template <typename A, typename B> struct is_same { static constexpr auto value = false; };
template <typename A> struct is_same<A, A>{ static constexpr auto value = true; };

template <bool C, typename = void> struct EnableIf;
template <typename Type> struct EnableIf<true, Type>{ using type = Type; };

// Base case: Machine is in accepting or rejecting state
template <typename InputTape, typename State, int Position, typename Transitions>
struct TransitionFunction<Configuration<InputTape, State, Position>, Transitions,
                          typename EnableIf<is_same<State, QAccept>::value ||
                                            is_same<State, QReject>::value>::type>
{
    using end_config = Configuration<InputTape, State, Position>;
    using end_input = InputTape;
    using end_state = State;
    static constexpr int end_position = Position;
};

// Inductive case: Machine is not in an accepting or rejecting state
template <typename Config, typename Transitions, typename>
struct TransitionFunction
{
    using input_tape = typename Config::input_tape;
    using state = typename Config::state;
    static constexpr int position = Config::position;
    using current_input = typename input_tape::template at<position>;

    template <typename Rule>
    struct Matches
    {
        static constexpr bool value = is_same<typename Rule::old_state, state>::value &&
                                      is_same<typename Rule::input, current_input>::value;
    };

    template <typename List>
    struct FindRule;

    template <typename Head, typename... Tail>
    struct FindRule<List<Head, Tail...>>
    {
        using type = typename std::conditional<Matches<Head>::value, Head, typename FindRule<List<Tail...>>::type>::type;
    };

    template <>
    struct FindRule<List<>>
    {
        using type = Rule<state, current_input, QReject, current_input, STOP>;
    };

    using rule = typename FindRule<Transitions>::type;
    using new_input_tape = typename input_tape::template replace<position, typename rule::output>;
    using new_config = Configuration<new_input_tape, typename rule::new_state, position + rule::direction>;

    using next_transition = TransitionFunction<new_config, Transitions>;

    using end_config = typename next_transition::end_config;
    using end_input = typename next_transition::end_input;
    using end_state = typename next_transition::end_state;
    static constexpr int end_position = next_transition::end_position;
};
