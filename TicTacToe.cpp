#include "TicTacToe.h"

#include <eosiolib/print.hpp>

namespace tictactoe {

static uint8_t coord(const uint8_t row, const uint8_t col)
{
  eosio_assert(row >= 0 && row < 3, "Row value must be in [0,2]!");
  eosio_assert(col >= 0 && col < 3, "Column value must be in [0,2]!");
  return (row * 3) + col;
}

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
    game.moves = 0;
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

  const auto c = coord(row, col);
  eosio_assert(it->board[c] == ' ', "Position is occupied!");

  games_.modify(it, account, [&](auto &game) {
    game.board[c] = 'x';
    game.moves++;

    // The last move is made by the player.
    if (game.moves < 9) {
      game.enemyMove();
      game.moves++;
    }

    game.updateState();
  });
}

uint64_t TicTacToe::game::primary_key() const
{
  return account;
}

void TicTacToe::game::enemyMove()
{
  // Check horizontal lines.
  for (uint8_t row = 0; row < 3; row++) {
    // (o) x x
    if (isFree(row, 0) && isCross(row, 1) && isCross(row, 2)) {
      board[coord(row, 0)] = 'o';
      return;
    }

    // x (o) x
    else if (isCross(row, 0) && isFree(row, 1) && isCross(row, 2)) {
      board[coord(row, 1)] = 'o';
      return;
    }

    // x x (o)
    else if (isCross(row, 0) && isCross(row, 1) && isFree(row, 2)) {
      board[coord(row, 2)] = 'o';
      return;
    }
  }

  // Check vertical lines.
  for (uint8_t col = 0; col < 3; col++) {
    // (o)
    //  x
    //  x
    if (isFree(0, col) && isCross(1, col) && isCross(2, col)) {
      board[coord(0, col)] = 'o';
      return;
    }

    //  x
    // (o)
    //  x
    else if (isCross(0, col) && isFree(1, col) && isCross(2, col)) {
      board[coord(1, col)] = 'o';
      return;
    }

    //  x
    //  x
    // (o)
    else if (isCross(0, col) && isCross(1, col) && isFree(2, col)) {
      board[coord(2, col)] = 'o';
      return;
    }
  }

  // Check diagonal lines

  // (o)
  //    x
  //      x
  if (isFree(0, 0) && isCross(1, 1) && isCross(2, 2)) {
    board[coord(0, 0)] = 'o';
  }

  // x
  //  (o)
  //     x
  else if (isCross(0, 0) && isFree(1, 1) && isCross(2, 2)) {
    board[coord(1, 1)] = 'o';
  }

  // x
  //   x
  //    (o)
  else if (isCross(0, 0) && isCross(1, 1) && isFree(2, 2)) {
    board[coord(2, 2)] = 'o';
  }

  //    (o)
  //   x
  // x
  if (isFree(0, 2) && isCross(1, 1) && isCross(2, 1)) {
    board[coord(0, 2)] = 'o';
  }

  //     x
  //  (o)
  // x
  else if (isCross(0, 2) && isFree(1, 1) && isCross(2, 0)) {
    board[coord(1, 1)] = 'o';
  }

  //      x
  //    x
  // (o)
  else if (isCross(0, 2) && isCross(1, 1) && isFree(2, 0)) {
    board[coord(2, 0)] = 'o';
  }

  // If no patterns are matched then take the first free one.
  else {
    // If the center isn't taken then take it!
    if (isFree(1, 1)) {
      board[coord(1, 1)] = 'o';
      return;
    }

    for (int c = 0; c < 9; c++) {
      if (board[c] == ' ') {
        board[c] = 'o';
        return;
      }
    }
  }
}

void TicTacToe::game::updateState()
{
  for (uint8_t row = 0; row < 3; row++) {
    const auto sym = rowWinner(row);
    if (sym == 'o') {
      state = static_cast<uint8_t>(State::Lost);
      return;
    }
    else if (sym == 'x') {
      state = static_cast<uint8_t>(State::Won);
      return;
    }
  }

  for (uint8_t col = 0; col < 3; col++) {
    const auto sym = colWinner(col);
    if (sym == 'o') {
      state = static_cast<uint8_t>(State::Lost);
      return;
    }
    else if (sym == 'x') {
      state = static_cast<uint8_t>(State::Won);
      return;
    }
  }

  const auto sym = diagWinner();
  if (sym == 'o') {
    state = static_cast<uint8_t>(State::Lost);
    return;
  }
  else if (sym == 'x') {
    state = static_cast<uint8_t>(State::Won);
    return;
  }

  if (moves == 9) {
    state = static_cast<uint8_t>(State::Draw);
  }
}

bool TicTacToe::game::isFree(const uint8_t row, const uint8_t col) const
{
  return board[coord(row, col)] == ' ';
}

bool TicTacToe::game::isCross(const uint8_t row, const uint8_t col) const
{
  return board[coord(row, col)] == 'x';
}

bool TicTacToe::game::isNought(const uint8_t row, const uint8_t col) const
{
  return board[coord(row, col)] == 'o';
}

char TicTacToe::game::rowWinner(const uint8_t row) const
{
  const auto v0 = board[coord(row, 0)], v1 = board[coord(row, 1)], v2 = board[coord(row, 2)];
  return (v0 == v1 && v1 == v2 ? v0 : ' ');
}

char TicTacToe::game::colWinner(const uint8_t col) const
{
  const auto v0 = board[coord(0, col)], v1 = board[coord(1, col)], v2 = board[coord(2, col)];
  return (v0 == v1 && v1 == v2 ? v0 : ' ');
}

char TicTacToe::game::diagWinner() const
{
  const auto nw = board[coord(0, 0)], c = board[coord(1, 1)], se = board[coord(2, 2)],
             sw = board[coord(2, 0)], ne = board[coord(0, 2)];
  if (nw == c && c == se) {
    return nw;
  }
  return (sw == c && c == ne ? sw : ' ');
}

} // tictactoe
