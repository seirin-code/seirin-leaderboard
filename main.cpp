#include <algorithm>
#include <chrono>
#include <filesystem>
#include <format>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include <tgbot/tgbot.h>

#include "leaderboard.hpp"

namespace fs = std::filesystem;

int main()
{
  auto token_file = fs::current_path().string().append("/token.data");
  std::string token{};
  if (!fs::exists(token_file) || fs::is_empty(token_file))
  {
    std::cerr << "No token file or empty\n";
    return 1;
  } else {
    std::ifstream tkn(token_file);
    tkn >> token;
  }
  TgBot::Bot bot(token);

  auto players_file = fs::current_path().string().append("/seirin.data");

  seirin::Leaderboard leaderboard(players_file);

  std::thread updater([&leaderboard](){
    while (true)
    {
      leaderboard.update();
      std::cout << "updated leaderboard\n";

      std::this_thread::sleep_for(std::chrono::hours(1));
    }
  });

  bot.getEvents().onCommand("show", [&bot, &leaderboard](TgBot::Message::Ptr message) {
    if (message->chat->type == TgBot::Chat::Type::Supergroup || message->chat->type == TgBot::Chat::Type::Group)
    {
      //seirin::Leaderboard leaderboard(players_file);
      auto frmt_lbrd = leaderboard.format();

      std::string full{};
      for (const auto& pos: frmt_lbrd)
      {
        full.append(pos);
        full.append("\n");
      }

      bot.getApi().sendMessage(message->chat->id, full, nullptr, nullptr, nullptr, "MarkdownV2");
    }
  });

  // bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
  //     std::cout << "User wrote " << message->text << std::endl;
  // });

  try {
    std::cout << "Bot username: " << bot.getApi().getMe()->username << std::endl;
    TgBot::TgLongPoll longPoll(bot);
    while (true) {
      std::cout << "Long poll started" << std::endl;
      longPoll.start();
    }
  } catch (TgBot::TgException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}