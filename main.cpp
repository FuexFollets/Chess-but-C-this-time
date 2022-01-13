#include <iostream>
#include <vector>
#include <string_view>
#include <map>
#include <array> 
#include <cstdlib>

// definitions, data, and primitive functions

class exec_outer_main {
  public:
    exec_outer_main(void (*exfn)()) { exfn(); }
};

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

template <typename Con, int ui_len> void appen_arr(std::array<Con, ui_len>& arr, const Con obj) {
  for (Con& val: arr) {
    if (!val) { val = obj; return;}
  }
}

template <typename I, typename Con> std::vector<Con> to_vect(I iter) {
  std::vector<Con> vect(iter.size()); int ind{};
  for (const Con& val: iter) {
    vect[ind++] = val;
  }
  return vect;
} 

template <typename Con> inline std::vector<Con> extend_v() {}

typedef std::vector<std::vector<int>> b88;

b88 zero_board(8, std::vector<int>(8, 0));

std::string_view ascii_pieces{"▢♙♘♗♖♕♔♟♞♝♜♛♚"};
std::string_view start_pgn{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"};
std::string_view pieces{"_pnbrqkPNBRQK"};

auto piece_id{enumerate_m<char, std::string_view>(pieces)};

typedef std::array<std::array<int, 2>, 2> move_type; // start y, x | end y, x

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

namespace mov {
  namespace {
    typedef struct {
      int ym; int xm; int ind;
    } yxd;
    
    std::vector<yxd> n_p{{1, 1, 0}, {-1, 1, 1}, {1, -1, 2}, {-1, -1, 3}};
    std::vector<yxd> rm{{0, 1, 0}, {1, 0, 1}, {0, -1, 2}, {-1, 0, 3}};
  }

  typedef struct {
    int yd;
    int xd;
  } m_diff;

  std::vector<m_diff> knight, king;
  std::vector<std::vector<m_diff>> bishop, rook, queen;
  std::map<int, std::vector<m_diff>&> m1{{2, knight}, {6, king}};
  std::map<int, std::vector<std::vector<m_diff>>&> m2{{3, bishop}, {4, rook}, {5, queen}};
  std::vector<m_diff> pawn{{1, 0}, {2, 0}};

  inline int get_dir(int color) {
    return (color * (-2) + 1);
  }
  
  inline void initialize_v(); 
  
}

inline void mov::initialize_v() {
  for (const auto&[yd, xd, ind]: n_p) {
    knight.push_back(m_diff{2 * yd, xd}); knight.push_back(m_diff{yd, 2 * xd});
    king.push_back(m_diff{yd, yd + xd}); king.push_back(m_diff{yd + xd, xd});
    for (int x{1}; x < 9; x++) { bishop[ind].push_back(m_diff{x * yd, x * xd}); }
  }
  for (const auto&[yd, xd, ind]: rm) {
    for (int x{1}; x < 9; x++) { rook[ind].push_back(m_diff{x * yd, x * xd}); }
  }
  queen = rook;
  queen.insert(queen.end(), bishop.begin(), bishop.end());
}

auto _1{exec_outer_main(&(mov::initialize_v))};

// board meathods

b88 after_move(const b88 b, const move_type m) {
  b88 new_b{b};
  new_b[m[1][0]][m[1][1]] = b[m[0][0]][m[0][1]]; new_b[m[0][0]][m[0][1]] = 0;
  return new_b;
}

inline void move(b88& b, const move_type m) { b = after_move(b, m); }

std::vector<move_type> square_moves(const b88 board, const int y, const int x, const move_type past_moves = move_type{{{11}, {11}}}) {
  int piece{board[y][x]}; int type{get_type(piece)}; int color{get_color(piece)};
  std::vector<move_type> valid_moves;
  if (type == 1) { // if pawn
    int dir{get_dir(color)};
    if (!board[y + dir][x]) { //moving forward
      valid_moves.push_back(move_type{{{y, x}, {y + dir, x}}});
      if ((!(board[y + 2 * dir][xdir])) && (!(x - (-3) * dir) + 5)){
        valid_moves.push_back(move_type{{{y, x}, {y + 2 * dir, x}}});
      }
    }
    for (int xd{-1}; xd < 2; xd++) { // taking pieces
        if (((x + xd) < 8) && (x + xd) > -1) {
          if ((get_color(board[y + dir][x + xd]) ^ color) && (!(board[y + dir][x + xd]))) {
            valid_moves.push_back({{{y, x}, {y + dir, x + xd}}});
          }
        }
      }
    if (last_move[0][0] - 11) {
      if ((get_type(board[last_move[1][0]][last_move[1][1]]) == 1) && ((last_move[0][0] + 2 * dir) == ((last_move[1][0])) && (y == last_move[1][0]) && (std::abs(last_move[1][1]) - x) == 1) {
        valid_moves.push_back({{{y, x}, {y + dir, last_move[1][1]}}});
      }  
    }
  } else if ((type == 2) || (type == 6)) { // if king or knight
    std::vector<mov::m_diff>& space_iter{mov::m1[type]};
    for (const auto&[yd, xd]: space_iter) {
      int space{board[y + yd][x + xd]};
      if (!(space) || (color ^ get_color(space))) { valid_moves.push_back(move_type{{{y, x}, {y + yd, x + xd}}}); }
    }
    
  } else {
    //std::vector<std::vector<mov::m_diff>>
    auto& space_iter{mov::m2[type]};
    for (const auto& direction: space_iter) {
      for (const auto&[yd, xd]: direction) {  
        int space{board[y + yd][x + xd]};
  
        if (!space) { valid_moves.push_back(move_type{{{y, x}, {y + yd, x + xd}}}); }
        else if (get_color(space) ^ color) {
          valid_moves.push_back(move_type{{{y, x}, {y + yd, x + xd}}});
          break;
        } else { break; }
      }
    }
  }
  return valid_moves;
}

class Cboard {
  public:
    Cboard(int t = 1, b88 set_board = initial_board, std::vector<move_type> moves = blank_move_list) {
      turn = t;
      board = set_board;
      move_arr = moves;
    }

    b88 board{};
    int turn{};
    std::vector<move_type> move_arr{};
};

int main() {
  return 0;
} 