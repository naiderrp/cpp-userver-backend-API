#pragma once

#include <fmt/format.h>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace cinema_service {
class GetSeats final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-get-seats";

  GetSeats(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                 userver::server::request::RequestContext&) const override;
private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};
} // namespace cinema_service