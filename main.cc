#include <drogon/drogon.h>
#include "controllers/TaskController.h"

using namespace drogon;

int main() {
    app().loadConfigFile("config.json");

    // Создаём таблицу, если её нет
    app().getLoop()->queueInLoop([]() {
        auto db = app().getDbClient();
        if (db) {
            db->execSqlSync(
                "CREATE TABLE IF NOT EXISTS tasks ("
                "id SERIAL PRIMARY KEY, "
                "title TEXT NOT NULL, "
                "done BOOLEAN DEFAULT FALSE)");
            LOG_INFO << "Table 'tasks' is ready.";
        }
    });

    app().run();
    return 0;
}
