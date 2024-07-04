#ifndef SEIRIN_LEADERBOARD_HPP
#define SEIRIN_LEADERBOARD_HPP

#include <map>
#include <string>
#include <vector>

namespace seirin
{
  struct player_t
  {
    std::string id;
    std::string name;
  };

  class Leaderboard
  {
  public:
    struct leaderboard_position_t
    {
      player_t player;
      int medal;
      int stars;
      int rank;
    };

    const std::map<int, std::string> RANKS =
    {
      {1, "Herald"},
      {2, "Guardian"},
      {3, "Crusader"},
      {4, "Archon"},
      {5, "Legend"},
      {6, "Ancient"},
      {7, "Divine"},
      {8, "Immortal"}
    };

    const std::map<int, std::string> EMOJIS = 
    {
      {1, "🐛"},
      {2, "🥚"},
      {3, "🥚"},
      {4, "🐣"},
      {5, "🐣"},
      {6, "🐧"},
      {7, "🦅"},
      {8, "🐉"},
      {9, "🐲"}
    };

    Leaderboard(const std::string& playersPath);

    int parseMedal(const std::string& response) const;
    int parseRank(const std::string& response) const;

    void sort();
    void update();
    void add(const std::string& id);

    std::vector<std::string> format();

  private:
    std::vector<leaderboard_position_t> leaderboard_;
    std::string apiToken_;

    void apiToken();

  };
}

#endif
