#include "TicTacToe.h"

#include <eosiolib/print.hpp>

namespace tictactoe {

TicTacToe::TicTacToe(account_name self) : contract(self)
{
}

void TicTacToe::newgame(const account_name account)
{
  require_auth(account);

  games games_(_self, _self);
  eosio_assert(games_.find(account) == games_.end(), "Account already has a game!");

  games_.emplace(account, [&](auto &game) {
    game.account = account;
    game.state = static_cast<int>(State::PlayersTurn);
    game.board = std::vector<char>(3 * 3, ' ');
  });
}

void TicTacToe::endgame(const account_name account)
{
  require_auth(account);

  games games_(_self, _self);
  auto it = games_.find(account);
  eosio_assert(it != games_.end(), "No game for account!");
  games_.erase(it);
  eosio_assert(it != games_.end(), "Could not remove game!");
}

void TicTacToe::play(const account_name account, uint8_t row, uint8_t col)
{
  require_auth(account);

  games games_(_self, _self);
  auto it = games_.find(account);
  eosio_assert(it != games_.end(), "No game for account!");

  eosio_assert(row >= 0 && row < 3, "Row value must be in [0,2]!");
  eosio_assert(col >= 0 && col < 3, "Column value must be in [0,2]!");
  const auto coord = (row * 3) + col;
  eosio_assert(it->board[coord] == ' ', "Position is occupied!");

  games_.modify(it, account, [&](auto &game) {
    game.board[coord] = 'x';

    // TODO: Place random enemy "nought"
    // TODO: Determine state from board, like draw etc.
  });
}

uint64_t TicTacToe::game::primary_key() const
{
  return account;
}

} // tictactoe
