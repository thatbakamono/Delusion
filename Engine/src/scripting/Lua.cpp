#include "delusion/scripting/Lua.hpp"

std::optional<Table> Lua::load(const std::string &name, const std::string &source) {
    Luau::BytecodeBuilder bytecodeBuilder{};

    Luau::compileOrThrow(bytecodeBuilder, source);

    int result = luau_load(state, name.c_str(), bytecodeBuilder.getBytecode().c_str(),
                           bytecodeBuilder.getBytecode().size(), 0);

    if (result != 0) {
        throw std::exception("Failed to load bytecode");
    }

    if (lua_pcall(state, 0, 1, 0) != 0) {
        throw Exception(std::format("Failed to execute bytecode: {}", lua_tostring(state, -1)));
    }

    if (lua_isnil(state, -1) != 0) {
        return {};
    } else if (lua_istable(state, -1) != 0) {
        Table table = Table(state, lua_ref(state, -1));

        lua_pop(state, 1);

        return table;
    } else {
        throw std::exception("Bytecode did not return a table");
    }
}

std::optional<Table> Lua::load(const std::string &path) {
    std::string source = readAsString(path.c_str()).value();

    Luau::BytecodeBuilder bytecodeBuilder{};

    Luau::compileOrThrow(bytecodeBuilder, source);

    int result = luau_load(state, path.c_str(), bytecodeBuilder.getBytecode().c_str(),
                           bytecodeBuilder.getBytecode().size(), 0);

    if (result != 0) {
        throw std::exception("Failed to load bytecode");
    }

    if (lua_pcall(state, 0, 1, 0) != 0) {
        throw Exception(std::format("Failed to execute bytecode: {}", lua_tostring(state, -1)));
    }

    if (lua_isnil(state, -1) != 0) {
        return {};
    } else if (lua_istable(state, -1) != 0) {
        Table table = Table(state, lua_ref(state, -1));

        lua_pop(state, 1);

        return table;
    } else {
        throw std::exception("Bytecode did not return a table");
    }
}
