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

#ifndef _STDINT_
#define _STDINT_
#include <stdint.h>
#endif


#define RSEED 42
#define FIRST_ROLL -1

#define TOTAL_PIECES_PLAYER 4
#define TOTAL_PLAYERS 4
#define TOTAL_PIECES  TOTAL_PIECES_PLAYER*TOTAL_PLAYERS
#define TOTAL_CELLs 52
#define CELLS_IN_HOME 6
#define TOO_FAR 255

// #define BASE 58
#define HOME_START 52
// #define HOME 57
#define APPROACH_CELL 0
#define STARTING_CELL 2

#define PIECE_AT(game, i, j) (game)->pieceArr[(i)*TOTAL_PIECES_PLAYER + (j)]
#define PLAYER_AT(game, i) (game)->playerArr[(i)]

#define MODULAR_IS_EQL(a, b) ((a).value == (b).value) ? true: false

typedef enum Color {YELLO, BLUE, RED, GREEN} Color;
typedef enum ActionType {BRING_TO_PATH, BLOCKED, MOVE_PATH, MOVE_TO_HOME_S, MOVE_IN_HOME_S, BREAK_BLOCK, CREAT_BLOCK, UPDATE_BLOCK, CAPTURE, MOVE_BLOCK, BLOCK_CAPTURE}ActionType;
typedef enum Region {BASE, PATH, HOME}Region;

typedef struct 
{
    short value;
    short max;
}ModularInt;

ModularInt modular_new(short value, short max);
ModularInt modular_add(ModularInt a, short b);
void modular_assign(ModularInt *a, short value);
bool modular_is_between(ModularInt lower, ModularInt upper, ModularInt a);
// short mod_value(ModularInt a);

typedef union 
{
    short far_from_home;
    ModularInt location;
}PieceLocation;

typedef struct 
{
    short no_of_captures;
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

typedef struct 
{
    short no_of_pieces;
    ModularInt location;
    Color color;
    Piece *pieces;
    bool movable;
    bool direction;
}Block;

typedef struct Player Player;
typedef struct Action Action;
typedef struct ActionSpace ActionSpace;

typedef struct Player
{
    short no_pieces_in_home;
    short no_pieces_in_path;
    short no_pieces_in_base;
    Color color;
    Piece *pieces;
    char color_name[6];
    Action (* perform_strategy)(Player *self, ActionSpace action);
    short same_rolls;
    short prev_rolled;    
}Player;

Player new_player(Color color, Piece *pieces, Action (* perform_strategy)(Player *self, ActionSpace action));

typedef union 
{
    Piece *piece;
    Player *player;
    Block *block;
    ModularInt standard_steps;
    short steps;
}Operand;

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
    short *player_pointer, *winner;
    short *starting_player;
}Game;

void new_game(Game *game);

typedef struct Action
{
    ActionType action;
    Player *player;
    Piece *piece;
    int steps;
    Operand operand1, operand2;
}Action;

typedef struct ActionSpace
{
    Action *action_space;
    short length;
}ActionSpace;

//gameplay stratagies
Action random_player(Player *player, ActionSpace action_space);


#endif