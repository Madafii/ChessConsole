Rules

- basic chess movements
- castling
  - two tiles left and right of king are free
  - king can't be attacked
  - rooks and king have not been moved yet
- that pawn thingy
  - can only happen in the next turn

Piece Names

- King K
- Queen Q
- Rook R
- Knight N
- Bishop B
- Pawn P

Input

- {from}:{to}
- from, to = xy = e4 or f6

### Game modes

- normal
- database (play against database)
- random
- againstRandom
- data
- ...

### TODO Features

- rest api for a website
- better input for terminal with previews of a selected move
- improve database filling
- ml with database data
- better board analyzer
- refactor chessInstance

### Build

git clone ...  
git submodule init  
git submodule update  
  
In the cloned repo:  
mkdir build  
cd build  
cmake --build .  
./chess_control  
