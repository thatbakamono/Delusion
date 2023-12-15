#pragma once

#include "delusion/UniqueId.hpp"
#include "delusion/scripting/Table.hpp"

class Script {
    private:
        UniqueId m_id;

        std::string m_name;
        std::string m_source;

        std::unique_ptr<Table> m_table;
    public:
        Script(UniqueId id, std::string name, std::string source) : m_id(id), m_name(std::move(name)), m_source(std::move(source)) {}

        Script(const Script &other) = delete;
        Script(Script &&other) noexcept = delete;

        Script &operator=(const Script &other) = delete;
        Script &operator=(Script &&other) noexcept = delete;

        [[nodiscard]] UniqueId id() const {
            return m_id;
        }

        [[nodiscard]] const std::string &name() const {
            return m_name;
        }

        [[nodiscard]] const std::string &source() const {
            return m_source;
        }

        void setTable(std::unique_ptr<Table> table) {
            m_table = std::move(table);
        }

        void resetTable() {
            m_table.reset();
        }

        [[nodiscard]] Table *table() const {
            return m_table.get();
        }
};
