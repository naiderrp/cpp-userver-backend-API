#include "movies.hpp"

#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/utils/datetime.hpp>

namespace cinema_service {
GetMovies::GetMovies(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-cinema").GetCluster()) {}

std::string GetMovies::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                          userver::server::request::RequestContext&) const {
  using userver::formats::json::ValueBuilder;

  ValueBuilder result_json;

  auto result = pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster,
    R"sql(
      SELECT 
        m.id AS movie_id,
        m.title,
        m.description,
        m.duration,
        g.name AS genre_name
      FROM cinema_schema.movies m
      LEFT JOIN cinema_schema.movie_genres mg ON m.id = mg.movie_id
      LEFT JOIN cinema_schema.genres g ON mg.genre_id = g.id
      ORDER BY m.id
    )sql"
  );

  std::unordered_map<int, ValueBuilder> movies_by_id;

  for (const auto& row : result) {
    int movie_id = row["movie_id"].As<int>();

    if (movies_by_id.find(movie_id) == movies_by_id.end()) {
      ValueBuilder movie;
      movie["id"] = movie_id;
      movie["title"] = row["title"].As<std::string>();
      
      if (!row["description"].IsNull()) {
        movie["description"] = row["description"].As<std::string>();
      } else {
        movie["description"] = nullptr;
      }
      
      movie["duration"] = row["duration"].As<int>();
      movie["genres"] = userver::formats::json::ValueBuilder();
      movies_by_id[movie_id] = std::move(movie);
    }

    if (!row["genre_name"].IsNull()) {
      movies_by_id[movie_id]["genres"].PushBack(row["genre_name"].As<std::string>());
    }
  }

 for (auto& [_, movie] : movies_by_id) {
    result_json.PushBack(movie.ExtractValue());
  }

  return userver::formats::json::ToString(result_json.ExtractValue());
}

PostMovies::PostMovies(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context)
  : HttpHandlerBase(config, context),
    pg_cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-cinema").GetCluster()) {}

std::string PostMovies::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                              userver::server::request::RequestContext&) const {
  const auto& json = userver::formats::json::FromString(request.RequestBody());

  const std::string title = json["title"].As<std::string>();
  const std::optional<std::string> description = 
    json.HasMember("description") ? std::make_optional<std::string>(json["description"].As<std::string>()) : std::nullopt; 
  const int duration = json["duration"].As<int>();

  auto movie_result = pg_cluster_->Execute(
    userver::storages::postgres::ClusterHostType::kMaster,
    "INSERT INTO cinema_schema.movies (title, description, duration) VALUES ($1, $2, $3) RETURNING id", 
    title, description, duration
  );

  int movie_id = movie_result.Front()["id"].As<int>();

  if (json.HasMember("genres")) {
    const auto& genres = json["genres"];

    for (const auto& genre_node : genres) {
      const std::string genre_name = genre_node.As<std::string>();

      auto genre_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO cinema_schema.genres (name) VALUES ($1) ON CONFLICT (name) DO UPDATE SET name = EXCLUDED.name RETURNING id",
        genre_name
      );

      int genre_id = genre_result.Front()["id"].As<int>();

      pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO cinema_schema.movie_genres (movie_id, genre_id) VALUES ($1, $2) ON CONFLICT DO NOTHING",
        movie_id, genre_id
      );
    }
  }

  return R"({"status":"ok"})";
}
} // namespace cinema_service