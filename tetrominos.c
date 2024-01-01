#include "tetris.h"
// color guide: {COLOR_BLACK, COLOR_CYAN, COLOR_BLUE, COLOR_WHITE, COLOR_YELLOW, COLOR_GREEN, COLOR_MAGENTA, COLOR_RED};

// piece initialization
void reset_piece_grid(struct tetromino * omino){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            omino->grid[i][j] = 0;
        }
    }
}

void init_i(struct tetromino * omino){
    reset_piece_grid(omino);
    for(int i = 0; i < 4; i++){
        omino->grid[1][i] = 1;
    }

    omino->type = I;
    omino->y = 0;
    omino->x = 3;
    omino->rotation = 0;
}

void init_l_reverse(struct tetromino * omino){
    reset_piece_grid(omino);
    omino->grid[0][0] = 2;
    for(int i = 0; i < 3; i++){
        omino->grid[1][i] = 2;
    }

    omino->type = L_reverse;
    omino->y = 0;
    omino->x = 3;
    omino->rotation = 0;
}

void init_l(struct tetromino * omino){
    reset_piece_grid(omino);
    omino->grid[0][2] = 3;
    for(int i = 0; i < 3; i++){
        omino->grid[1][i] = 3;
    }

    omino->type = L;
    omino->y = 0;
    omino->x = 3;
    omino->rotation = 0;
}

void init_square(struct tetromino * omino){
    reset_piece_grid(omino);
    for(int i = 1; i < 3; i++){
        for(int j = 0; j < 2; j++){
            omino->grid[i][j] = 4;
        }
    }

    omino->type = SQUARE;
    omino->y = 0;
    omino->x = 3;
    omino->rotation = 0;
}

void init_z(struct tetromino * omino){
    reset_piece_grid(omino);
    for(int i = 1; i < 3; i++){
        omino->grid[0][i] = 5;
    }
    for(int i = 0; i < 2; i++){
        omino->grid[1][i] = 5;
    }

    omino->type = Z;
    omino->y = 0;
    omino->x = 3;
    omino->rotation = 0;
}

void init_t(struct tetromino * omino){
    reset_piece_grid(omino);
    omino->grid[0][1] = 6;
    for(int i = 0; i < 3; i++){
        omino->grid[1][i] = 6;
    }

    omino->type = T;
    omino->y = 0;
    omino->x = 3;
    omino->rotation = 0;
}

void init_z_reverse(struct tetromino * omino){
    reset_piece_grid(omino);
    omino->grid[0][0] = 7;
    omino->grid[0][1] = 7;
    omino->grid[1][1] = 7;
    omino->grid[1][2] = 7;

    omino->type = Z_reverse;
    omino->y = 0;
    omino->x = 3;
    omino->rotation = 0;
}

// rotation handling
void rotate_i(struct tetromino * omino, int new_rot){
    omino->rotation = new_rot;
    reset_piece_grid(omino);
    switch(omino->rotation){
        case 0: 
            for(int i = 0; i < 4; i++){
                omino->grid[1][i] = 1;
            }
            break;
        case 1:
            for(int i = 0; i < 4; i++){
                omino->grid[i][2] = 1;
            }
            break;
        case 2:
            for(int i = 0; i < 4; i++){
                omino->grid[2][i] = 1;
            }
            break;
        case 3:
            for(int i = 0; i < 4; i++){
                omino->grid[i][1] = 1;
            }
            break;
    }
}

void rotate_l(struct tetromino * omino, int new_rot){
    omino->rotation = new_rot;
    reset_piece_grid(omino);
    switch(omino->rotation){
        case 0: 
            omino->grid[0][2] = 3;
            for(int i = 0; i < 3; i++){
                omino->grid[1][i] = 3;
            }
            break;
        case 1:
            for(int i = 0; i < 3; i++){
                omino->grid[i][1] = 3;
            }
            omino->grid[2][2] = 3;
            break;
        case 2:
            for(int i = 0; i < 3; i++){
                omino->grid[1][i] = 3;
            }
            omino->grid[2][0] = 3;
            break;
        case 3:
            for(int i = 0; i < 3; i++){
                omino->grid[i][1] = 3;
            }
            omino->grid[0][0] = 3;
            break;
    }
}

void rotate_l_reverse(struct tetromino * omino, int new_rot){
    omino->rotation = new_rot;
    reset_piece_grid(omino);
    switch(omino->rotation){
        case 0: 
            omino->grid[0][0] = 2;
            for(int i = 0; i < 3; i++){
                omino->grid[1][i] = 2;
            }
            break;
        case 1:
            for(int i = 0; i < 3; i++){
                omino->grid[i][1] = 2;
            }
            omino->grid[0][2] = 2;
            break;
        case 2:
            for(int i = 0; i < 3; i++){
                omino->grid[1][i] = 2;
            }
            omino->grid[2][2] = 2;
            break;
        case 3:
            for(int i = 0; i < 3; i++){
                omino->grid[i][1] = 2;
            }
            omino->grid[2][0] = 2;
            break;
    }
}

void rotate_z(struct tetromino * omino, int new_rot){
    omino->rotation = new_rot;
    reset_piece_grid(omino);
    switch(omino->rotation){
        case 0: 
            for(int i = 1; i < 3; i++){
                omino->grid[0][i] = 5;
            }
            for(int i = 0; i < 2; i++){
                omino->grid[1][i] = 5;
            }
            break;
        case 1:
            omino->grid[0][1] = 5;
            omino->grid[1][1] = 5;
            omino->grid[1][2] = 5;
            omino->grid[2][2] = 5;
            break;
        case 2:
            omino->grid[1][1] = 5;
            omino->grid[1][2] = 5;
            omino->grid[2][0] = 5;
            omino->grid[2][1] = 5;
            break;
        case 3:
            omino->grid[0][0] = 5;
            omino->grid[1][0] = 5;
            omino->grid[1][1] = 5;
            omino->grid[2][1] = 5;
            break;
    }
}

void rotate_t(struct tetromino * omino, int new_rot){
    omino->rotation = new_rot;
    reset_piece_grid(omino);
    switch(omino->rotation){
        case 0: 
            omino->grid[0][1] = 6;
            for(int i = 0; i < 3; i++){
                omino->grid[1][i] = 6;
            }
            break;
        case 1:
            omino->grid[0][1] = 6;
            omino->grid[1][1] = 6;
            omino->grid[2][1] = 6;
            omino->grid[1][2] = 6;
            break;
        case 2:
            omino->grid[1][0] = 6;
            omino->grid[1][1] = 6;
            omino->grid[1][2] = 6;
            omino->grid[2][1] = 6;
            break;
        case 3:
            omino->grid[1][0] = 6;
            omino->grid[0][1] = 6;
            omino->grid[1][1] = 6;
            omino->grid[2][1] = 6;
            break;
    }
}

void rotate_z_reverse(struct tetromino * omino, int new_rot){
    omino->rotation = new_rot;
    reset_piece_grid(omino);
    switch(omino->rotation){
        case 0: 
            omino->grid[0][0] = 7;
            omino->grid[0][1] = 7;
            omino->grid[1][1] = 7;
            omino->grid[1][2] = 7;
            break;
        case 1:
            omino->grid[0][2] = 7;
            omino->grid[1][1] = 7;
            omino->grid[1][2] = 7;
            omino->grid[2][1] = 7;
            break;
        case 2:
            omino->grid[1][0] = 7;
            omino->grid[1][1] = 7;
            omino->grid[2][1] = 7;
            omino->grid[2][2] = 7;
            break;
        case 3:
            omino->grid[0][1] = 7;
            omino->grid[1][0] = 7;
            omino->grid[1][1] = 7;
            omino->grid[2][0] = 7;
            break;
    }
}