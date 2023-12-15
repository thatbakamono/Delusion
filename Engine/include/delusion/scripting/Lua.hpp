#pragma once

#include <stdexcept>

#include "delusion/Exception.hpp"
#include "delusion/io/FileUtilities.hpp"
#include "delusion/scripting/Table.hpp"

#include <lua.h>
#include <luacode.h>
#include <lualib.h>

#include <Luau/BytecodeBuilder.h>
#include <Luau/Compiler.h>
#include <Luau/Common.h>

class Lua {
    private:
        lua_State *state {};
    public:
        Lua() {
            state = luaL_newstate();

            luaL_openlibs(state);
        }

        ~Lua() {
            lua_close(state);
        }

        std::optional<Table> load(const std::string &name, const std::string &source);

        std::optional<Table> load(const std::string &path);

        Table globals() {
            lua_checkstack(state, 1);

            lua_pushvalue(state, LUA_GLOBALSINDEX);

            Table table = Table(state, lua_ref(state, -1));

            lua_pop(state, 1);

            return table;
        }

        std::optional<Table> get(const std::string &name) {
            lua_checkstack(state, 1);

            lua_getglobal(state, name.c_str());

            if (lua_isnil(state, -1) != 0) {
                lua_pop(state, 1);

                return {};
            } else if (lua_istable(state, -1) != 0) {
                Table table = Table(state, lua_ref(state, -1));

                lua_pop(state, 1);

                return table;
            } else {
                throw std::exception();
            }
        }
};
