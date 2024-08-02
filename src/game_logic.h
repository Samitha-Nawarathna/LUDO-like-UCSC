#ifndef _GAME_LOGIC_
#define _GAME_LOGIC_

#include "types.h"


#define PICECE_AT(a, i, j) (a)[(i)*TOTAL_PIECES_PLAYER + (j)]

void init_game();

void select_player();

void player_pointer_next(Player *player_pointer);
void player_pointer_update(Player *player_pointer);

int roll_die(Player *player);

void move_std(Piece *piece, int steps);
void move_mistory(Piece *piece, int steps);
void move_home(Piece *piece, int steps);

void create_block(Piece *piece1, Piece *piece2);
void update_block(Block *block, Piece *piece);
void add_to_block(Block *block);
void remove_from_block(Block *block);
void get_blocked(Piece *piece, int step);

void capture(Piece *piece1, Piece *piece2);

ActionSpace generate_action_space(Player player);
void roll_logic(Piece *piece);
void move_logic(Piece *piece);

#endif