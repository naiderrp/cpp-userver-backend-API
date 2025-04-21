#include "genres.hpp"

#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/utils/datetime.hpp>

namespace cinema_service {
GetGenres::GetGenres(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-cinema").GetCluster()) {}

std::string GetGenres::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                userver::server::request::RequestContext&) const {
  using userver::formats::json::ValueBuilder;

  ValueBuilder result_json;

  auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "SELECT id, name FROM cinema_schema.genres"
  );

  for (const auto& row : result) {
    ValueBuilder genre;
    
    genre["id"] = row["id"].As<int>();
    genre["name"] = row["name"].As<std::string>();

    result_json.PushBack(std::move(genre));
  }

  return userver::formats::json::ToString(result_json.ExtractValue());
}

PostGenres::PostGenres(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context)
  : HttpHandlerBase(config, context),
    pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-cinema").GetCluster()) {}

std::string PostGenres::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                              userver::server::request::RequestContext&) const {
  const auto& json = userver::formats::json::FromString(request.RequestBody());

  const std::string name = json["name"].As<std::string>();
  
  pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster, 
    "INSERT INTO cinema_schema.genres (name) VALUES ($1)", name);

  return R"({"status":"ok"})";
}
} // namespace cinema_service