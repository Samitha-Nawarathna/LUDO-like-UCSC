#include <stdio.h>
#include <stdlib.h>
#include "types.h"

// void arraylist_alloc(int capacity)
// {
//     ArrayList a;
//     a.capacity = capacity;
//     a.size = 0;
//     a.es = malloc(sizeof(void *)*capacity);
// }

// void arraylist_add(ArrayList *a, void *element);


Piece *init_piece(Color color)
{
    Piece *piece = (Piece *)malloc(sizeof(Piece));
    piece->no_of_captures = 0;
    piece->energize = false;
    piece->movable = true;
    piece->location = BASE;
    piece->approach = false;
    piece->color = color;

    return piece; 
}

Player *init_player(Color color, Piece *pieces)
{
    Player *player = (Player *)malloc(sizeof(Player));
    player->color = color;
    player->no_in_home_pieces = 0;
    player->pieces = pieces;

    return player; 
}

Board *init_board()
{
    Board *board = (Board *)malloc(sizeof(Board));
    board->std_path = (bool *)malloc(sizeof(bool)*TOTAL_PIECES);

    return board;
}