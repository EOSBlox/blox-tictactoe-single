# blox-tictactoe-single
Single player game of TicTacToe against the smart contract.

## Actions
The smart contract has three actions:
  * `newgame(account)`
  * `endgame(account)`
  * `play(account, row, col)`

## Compilation
EOSIO is expected to be compiled and available on the system, especially `eosiocpp`:
```
% make
eosiocpp -o TicTacToe.wast TicTacToe.cpp
eosiocpp -g TicTacToe.abi TicTacToe.cpp
Generated TicTacToe.abi ...
```

## Usage
Substitute `ACCOUNT` with the actual account name in the following.

The smart contract can be set onto a blockchain via:
```
% cleos set contract ACCOUNT . TicTacToe.wast TicTacToe.abi
```

Creating a new game:
```
% cleos push action ACCOUNT newgame '["ACCOUNT"]' -p ACCOUNT
executed transaction: 896feba081d71b3ce2e14f9c35850b11327b66dedf8103ad7a01445479ad44cf  104 bytes  1209 us
#        ACCOUNT <= ACCOUNT::newgame              {"account":"ACCOUNT"}
```

As an example, place a cross in the middle of the board as the first turn:
```
% cleos push action ACCOUNT play '["ACCOUNT",1,1]' -p ACCOUNT
executed transaction: 841ed5c583d53387db75c28249f03c3150d4bd5a36ac9e1ec0a9dda9d839bb79  104 bytes  1993 us
#        ACCOUNT <= ACCOUNT::play                 {"account":"ACCOUNT","row":1,"col":1}
```

The state of the game can be checked like this:
```
% cleos get table ACCOUNT ACCOUNT game
{
  "rows": [{
      "account": "ACCOUNT_ID",
      "state": 0,
      "moves": 2,
      "board": [
        111,  32, 32,
         32, 120, 32,
         32,  32, 32
      ]
    }
  ],
  "more": false
}
```

When a game is finished it can be ended so another game can be created for the same account:
```
% cleos push action ACCOUNT endgame '["ACCOUNT"]' -p ACCOUNT
executed transaction: 4b2543aafbbbb39709e920320b238e2dc867101d88c350b88b9770a556a9654c  104 bytes  1153 us
#        ACCOUNT <= ACCOUNT::endgame              {"account":"ACCOUNT"}
```
