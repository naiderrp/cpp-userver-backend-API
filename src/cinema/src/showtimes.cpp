#include "showtimes.hpp"
#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/utils/datetime.hpp>

namespace cinema_service {
PostShowtimes::PostShowtimes(const userver::components::ComponentConfig& config,
                             const userver::components::ComponentContext& context)
  : HttpHandlerBase(config, context),
    pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-cinema").GetCluster()) {}

std::string PostShowtimes::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                              userver::server::request::RequestContext&) const {
  const auto& json = userver::formats::json::FromString(request.RequestBody());

  const std::string movie_title = json["movie_title"].As<std::string>();
  const std::string hall_name = json["hall_name"].As<std::string>();

  auto movie_result = pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster,
    "SELECT id FROM cinema_schema.movies WHERE title = $1", movie_title);

  int movie_id = movie_result[0]["id"].As<int>();

  auto hall_result = pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster,
    "SELECT id FROM cinema_schema.halls WHERE name = $1",
    hall_name);

  int hall_id = hall_result[0]["id"].As<int>();

  pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster,
    "INSERT INTO cinema_schema.showtimes (movie_id, hall_id) VALUES ($1, $2)",
    movie_id, hall_id);

  return R"({"status":"ok"})";
}
} // namespace cinema_service
