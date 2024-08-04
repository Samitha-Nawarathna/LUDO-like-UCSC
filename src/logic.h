#ifndef _LOGIC_H_
#define _LOGIC_H_

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

#include "types.h"

void init_game(Piece *pieceArr, Player *playerArr, Board *board, int *player_pointer, int *winner);

void start_game(Game *game);

void player_pointer_next(Game *game);
int player_pointer_update(Game *game, int player_id);

int roll_die(Player *player);
ModularInt get_destination(Piece *piece, int rolled);
Piece *get_piece(Game *game, ModularInt destination);

void move_std(Piece *piece, int steps);
void move_mistory(Piece *piece, int steps);
void move_home(Piece *piece, int steps);

void create_block(Piece *piece1, Piece *piece2);
void update_block(Block *block, Piece *piece);
void add_to_block(Block *block);
void remove_from_block(Block *block);
void get_blocked(Piece *piece, int step);

void capture(Piece *piece1, Piece *piece2);

ActionSpace generate_action_space(Game *game, int player_id, int rolled);
void roll_logic(Piece *piece);
void move_logic(Piece *piece);

void perform(Action action);
// void round(Piece **pieceArr, Player **playerArr, int player_pointer, Player *winner);


#endif