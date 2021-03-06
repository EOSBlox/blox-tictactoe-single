#include "TicTacToe.h"

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
    game.moves = 0;
    game.board = std::vector<char>(3 * 3, ' ');
    game.setState(State::PlayersTurn);
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

  eosio_assert(static_cast<State>(it->state) == State::PlayersTurn, "Game is finished!");

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
    // (o) o o
    // (o) x x
    if (isFree(row, 0) && isSame(row, 1, row, 2)) {
      board[coord(row, 0)] = 'o';
      return;
    }

    // x (o) x
    // o (o) o
    else if (isFree(row, 1) && isSame(row, 0, row, 2)) {
      board[coord(row, 1)] = 'o';
      return;
    }

    // x x (o)
    // o o (o)
    else if (isFree(row, 2) && isSame(row, 0, row, 1)) {
      board[coord(row, 2)] = 'o';
      return;
    }
  }

  // Check vertical lines.
  for (uint8_t col = 0; col < 3; col++) {
    // (o) (o)
    //  x   o
    //  x   o
    if (isFree(0, col) && isSame(1, col, 2, col)) {
      board[coord(0, col)] = 'o';
      return;
    }

    //  x   o
    // (o) (o)
    //  x   o
    else if (isFree(1, col) && isSame(0, col, 2, col)) {
      board[coord(1, col)] = 'o';
      return;
    }

    //  x   o
    //  x   o
    // (o) (o)
    else if (isFree(2, col) && isSame(0, col, 1, col)) {
      board[coord(2, col)] = 'o';
      return;
    }
  }

  // Check diagonal lines

  // (o)     (o)
  //    x       o
  //      x       o
  if (isFree(0, 0) && isSame(1, 1, 2, 2)) {
    board[coord(0, 0)] = 'o';
  }

  // x     o
  //  (o)   (o)
  //     x     o
  else if (isFree(1, 1) && isSame(0, 0, 2, 2)) {
    board[coord(1, 1)] = 'o';
  }

  // x      o
  //   x      o
  //    (o)    (o)
  else if (isFree(2, 2) && isSame(0, 0, 1, 1)) {
    board[coord(2, 2)] = 'o';
  }

  //    (o)    (o)
  //   x      o
  // x      o
  if (isFree(0, 2) && isSame(1, 1, 2, 1)) {
    board[coord(0, 2)] = 'o';
  }

  //     x     o
  //  (o)   (o)
  // x     o
  else if (isFree(1, 1) && isSame(0, 2, 2, 0)) {
    board[coord(1, 1)] = 'o';
  }

  //      x      o
  //    x      o
  // (o)    (o)
  else if (isFree(2, 0) && isSame(0, 2, 1, 1)) {
    board[coord(2, 0)] = 'o';
  }

  else {
    // If the center isn't taken then take it!
    if (isFree(1, 1)) {
      board[coord(1, 1)] = 'o';
      return;
    }

    // Otherwise take the first available spot.
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
  const auto checkSym = [this](const char sym) {
    if (sym == ' ') {
      return false;
    }
    if (sym == 'o') {
      setState(State::Lost);
    }
    else if (sym == 'x') {
      setState(State::Won);
    }
    return true;
  };

  for (uint8_t row = 0; row < 3; row++) {
    if (checkSym(rowWinner(row))) return;
  }

  for (uint8_t col = 0; col < 3; col++) {
    if (checkSym(colWinner(col))) return;
  }

  if (checkSym(diagWinner())) return;

  if (moves == 9) {
    setState(State::Draw);
  }
}

void TicTacToe::game::setState(const State state)
{
  this->state = static_cast<uint8_t>(state);
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

bool TicTacToe::game::isSame(const uint8_t row1, const uint8_t col1, const uint8_t row2,
                             const uint8_t col2) const
{
  return ((isCross(row1, col1) && isCross(row2, col2)) ||
          (isNought(row1, col1) && isNought(row2, col2)));
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
