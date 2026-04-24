#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include "../models/Task.h"

using namespace drogon;
using namespace drogon::orm;

class TaskController : public HttpController<TaskController> {
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TaskController::health, "/health", Get);
    ADD_METHOD_TO(TaskController::getAll, "/api/tasks", Get);
    ADD_METHOD_TO(TaskController::create, "/api/tasks", Post);
    METHOD_LIST_END

    void health(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) {
        Json::Value ret;
        ret["status"] = "ok";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }

    void getAll(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) {
        auto db = app().getDbClient();
        // Используем прямой SQL через DbClient, чтобы не зависеть от Mapper
        db->execSqlAsync(
            "SELECT id, title, done FROM tasks ORDER BY id",
            [callback](const Result &result) {
                Json::Value arr(Json::arrayValue);
                for (const auto &row : result) {
                    auto t = drogon_model::todo::Task::fromRow(row);
                    Json::Value item;
                    item["id"] = t.id;
                    item["title"] = t.title;
                    item["done"] = t.done;
                    arr.append(item);
                }
                auto resp = HttpResponse::newHttpJsonResponse(arr);
                callback(resp);
            },
            [callback](const DrogonDbException &e) {
                auto resp = HttpResponse::newHttpResponse();
                resp->setBody(e.base().what());
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            });
    }

    void create(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) {
        auto json = req->getJsonObject();
        if (!json || !(*json)["title"]) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
        drogon_model::todo::Task task;
        task.title = (*json)["title"].asString();
        task.done = false;

        auto db = app().getDbClient();
        // Вставляем через прямую SQL-команду с параметрами
        db->execSqlAsync(
            "INSERT INTO tasks (title, done) VALUES ($1, $2) RETURNING id, title, done",
            [callback](const Result &result) {
                auto row = result.front();
                auto t = drogon_model::todo::Task::fromRow(row);
                Json::Value item;
                item["id"] = t.id;
                item["title"] = t.title;
                item["done"] = t.done;
                auto resp = HttpResponse::newHttpJsonResponse(item);
                resp->setStatusCode(k201Created);
                callback(resp);
            },
            [callback](const DrogonDbException &e) {
                auto resp = HttpResponse::newHttpResponse();
                resp->setBody(e.base().what());
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            },
            task.title, task.done);
    }
};
