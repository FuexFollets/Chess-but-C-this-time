#include <iostream>
#include <vector>
#include <string_view>
#include <map>
#include <array>
#include <cstdlib>
#include <memory>
#include <stdexcept>
 
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
 
//typedef std::vector<std::vector<int>> b88;
typedef std::array<std::array<int, 8>, 8> b88;
 
b88 zero_board{};//(8, std::vector<int>(8, 0));
 
std::string_view ascii_pieces{"▢♙♘♗♖♕♔♟♞♝♜♛♚"};
std::string_view start_pgn{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"};
std::string_view pieces{"_pnbrqkPNBRQK"};
 
auto piece_id{enumerate_m<char, std::string_view>(pieces)};
 
typedef std::array<std::array<int, 2>, 2> move_type; // start y, x | end y, x
 
std::vector<move_type> blank_move_list{};
 
auto __{[](){
  blank_move_list.push_back(move_type{{{11, 11}, {11, 11}}});
}};
 
// chess meathods
 
inline int get_id(const char piece) { return piece_id[piece]; }
 
inline int get_color(const int id) { return static_cast<int>(id > 7); }
 
inline int get_type(const int id) { return ((id - 1) % 6) + 1; }
 
inline int get_dir(const int turn) { return (turn * (-2) + 1); }    
 
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
  for (const char& v: ex_pgn) {
    if (v - slash) {
      b[row][X++] = get_id(v);
    } else { row++; X = 0; }
  }
  return b;
}
 
b88 initial_board{from_pgn(start_pgn)};
 
// piece movements as vectors of y, x differences

typedef struct {
  int yd;
  int xd;
} m_diff;
 
struct m_init {
    std::vector<m_diff> knight, king;
    std::vector<std::vector<m_diff>> bishop, rook, queen;
 
    std::map<int, std::vector<m_diff>&> m1{{2, knight}, {6, king}};
    std::map<int, std::vector<std::vector<m_diff>>&> m2{{3, bishop}, {4, rook}, {5, queen}};
};
 
m_init initialize_diff() {
    typedef struct {
        int ym; int xm; int ind;
    } yxd;
   
    std::array<yxd, 4> n_p{{{1, 1, 0}, {-1, 1, 1}, {1, -1, 2}, {-1, -1, 3}}};
    std::array<yxd, 4> rm{{{0, 1, 0}, {1, 0, 1}, {0, -1, 2}, {-1, 0, 3}}};
   
    std::vector<m_diff> knight, king;
    std::vector<std::vector<m_diff>> bishop(4), rook(4), queen;
   
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
 
    return m_init{knight, king, bishop, rook, queen};
}
 
const auto mov_s{initialize_diff()};
 
// board methods
 
b88 after_move(const b88 b, const move_type m, const int turn) {
  b88 new_b{b};
 
  if ((-1 < m[0][0]) && (9 > m[0][0])) {
    new_b[m[1][0]][m[1][1]] = b[m[0][0]][m[0][1]]; new_b[m[0][0]][m[0][1]] = 0;
  } else {
    if (m[0][0] == 9) {
      new_b[turn * 7][6] = b[turn * 7][4];
      new_b[turn * 7][5] = b[turn * 7][7];
      new_b[turn * 7][4] = 0; new_b[turn * 7][7] = 0;
    } else {
      new_b[turn * 7][2] = b[turn * 7][4];
      new_b[turn * 7][3] = b[turn * 7][0];
      new_b[turn * 7][4] = 0; new_b[turn * 7][0] = 0;
    }
  }
  return new_b;
}
 
int has_king(const b88 board, const int color) {
  for (const std::array<int, 8>& row: board) {
    for (const int& id: row) {
      if ((get_type(id) == 6) && (get_color(id) == color)) {
        return 1;
      }
    }
  }
  return 0;
}
 
std::array<int, 2> king_loc(const b88 board, int color) {
  int ck{6 * color + 6};
  for (int y{}; y < 8; y++) {
    for (int x{}; x < 8; x++) {
      if (board[y][x] == ck) { return std::array<int, 2>{{y, x}}; }
    }
  }
  throw std::invalid_argument("Board does not contain king");
  return std::array<int, 2>{-1, -1};
}
 
std::vector<move_type> square_moves(const b88 board, const int y, const int x, const std::vector<move_type> past_moves = blank_move_list) {
  auto last_move{past_moves[0]};
  int piece{board[y][x]}; int type{get_type(piece)}; int color{get_color(piece)};
  std::vector<move_type> valid_moves;
  if (type == 1) { // if pawn
    int dir{get_dir(color)};
    if (!board[y + dir][x]) { //moving forward
      valid_moves.push_back(move_type{{{y, x}, {y + dir, x}}});
      if ((!(board[y + 2 * dir][x])) && (!(x - (-3) * dir) + 5)){
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
      if ((get_type(board[last_move[1][0]][last_move[1][1]]) == 1) && ((last_move[0][0] + 2 * dir) == ((last_move[1][0])) && (y == last_move[1][0]) && (std::abs(last_move[1][1]) - x) == 1)) {
        valid_moves.push_back({{{y, x}, {y + dir, last_move[1][1]}}});
      }  
    }
  } else if ((type == 2) || (type == 6)) { // if king or knight
    const std::vector<m_diff>& space_iter{(mov_s.m1).at(type)};
    for (const auto&[yd, xd]: space_iter) {
      int space{board[y + yd][x + xd]};
      if (!(space) || (color ^ get_color(space))) { valid_moves.push_back(move_type{{{y, x}, {y + yd, x + xd}}}); }
    }
  } else {
    const auto& space_iter{(mov_s.m2).at(type)};
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
 
int is_cont(b88 board, int y, int x, int turn) {
  for (int sel_y{}; sel_y < 8; sel_y++) {
    for (int sel_x{}; sel_x < 8; sel_x++) {
      auto opm{square_moves(board, y, x)};
      for (const auto& move: opm) {
        if ((move[1][0] == y) && (move[1][1] == x)) {
          return 1;
        }
      }
    }
  }
  return 0;
}
 
std::vector<move_type> av_moves(const b88 board, const std::vector<move_type> past_moves, const int color) {
  std::vector<move_type> moves{};
  for (int y{}; y < 8; y++) {
    for (int x{}; x < 8; x++) {
      if (board[y][x]) {
        if (get_color(board[y][x]) == color) {
          auto m_for_square{square_moves(board, y, x, past_moves)};
          moves.insert(moves.end(), m_for_square.begin(), m_for_square.end());
        }
      }
    }
  }
 
  for (int side{2}; side < 6; side += 3) {
    for (int space{}; space < 2; space++) {
      if (is_cont(board, color * 7, side + space, !color) || (!(board[color * 7][side + space]))) {
        break;
      } else if (space) {  
        int h{(side + 25)/3};
        moves.push_back(move_type{{{h, h}, {h, h}}});
      }
    }
  }
  return moves;
}
 
std::vector<b88> p_from_pos(const b88 board, const int turn, std::vector<move_type> past_moves) {
  std::vector<b88> p_pos{};
  std::vector<move_type> avm{av_moves(board, past_moves, turn)};
 
  for (const move_type& move: avm) { p_pos.push_back(after_move(board, move, turn)); }
  return p_pos;
}
 
int is_legal_pos(b88 board, int color) {
  auto kl{king_loc(board, color)};
  return is_cont(board, kl[0], kl[1], color);
}
 
std::vector<move_type> legal_moves(const b88 board, const int turn, const std::vector<move_type> past_moves = blank_move_list) {
  auto vm{av_moves(board, past_moves, turn)};
  std::vector<move_type> lg_moves;
  for (const move_type& move: vm) {
    if (is_legal_pos(after_move(board, move, turn), turn)) {
      lg_moves.push_back(move);
    }
  }
  return lg_moves;
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

typedef struct {
  float val;
  bool init{0};
} init_fl;

void get_leaves(const C_node& h_node, std::vector<C_node&> leaves) {
  if (h_node.branches.empty()) {
    leaves.push_back(h_node);
  } else {
    for (const C_node& node: h_node) {
      get_leaves(h_node, leaves);
    }
  }
}

float surface_eval(b88 board);

//bool trunicate();

void set_depth_ev(C_node& );

class C_node {
  public:
    static int n_count;
 
    C_node(Cboard board) {
      n_count++;
      node_board = board;
    }
 
    ~C_node() {
      n_count--;
      for (C_node& node_ptr: branches) { delete node_ptr; }
    }
 
    Cboard node_board;
    float eval;
    std::vector<C_node&> branches;
   
    inline void add_branch(C_node* b_ptr) { branches.push_back(b_ptr); }
 
};
 
int main() {
    std::cout << "no errors\n";
 
   
 
    return 0;
}