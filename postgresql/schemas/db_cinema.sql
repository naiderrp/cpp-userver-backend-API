DROP SCHEMA IF EXISTS cinema_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS cinema_schema;

CREATE TABLE IF NOT EXISTS cinema_schema.movies (
    id SERIAL PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    description TEXT,
    duration INTEGER
);

CREATE TABLE IF NOT EXISTS cinema_schema.genres (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS cinema_schema.movie_genres (
    movie_id INT REFERENCES cinema_schema.movies(id) ON DELETE CASCADE,
    genre_id INT REFERENCES cinema_schema.genres(id) ON DELETE CASCADE,
    PRIMARY KEY (movie_id, genre_id)
);

CREATE TABLE IF NOT EXISTS cinema_schema.halls (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    capacity INT NOT NULL
);

CREATE TABLE IF NOT EXISTS cinema_schema.seats (
    id SERIAL PRIMARY KEY,
    hall_id INT REFERENCES cinema_schema.halls(id) ON DELETE CASCADE,
    local_id INT NOT NULL,
    is_taken BOOLEAN DEFAULT FALSE
);

CREATE TABLE IF NOT EXISTS cinema_schema.showtimes (
    id SERIAL PRIMARY KEY,
    movie_id INT REFERENCES cinema_schema.movies(id),
    hall_id INT REFERENCES cinema_schema.halls(id)
);

CREATE TABLE IF NOT EXISTS cinema_schema.tickets (
    id SERIAL PRIMARY KEY,
    showtime_id INT REFERENCES cinema_schema.showtimes(id) ON DELETE CASCADE,
    seat_id INT REFERENCES cinema_schema.seats(id)
);
