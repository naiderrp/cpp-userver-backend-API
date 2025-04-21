#include <userver/utest/utest.hpp>
#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>

UTEST(JsonParsingTest, ParseMoviePayload) {
  const std::string body = R"json({
    "title": "Tenet",
    "description": "Time is reversed.",
    "duration": 150,
    "genres": ["Action", "Sci-Fi"]
  })json";

  const auto json = userver::formats::json::FromString(body);

  EXPECT_EQ(json["title"].As<std::string>(), "Tenet");
  EXPECT_EQ(json["duration"].As<int>(), 150);
  EXPECT_EQ(json["genres"][1].As<std::string>(), "Sci-Fi");
}

UTEST(JsonParsingTest, HandleMissingOptionalField) {
  const std::string body = R"json({
    "title": "Dune",
    "duration": 155
  })json";

  const auto json = userver::formats::json::FromString(body);

  EXPECT_EQ(json["title"].As<std::string>(), "Dune");
  EXPECT_FALSE(json.HasMember("description"));
}
