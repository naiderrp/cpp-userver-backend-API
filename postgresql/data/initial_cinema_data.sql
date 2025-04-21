INSERT INTO cinema_schema.genres (name) VALUES 
('Action'), ('Comedy'), ('Drama'), ('Sci-Fi'), ('Horror');

INSERT INTO cinema_schema.movies (title, description, duration) VALUES
('Inception', 'Dreams within dreams.', 148),
('The Matrix', 'Virtual reality and choice.', 136),
('Interstellar', 'Space exploration to save Earth.', 169);

INSERT INTO cinema_schema.movie_genres (movie_id, genre_id) VALUES
(1, 1), (1, 4),
(2, 1), (2, 4),
(3, 4), (3, 3);

INSERT INTO cinema_schema.halls (name, capacity) VALUES
('Red Hall', 100),
('Blue Hall', 80),
('Green Hall', 120);

DO $$
DECLARE
    h RECORD;
    r INT;
    c INT;
BEGIN
  FOR h IN SELECT * FROM cinema_schema.halls LOOP
    FOR r IN 1..(h.capacity / 10) LOOP
      FOR c IN 1..10 LOOP
        INSERT INTO cinema_schema.seats (hall_id, row, column) VALUES (h.id, r, c);
      END LOOP;
    END LOOP;
  END LOOP;
END$$;

INSERT INTO cinema_schema.showtimes (movie_id, hall_id) VALUES
(1, 1),
(2, 2),
(3, 3),
(1, 2),
(2, 1);

INSERT INTO cinema_schema.tickets (showtime_id, seat_id, price) VALUES
(1, 1),
(1, 2),
(2, 21),
(3, 31),
(4, 41);
