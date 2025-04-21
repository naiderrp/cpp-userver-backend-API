#include "cinema_service.hpp"

#include "movies.hpp"
#include "genres.hpp"
#include "halls.hpp"
#include "seats.hpp"
#include "showtimes.hpp"
#include "tickets.hpp"

namespace cinema_service {
void AppendCinema(userver::components::ComponentList& component_list) {
  component_list.Append<GetMovies>();
  component_list.Append<PostMovies>();
  component_list.Append<GetGenres>();
  component_list.Append<PostGenres>();
  component_list.Append<GetHalls>();
  component_list.Append<PostHalls>();
  component_list.Append<GetSeats>();
  component_list.Append<PostShowtimes>();
  component_list.Append<GetTickets>();
  component_list.Append<PostTickets>();
  component_list.Append<userver::components::Postgres>("postgres-db-cinema");
  component_list.Append<userver::clients::dns::Component>();
}

} // namespace cinema_service