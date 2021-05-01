#include "interpreter.hpp"

#include "meta.hpp"
#include "ast.hpp"

#include <algorithm>
#include <iostream>

namespace elderLISP {
namespace interpreter {
    template<class T>
    auto constexpr atomIndex = meta::variantIndex<T, ast::Atom>;

    template<class T>
    auto constexpr nodeIndex = meta::variantIndex<T, ast::Node>;

    auto
    interpret(ast::Atom const& atom, Environment& environment) -> ast::Node
    {
        if(std::holds_alternative<ast::Name>(atom)) {
            auto&& [name] = std::get<atomIndex<ast::Name>>(atom);

            for(auto it = environment.crbegin(); it != environment.crend();
                it++) {
                auto const& map = *it;
                if(map.contains(name)) {
                    return map.find(name)->second;
                }
            }
        }

        return atom;
    }

    template<class T>
    auto
    get_atom_t(ast::Node const& node) -> T
    {
        return std::get<atomIndex<T>>(std::get<nodeIndex<ast::Atom>>(node));
    }

    auto
    interpret(ast::List expression, Environment& environment) -> ast::Node
    {
        auto scope = Scope(environment);

        auto const interpreter = [&](auto&& node) -> ast::Node {
            return interpret(node, environment);
        };

        auto constexpr isAtomic = [](auto&& node) -> bool {
            if(std::holds_alternative<ast::Atom>(node)) {
                return true;
            }

            return std::get<nodeIndex<ast::List>>(node).empty();
        };

        auto constexpr getList = [](auto&& node) -> ast::List {
            if(std::holds_alternative<ast::List>(node)) {
                return std::get<nodeIndex<ast::List>>(node);
            }

            return {};
        };

        auto constexpr getAtom = [=](auto&& node) -> ast::Atom {
            if(std::holds_alternative<ast::Atom>(node)) {
                return std::get<nodeIndex<ast::Atom>>(node);
            }

            return ast::NIL{};
        };

        auto const expandNames = [&](ast::List expr) {
            for(auto i = 0ul;
                !std::holds_alternative<ast::Quote>(getAtom(expr[0]))
                && i < expr.size();
                i++) {
                auto node = expr[i];

                if(isAtomic(node)) {
                    auto atom = getAtom(node);
                    if(std::holds_alternative<ast::Name>(atom)) {
                        auto interpNode = interpret(atom, environment);
                        if(std::holds_alternative<ast::List>(interpNode)) {
                            auto list = getList(interpNode);

                            expr.insert(
                                    expr.erase(expr.begin() + i),
                                    list.begin(),
                                    list.end());

                            i += list.size() - 1;
                        }
                        else {
                            expr[i] = interpNode;
                        }
                    }
                }
            }

            return expr;
        };

        expression = expandNames(std::move(expression));
        std::cerr << "interp 1\n";
        auto const action = getAtom(expression[0]);
        std::cerr << action.index() << " : action\n";

        switch(action.index()) {
        case atomIndex<ast::Atomic>: {
            return isAtomic(std::visit(interpreter, expression[1]))
                           ? ast::Atom{ast::True{}}
                           : ast::Atom{ast::False{}};
        } break;
        case atomIndex<ast::Equal>: {
            std::cerr << expression.size() << " : interp 2\n";
            auto lhsNode = std::visit(interpreter, expression[1]);
            std::cerr << "interp 3\n";
            auto rhsNode = std::visit(interpreter, expression[2]);
            std::cerr << "interp 4\n";

            if(!isAtomic(lhsNode) || !isAtomic(rhsNode)) {
                return ast::False{};
            }

            auto lhsAtom = getAtom(lhsNode);
            auto rhsAtom = getAtom(rhsNode);

            if(lhsAtom.index() != rhsAtom.index()) {
                return ast::False{};
            }

            switch(lhsAtom.index()) {
            case atomIndex<ast::StringLiteral>: {
                auto lhsString = get_atom_t<ast::StringLiteral>(lhsAtom).data;
                auto rhsString = get_atom_t<ast::StringLiteral>(rhsAtom).data;

                return lhsString == rhsString ? ast::Atom{ast::True{}}
                                              : ast::Atom{ast::False{}};
            } break;
            case atomIndex<ast::IntegerLiteral>: {
                auto lhsInteger = get_atom_t<ast::IntegerLiteral>(lhsAtom).data;
                auto rhsInteger = get_atom_t<ast::IntegerLiteral>(rhsAtom).data;

                return lhsInteger == rhsInteger ? ast::Atom{ast::True{}}
                                                : ast::Atom{ast::False{}};
            } break;
            }

            return ast::True{};
        } break;
        case atomIndex<ast::First>: {
            return getList(std::visit(interpreter, expression[1]))[0];
        } break;
        case atomIndex<ast::Rest>: {
            auto list = getList(std::visit(interpreter, expression[1]));
            list.erase(list.begin());

            if(list.empty()) {
                return ast::NIL{};
            }
            return list;
        } break;
        case atomIndex<ast::Combine>: {
            auto concatList = ast::List{};

            for(auto i = 1ul; i < expression.size(); i++) {
                auto node = std::visit(interpreter, expression[i]);
                if(isAtomic(node)) {
                    if(std::holds_alternative<ast::NIL>(getAtom(node))) {
                        continue;
                    }
                    concatList.push_back(node);
                }
                else {
                    auto list = getList(node);

                    concatList.insert(
                            concatList.cend(),
                            list.begin(),
                            list.end());
                }
            }

            return concatList;
        } break;
        case atomIndex<ast::Quote>: {
            return expression[1];
        } break;
        case atomIndex<ast::Condition>: {
            std::cerr << expression.size() - 1 << " : cond 1\n";
            for(auto i = 1ul; i < expression.size(); i++) {
                auto const condition = expandNames(getList(expression[i]));
                std::cerr << condition.size() << " : cond 2\n";

                auto const& predicate = condition[0];
                std::cerr << predicate.index() << " : cond 3\n";
                auto const result = getAtom(std::visit(interpreter, predicate));

                if(std::holds_alternative<ast::True>(result)) {
                    std::cerr << "cond 4\n";
                    auto const& body = condition[1];
                    return std::visit(interpreter, body);
                }
            }
        } break;
        case atomIndex<ast::Lambda>: {
            auto const arguments = getList(expression[1]);
            auto const body      = getList(expression[2]);
            auto const values    = getList(expression[3]);

            for(auto i = 0ul; i < arguments.size(); i++) {
                auto const name = get_atom_t<ast::Name>(getAtom(arguments[i]));
                environment.front()[name.name] = values[i];
            }

            return interpret(body, environment);
        } break;
        case atomIndex<ast::Let>: {
            auto const name = get_atom_t<ast::Name>(getAtom(expression[1]));
            auto& map       = *(environment.rbegin() + 1);

            map[name.name] = expression[2];
        } break;
        }
        return expression;
        // if(expression.size() > 2) {
        // auto list = expression;
        // list.erase(list.begin());
        // expression[1] = list;
        //}
        // return expression[1];
        //} break;
        // case atomIndex<ast::Let>: {
        // auto name = getName(expression[1]);

        // auto& env      = *(environment.rbegin() + 1);
        // env[name.name] = std::visit(interpreter, expression[2]);

        // return environment.front()[name.name];
        //} break;
        // case atomIndex<ast::Lambda>: {
        // std::cerr << "lambda\n";
        // auto names  = getList(expression[1]);
        // auto values = getList(expression[3]);
        // std::cerr << "lambda 2\n";

        // if(names.size() > 1) {
        // for(auto i = 0ul; i < names.size(); i++) {
        // auto name                     = getName(names[i]);
        // environment.back()[name.name] = values[i];
        //}
        //}
        // else if(names.size() == 1) {
        // auto name = getName(ast::Node{names});
        // std::cerr << "lambda 3\n";
        // for(auto i : values) {
        // std::cerr << std::get<atomIndex<ast::IntegerLiteral>>(
        // getAtom(i))
        //.data
        //<< '\n';
        //}
        // environment.back()[name.name] = values;
        //}
        // std::cerr << "lambda 4\n";

        // return std::visit(interpreter, expression[2]);
        //} break;
        //}
    }
}    // namespace interpreter
}    // namespace elderLISP
