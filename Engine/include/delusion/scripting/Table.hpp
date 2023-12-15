#pragma once

#include <format>
#include <optional>
#include <stdexcept>

#include <lua.h>

#include "delusion/Exception.hpp"

class Table {
    private:
        lua_State *state {};
        int reference {};
    public:
        Table(lua_State *state, int reference) : state(state), reference(reference) {}

        Table(const Table& other) = delete;

        Table(Table&& other) noexcept {
            state = std::exchange(other.state, nullptr);
            reference = std::exchange(other.reference, 0);
        }

        ~Table() {
            if (state != nullptr) {
                lua_unref(state, reference);
            }
        }

        Table& operator=(const Table& other) = delete;

        Table& operator=(Table&& other) noexcept = delete;

        std::optional<Table> get(const std::string &name) {
            lua_rawgeti(state, LUA_REGISTRYINDEX, reference);
            lua_pushstring(state, name.c_str());
            lua_gettable(state, -2);

            if (lua_isnil(state, -1) != 0) {
                lua_pop(state, 1);

                return {};
            } else if (lua_istable(state, -1) != 0) {
                Table table = Table(state, lua_ref(state, -1));

                lua_pop(state, 1);

                return std::make_optional<Table>(std::move(table));
            } else {
                throw Exception(std::format("Failed to get table: {}", lua_tostring(state, -1)));
            }

            //Table table = Table(state, lua_ref(state, -1));

            //lua_pop(state, 1);

            //return std::make_optional<Table>(std::move(table));

            /*if (lua_isnil(state, -1) != 0) {
                return {};
            } else if (lua_istable(state, -1) != 0) {
                Table table = Table(state, lua_ref(state, -1));

                lua_pop(state, 1);

                return table;
            } else {
                throw Exception(std::format("Failed to get table: {}", lua_tostring(state, -1)));
            }*/
        }

        void set(const std::string &name, const std::string &value) {
            lua_rawgeti(state, LUA_REGISTRYINDEX, reference);
            lua_pushstring(state, name.c_str());
            lua_pushstring(state, value.c_str());
            lua_settable(state, -3);
        }

        void call(const std::string &name) {
            lua_rawgeti(state, LUA_REGISTRYINDEX, reference);
            lua_pushstring(state, name.c_str());
            lua_gettable(state, -2);

            if (lua_pcall(state, 0, 0, 0) != 0) {
                throw Exception(std::format("Failed to call function: {}", lua_tostring(state, -1)));
            }

            lua_pop(state, 1);
        }
};
