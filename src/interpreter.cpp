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

    auto
    interpret(ast::List const& expression, Environment& environment)
            -> ast::Node
    {
        auto const interpreter = [&](auto&& node) -> ast::Node {
            return interpret(node, environment);
        };

        auto constexpr getAtom = [](auto&& node) -> ast::Atom {
            return std::get<nodeIndex<ast::Atom>>(node);
        };

        auto constexpr getName = [getAtom](auto&& node) -> ast::Name {
            return std::get<atomIndex<ast::Name>>(getAtom(node));
        };

        auto constexpr getList = [](auto&& node) -> ast::List {
            return std::get<nodeIndex<ast::List>>(node);
        };

        auto interpExpression   = ast::List{};
        auto const insertResult = [&](auto&& node) -> void {
            auto const interpNode = std::visit(interpreter, node);

            if(std::holds_alternative<ast::Atom>(node)
               && std::holds_alternative<ast::Name>(getAtom(node))) {
                switch(interpNode.index()) {
                case nodeIndex<ast::List>: {
                    auto&& list = getList(interpNode);
                    interpExpression.insert(
                            interpExpression.cend(),
                            list.begin(),
                            list.end());
                } break;
                case nodeIndex<ast::Atom>: {
                    interpExpression.push_back(interpNode);
                } break;
                }
            }
            else {
                interpExpression.push_back(interpNode);
            }
        };

        insertResult(expression[0]);
        auto const action = getAtom(interpExpression[0]);

        switch(action.index()) {
        case atomIndex<ast::First>: {
            insertResult(expression[1]);
            auto node = interpExpression[1];

            switch(node.index()) {
            case nodeIndex<ast::List>: {
                return getList(node)[0];
            } break;
            case nodeIndex<ast::Atom>: {
                return node;
            } break;
            }
        } break;
        case atomIndex<ast::Rest>: {
            insertResult(expression[1]);
            auto node = interpExpression[1];

            switch(node.index()) {
            case nodeIndex<ast::List>: {
                auto list = getList(node);
                list.pop_front();

                return list;
            } break;
            case nodeIndex<ast::Atom>: {
                return ast::List{};
            } break;
            }
        } break;
        case atomIndex<ast::Combine>: {
            auto concatList = ast::List{};

            for(auto i = 1ul; i < expression.size(); i++) {
                insertResult(expression[i]);
            }
            for(auto i = 1ul; i < interpExpression.size(); i++) {
                auto node = interpExpression[i];

                switch(node.index()) {
                case nodeIndex<ast::List>: {
                    auto list = getList(node);
                    concatList.insert(
                            concatList.cend(),
                            list.begin(),
                            list.end());
                } break;
                case nodeIndex<ast::Atom>: {
                    concatList.push_back(getAtom(node));
                } break;
                }
            }

            return concatList;
        } break;
        case atomIndex<ast::Quote>: {
            return expression[1];
        } break;
        case atomIndex<ast::Let>: {
            auto name = getName(expression[1]);

            insertResult(expression[2]);
            environment.front()[name.name] = interpExpression[1];

            return environment.front()[name.name];
        } break;
        case atomIndex<ast::Lambda>: {
            auto names  = getList(expression[1]);
            auto values = getList(expression[3]);

            for(auto i = 0ul; i < names.size(); i++) {
                auto quoteStub = ast::List{};
                quoteStub.push_back

                auto stub = ast::List{
                        {ast::Atom{ast::Let{}}},
                        {names[i]},
                        {ast::List{
                                ast::Node{ast::Atom{ast::Quote{}}},
                                ast::Node{values[i]}}}};
                interpret(, environment);
            }
        }
        }
        break;
    }

    return expression;
}
}    // namespace interpreter
}    // namespace elderLISP
