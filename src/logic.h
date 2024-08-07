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

void init_game(Piece *pieceArr, Player *playerArr, Board *board, short *player_pointer, short *winner);

void start_game(Game *game);

void player_pointer_next(Game *game);
void player_pointer_update(Game *game, short player_id);

short roll_die(Player *player, bool verbose);
ModularInt get_destination(Piece *piece, short rolled);
Piece *get_piece(Game *game, ModularInt destination);

void move_std(Piece *piece, short steps);
void move_mistory(Piece *piece, short steps);
void move_home(Piece *piece, short steps);

void create_block(Piece *piece1, Piece *piece2);
void update_block(Block *block, Piece *piece);
void add_to_block(Block *block);
void remove_from_block(Block *block);
void get_blocked(Piece *piece, short step);

void capture(Player *player, Piece *piece1, Piece *piece2);

ActionSpace generate_action_space(Game *game, short player_id, short rolled);
ActionSpace action_space_filter(ActionSpace action_space, Player *player, short rolled);
Action search_optimal_action(Player *player, ActionSpace action_space);
void perform(Player *player, Action action);

void play(Game *game, short *player_pointer);
void play_round(Game *game);
bool roll_logic(Game *game, short current_player_id, short rolled);
void action_logic(Game *game, Action selected_action);

// void round(Piece **pieceArr, Player **playerArr, short player_pointer, Player *winner);


#endif