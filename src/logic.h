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
#define NEWLINE printf("\n");


void init_game(Piece *pieceArr, Player *playerArr, Board *board, short *player_pointer, short *winner);

void start_game(Game *game);

void player_pointer_next(Game *game);
void player_pointer_update(Game *game, short player_id);

short roll_die(Player *player, bool verbose);
ModularInt get_destination(Piece *piece, short die_value);
short get_piece(Game *game, ModularInt destination);

// void bring_to_path(Player *player, Piece *piece, bool verbose);
// void move(Player *player, Piece *piece, ModularInt dest, bool verbose);
// void move_to_home_straight(Player *player, Piece *piece, int steps_in_home_straight, bool verbose);
// void move_in_home_straight(Player *player, Piece *piece, int steps, bool verbose);
// void capture(Player *player, Piece *piece, Player *target_player, Piece *target_piece, bool verbose);

void capture(Game *game, short player_color, short piece_idx, short target_piece_idx, bool verbose);
void move_path(Game *game, short player_color, short piece_idx, short steps, short real_destination, bool verbose);
void move_to_home_straight(Game *game, short player_color, short piece_idx, short steps, bool verbose);
void move_in_home_straight(Game *game, short player_color, short piece_idx, short steps);
void blocked(Game *game, short player_color, short piece_idx, short blocked_player_color, short block_idx, short init_destination, bool verbose);
void create_block(Game *game, short player_color, short piece_idx, short other_piece_idx);
void add_to_block(Game *game, short player_color, short piece_idx, short block_idx);
void remove_from_block(Game *game, short player_color, short piece_idx, short block_idx);
void move_block(Game *game, short block_idx, short steps);
void capture_pieces_by_block(Game *game, short player_color, short block_idx, short target_player_color, short target_piece_idx);
void capture_block_by_block(Game *game, short block_idx, short target_block);
void break_block(Game *game, short block_id);
void merge_block(Game *game, short block_id, short taret_block_id);
void land_on_mystery_cell(Game *game, short player_idx, short piece_idx);

void append_action(ActionSpace *action_space, Action action);
Action explore_actionspace_of_pieces(Game *game, int player_color, int piece_idx, int steps);
// ModularInt get_destination(Piece *piece, short die_value);

// void create_block(Piece *piece1, Piece *piece2);
// void update_block(Block *block, Piece *piece);
// void add_to_block(Block *block);
// void remove_from_block(Block *block);
// void get_blocked(Piece *piece, short step);

ActionSpace generate_action_space(Game *game, Color player_color, short die_value);
ActionSpace action_space_filter(ActionSpace action_space, Player *player, short rolled);
Action search_optimal_action(Player *player, ActionSpace action_space);
void perform(Game *game, short player_color, Action action);

void play(Game *game, short *player_pointer);
void play_round(Game *game);
bool roll_logic(Game *game, short current_player_id, short rolled);
void action_logic(Game *game, Action selected_action);

// void round(Piece **pieceArr, Player **playerArr, short player_pointer, Player *winner);


#endif