#include <stdio.h>
#include "logic.h"
#include "types.h"
#include <stdlib.h>
#include <time.h>

/*
BUGS:
 * no way to distingush the move after blocking from just moving?
    #create msg to say whether it is blocked or not but then 3 channels are not enugh
    

*/

/*
TODO:
    *perform action
        #all the things upto pieces are done 
    *debug blocks action searchs
        #done
    *change piece functions to handle base
        #done
    *debug block actions and capture function of  pieces
        #capture done other remains
    *come up with a way to handle counting in mystery cell things ?  
        #have an idea  
*/


Game game;

void print_action_space(ActionSpace action_space)
{
    for (short i = 0; i < action_space.length; i++)
    {
        printf("{%d}, {%d}, {%d}, {%d}\n", action_space.action_space[i].action, action_space.action_space[i].input1, action_space.action_space[i].input2, action_space.action_space[i].input3);
    }
    
}

void print_player(Player *player)
{
    printf("\t%s\n", player->color_name);
    printf("========================================\n");
    printf("no 0f pieces in home: %d\n", player->no_pieces_in_home);
    printf("no 0f pieces in base: %d\n", player->no_pieces_in_base);
    printf("no 0f pieces in path: %d\n", player->no_pieces_in_path);
    printf("Same rolls : %d\n", player->same_rolls);
    printf("Same rolls : %d\n", player->prev_rolled);
    NEWLINE;
}

void print_piece(Piece *piece)
{
    printf("\t%s\n", piece->color_name);
    printf("========================================\n");
    printf("region :%d\n", piece->region);
    printf("location : %d\n", piece->location.location.value);
    printf("direction : %d\n", piece->direction);
    printf("approach: %d\n", piece->approach);
    printf("multiplier : %f\n", piece->multiplier);
    printf("block_id : %d\n", piece->block_id);
    NEWLINE;
}


void print_block(Block *block)
{
    printf("\t%d\n", block->color);
    printf("========================================\n");
    printf("location : %d\n", block->location.value);
    printf("direction : %d\n", block->direction);
    printf("movable: %d\n", block->movable);
    printf("multiplier : %f\n", block->multiplier);
    NEWLINE;
}

short main()
{
    srand(time(NULL));
    new_game(&game);
    start_game(&game);

    *game.player_pointer = GREEN;
    Piece *op_piece1 = &PIECE_AT(&game,GREEN, 0);
    op_piece1->region = PATH;
    op_piece1->approach = 0;
    op_piece1->location.location = modular_add(START_AT(GREEN), 5);
    op_piece1->block_id = NULL_BLOCK;

    short block1_id = new_block(&game, GREEN, 0, 1);
    print_block(BLOCK_AT(&game, block1_id));
    BLOCK_AT(&game, block1_id)->movable = 1;


    Piece *op_piece2 = &PIECE_AT(&game,GREEN, 2);
    op_piece2->region = PATH;
    op_piece2->location.location = modular_add(START_AT(GREEN), 7);
    op_piece2->block_id = NULL_BLOCK;

    // short block2_id = new_block(&game, GREEN, 2, 3);
    // print_block(BLOCK_AT(&game, block2_id));
    // BLOCK_AT(&game, block2_id)->movable = 1;
    // print_piece(op_piece2);

    ActionSpace action_space = generate_action_space(&game, *game.player_pointer, 4);
    
    print_action_space(action_space);
    Action selected_action = action_space.action_space[0];
    printf("selected action: %d\n", selected_action.action);
    perform(&game, GREEN, selected_action);

   return 0;
}