#pragma once
#include <drogon/orm/Row.h>
#include <string>

namespace drogon_model {
namespace todo {

struct Task {
    int id{0};
    std::string title;
    bool done{false};

    // Преобразование строки БД в объект
    static Task fromRow(const drogon::orm::Row &row) {
        Task t;
        t.id = row["id"].as<int>();
        t.title = row["title"].as<std::string>();
        t.done = row["done"].as<bool>();
        return t;
    }
};

} // namespace todo
} // namespace drogon_model
