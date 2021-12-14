#ifndef ELDERLISP_AST_HPP
#define ELDERLISP_AST_HPP

#include <variant>
#include <functional>
#include <vector>

namespace el {

// forward declarations
struct UnaryFunction;
struct NullaryFunction;

struct Scalar;
struct Tuple;

struct ScalarOrTuple;

// definitions
struct UnaryFunction : std::function<ScalarOrTuple(ScalarOrTuple)> {};
struct NullaryFunction : std::function<ScalarOrTuple()> {};

struct Scalar : std::variant<long, char, UnaryFunction, NullaryFunction> {};
struct Tuple : std::vector<std::variant<Scalar, Tuple>> {};

struct ScalarOrTuple : std::variant<Scalar, Tuple> {};
}    // namespace el

#endif    // ELDERLISP_AST_HPP
