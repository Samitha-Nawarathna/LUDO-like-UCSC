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
#define DO_NOT_CARE -1
#define NULL_BLOCK -1
#define NULL_PIECE -1
#define NOT_ON_BOARD -1
#define CLOCKWISE 1
#define COUNTERCLOCKWISE -1
#define NOT_ACTIVATED -1
#define DEFAULT_MULTIPLIER 1

#define BHAWANA 9
#define KOTUWA 27
#define PITAKOTUWA 46

#define TOTAL_PIECES_PLAYER 4
#define TOTAL_PLAYERS 4
#define TOTAL_PIECES  TOTAL_PIECES_PLAYER*TOTAL_PLAYERS
#define MAX_BLOCK_COUNT TOTAL_PLAYERS*2
#define TOTAL_CELLs 52
#define CELLS_IN_HOME 6
#define TOO_FAR 255
#define YELLOW_START 2 

#define PIECE_AT(game, i, j) (game)->pieceArr[(i)*TOTAL_PIECES_PLAYER + (j)]
#define PLAYER_AT(game, i) (game)->playerArr[(i)]
#define BLOCK_AT(game, block_idx) (game)->blockArr[block_idx]
#define START_AT(player_color) modular_new(YELLOW_START + player_color*13, TOTAL_CELLs)
#define APPROACH_CELL(player_color) modular_add(START_AT((player_color)), TOTAL_CELLs - 2)

#define MODULAR_IS_EQL(a, b) ((a).value == (b).value) ? true: false

typedef enum Color {YELLO, BLUE, RED, GREEN} Color;
typedef enum ActionType {NULL_ACTION = -1, BRING_TO_PATH, BLOCKED, MOVE_PATH, MOVE_TO_HOME_S, MOVE_IN_HOME_S, CREAT_BLOCK, ADD_TO_BLOCK, REMOVE_FROM_BLOCK, CAPTURE, MOVE_BLOCK, CAPTURE_PIECE_BY_BLOCK, CAPTURE_BLOCK_BY_BLOCK, MERGE_BLOCK, BREAK_BLOCK, LAND_ON_MYSTORY_CELL}ActionType;
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
bool modular_is_between_directional(ModularInt lower, ModularInt upper, ModularInt c, bool direction);
ModularInt MODULAR_SUB(ModularInt a, ModularInt b, short direction);



typedef union 
{
    short dist_from_home;
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
    short block_id;
    short rounds_affected;


}Piece;

Piece new_piece(Color color);




typedef struct Game Game;

typedef struct 
{
    short no_of_pieces;
    ModularInt location;
    Color color;
    bool *pieces;
    short multiplier;
    bool movable;
    bool direction;
}Block;

short new_block(Game *game, Color player_color, short piece1_idx, short piece2_idx);
void append_to_block(Game *game, short player_color, short piece_idx, short block_idx);
void _remove_from_block(Game *game, short player_color, short piece_idx);




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



typedef struct Game
{
    Player *playerArr;
    Piece *pieceArr;
    Block **blockArr;
    Board *board;
    short *player_pointer, *winner;
    short *starting_player;
    ModularInt *mystery_cell;

}Game;




void new_game(Game *game);

typedef struct Action
{
    ActionType action;
    short input1, input2, input3;
}Action;



typedef struct ActionSpace
{
    Action *action_space;
    short length;
}ActionSpace;



ActionSpace new_actionspace();
void append(ActionSpace actionspace, Action action);



//gameplay stratagies
Action random_player(Player *player, ActionSpace action_space);


#endif