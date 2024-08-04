#ifndef _TYPES_H_
#define _TYPES_H_

#ifndef _STDIO_
#define _STDIO_
#include <stdio.h>
#endif

#ifndef _STDLIB_
#define _STDLIB_
#include <stdlib.h>
#endif

#ifndef _STDBOOL_
#define _STDBOOL_
#include <stdbool.h>
#endif

#ifndef _ASSERT_
#define _ASSERT_
#include <assert.h>
#endif

#ifndef _STRING_
#define _STRING_
#include <string.h>
#endif



#define RSEED 42

#define TOTAL_PIECES_PLAYER 4
#define TOTAL_PLAYERS 4
#define TOTAL_PIECES  TOTAL_PIECES_PLAYER*TOTAL_PLAYERS
#define TOTAL_CELLs 52
#define CELLS_IN_HOME 6
#define TOO_FAR 999

// #define BASE 58
#define HOME_START 52
// #define HOME 57
#define APPROACH_CELL 50

#define PIECE_AT(game, i, j) (game)->pieceArr[(i)*TOTAL_PIECES_PLAYER + (j)]
#define PLAYER_AT(game, i) (game)->playerArr[(i)]

#define MODULAR_IS_EQL(a, b) ((a).value == (b).value) ? true: false

typedef enum Color {YELLO, BLUE, RED, GREEN} Color;
typedef enum ActionType {BRING_TO_PATH, MOVE_PATH, MOVE_TO_HOME_S, MOVE_IN_HOME_S, CAPTURE}ActionType;
typedef enum Region {BASE, PATH, HOME}Region;

typedef struct 
{
    int value;
    int max;
}ModularInt;

ModularInt modular_new(int max);
ModularInt modular_add(ModularInt a, int b);
void modular_assign(ModularInt *a, int value);
bool modular_is_between(ModularInt lower, ModularInt upper, ModularInt a);
// int mod_value(ModularInt a);

typedef union 
{
    int far_from_home;
    ModularInt location;
}PieceLocation;

typedef struct 
{
    int no_of_captures;
    char name[3];
    char color_name[6];    
    Region region;
    PieceLocation location;
    ModularInt start_location;
    Color color;
    short direction;    
    bool approach;
    float multiplier;
}Piece;

Piece new_piece(Color color);


typedef union 
{
    Piece *piece;
    short rolled;
}Operand;


typedef struct 
{
    int no_of_pieces;
    ModularInt location;
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
    char color_name[6];
}Player;

Player new_player(Color color, Piece *pieces);

typedef struct 
{
    bool *std_path;
}Board;

Board new_board();


typedef struct
{
    Player *playerArr;
    Piece *pieceArr;
    Board *board;
    int *player_pointer, *winner;
}Game;

void new_game(Game *game);

typedef struct 
{
    ActionType action;
    Operand operand1, operand2;
}Action;

typedef struct 
{
    Action *action_space;
    int length;
}ActionSpace;




#endif