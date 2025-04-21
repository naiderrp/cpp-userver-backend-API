#include "tickets.hpp"

#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/utils/datetime.hpp>

namespace cinema_service {
GetTickets::GetTickets(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-cinema").GetCluster()) {}

std::string GetTickets::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                userver::server::request::RequestContext&) const {
  using userver::formats::json::ValueBuilder;

  ValueBuilder result_json;

  auto result = pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster,
    R"sql(
      SELECT 
        t.id AS ticket_id,
        m.title AS movie_title,
        h.name AS hall_name,
        s.local_id AS seat_local_id
      FROM cinema_schema.tickets t
      JOIN cinema_schema.showtimes st ON t.showtime_id = st.id
      JOIN cinema_schema.movies m ON st.movie_id = m.id
      JOIN cinema_schema.halls h ON st.hall_id = h.id
      JOIN cinema_schema.seats s ON t.seat_id = s.id
    )sql");

  for (const auto& row : result) {
    ValueBuilder ticket;
    
    ticket["ticket_id"] = row["ticket_id"].As<int>();
    ticket["movie_title"] = row["movie_title"].As<std::string>();
    ticket["hall_name"] = row["hall_name"].As<std::string>();
    ticket["seat_local_id"] = row["seat_local_id"].As<int>();

    result_json.PushBack(std::move(ticket));
  }

  return userver::formats::json::ToString(result_json.ExtractValue());
}

PostTickets::PostTickets(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context)
  : HttpHandlerBase(config, context),
    pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-cinema").GetCluster()) {}

std::string PostTickets::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                              userver::server::request::RequestContext&) const {
  const auto& json = userver::formats::json::FromString(request.RequestBody());

  const int showtime_id = json["showtime_id"].As<int>();
  const int seat_id = json["seat_id"].As<int>();

  pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster, 
    "INSERT INTO cinema_schema.tickets (showtime_id, seat_id) VALUES ($1, $2)", 
    showtime_id, seat_id);

  pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster, 
    "UPDATE cinema_schema.seats SET is_taken=True WHERE id=$1", 
    seat_id);  

  return R"({"status":"ok"})";
}
} // namespace cinema_service