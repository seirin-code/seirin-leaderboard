#include "leaderboard.hpp"

#include <filesystem>
#include <format>
#include <fstream>

#include <iostream>

#include <nlohmann/json.hpp>

#include "detail.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

seirin::Leaderboard::Leaderboard(const std::string& playersPath)
{
  leaderboard_ = std::vector<leaderboard_position_t>{};

  if (!fs::exists(playersPath) || fs::is_empty(playersPath))
  {
    throw std::invalid_argument("No players file or empty\n");
  }

  std::ifstream in(playersPath);
  std::string player{};

  while (std::getline(in, player))
  {
    std::string id = player.substr(0, player.find(","));
    std::string name = player.substr(player.find(",")).substr(1);

    apiToken();
    std::string resp = detail::fetchApiData(std::format("https://api.stratz.com/api/v1/Player/{}", id), apiToken_);
    int medal = parseMedal(resp);
    int rank = parseRank(resp);

    leaderboard_position_t pos
    {
      {id, name}, //player
      medal / 10, //medal
      medal % 10, //stars
      rank,       //rank
    };
    
    leaderboard_.push_back(pos);
  }

  sort();
}

int seirin::Leaderboard::parseMedal(const std::string& response) const
{
  try
  {
    auto jsonResponse = json::parse(response);
    //auto result = jsonResponse["rank_tier"].get<int>();
    auto result = jsonResponse["steamAccount"]["seasonRank"].get<int>();

    return result;
  }
  catch (json::parse_error& e)
  {
    throw std::runtime_error("Error parsing JSON response: " + std::string(e.what()));
  }
  catch (json::type_error& e)
  {
    throw std::runtime_error("Error accessing JSON fields: " + std::string(e.what()));
  }
  catch (std::exception& e) 
  {
    throw std::runtime_error("An error occurred: " + std::string(e.what()));
  }
}

int seirin::Leaderboard::parseRank(const std::string& response) const
{
  try
  {
    auto jsonResponse = json::parse(response);
    if (jsonResponse["steamAccount"]["seasonLeaderboardRank"].is_null())
    {
      return 0;
    } 
    else
    {
      return jsonResponse["steamAccount"]["seasonLeaderboardRank"].get<int>();
    }
  }
  catch (json::parse_error& e)
  {
    throw std::runtime_error("Error parsing JSON response: " + std::string(e.what()));
  }
  catch (json::type_error& e)
  {
    throw std::runtime_error("Error accessing JSON fields: " + std::string(e.what()));
  }
  catch (std::exception& e)
  {
    throw std::runtime_error("An error occurred: " + std::string(e.what()));
  }
}

void seirin::Leaderboard::sort()
{
  std::sort(leaderboard_.begin(), leaderboard_.end(), [](const auto& lhs, const auto& rhs){
    if (lhs.rank != 0 && rhs.rank == 0)
    {
      return true;
    }
    else if (lhs.rank == 0 && rhs.rank != 0)
    {
      return false;
    }
    else if (lhs.rank != 0 && rhs.rank != 0)
    {
      return lhs.rank < lhs.rank;
    }
    else
    {
      return lhs.medal == rhs.medal ? lhs.stars > rhs.stars : lhs.medal > rhs.medal;
    }
  });
}

void seirin::Leaderboard::update()
{
  for (auto it = leaderboard_.begin(); it != leaderboard_.end(); it++)
  {
    std::string id = it->player.id;

    std::string resp = detail::fetchApiData(std::format("https://api.stratz.com/api/v1/Player/{}", id), apiToken_);
    int medal = parseMedal(resp);
    int rank = parseRank(resp);

    it->medal = medal / 10;
    it->stars = medal % 10;
    it->rank = rank;
  }

  sort();
}

void seirin::Leaderboard::add(const std::string& id)
{

}

std::vector<std::string> seirin::Leaderboard::format()
{
  std::vector<std::string> formatted{};
  auto maxName = std::max_element(leaderboard_.begin(), leaderboard_.end(), [](const auto& lhs, const auto& rhs){
    return lhs.player.name.length() < rhs.player.name.length();
  })->player.name;
  int maxSize = maxName.length();

  for (auto i = leaderboard_.begin(); i != leaderboard_.end(); i++)
  {
    while (i->player.name.length() <= maxSize)
    {
      i->player.name.append(" ");
    }
  }

  std::size_t counter = 0;
  for (const auto& pos: leaderboard_)
  {
    std::string medal = RANKS.at(pos.medal);
    std::string emoji{};
    if (leaderboard_.begin()->player.id == pos.player.id && pos.rank != 0)
    {
      emoji = EMOJIS.at(9);
    }
    else
    {
      emoji = EMOJIS.at(pos.medal);
    }
    std::string rank = pos.rank == 0 ? "" : std::format(" ({})", pos.rank);
    std::string stars = pos.stars == 0 ? "" : std::to_string(pos.stars);
    ++counter;
    std::string space = counter < 10 ? " " : "";

    std::string frmt_plr = std::format("{} `{}. {}{} {}{} {}`", emoji, counter, space, pos.player.name, medal, rank, stars); 
    formatted.push_back(frmt_plr);
  }

  return formatted;
}

void seirin::Leaderboard::apiToken()
{
    auto token_file = fs::current_path().string().append("/tokenapi.data");
    if (!fs::exists(token_file) || fs::is_empty(token_file))
    {
      throw std::invalid_argument("No token file or empty\n");
    } else {
      std::ifstream tkn(token_file);
      tkn >> apiToken_;
    }
}
