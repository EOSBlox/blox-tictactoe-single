# CONTRACT FOR tictactoe::newgame

## ACTION NAME: newgame

### Parameters
Input parameters:

* `account` (person initiating game)

### Intent
INTENT. The intent of the `{{newgame}}` action is to create a new game of Tic Tac Toe in which `{{account}}` plays against computer logic.

I understand that `{{account}}` pays for the RAM used to store the game state, and there can only be one active game per account.

### Term
TERM. This Contract expires after the game is finished, either by winning, losing, draw, or early termination via the `{{endgame}}` function.
