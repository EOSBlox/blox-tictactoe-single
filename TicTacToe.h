#include <eosiolib/eosio.hpp>
#include <vector>

namespace tictactoe {

class TicTacToe : public eosio::contract {
  using eosio::contract::contract;

public:
  TicTacToe(account_name self);

  // @abi action
  void newgame(const account_name account);

  // @abi action
  void endgame(const account_name account);

  // @abi action
  void play(const account_name account, uint8_t row, uint8_t col);

private:
  enum class State : uint8_t { PlayersTurn, Draw, Lost, Won };

  // @abi table game i64
  struct game {
    uint64_t account;
    uint8_t state; ///< Cannot use enum in ABI so we cast to/from uint8_t.
    uint8_t moves;
    std::vector<char> board;

    uint64_t primary_key() const;
    void enemyMove();
    void updateState();
    void setState(const State state);

    EOSLIB_SERIALIZE(game, (account)(state)(moves)(board))

  private:
    bool isFree(const uint8_t row, const uint8_t col) const;
    bool isCross(const uint8_t row, const uint8_t col) const;
    bool isNought(const uint8_t row, const uint8_t col) const;
    char rowWinner(const uint8_t row) const;
    char colWinner(const uint8_t col) const;
    char diagWinner() const;
  };

  using games = eosio::multi_index<N(game), game>;
};

EOSIO_ABI(TicTacToe, (newgame)(endgame)(play))

} // tictactoe
