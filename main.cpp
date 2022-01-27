#include <iostream>
#include <vector>
#include <string_view>
#include <map>
#include <array>
#include <cstdlib>
#include <stdexcept>
#include <unordered_map>
#include <thread>
#include <chrono>
#include <string>
#include <tuple>


// definitions, data, and primitive functions

template <typename T> class t_obj {
  public:
    t_obj(T fn) : function(fn) {}
    T function;
    std::thread process;
    void start() {
      process = std::thread(function);
    }

    inline void join() { process.join(); }
};

template <typename Con> class map_set {
  private:
    int appen{0};
  public:
    std::unordered_map<int, Con> as_map;  
    void append(Con obj) {
      as_map[appen++] = obj;
    }

    void append_set(map_set<Con> ap_set) {
      for (const auto&[_, obj]: ap_set) {
        append(obj);
      }
    }

    inline auto begin() { return as_map.begin(); }
    inline auto end() { return as_map.end(); }
    inline bool empty() { return as_map.empty(); }
    inline void erase(int key) { as_map.erase(key); }
    
    friend std::ostream& operator<< (std::ostream& out, map_set set) {
        out << "{ ";
        for (const auto&[ind, obj]: set) {
            out << '(' << ind << ": " << obj << "), ";
        }
        out << '}';
        return out;
    }
};
 
template <typename T, int Len> class append_arr {
    private:
        int append_ind{};
    public:
        std::array<T, Len> this_arr;
        inline auto begin() { return this_arr.begin(); }
        inline auto end() { return this_arr.end(); }
        inline void append(T obj) {
           this_arr[append_ind++] = obj;
       }
};
 
template <typename Con, typename I_Type> std::map<Con, int> enumerate_m(const I_Type& iterable) {
    int len{static_cast<int>(iterable.size())};
    std::map<Con, int> as_en{};
    int i{0};
    for (const Con& v: iterable) { as_en[v] = i++; }
    return as_en;
}
 

//typedef std::vector<std::vector<int>> b88;
typedef std::array<std::array<int, 8>, 8> b88;
 
b88 zero_board{};//(8, std::vector<int>(8, 0));
 
std::string ascii_pieces{"▢♙♘♗♖♕♔♟♞♝♜♛♚"};
std::string start_pgn{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"};
std::string pieces{"_pnbrqkPNBRQK"};
 
auto piece_id{enumerate_m<char, std::string>(pieces)};
 
//typedef std::array<std::array<int, 2>, 2> move_type; // start y, x | end y, x

struct move_type {
  int y; int x;
  int new_y; int new_x;
  move_type(int y_=0, int x_=0, int new_y_=0, int new_x_=0) :
    y{y_}, x{x_}, new_y{new_y_}, new_x{new_x_} {}

  friend bool operator==(move_type m1, move_type m2) {
    return ((m1.y == m2.y) && (m1.x == m2.x) && (m1.new_y == m2.new_y) && (m1.new_x == m2.new_x));
  }

  inline friend std::ostream& operator<< (std::ostream& out, move_type move) {
      out << '[' << move.y << ' ' << move.x << "][" << move.new_y << ' ' << move.new_x << ']';
      return out;
  }
};

std::vector<move_type> blank_move_list{move_type(11, 11, 11, 11)};
 
// chess meathods

inline bool c_is_int(char x) { return ((-1 < (x - '0')) && ((x - '0') < 10)); }

inline int get_id(const char piece) { return piece_id[piece]; }
 
inline int get_color(const int id) { return static_cast<int>(id > 7); }
 
inline int get_type(const int id) { return ((id - 1) % 6) + 1; }
 
inline int get_dir(const int turn) { return (turn * (-2) + 1); }    

inline int get_c(const int color) { return color * 2 - 1; }

inline bool ib(const int x, const int h, const int l) { return ((x < h) && (x > l)); }

b88 from_pgn(const std::string pgn) {
  b88 b;
  std::array<int, 64> as_arr{};
  int sel{0};
  char slash{47};

  for (const char& c: pgn) {
    if (c != slash) {
      if (c_is_int(c)) {
        sel += c - '0';
      } else {
        as_arr[sel++] = piece_id[c];
      }
    }
  }
    for (int row{}; row < 8; row++) {
        for (int X{}; X < 8; X++) {
            b[row][X] = as_arr[row * 8 + X];
        }
    }

  return b;
}

b88 initial_board{from_pgn(start_pgn)};
 
// piece movements as vectors of y, x differences

typedef struct {
  int yd;
  int xd;
} m_diff;

template <typename T, int len> struct App_arr {
  void append(T obj) { as_arr[append_index++] = obj; }
  std::array<T, len> as_arr;

  template <int L> void append(std::array<T, L> obj_arr) {
    for (const auto&v: obj_arr) {
      append(v);
    }
  }
  inline auto begin() { return as_arr.begin(); }
  inline auto end() { return as_arr.end(); }

  private:
    int append_index{0};
};

const auto mov_s{[](){
  App_arr<m_diff, 8> knight, king;
  std::array<App_arr<m_diff, 8>, 4> bishop, rook;
  App_arr<App_arr<m_diff, 8>, 8> queen;

  constexpr const std::array<std::array<int, 3>, 4> n_p{{{1, 1, 0}, {-1, 1, 1}, {1, -1, 2}, {-1, -1, 3}}};
  constexpr const std::array<std::array<int, 3>, 4> rm{{{0, 1, 0}, {1, 0, 1}, {0, -1, 2}, {-1, 0, 3}}};

  for (const auto&[yd, xd, ind]: n_p) {
    knight.append(m_diff{2 * yd, xd}); knight.append(m_diff{yd, 2 * xd});
    for (int x{1}; x < 9; x++) { bishop[ind].append(m_diff{x * yd, x * xd}); }
  }

  for (const auto&[yd, xd, ind]: rm) {
    for (int x{1}; x < 9; x++) { rook[ind].append(m_diff{x * yd, x * xd}); }
  }

  for (int x{-1}; x < 2; x++) {
    for (int y{-1}; y < 2; y++) {
      if (y || x) { king.append(m_diff{y, x}); }
    }
  }

  for (const auto& dir: bishop) { queen.append(dir); }
  for (const auto& dir: rook) { queen.append(dir); }

  return std::make_tuple(0, 0, knight.as_arr, [bishop](){
    std::array<std::array<m_diff, 8>, 4> as_2D;
    int dir_n{};
    for (const App_arr<m_diff, 8>& dir: bishop) {
      as_2D[dir_n++] = dir.as_arr;
    }
    return as_2D;
  }(), [rook](){
    std::array<std::array<m_diff, 8>, 4> as_2D;
    int dir_n{};
    for (const App_arr<m_diff, 8>& dir: rook) {
      as_2D[dir_n++] = dir.as_arr;
    }
    return as_2D;
  }(), [queen](){
    std::array<std::array<m_diff, 8>, 8> as_2D;
    int dir_n{};
    for (const App_arr<m_diff, 8>& dir: queen.as_arr) {
      as_2D[dir_n++] = dir.as_arr;
    }
    return as_2D;
  }(), king.as_arr);
}()};

// board methods

class C_board {
  public:
    C_board(b88 set_board = initial_board, int t = 1, std::vector<move_type> past_moves = blank_move_list) : turn{t}, pos{set_board}, move_arr{past_moves} {}
 
    b88 pos{};
    int turn{};
    std::vector<move_type> move_arr{};

    inline friend bool operator==(C_board b1, C_board b2) {
    return ((b1.turn == b2.turn) && (b1.pos == b2.pos) && (b1.move_arr == b2.move_arr));
    }

    friend std::ostream& operator<< (std::ostream& out, C_board cb) {
      out << "turn: " << cb.turn << "\n";
      for (const auto& row: cb.pos) {
        for (const int& id: row) {
          out << id << " ";
        }
        out << "\n";
      }
      return out;
    }

};

b88 after_move(const b88 b, const move_type m, const int turn) {
  b88 new_b{b};
 
  if ((-1 < m.y) && (9 > m.y)) {
    new_b[m.new_y][m.new_x] = b[m.y][m.x]; new_b[m.y][m.x] = 0;
  } else {
    if (m.y == 9) {
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

inline C_board cboard_after_move(C_board cb, move_type m) {
  return C_board(after_move(cb.pos, m, cb.turn), 1 - cb.turn, cb.move_arr);
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
 
map_set<move_type> square_moves(const b88 board, const int y, const int x, const std::vector<move_type> past_moves = blank_move_list) {
  auto last_move{past_moves[0]};
  int piece{board[y][x]};
  int type{get_type(piece)};
  int color{get_color(piece)};
  map_set<move_type> valid_moves;
  
  if (type == 1) { // if pawn
    int dir{get_dir(color)};
    if (!board[y + dir][x]) { //moving forward
      valid_moves.append(move_type{y, x, y + dir, x});
      if ((!(board[y + 2 * dir][x])) && (!(x - (-3) * dir) + 5)){
        valid_moves.append(move_type{y, x, y + 2 * dir, x});
      }
    }
    for (int xd{-1}; xd < 2; xd++) { // taking pieces
        if (((x + xd) < 8) && (x + xd) > -1) {
          if ((get_color(board[y + dir][x + xd]) ^ color) && (!(board[y + dir][x + xd]))) {
            valid_moves.append(move_type{y, x, y + dir, x + xd});
          }
        }
      }
    if (last_move.y - 11) {
      if ((get_type(board[last_move.new_y][last_move.new_x]) == 1) && ((last_move.y + 2 * dir) == ((last_move.new_y)) && (y == last_move.new_y) && (std::abs(last_move.new_x) - x) == 1)) {
        valid_moves.append(move_type{y, x, y + dir, last_move.new_x});
      }  
    }
  } else if ((type == 2) || (type == 6)) { // if king or knight
    const auto& space_iter{std::get<type>(mov_s)};
    for (const auto&[yd, xd]: space_iter) {
      int space{board[y + yd][x + xd]};
      if (!(space) || (color ^ get_color(space))) { valid_moves.append(move_type{y, x, y + yd, x + xd}); }
    }
  } else {
    const auto& space_iter{std::get<type>(mov_s)};
    for (const auto& direction: space_iter) {
      for (const auto&[yd, xd]: direction) {  
        int space{board[y + yd][x + xd]};
        if (!space) { valid_moves.append(move_type{y, x, y + yd, x + xd}); }
        else if (get_color(space) ^ color) {
          valid_moves.append(move_type{y, x, y + yd, x + xd});
          break;
        } else { break; }
      }
    }
  }
  return valid_moves;
}
 
int is_cont(const b88 board, const int y, const int x, const int turn) {
  for (int sel_y{}; sel_y < 8; sel_y++) {
    for (int sel_x{}; sel_x < 8; sel_x++) {
      auto opm{square_moves(board, y, x).as_map};
      for (const auto&[_, move]: opm) {
        if ((move.new_y == y) && (move.new_x == x)) {
          return 1;
        }
      }
    }
  }
  return 0;
}
 
map_set<move_type> av_moves(const b88 board, const std::vector<move_type> past_moves, const int color) {
  map_set<move_type> moves;
  for (int y{}; y < 8; y++) {
    for (int x{}; x < 8; x++) {
      if (board[y][x]) {
        if (get_color(board[y][x]) == color) {
          auto m_for_square{square_moves(board, y, x, past_moves)};
          moves.append_set(m_for_square);
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
        moves.append(move_type{h, h, h, h});
      }
    }
  }
  return moves;
}
 
int is_legal_pos(b88 board, int color) {
  auto kl{king_loc(board, color)};
  return is_cont(board, kl[0], kl[1], color);
}
 
map_set<move_type> legal_moves(const b88 board, const int turn, const std::vector<move_type> past_moves = blank_move_list) {
  auto vm{av_moves(board, past_moves, turn).as_map};
  map_set<move_type> lg_moves;
  for (const auto&[_, move]: vm) {
    if (is_legal_pos(after_move(board, move, turn), turn)) {
      lg_moves.append(move);
    }
  }
  return lg_moves;
}

map_set<move_type> legal_moves(const C_board cb) {
  const auto board{cb.pos}; const auto past_moves{cb.move_arr}; const auto turn{cb.turn};
  auto vm{av_moves(board, past_moves, turn).as_map};
  map_set<move_type> lg_moves;
  for (const auto&[_, move]: vm) {
    if (is_legal_pos(after_move(board, move, turn), turn)) {
      lg_moves.append(move);
    }
  }
  return lg_moves;
}

int count_ID(const b88 board, const int id) {
  int count{0};
  for (const auto& row: board) {
    for (const int& val: row) {
      if (!(val - id)) {
        count++;
      }
    }
  }
  return count;
}

struct init_fl {
  float val;
  bool init{false};
  init_fl(float val_=0, bool init_=false) :
    val{val_}, init{init_} {} 
};

class C_node {
  public:
    C_node(C_board board, int* n_count_inp) {
      node_board = board;
      node_count = n_count_inp;
      (*node_count)++;
    }
 
    ~C_node() {
      (*node_count)--;
      for (auto&[ind, node_ptr]: branches.as_map) { delete node_ptr; }
    }
 
    C_board node_board;
    init_fl eval;
    map_set<C_node*> branches;
    int& turn{node_board.turn};
    int* node_count;
   
    inline void add_branch(C_node* new_node_ptr) { branches.append(new_node_ptr); }
 
};

// Z0 (zero depth) evaluation functions //

float surface_eval(b88 board) { return 0.0F; }

bool trunicate() { return false; }

// //

float min_max_eval(const C_node* h_node, int min_max) { // 1=max -1=min
  float mm{0.0F};
  for (const auto[_, node]: (h_node->branches.as_map)) {
    if (mm < (node->eval.val) * min_max) { mm = node->eval.val; }
  }
  return mm;
}

void set_depth_ev(C_node* h_node, bool& toggle) {
  if (h_node->branches.empty() && h_node->eval.init && toggle) {
    h_node->eval = init_fl{surface_eval(h_node->node_board.pos), true};
  } else if (toggle) {
    if (!h_node->branches.as_map[0]->eval.init) {
      for (auto&[_, node_ptr]: h_node->branches) { set_depth_ev(h_node, toggle); }
    }
    h_node->eval = init_fl{min_max_eval(h_node, get_dir(h_node->turn)), true};
  }
}

void form_branches(C_node* h_node, bool& toggle) {
  if (h_node->branches.empty() && toggle) {
    auto moves{legal_moves(h_node->node_board)}; 
    for (const auto&[_, move]: moves) {
      if (!trunicate()) {
        h_node->add_branch(new C_node(cboard_after_move(h_node->node_board, move), h_node->node_count));
      }
    }
  } else if (toggle) {
    for (const auto&[_, node]: h_node->branches) {
      form_branches(node, toggle);
    }
  }
}

void un_init(C_node* h_node) {
  h_node->eval.init = false;
  if (!h_node->branches.empty()) {
    for (auto[_, node]: h_node->branches) {
      un_init(node);
    }
  }
}

bool compare_cboard(C_board b1, C_board b2) {
  return ((b1.turn == b2.turn) && (b1.pos == b2.pos) && (b1.move_arr == b2.move_arr));
}

void tree_move(C_node* h_node, const move_type move) {
  const auto pos{cboard_after_move(h_node->node_board, move)}; bool has_branch{false};
  for (auto[key, node]: h_node->branches) {
    if (node->node_board == pos) {
      has_branch = true;
      h_node->branches.erase(key);
      delete h_node;
      h_node = node;
      un_init(h_node);
      break;
    }
  }
  if (!has_branch) {
    delete h_node;
    h_node->add_branch(new C_node(pos, h_node->node_count));
  }
}

class Chess_game {
  public:
    Chess_game(C_board start=C_board()) : board(start) {}
    C_board board;

    map_set<move_type> possible_moves() {
      return legal_moves(board);
    }

    void make_move(move_type m) {
      
    }

};

/*
class Evaluation_thread {
  public:
    Evaluation_thread(C_board cb) {
      main_node = new C_node(cb, &total_nodes);
    }

    void start() {
      
      std::thread t1([this](){
        while (is_calc) {
          set_depth_ev(main_node, is_calc);
        } return;
      });

      std::thread t2([this](){
        while (is_calc) {
          form_branches(main_node, is_calc);
        } return;
      });

      threads = {&t1, &t2};
      
    }

    void stop_calc() {
      is_calc = false;
    }

    void join() {
      threads[0]->join();
      threads[1]->join();
    }

    void make_move(move_type move) {
      stop_calc();
      tree_move(main_node, move);
      start();
    }

    int total_nodes;
  private:
    C_node* main_node;
    bool is_calc{false};
    std::array<std::thread*, 2> threads;
};
*/
int main() {


  C_board b;
  std::cout << b;

  std::cout << '\n';

  std::cout << square_moves(b.pos, 0, 1);

  std::cout << '\n';

  std::cout << "\n\nCompilation Sucessful\n";
  return 0;
}