# C++ Backend API Using Userver

This is a backend service for a simple cinema simulation, developed for educational purposes. Originally implemented with **Django**, the project was fully rewritten in C++ to explore high-performance backend development and gain experience with asynchronous I/O and microservices.

## Database Schema Overview

All tables are defined under the `cinema_schema` namespace.

### `movies`
Represents the list of movies available in the cinema, including basic metadata like title, description, and duration.

### `genres`
Stores predefined movie genres such as Action, Comedy, Sci-Fi, etc. Used for categorizing movies.

### `movie_genres`
A linking table that enables a many-to-many relationship between movies and genres.

### `halls`
Represents physical cinema halls where movies are screened. Each hall has a unique name and a seating capacity.

### `seats`
Defines individual seats for each hall. Seats are identified by a `local_id` and may be marked as taken or free.

### `showtimes`
Represents scheduled movie screenings. Each showtime links a movie to a specific hall.

### `tickets`
Represents purchased tickets, linking a showtime to a specific seat that is then marked as taken.

## Requirements

The project is intended to be run in a containerized development environment using **DevContainer**.

### Tools Needed

- [Docker](https://www.docker.com/)
- [VS Code](https://code.visualstudio.com/)
- [DevContainers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

## Steps to Run the Project

1. **Clone the Repository**

   ```bash
   git clone git@github.com:naiderrp/cpp-userver-backend-API.git
   cd cpp-userver-backend-API
   ```

2. **Open in VS Code with DevContainer**

   > Open the folder in VS Code and reopen it in the DevContainer when prompted.

3. **Build and Run**

   Inside the container terminal:

   ```bash
   make build-release && \
   make start-release
   ```

## API Overview

Available endpoints:

- `GET /movies` — List all movies
- `POST /movies` — Add a new movie
- `GET /genres` — List all genres
- `GET /halls` — List all halls
- `POST /hall` — Add a new hall
- `GET /seats` — List all seats
- `POST /showtimes` — Create a new showtime
- `GET /tickets` — View all tickets
- `POST /tickets` — Add a ticket

# Examples of Using `CinemaService`

This section contains usage examples to demonstrate how the backend API can interact with HTTP requests.

### Add a New Movie

To register a new movie with genres:

**HTTP Request**

```bash
curl -X POST http://localhost:8080/movies \
  -H "Content-Type: application/json" \
  -d '{
    "title": "Tenet",
    "description": "Time runs backwards?",
    "duration": 150,
    "genres": ["Action", "Sci-Fi"]
}'
```

**Expected Response**

```json
{
  "status": "ok"
}
```

### View All Tickets

To get a list of all issued tickets, along with movie and seat info:

**HTTP Request**

```bash
curl http://localhost:8080/tickets
```

**Expected Response**

```json
[
  {
    "ticket_id": 1,
    "movie_title": "Inception",
    "hall_name": "Red Hall",
    "seat_local_id": 1
  },
  {
    "ticket_id": 2,
    "movie_title": "Inception",
    "hall_name": "Red Hall",
    "seat_local_id": 2
  }
]
```

## Testing

Unit tests are implemented using `userver/utest`, and integration tests are written with `pytest` using a Postgres test database. Sample data is loaded via `initial_cinema_data.sql`.