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

        // auto const interpreter = [&](auto&& node) -> ast::Node {
        // return interpret(node, environment);
        //};

        auto constexpr isAtomic = [](auto&& node) -> bool {
            if(std::holds_alternative<ast::Atom>(node)) {
                return true;
            }

            return std::get<nodeIndex<ast::List>>(node).size() == 1;
        };

        auto constexpr getList = [](auto&& node) -> ast::List {
            return std::get<nodeIndex<ast::List>>(node);
        };

        auto constexpr getAtom = [getList](auto&& node) -> ast::Atom {
            if(std::holds_alternative<ast::Atom>(node)) {
                return std::get<nodeIndex<ast::Atom>>(node);
            }

            return std::get<nodeIndex<ast::Atom>>(getList(node).front());
        };

        for(auto i = (int)expression.size() - 1; i >= 0; i--) {
            auto node = expression[i];

            if(std::holds_alternative<ast::Atom>(node)) {
                auto atom = getAtom(node);
                if(std::holds_alternative<ast::Name>(atom)) {
                    auto interpNode = interpret(atom, environment);
                    if(std::holds_alternative<ast::List>(interpNode)) {
                        auto list = getList(interpNode);
                        expression.insert(
                                expression.erase(expression.begin() + i),
                                list.begin(),
                                list.end());
                    }
                    else {
                        expression[i] = interpNode;
                    }
                }
            }
        }

        std::cerr << "interp 1\n";
        auto const action = getAtom(expression[0]);

        switch(action.index()) {
        case atomIndex<ast::Atomic>: {
            return isAtomic(expression[1]) ? ast::Atom{ast::True{}}
                                           : ast::Atom{ast::False{}};
        } break;
        case atomIndex<ast::Equal>: {
            if(!isAtomic(expression[1]) || !isAtomic(expression[2])) {
                return ast::False{};
            }

            auto lhs = getAtom(expression[1]);
            auto rhs = getAtom(expression[2]);

            if(lhs.index() != rhs.index()) {
                return ast::False{};
            }

            switch(lhs.index()) {
            case atomIndex<ast::StringLiteral>: {
                auto lhsString = get_atom_t<ast::StringLiteral>(lhs).data;
                auto rhsString = get_atom_t<ast::StringLiteral>(rhs).data;

                return lhsString == rhsString ? ast::Atom{ast::True{}}
                                              : ast::Atom{ast::False{}};
            } break;
            case atomIndex<ast::IntegerLiteral>: {
                auto lhsInteger = get_atom_t<ast::IntegerLiteral>(lhs).data;
                auto rhsInteger = get_atom_t<ast::IntegerLiteral>(rhs).data;

                return lhsInteger == rhsInteger ? ast::Atom{ast::True{}}
                                                : ast::Atom{ast::False{}};
            } break;
            }

            return ast::True{};
        } break;
        }
        return expression;
        // case atomIndex<ast::First>: {
        // auto list = getList(std::visit(interpreter, expression[1]));
        // if(list.empty()) {
        // return ast::List{};
        //}
        // return list[0];
        //} break;
        // case atomIndex<ast::Rest>: {
        // if(expression.size() == 1) {
        // return ast::List{};
        //}
        // if(expression.size() > 2) {
        // auto list = expression;
        // list.erase(list.begin());
        // expression[1] = list;
        //}

        // auto list = getList(std::visit(interpreter, expression[1]));
        // if(list.size() <= 1) {
        // return ast::List{};
        //}

        // list.erase(list.begin());
        // return list;
        //} break;
        // case atomIndex<ast::Combine>: {
        // auto concatList = ast::List{};

        // for(auto i = 1ul; i < expression.size(); i++) {
        // auto list = getList(std::visit(interpreter, expression[i]));
        // concatList.insert(
        // concatList.cend(),
        // list.begin(),
        // list.end());
        //}

        // return concatList;
        //} break;
        // case atomIndex<ast::Quote>: {
        // if(expression.size() == 1) {
        // return ast::List{};
        //}
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
