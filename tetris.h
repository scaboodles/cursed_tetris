#ifndef TETRIS_H
#define TETRIS_H

enum Tetromino_type{
    L_reverse,
    L,
    SQUARE,
    I,
    T,
    Z,
    Z_reverse
};

struct tetromino{
    int ** grid;
    enum Tetromino_type type;
    int y;
    int x;
    int rotation;
    int offset_y;
};

extern const int HEIGHT;
extern const int WIDTH;


void init_l_reverse(struct tetromino *);
void rotate_l_reverse(struct tetromino *, int);

void init_i(struct tetromino *);
void rotate_i(struct tetromino *, int);

void init_l(struct tetromino *);
void rotate_l(struct tetromino *, int);

void init_square(struct tetromino *);

void init_z(struct tetromino *);
void rotate_z(struct tetromino *, int);

void init_t(struct tetromino *);
void rotate_t(struct tetromino *, int);

void init_z_reverse(struct tetromino *);
void rotate_z_reverse(struct tetromino *, int);

#endif