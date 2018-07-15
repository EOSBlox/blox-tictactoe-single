# CONTRACT FOR tictactoe::play

## ACTION NAME: play

### Parameters
Input parameters:

* `account` (person playing a turn in the game)
* `row` (row on 3x3 board to place cross)
* `col` (colum on 3x3 board to place cross)

### Intent
INTENT. The intent of the `{{play}}` action is to place a cross at `{{row}}` and `{{col}}`, and thus making a move - playing a turn of the game. The position must not be preoccupied on the board and the total amount of moves cannot exceed 9, where a maximum of 5 can be made by `{{account}}` as the player.

I understand that `{{account}}` pays for the RAM used to alter the game state, including the computer logic because it happens at the same time.

### Term
TERM. This Contract expires at the conclusion of code execution.
