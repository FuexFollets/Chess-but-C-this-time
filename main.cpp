#include <iostream>
#include <vector>
#include <string_view>
#include <map>
#include <array>

// definitions, data, and primitive functions

template <typename I1, typename I2> std::map<I1, I2> zip2map(const std::vector<I1>& iter1, const std::vector<I2>& iter2) {
  std::map<I1, I2> zipped;
  int inst{};
  for (const I1& v: iter1) { zipped[v] = iter2[inst++]; }
  return zipped;
}

template <typename I_Type, typename Con> std::vector<std::array<Con, 2>> zip2same(const I_Type& iter1, const I_Type& iter2) {
    int len{iter1.size()};
    std::vector<std::array<Con, 2>> zipped(len);
    for (int i{}; i < len; i++) { zipped[i] = std::array<Con, 2>{iter1[i], iter2[i]}; }
    return zipped;
}

std::vector<int> v_range(const int r1, const int r2 = 0, const int step = 1) {
  int start{r2 ? r1: 0};
  int end{r2 ? r2: r1};
  std::vector<int> range((end-start)/step);
  int vi{};
  for (int x{start}; x < end; x += step) {
    range[vi++] = x;
  }
  return range;
}

template <typename Con, typename I_Type> std::map<Con, int> enumerate_m(const I_Type& iterable) {
    int len{static_cast<int>(iterable.size())};
    std::map<Con, int> as_en{};
    int i{0};
    for (const Con& v: iterable) { as_en[v] = i++; }
    return as_en;
}

typedef std::vector<std::vector<int>> b88;

b88 zero_board(8, std::vector<int>(8, 0));

std::string_view ascii_pieces{"▢♙♘♗♖♕♔♟♞♝♜♛♚"};
std::string_view start_pgn{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"};
std::string_view pieces{"_pnbrqkPNBRQK"};

auto piece_id{enumerate_m<char, std::string_view>(pieces)};

typedef int move_type[2][2]; // start y, x | end y, x

std::vector<move_type> blank_move_list{};

// chess meathods

inline int get_id(const char piece) { return piece_id[piece]; }

inline int get_color(const int id) { return static_cast<int>(id > 7); }

inline int get_type(const int id) { return ((id - 1) % 6) + 1; }

b88 from_pgn(const std::string_view pgn) {
  b88 b{zero_board};
  char ex_pgn[72]; int sel{0};
  char underscore{95}; char slash{47};
  for (auto& c : pgn) {
    if (isdigit(c)) {
      int d{c - '0'};
      for (int x; x < d; x++) { ex_pgn[sel++] = underscore; }
    } else {
      ex_pgn[sel++] = c;
    }
    ex_pgn[sel++] = slash;
  }

  int row{0}; int X{0};
  for (char& v : ex_pgn) {
    if (v - slash) {
      b[row][X++] = get_id(v);
    } else { row++; X = 0; }
  }
  return b;
}

b88 initial_board{from_pgn(start_pgn)};

// piece movements as vectors of y, x differences

std::vector<std::array<int, 3>> n_p{{1, 1, 0}, {-1, 1, 1}, {1, -1, 2}, {-1, -1, 3}};
std::vector<std::array<int, 3>> rm{{0, 1, 0}, {1, 0, 1}, {0, -1, 2}, {-1, 0, 3}};
typedef std::array<int, 2> m_diff;

class movements_by_yx {
  public:
    std::vector<m_diff> knight, king;
    std::vector<std::vector<m_diff>> bishop, rook, queen;
    movements_by_yx() {
      for (const auto&[yd, xd, ind]: n_p) {
        knight.push_back(m_diff{2 * yd, xd}); knight.push_back(m_diff{yd, 2 * xd});
        king.push_back(m_diff{yd, yd + xd}); king.push_back(m_diff{yd + xd, xd});
        for (int x{1}; x < 9; x++) { bishop[ind].push_back(m_diff{x * yd, x * xd}); }
      }
      for (const auto&[yd, xd, ind]: rm) { 
        for (int x{1}; x < 9; x++) { rook[ind].push_back(m_diff{x * yd, x * xd}); }
      }
    }
};

auto movements{movements_by_yx()};
auto move_tu{std::make_tuple(movements)};

// board meathods

b88 after_move(const b88 b, const move_type m) {
  b88 new_b{b};
  new_b[m[1][0]][m[1][1]] = b[m[0][0]][m[0][1]]; new_b[m[0][0]][m[0][1]] = 0;
  return new_b;
}

inline void move(b88& b, const move_type m) { b = after_move(b, m); }

std::vector<move_type> av_moves(const b88 board, const int y, const int x, const std::vector<move_type> past_moves) {
  int piece{board[y][x]}; int type{get_type(piece)}; int color{get_color(piece)};
  if (type == 1) { // if pawn

  } else if ((type == 2) || (type == 6)) { // if king or knight

  } else {
    auto mm{std::get<3>(move_tu)};
  }
}

class Cboard {
  public:
    Cboard(int t = 1, b88 set_board = initial_board, std::vector<move_type> moves = blank_move_list) {
      turn = t; board = set_board; move_arr = moves;
    }

    b88 board;
    int turn{};
    std::vector<move_type> move_arr;
};

int main() {
  return 0;
} 