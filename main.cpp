#include <iostream>
#include <vector>
#include <string_view>
#include <map>
#include <tuple>

// definitions, data, and primitive functions

template <typename T, typename RK> std::map<RK, int> enumerate_hash(T iterator) { // RK = return key type, T = iterator type
  std::map<RK, int> en;
  int iteration{0};
  for (auto& val : iterator) { en[static_cast<RK>(val)] = iteration++; }
  return en;
}

template <typename T, typename IND> std::vector<std::tuple<IND, int>> enumerate_ve(T iterator) { // IND = indence type, T = iterator type
  int len = iterator.length();
  typedef std::tuple<IND, int> pair;
  pair en_arr[len];

  for (int inst{0}; inst < len; inst++) {
    pair p{static_cast<IND>(iterator[inst]), inst};
    en_arr[inst] = p;
  }

  return static_cast<std::vector<pair>>(en_arr);
}

template <typename T> std::vector<T> vector_rep(int repititions, T obj) { // T = repeaded object type
  T arr[repititions];
  for (T& ind : arr) {
    ind = obj;
  }
  return static_cast<std::vector<T>>(arr);
}


typedef std::vector<std::vector<int>> b88;

b88 zero_board{vector_rep<std::vector<int>>(8, vector_rep<int>(8, 0))};

std::string_view ascii_pieces{"▢♙♘♗♖♕♔♟♞♝♜♛♚"};
std::string_view start_pgn{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"};
std::string_view pieces{"_pnbrqkPNBRQK"};

auto piece_id{enumerate_hash<std::string_view, char>(pieces)};

//typedef std::tuple<std::tuple<int, int>, std::tuple<int, int>> move_type;

typedef int move_type[2][2]; // start y, x | end y, x



// chess meathods

int get_id(char piece) { return piece_id[piece]; }

int get_color(int id) { return static_cast<int>(id > 7); }

int get_type(int id) { return ((id - 1) % 6) + 1; }

b88 from_pgn(std::string_view pgn) {
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

// board meathods

class Cboard {
  public:
    Cboard() 
    b88 board;
    int turn{};
    std::vector<move_type> move_arr;
};

int main() {
  return 0;
} 