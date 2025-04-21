import pytest

from testsuite.databases import pgsql


async def test_db_updates(service_client):
    response = await service_client.get('/genres')
    assert response.status == 200
    genres = response.json()
    assert isinstance(genres, list)


@pytest.mark.pgsql('db_cinema', files=['initial_cinema_data.sql'])
async def test_movies_with_genres(service_client):
    response = await service_client.get('/movies')
    assert response.status == 200
    movies = response.json()
    assert isinstance(movies, list)
    assert len(movies) >= 3
    assert any("genres" in movie for movie in movies)


@pytest.mark.pgsql('db_cinema', files=['initial_cinema_data.sql'])
async def test_get_genres(service_client):
    response = await service_client.get('/genres')
    assert response.status == 200
    genres = response.json()
    assert any(g["name"] == "Sci-Fi" for g in genres)


@pytest.mark.pgsql('db_cinema', files=['initial_cinema_data.sql'])
async def test_get_tickets(service_client):
    response = await service_client.get('/tickets')
    assert response.status == 200
    tickets = response.json()
    assert isinstance(tickets, list)
    assert len(tickets) >= 5
    assert all("movie_title" in ticket for ticket in tickets)


@pytest.mark.pgsql('db_cinema', files=['initial_cinema_data.sql'])
async def test_post_ticket(service_client):
    response = await service_client.post(
        '/tickets',
        json={"showtime_id": 1, "seat_id": 3}
    )
    assert response.status == 200
    assert response.json() == {"status": "ok"}

    response = await service_client.get('/tickets')
    tickets = response.json()
    assert any(t["seat_local_id"] == 3 for t in tickets)


@pytest.mark.pgsql('db_cinema', files=['initial_cinema_data.sql'])
async def test_post_movie(service_client):
    new_movie = {
        "title": "Tenet",
        "description": "Time inversion.",
        "duration": 150,
        "genres": ["Action", "Sci-Fi"]
    }
    response = await service_client.post('/movies', json=new_movie)
    assert response.status == 200
    assert response.json() == {"status": "ok"}

    response = await service_client.get('/movies')
    titles = [movie["title"] for movie in response.json()]
    assert "Tenet" in titles
