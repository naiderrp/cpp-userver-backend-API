#include "halls.hpp"

#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/utils/datetime.hpp>

namespace cinema_service {
GetHalls::GetHalls(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-cinema").GetCluster()) {}

std::string GetHalls::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                userver::server::request::RequestContext&) const {
  using userver::formats::json::ValueBuilder;

  ValueBuilder result_json;

  auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "SELECT id, name, capacity FROM cinema_schema.halls"
  );

  for (const auto& row : result) {
    ValueBuilder hall;
    
    hall["id"] = row["id"].As<int>();
    hall["name"] = row["name"].As<std::string>();
    hall["capacity"] = row["capacity"].As<int>();

    result_json.PushBack(std::move(hall));
  }

  return userver::formats::json::ToString(result_json.ExtractValue());
}

PostHalls::PostHalls(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context)
  : HttpHandlerBase(config, context),
    pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-cinema").GetCluster()) {}

std::string PostHalls::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                              userver::server::request::RequestContext&) const {
  const auto& json = userver::formats::json::FromString(request.RequestBody());

  const std::string name = json["name"].As<std::string>();
  const int capacity = json["capacity"].As<int>();

  auto result = pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster, 
    "INSERT INTO cinema_schema.halls (name, capacity) VALUES ($1, $2) RETURNING id", 
    name, capacity);

  int hall_id = result[0]["id"].As<int>();

  for (int i = 1; i <= capacity; ++i) {
    pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO cinema_schema.seats (hall_id, local_id) "
      "VALUES ($1, $2)", 
      hall_id, i);
  }

  return R"({"status":"ok"})";
}
} // namespace cinema_service