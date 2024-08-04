#include <stdio.h>
#include "logic.h"
#include "types.h"
#include <stdlib.h>

Game game;
void *available_actions[] = {};

void print_action_space(ActionSpace *action_space)
{
    for (short i = 0; i < action_space->length; i++)
    {
        // printf("i = %d\n", ((Piece *)action_space->action_space[i].operand1)->color);
        Action action = action_space->action_space[i];
        printf("{%d} {%s}\n", action.action, action.operand1.piece->name);
    }
    
}

int main()
{
    // Piece *piece = init_piece(RED, pieceArr, playerArr, player_pointer, winner);
    
    new_game(&game);
    // printf("Start\n\n");
    start_game(&game);
    printf("curr_player = %s\n", PLAYER_AT(&game, *game.player_pointer).color_name);

    ModularInt loc1 = modular_new(52);
    loc1.value = 7;

    // ModularInt loc2 = modular_new(52);
    ModularInt loc2 = modular_add(modular_add(PIECE_AT(&game, *game.player_pointer, 2).start_location, APPROACH_CELL), -4);

    game.board->std_path[*game.player_pointer*TOTAL_PIECES_PLAYER + 1] = 1;
    game.board->std_path[*game.player_pointer*TOTAL_PIECES_PLAYER + 1] = 1;
    game.board->std_path[*game.player_pointer*TOTAL_PIECES_PLAYER + 1] = 1;

    PIECE_AT(&game, *game.player_pointer - 1, 1).location.location = modular_add(loc1, 2);
    PIECE_AT(&game, *game.player_pointer - 1, 1).region = PATH;


    PIECE_AT(&game, *game.player_pointer, 1).location.location = loc1;
    PIECE_AT(&game, *game.player_pointer, 1).multiplier = 1;
    PIECE_AT(&game, *game.player_pointer, 1).region = PATH; 

    PIECE_AT(&game, *game.player_pointer, 2).location.location = loc2;
    PIECE_AT(&game, *game.player_pointer, 2).multiplier = 1;
    PIECE_AT(&game, *game.player_pointer, 2).region = PATH;
    PIECE_AT(&game, *game.player_pointer, 2).approach = 1; 

    PIECE_AT(&game, *game.player_pointer, 3).location.far_from_home = 4;
    PIECE_AT(&game, *game.player_pointer, 3).multiplier = 1;
    PIECE_AT(&game, *game.player_pointer, 3).region = HOME;    
    // PIECE_AT(&game, *game.player_pointer, 2).location = 2;
    // PIECE_AT(&game, *game.player_pointer, 2).multiplier = -1;
    // PIECE_AT(&game, *game.player_pointer, 3).location = HOME;
    // printf("new_location = %d\n", get_destination(&PIECE_AT(&game, *game.player_pointer, 2), 6));
    // PICECE_AT(pieceArr, player_pointer, 3)->multiplier = 1;

    // for (short i = 0; i < TOTAL_PLAYERS; i++)
    // {
    //     printf("%d\n", PIECE_AT(&game, 1, i).color);
    // }
    
    printf("Piece = %d, *Piece = %d\n", sizeof(Piece), sizeof(Piece *));
    

    ActionSpace action_space = generate_action_space(&game, *game.player_pointer, 2);
    print_action_space(&action_space);
    perform(action_space.action_space[0]);
    
    return 0;
}