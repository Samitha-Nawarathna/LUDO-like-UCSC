#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define RSEED 42

#define TOTAL_PIECES_PLAYER 4
#define TOTAL_PLAYERS 4
#define TOTAL_PIECES  TOTAL_PIECES_PLAYER*TOTAL_PLAYERS
#define TOTAL_CELLs 52

#define BASE -1
#define HOME -2

// typedef struct 
// {
//     void **es;
//     int capacity;
//     int size;
// }ArrayList;

// void arraylist_alloc(int capacity);
// void arraylist_add(ArrayList *a, void *element);
// void arraylist_remove(ArrayList *a, void *element);
// void arraylist_at(ArrayList *a, int idx);
// void arraylist_resize(ArrayList *a, int size);

typedef enum Color {RED, GREEN, BLUE, YELLO} Color;

typedef struct 
{
    int no_of_captures;
    int location;
    Color color;
    bool direction;    
    bool approach;
    bool movable;
    bool energize;
}Piece;

Piece *init_piece(Color color);

typedef struct 
{
    int no_of_pieces;
    Color color;
    Piece *pieces;
    bool movable;
    bool direction;
}Block;

typedef struct 
{
    int no_in_home_pieces;
    Color color;
    Piece *pieces;
}Player;

Player *init_player(Color color, Piece *pieces);

typedef struct 
{
    bool *std_path;
}Board;

Board *init_board();

typedef struct 
{
    //HERE
}Action;


typedef struct 
{
    int no_of_action;
    Action *actions;
}ActionSpace;


Piece *pieceArr[TOTAL_PLAYERS*TOTAL_PIECES_PLAYER];
Player *playerArr[TOTAL_PLAYERS];

Player *player_pointer = NULL;
Player *winner = NULL;

#endif