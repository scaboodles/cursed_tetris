#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>
#include "tetris.h"
#include <string.h>
#include <math.h>

int initialize();
int drop_tetronimo(WINDOW *, WINDOW *, int **);
void draw_grid(WINDOW *, int **);
int check_lines(int **);
void update_ui(WINDOW *, int);
void render_held(WINDOW *);
void free_grid(int **);

const int HEIGHT = 20;
const int WIDTH = 10;
const int UI_WIDTH = 9;
int color_pairs[8][8];

double speed = 1.0;

struct tetromino * held = NULL;
int switched = 0;

int main(){
    if(initialize() == 1){
        return 1;
    }

    int game_x = 0;
    int game_y = 0;

    WINDOW *border = newwin(HEIGHT / 2 +2, WIDTH+2, game_y, game_x); // Create a new window
    box(border, ACS_VLINE, ACS_HLINE); // Draw a box around the window

    refresh();
    wrefresh(border);

    WINDOW *game = newwin(HEIGHT /2 , WIDTH, game_y + 1, game_x + 1); // Create a new window

    wrefresh(game);
    refresh();

    int num_cleared = 0;

    WINDOW * lines_cleared = newwin(5, UI_WIDTH, 0, game_x + 2 + WIDTH);
    box(lines_cleared, ACS_VLINE, ACS_HLINE); 
    mvwprintw(lines_cleared, 1, 2, "lines:");
    update_ui(lines_cleared, num_cleared);

    WINDOW * held_window = newwin(7, UI_WIDTH, 5, game_x + 2 + WIDTH);
    box(held_window, ACS_VLINE, ACS_HLINE); 
    mvwprintw(held_window, 1, 2, "held:");
    render_held(held_window);

    int ** char_grid = malloc(HEIGHT * sizeof(int *));
    for(int i = 0; i < HEIGHT; i++){
        char_grid[i] = malloc(WIDTH * sizeof(int));
    }

    for(int i = 0; i < HEIGHT; i++){
        for(int j = 0; j < WIDTH; j++){
            char_grid[i][j] = 0;
        }
    }

    double k = 0.05;
    int ret;
    while((ret = drop_tetronimo(game, held_window, char_grid)) != 0){
        if(ret != 2){
            switched = 0;
        }

        render_held(held_window);
        num_cleared += check_lines(char_grid);
        update_ui(lines_cleared, num_cleared);
        speed = 1 * exp(-k * num_cleared);
    }

    attron(COLOR_PAIR(color_pairs[0][3]));
    char * game_over = "GAME OVER";
    for(int i = 0; i < strlen(game_over); i++){
        mvaddch(5, 1+i, game_over[i]);
        refresh();
        usleep(300000);
    }

    flushinp();//clear buffer
    timeout(-1);
    getch(); 

    free_grid(char_grid);
    endwin();
    return 0;
}


int count_digits(int n){
    int num = n;
    if(num == 0) {
        return 1;
    }

    int count = 0;
    while (num > 0) {
        num /= 10;
        count++;
    }

    return count;
}

void render_held(WINDOW * held_window){
    if(held != NULL){
        wchar_t * tophalf = L"\u2580\0";
        for(int i = 0; i < 4; i+=2){
            for(int j = 0; j < 4; j++){
                wattron(held_window, COLOR_PAIR(color_pairs[held->grid[i][j]][held->grid[i + 1][j]]));
                mvwaddwstr(held_window, (i / 2) + 3, j + 3, tophalf);
            }
        }
    }
    wrefresh(held_window);
    refresh();
}

void update_ui(WINDOW * ui, int num_cleared){
    //update held
    // get x pos of where new string should go
    int digit_len = count_digits(num_cleared);
    int delta = UI_WIDTH - digit_len;
    if(delta % 2 == 1){
        delta -= 1;
    }
    // clear old
    for(int i = 1; i < UI_WIDTH - 2; i++){
        mvwprintw(ui, 2, i, " ");
    }

    // print new
    mvwprintw(ui, 2, delta/2, "%d", num_cleared);
    wrefresh(ui);
    refresh();
}

struct tetromino * init_tetromino(){
    struct tetromino * omino = malloc(sizeof(struct tetromino));
    int ** grid = malloc(4 * sizeof (int *));

    for(int i = 0; i < 4; i++){
        grid[i] = malloc(4 * sizeof(int));
    }

    omino->grid = grid;
    int pick = rand() % 7;
    switch(pick){
        case 0:
            init_l_reverse(omino);
            break;
        case 1:
            init_i(omino);
            break;
        case 2:
            init_l(omino);
            break;
        case 3:
            init_square(omino);
            break;
        case 4:
            init_z(omino);
            break;
        case 5:
            init_t(omino);
            break;
        case 6:
            init_z_reverse(omino);
            break;
    }

    return omino;
}

int ** dupe_grid(int** original) {
    // Allocate memory for the new 2D array
    int** duplicate = (int**) malloc(HEIGHT * sizeof(int*));
    for (int i = 0; i < HEIGHT; i++) {
        duplicate[i] = (int*) malloc(WIDTH * sizeof(int));
        for (int j = 0; j < WIDTH; j++) {
            duplicate[i][j] = original[i][j];
        }
    }

    return duplicate;
}

void interpolate_tetromino(int ** grid, struct tetromino * piece){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(i + piece->y < HEIGHT && j + piece->x <= WIDTH && piece->grid[i][j] != 0){
                grid[i + piece->y][j + piece->x] = piece->grid[i][j];
            }
        }
    }
}

void free_tetromino(struct tetromino * piece){
    for(int i = 0; i < 4; i++){
        free(piece->grid[i]);
    }
    free(piece->grid);
    free(piece);
}

int bake_tetromino(int ** grid, struct tetromino * piece){
    interpolate_tetromino(grid, piece);
    int ret = piece->y == 0 ? 0 : 1;
    free_tetromino(piece);
    return ret;
}

int ** tetromino_to_grid(int ** grid, struct tetromino * piece){
    int ** dupe = dupe_grid(grid);
    interpolate_tetromino(dupe, piece);
    return dupe;
}

void draw_grid(WINDOW * game, int ** grid){
    wchar_t * tophalf = L"\u2580\0";
    for(int i = 0; i < HEIGHT; i+=2){
        for(int j = 0; j < WIDTH; j++){
            wattron(game, COLOR_PAIR(color_pairs[grid[i][j]][grid[i + 1][j]]));
            mvwaddwstr(game, i / 2, j, tophalf);
        }
    }

    wrefresh(game);
    refresh();
}

void free_grid(int ** grid){
    for(int i = 0; i < HEIGHT; i++){
        free(grid[i]);
    }
    free(grid);
}

int check_right(int ** grid, struct tetromino * piece){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(piece->grid[i][j] != 0){
                if(piece->x + j >= WIDTH - 1){
                    return 0;
                }else if(grid[i + piece->y][j + piece->x + 1] != 0){
                    return 0;
                }
            }
        }
    }

    return 1;
}

int check_right_loose(int ** grid, struct tetromino * piece){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(piece->grid[i][j] != 0){
                if(piece->x + j > WIDTH - 1){
                    return 0;
                }else if(grid[i + piece->y][j + piece->x + 1] != 0){
                    return 0;
                }
            }
        }
    }

    return 1;
}

int check_left(int ** grid, struct tetromino * piece){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(piece->grid[i][j] != 0){
                if(piece->x + j <= 0){
                    return 0;
                }else if(grid[i + piece->y][j + piece->x - 1] != 0){
                    return 0;
                }
            }
        }
    }

    return 1;
}

int check_left_loose(int ** grid, struct tetromino * piece){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(piece->grid[i][j] != 0){
                if(piece->x + j < 0){
                    return 0;
                }else if(grid[i + piece->y][j + piece->x - 1] != 0){
                    return 0;
                }
            }
        }
    }

    return 1;
}

int check_bottom(int ** grid, struct tetromino * piece){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(piece->grid[i][j] != 0){
                if(i + piece->y + 1 >= HEIGHT){
                    return 0;
                }else if(grid[i + piece->y + 1][j + piece->x] != 0){
                    return 0;
                }
            }
        }
    }
    return 1;
}

int check_top(int ** grid, struct tetromino * piece){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(piece->grid[i][j] != 0){
                if(piece->y + i - 1 >= 0 && grid[i + piece->y - 1][j + piece->x] != 0){
                    return 0;
                }
            }
        }
    }
    return 1;
}

int verify_rotation(int ** grid, struct tetromino * piece){
    if(check_bottom(grid, piece) == 1 && check_top(grid, piece) == 1 && check_left_loose(grid, piece) == 1 && check_right_loose(grid, piece) == 1){
        return 1;
    }else{
        return 0;
    }
}

void rotate(struct tetromino * piece, int ** grid){
    int initial_rot = piece->rotation;
    switch(piece->type){
        case L_reverse:
            rotate_l_reverse(piece, (piece->rotation + 1) % 4);
            if(verify_rotation(grid, piece) != 1){
                rotate_l_reverse(piece, initial_rot);
            }
            break;
        case L:
            rotate_l(piece, (piece->rotation + 1) % 4);
            if(verify_rotation(grid, piece) != 1){
                rotate_l(piece, initial_rot);
            }
            break;
        case SQUARE:
            break;
        case I:
            rotate_i(piece, (piece->rotation + 1) % 4);
            if(verify_rotation(grid, piece) != 1){
                rotate_i(piece, initial_rot);
            }
            break;
        case T:
            rotate_t(piece, (piece->rotation + 1) % 4);
            if(verify_rotation(grid, piece) != 1){
                rotate_t(piece, initial_rot);
            }
            break;
        case Z:
            rotate_z(piece, (piece->rotation + 1) % 4);
            if(verify_rotation(grid, piece) != 1){
                rotate_z(piece, initial_rot);
            }
            break;
        case Z_reverse:
            rotate_z_reverse(piece, (piece->rotation + 1) % 4);
            if(verify_rotation(grid, piece) != 1){
                rotate_z_reverse(piece, initial_rot);
            }
            break;
    }
}

int check_lines(int ** grid){
    int num_cleared = 0;
    for(int i = 0; i < HEIGHT; i++){
        int clear = 1;
        for(int j = 0; j < WIDTH; j++){
            if(grid[i][j] == 0){
                clear = 0;
                break;
            }
        }

        if(clear == 1){
            num_cleared ++;
            // clearline logic here => add points or somth
            for(int j = i; j > 0; j--){
                for(int k = 0; k < WIDTH; k++){
                    grid[j][k] = grid[j-1][k];
                    grid[j-1][k] = 0;
                }
            }
        }
    }
    return num_cleared;
}

int drop_tetronimo(WINDOW * game, WINDOW * held_window, int ** char_array){
    struct tetromino * piece = init_tetromino();
    int first = 1;
    while(check_bottom(char_array, piece) == 1){
        double duration = speed * 1000;
        clock_t start = clock();
        clock_t current = clock();

        if(first == 0){
            piece->y+=1;
        }else{
            first = 0;
        }

        while(((double)(current - start) / CLOCKS_PER_SEC) * 1000.0 < duration){
            current = clock();
            int skip = 0;
            int ch = getch();

            switch(ch){
                case KEY_LEFT:
                    if(check_left(char_array, piece) == 1){
                        piece->x -= 1;
                    }
                    break;
                case KEY_RIGHT:
                    if(check_right(char_array, piece) == 1){
                        piece->x += 1;
                    }
                    break;
                case KEY_DOWN:
                    skip = 1;
                    break;
                case KEY_UP:
                    rotate(piece, char_array);
                    break;
                case 'c':
                    if(switched == 0){
                        switched = 1;
                        if(held == NULL){
                            piece->y = 0;
                            piece->x = 3;
                            held = piece;
                            return 2;
                        }else{
                            struct tetromino * temp = piece;
                            temp->y = 0;
                            temp->x = 3;
                            piece = held;
                            held = temp;
                        }
                        render_held(held_window);
                    }
            };

            if(skip == 1){
                flushinp();
                break;
            }
            int ** dupe = tetromino_to_grid(char_array, piece);
            draw_grid(game, dupe);
            free_grid(dupe);
            wrefresh(game);
            refresh();
        }
        int ** dupe = tetromino_to_grid(char_array, piece);
        draw_grid(game, dupe);
        free_grid(dupe);
    }

    int ret = bake_tetromino(char_array, piece);
    draw_grid(game, char_array);
    wrefresh(game);
    refresh();
    return ret;
}

int initialize(){
    setlocale(LC_ALL, "en_US.UTF-8"); // w i d e  characters
    initscr(); cbreak(); noecho(); // curses bs 
    start_color();
    curs_set(0); // hide cursor
    timeout(0); // non blocking getch
    keypad(stdscr, TRUE); // enable arrow keys
    srand(time(NULL)); // set seed


    if(LINES <= HEIGHT / 2 || COLS <= WIDTH){
        printw("Window too small\npress any key to continue");
        refresh();
        getch();
        endwin();
        return 1;
    }


    // init color pairs
    const short colors[] = {COLOR_BLACK, COLOR_CYAN, COLOR_BLUE, COLOR_WHITE, COLOR_YELLOW, COLOR_GREEN, COLOR_MAGENTA, COLOR_RED};
    short int count = 1;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            init_pair(count, colors[i], colors[j]);
            color_pairs[i][j] = count;
            count++;
        }
    }
    return 0;
}