#include "list.hpp"

auto
main() -> int
{
    using namespace elder;
    auto defs = Definitions{};

    auto r = evaluate(defs, List{{Atomic{Builtin::LIST}, Atomic{Data{}}}});
    auto r2 = evaluate(defs, List{{Atomic{Builtin::REST}, r}});

    return std::get<Atomic>(r2).index();
}
