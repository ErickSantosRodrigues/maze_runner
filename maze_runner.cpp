#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>

// Representação do labirinto
using Maze = std::vector<std::vector<char>>;

// Estrutura para representar uma posição no labirinto
struct Position {
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
bool s_found = false;
std::stack<Position> valid_positions;

// Função para carregar o labirinto de um arquivo
// Retorna a posição inicial do labirinto
Position load_maze(const std::string& file_name) {
    std::ifstream file(file_name);
    Position initial_pos = {-1, -1};
    file >> num_rows >> num_cols;
    maze.resize(num_rows, std::vector<char>(num_cols));
    for(int i = 0; i < num_rows; i++) {
        for(int j = 0; j < num_cols; j++) {
            file >> maze[i][j];
            if (maze[i][j] == 'e') {
                initial_pos = {i, j};
            }
        }
    }

    file.close();
    return initial_pos;
    
    
}

// Função para imprimir o labirinto
void print_maze() {
    if (s_found)
        return;
    system("clear");
    std::cout << "\n";
    for(int i = 0; i < num_rows; i++) {
        for(int j = 0; j < num_cols; j++) {
            std::cout << maze[i][j];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}
//TODO: como diversas threads estão sendo acionadas, algumas não modificão 
// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {
    if (row >= 0 && row < num_rows && col >= 0 &&
        col >= 0 && col < num_cols && 
        (maze[row][col] == 'x' || maze[row][col] == 's')) 
        return true;
    
    return false;
}

// Função principal para navegar pelo labirinto
void walk(Position pos) {
    //clear cmd prompt
    print_maze();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    if(maze[pos.row][pos.col] == 's'){
        s_found = true;
        for(int i = 0; i < num_rows; i++) {
            for(int j = 0; j < num_cols; j++) {
                std::cout << maze[i][j];
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        return;
    }
    // algumas threads são criadas no meio tempo entre a thread de um caminho existente
    // passar naquele caminho valido e outra thead verificar se a posição é válida
    // Isso Cria uma nova thread para um caminho que já não é mais válido e pode dar corrupção de memoria
    
    // AINDA NÂO RESOLVEU PARA O maze3.txt 
    else if (maze[pos.row][pos.col] == '.')    
        return;

    maze[pos.row][pos.col] = '.';
    std::vector<Position> moves = {
        {pos.row - 1, pos.col}, {pos.row + 1, pos.col},
        {pos.row, pos.col - 1}, {pos.row, pos.col + 1}};

    for (const auto& next : moves) {
        if (is_valid_position(next.row, next.col)) {
            valid_positions.push(next);
        }
    }
    if (valid_positions.empty())
        return;

    //chamar threads para cada saida extra depois da primeira
    std::vector<std::thread> threads;
    while (!valid_positions.empty() ) {
        Position next_pos = valid_positions.top();
        valid_positions.pop();
        // continua no caminho
        if(valid_positions.size() == 0)
            walk(next_pos);
        else
            threads.emplace_back(walk, next_pos);

    }
    //
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Position initial_pos = load_maze(argv[1]);
    if (initial_pos.row == -1 || initial_pos.col == -1) {
        std::cerr << "Posicao inicial nao encontrada no labirinto." << std::endl;
        return 1;
    }


    walk(initial_pos);

    if (s_found) {
        std::cout << "Saida encontrada!" << std::endl;
    } else {
        std::cout << "Nao foi possivel encontrar a saida." << std::endl;
    }

    return 0;
}

// Nota sobre o uso de std::this_thread::sleep_for:
// 
// A função std::this_thread::sleep_for é parte da biblioteca <thread> do C++11 e posteriores.
// Ela permite que você pause a execução do thread atual por um período especificado.
// 
// Para usar std::this_thread::sleep_for, você precisa:
// 1. Incluir as bibliotecas <thread> e <chrono>
// 2. Usar o namespace std::chrono para as unidades de tempo
// 
// Exemplo de uso:
// std::this_thread::sleep_for(std::chrono::milliseconds(50));
// 
// Isso pausará a execução por 50 milissegundos.
// 
// Você pode ajustar o tempo de pausa conforme necessário para uma melhor visualização
// do processo de exploração do labirinto.
