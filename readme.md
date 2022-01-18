# Chess
## Documentation

Piece IDs are assigned as follows:

|p|n|b|r|q|k|P|N|B|R|Q|K|
|-|-|-|-|-|-|-|-|-|-|-|-|
|1|2|3|4|5|6|7|8|9|10|11|12|

Piece types are defined as:

|Pawn|Knight|Bishop|Rook|Queen|King|
|----|------|------|----|-----|----|
|1|2|3|4|5|6|

Piece colors are:

|Black|White|
|-----|-----|
|0|1|

## Move types

Moves are defined as `((y, x), (new y, new x))`

Kingside castle is defined as `((9, 9), (9, 9))`
Queenside castle is defned as `((10, 10), (10, 10))`

# Other information
Uses a alot of vectors because they are dynamic. Possible future versions might use an unordered map for optimization.