#include "seats.hpp"

#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/utils/datetime.hpp>

namespace cinema_service {
GetSeats::GetSeats(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context)
  : HttpHandlerBase(config, context),
    pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-cinema").GetCluster()) {}

std::string GetSeats::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                         userver::server::request::RequestContext&) const {
  const std::string hall_name = request.GetArg("hall_name");

  auto result = pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster,
    "SELECT id FROM cinema_schema.halls WHERE name = $1",
    hall_name);

  int hall_id = result[0]["id"].As<int>();

  auto seats = pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster,
    "SELECT id, local_id, is_taken FROM cinema_schema.seats WHERE hall_id = $1",
    hall_id);

  using userver::formats::json::ValueBuilder;
  ValueBuilder result_json;

  for (const auto& row : seats) {
    ValueBuilder seat;

    seat["id"] = row["id"].As<int>();
    seat["local_id"] = row["local_id"].As<int>();
    seat["is_taken"] = row["is_taken"].As<bool>();

    result_json.PushBack(std::move(seat));
  }

  return userver::formats::json::ToString(result_json.ExtractValue());
}
} // namespace cinema_service 