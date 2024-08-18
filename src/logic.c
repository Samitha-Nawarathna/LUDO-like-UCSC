#include "logic.h"
#include <stdlib.h>

#define RAND(min, max) rand()%(max) + (min)



void start_game(Game *game)
{
    short value[4];
    short player_ids[4] = {0, 1, 2, 3};

    for (short i = 0; i < TOTAL_PLAYERS; i++)
    {
        value[i] = roll_die(&PLAYER_AT(game, i), false);
        // printf("%d -> %d\n",i, value[i]);
    }

    for (short i = 0; i < 2; i++)
    {
        for (short j = 0; j < TOTAL_PLAYERS - 1; j++)
        {
            if (value[j] > value[j+1])
            {
                short temp = player_ids[j];
                player_ids[j] = player_ids[j+1];
                player_ids[j+1] = temp;

                temp = value[j];
                value[j] = value[j+1];
                value[j+1] = temp;                
            }
        }
    }

    if (value[player_ids[TOTAL_PLAYERS - 1]] != value[player_ids[TOTAL_PLAYERS - 2]])
    {
        *game->player_pointer = player_ids[TOTAL_PLAYERS - 1];
        *game->starting_player = player_ids[TOTAL_PLAYERS - 1];

        
        for (short i = 0; i < TOTAL_PLAYERS; i++)
        {
            printf("%s rolls %d\n", PLAYER_AT(game, player_ids[i]).color_name, value[i]);
        }
        printf("\n%s has the highest roll and will begin the game\n", PLAYER_AT(game, player_ids[TOTAL_PLAYERS - 1]).color_name);
        printf("The order of a single round is %s", PLAYER_AT(game, *game->player_pointer).color_name);

        for (short i = 0; i < TOTAL_PLAYERS - 1; i++)
        {
            player_pointer_next(game);
            printf(", %s", PLAYER_AT(game, *game->player_pointer).color_name);
        }
        player_pointer_next(game);
        printf("\n\n");
        
        
    }else{
        start_game(game);
    }
        
}



short roll_die(Player *player, bool verbose)
{
    short value = RAND(1, 6);
    
    if (!player->prev_rolled == FIRST_ROLL)
    {
        if (player->prev_rolled == value)
        {
            player->same_rolls++;
        }
        else
        {
            player->same_rolls = 0;
        }
    }
    
    player->prev_rolled = value;
    if (verbose)
    {
        printf("%s player rolled %d\n", player->color_name, value);
    }    

    return value;
}



void player_pointer_next(Game *game)
{
    *game->player_pointer += 1;
    *game->player_pointer = *game->player_pointer%4;
}



void player_pointer_update(Game *game, short player_id)
{
    assert (player_id < 4);

    *game->player_pointer = player_id;
}



void append_action(ActionSpace *action_space, Action action)
{
    assert (action_space->length <= TOTAL_PIECES);

    action_space->action_space[action_space->length] = action;
    action_space->length++;
}



short get_steps_in_home(Piece *piece, short die_value)
{
    short steps = die_value*piece->direction*piece->multiplier;
    ModularInt destination = modular_add(piece->location.location, steps);
    // printf("steps can be move: %d\n", steps);
    if (!piece->approach)
    {
        return 0;
    }
    

    if (piece->direction == 1)
    {
        if (!modular_is_between(piece->location.location, destination, APPROACH_CELL(piece->color)))
        {
            return 0;
        }

    }else if(piece->direction == -1)
    {
        if (!modular_is_between(destination, piece->location.location, APPROACH_CELL(piece->color)))
        {
            return 0;
        }
    }
    return MODULAR_SUB(destination, APPROACH_CELL(piece->color), piece->direction).value;
    
}

short find_nearset_block(Game *game, int player_color, ModularInt current_location, ModularInt destination, short direction)
{
    short nearset_block_id = NULL_BLOCK;
    // Piece piece = PIECE_AT(game, player_color, piece_idx);
    short min_dist = 255;
    
    for (size_t block_idx = 0; block_idx < MAX_BLOCK_COUNT; block_idx++)
    {
        Block *block = BLOCK_AT(game, block_idx);
        
        if(block != NULL)
        {
            if (block->color == player_color)
            {
                continue;
            }
            

            if (direction == 1)
            {
                if (!modular_is_between(current_location, destination, block->location))
                {
                    continue;
                }
            }else if(direction == -1)
            {
                if (!modular_is_between(destination, current_location, block->location))
                {
                    continue;
                }
            }


            short dist = MODULAR_SUB(block->location, current_location, direction).value;

            if (min_dist > dist)
            {
                //printf("");
                min_dist = dist;
                nearset_block_id = block_idx;
            }
        }

    }

    return nearset_block_id;    
}

short get_piece(Game *game, ModularInt destination)
{
    for (short piece_idx = 0; piece_idx < TOTAL_PIECES; piece_idx++)
    {
        Piece piece = game->pieceArr[piece_idx];
        if (piece.region == PATH && MODULAR_IS_EQL(destination, piece.location.location))
        {
            return piece_idx;
        }
   
    }
    return NULL_PIECE;
    
}

Action explore_actionspace_of_pieces(Game *game, int player_color, int piece_idx, int steps)
{   
    Piece piece = PIECE_AT(game, player_color, piece_idx);
    ModularInt current_location = piece.location.location;
    ModularInt destination = modular_add(current_location, piece.direction*steps);
    int original_destination = destination.value;

    short block_idx = find_nearset_block(game, player_color, modular_add(current_location, -1), modular_add(destination, 1), piece.direction);

    if (block_idx != NULL_BLOCK)
    {
        Block *block = BLOCK_AT(game, block_idx);

        if (MODULAR_IS_EQL(piece.location.location, modular_add(block->location, -1)))
        {
            Action action = {NULL_ACTION, DO_NOT_CARE, DO_NOT_CARE, DO_NOT_CARE}; 
            return action;
        }

        if (MODULAR_IS_EQL(destination, block->location))
        {
            Action action = {BLOCKED, piece_idx, block_idx, destination.value};
            return action;
        }
        steps = MODULAR_SUB(block->location, piece.location.location, piece.direction).value -1;
    }
    
    destination = modular_add(current_location, piece.direction*steps);
    short target_piece_idx = get_piece(game, destination);
    // printf("where piece going to land:  %d\n", destination.value);

    if (target_piece_idx == NULL_PIECE)
    {
        if (MODULAR_IS_EQL(*game->mystery_cell, destination))
        {
            Action action = {LAND_ON_MYSTORY_CELL, piece_idx, DO_NOT_CARE, DO_NOT_CARE};
            return action;
        }
        

        Action action = {MOVE_PATH, piece_idx, steps, original_destination}; 
        return action;
    }

    Piece target_piece = game->pieceArr[target_piece_idx];
    
    if (target_piece.color == piece.color)
    {
        if (target_piece.block_id == NULL_BLOCK)
        {
            // printf("TT");
            target_piece_idx = target_piece_idx - player_color*TOTAL_PIECES_PLAYER;
            Action action = {CREAT_BLOCK, piece_idx, target_piece_idx, DO_NOT_CARE};
            return action;
        }else{

            Action action = {ADD_TO_BLOCK, piece_idx, target_piece.block_id, DO_NOT_CARE};
            return action;

        }
        
    }else{

        Action action = {CAPTURE, piece_idx, target_piece_idx, DO_NOT_CARE};
        return action;
    }
    

}

bool contains_approachable_pieces(Game *game, short block_idx)
{
    Block *block = BLOCK_AT(game, block_idx);

    for (short piece_idx_rel = 0; piece_idx_rel < TOTAL_PIECES_PLAYER; piece_idx_rel++)
    {
        if (!block->pieces[piece_idx_rel])
        {
            continue;
        }

        Piece *piece = &PIECE_AT(game, block->color, piece_idx_rel);

        if (piece->approach && block->direction == piece->direction)
        {
            return true;
        }
        
    }

    return false;
    
}


Action identify_action(Game *game, short player_color, short piece_idx, short die_value)
{
    Player player = PLAYER_AT(game, player_color);

}

bool block_between_start_and_end(Game *game, short player_color, short piece_idx)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    
    for (short block_idx = 0; block_idx < MAX_BLOCK_COUNT; block_idx++)
    {
        if (BLOCK_AT(game, block_idx) == NULL)
        {
            continue;
        }
        if(modular_is_between_directional(piece->location.location, modular_add(APPROACH_CELL(player_color), 1), BLOCK_AT(game, block_idx)->location, piece->direction))
        {
            return 1;
        }
        
    }
    return 0;
}


ActionSpace generate_action_space(Game *game, Color player_color, short die_value)
{
    ActionSpace action_space = new_actionspace();
    Player player = PLAYER_AT(game, player_color);

    for (short piece_idx = 0; piece_idx < TOTAL_PIECES_PLAYER; piece_idx++)
    {
        Piece piece = PIECE_AT(game, player_color, piece_idx);
        Region region = piece.region;

        if (piece.multiplier == 0)
        {
            continue;
        }
        

        if (region == HOME)
        {   
            int distance_to_home = piece.location.dist_from_home;
            if (distance_to_home == 0)
            {
                continue;

            }else{
                
                int steps = die_value*piece.multiplier;
                if (distance_to_home < steps || steps == 0)
                {
                    continue;

                }else{
                    
                    Action action = {
                        MOVE_IN_HOME_S,
                        piece_idx,
                        steps,
                        DO_NOT_CARE
                    };

                    append_action(&action_space, action);
                }

            }
            
        }else if(region == BASE)
        {
            if (die_value != 6)
            {
                continue;

            }
            else
            {
                int steps = 1;
                Action action = explore_actionspace_of_pieces(game, player_color, piece_idx, steps);

                if (action.action != NULL_ACTION)
                {
                    if (action.action == MOVE_PATH)
                    {
                        action.action = BRING_TO_PATH;
                    }

                    append_action(&action_space, action);
                }else{
                    continue;
                }

            }
            
        }else if(region == PATH){
            
            int steps = piece.multiplier*die_value;

            if (steps == 0)
            {
                continue;
            }
            
            short steps_in_home = get_steps_in_home(&piece, die_value);
            // printf("steps_in_home %d\n", steps_in_home);
            if (steps_in_home)
            {
                steps = steps - steps_in_home;
                Action action;

                if (!block_between_start_and_end(game, player_color, piece_idx))
                {
                    printf("\n\nFUCK!\n\n");
                    action.action = MOVE_TO_HOME_S;
                    action.input1 = piece_idx;
                    action.input2 = steps_in_home;
                    
                }else{
                    action = explore_actionspace_of_pieces(game, player_color, piece_idx, steps + 1);
                }

                if (action.action == NULL_ACTION)
                {
                    continue;
                }

                append_action(&action_space, action);


            }else{
                
                Action action = explore_actionspace_of_pieces(game, player_color, piece_idx, steps);
                if (action.action == NULL_ACTION)
                {
                    continue;
                }

                append_action(&action_space, action);
            }

        }
    }

    for (short block_id = 0; block_id < MAX_BLOCK_COUNT; block_id++)
    {
        
        Block *block = BLOCK_AT(game, block_id);
        
        
        if (block != NULL && block->movable)
        {
        
            if (block->color != player_color)
            {
                continue;
            }

            

            short steps = block->multiplier*die_value / block->no_of_pieces;
            ModularInt destination = modular_add(block->location, block->direction*steps);



            if (!steps)
            {
                continue;
            }


            bool pass_the_approach_cell = modular_is_between_directional(block->location, destination, APPROACH_CELL(block->color) , block->direction);
            bool have_approachable_pieces = contains_approachable_pieces(game, block_id);

            if (pass_the_approach_cell && have_approachable_pieces)
            {
                continue;
            }

            

            short piece_at_destination_idx = get_piece(game, destination);
            Piece *piece_at_destination = &game->pieceArr[piece_at_destination_idx];

            if (piece_at_destination_idx == NULL_PIECE)
            {
                Action action = {MOVE_BLOCK, block_id, steps, DO_NOT_CARE, DO_NOT_CARE};
                append_action(&action_space, action); 
                continue;       
            }

            

            if (piece_at_destination->color == block->color)
            {
                if (piece_at_destination->block_id == NULL_BLOCK)
                {
                    Action action = {ADD_TO_BLOCK, piece_at_destination_idx, block_id, DO_NOT_CARE};
                    append_action(&action_space, action);
                    continue;
                }

                

                Action action = {MERGE_BLOCK,block_id, piece_at_destination->block_id, DO_NOT_CARE, true};
                append_action(&action_space, action);
                
            }



            if (piece_at_destination->color != block->color)
            {
                Action action = {CAPTURE_BLOCK_BY_BLOCK, block_id, piece_at_destination->block_id};
                append_action(&action_space, action);                
                continue;
            }
                 
        }
          
    }

    return action_space;   
}



void bring_to_path(Game *game, short player_color, short piece_idx, bool verbose)
{
    Player *player = &PLAYER_AT(game, player_color);
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    
    assert (piece->region == BASE);

    player->no_pieces_in_base--;
    player->no_pieces_in_path++;

    piece->location.location = START_AT(player_color);
    piece->region = PATH;

    if (verbose)
    {
        printf("%s player moves piece %s to starting point.\n", piece->color_name, piece->name);
        printf("%s player now has %d/4 pieces on the board and %d/4 pieces on the base.\n", piece->color_name, player->no_pieces_in_path, player->no_pieces_in_base);
    }
    
}




void capture(Game *game, short player_color, short piece_idx, short target_piece_idx, bool remove_from_block, bool verbose)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    Player *player = &PLAYER_AT(game, player_color);
    Piece *target_piece = game->pieceArr + target_piece_idx;
    Player *target_player = &PLAYER_AT(game, target_piece->color);

    if (piece->region == BASE)
    {
        bring_to_path(game, player_color, piece_idx, true);
    }
    // }else if (target_piece == BASE)
    // {
    //     bring_to_path(game, player_color, target_piece_idx, true);
    // } 
    
    assert (player_color != target_piece->color);
    assert (target_piece->region == PATH);

    piece->location = target_piece->location;
    piece->no_of_captures++;
    piece->approach = true;
    piece->region == PATH;

    target_piece->no_of_captures = 0;
    target_piece->approach = false;
    target_piece->direction = 1;
    target_piece->multiplier = 1;
    target_piece->location.location = modular_add(START_AT(player_color), -1);
    target_piece->region = BASE;

    target_player->no_pieces_in_base++;
    target_player->no_pieces_in_path--;

    if (remove_from_block && piece->block_id != NULL_BLOCK)
    {
        _remove_from_block(game, player_color, piece_idx);
    }    

    if(verbose)
    {
        printf("%s piece %s lands on squre %d, captures %s piece %s, and returns to the base.\n", player->color_name, piece->name, piece->location.location.value, target_piece->name, target_player->color_name);
        printf("%s player now has %d/4 pieces on the board and %d/4 pieces on the base.\n", target_piece->color_name, target_player->no_pieces_in_path, target_player->no_pieces_in_base);

    }

}



void move_path(Game *game, short player_color, short piece_idx, short steps, short real_destination_value, bool remove_from_block, bool verbose)
{
    Player *player = &PLAYER_AT(game, player_color);
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);

    ModularInt destination = modular_add(piece->location.location, piece->direction*steps);
    short current_location = piece->location.location.value;
    char direction[11] = " ";
    switch (piece->direction)
    {
    case COUNTERCLOCKWISE:
        strcpy(direction , " counter-");
        break;
    }


    piece->location.location = destination;
    piece->region == PATH;

    // if (real_destination_value != DO_NOT_CARE)
    // {
    //     blocked(game, piece_idx,  )
    // }

    if (remove_from_block && piece->block_id != NULL_BLOCK)
    {
        _remove_from_block(game, player_color, piece_idx);
    }
    
    

    if (verbose)
    {
        printf("%s moves piece %s from location %d to %d by %d units in%sclockwise direction.\n", 
        piece->color_name,
        piece->name,
        current_location,
        destination.value,
        steps,
        direction);
    }

}



void move_to_home_straight(Game *game, short player_color, short piece_idx, short steps, bool remove_from_block, bool verbose)
{
    Player *player = &PLAYER_AT(game, player_color);
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    
    assert (piece->region == PATH);
    assert (piece->approach == true);
    assert (steps < 7);

    ModularInt current_location = piece->location.location;

    piece->location.dist_from_home = CELLS_IN_HOME - steps;
    piece->region = HOME;
    player->no_pieces_in_path--;
    player->no_pieces_in_home++;
    
    
    char direction[8] = " ";
    switch (piece->direction)
    {
    case COUNTERCLOCKWISE:
        strcpy(direction , " counter-");
        break;
    }

    if (remove_from_block && piece->block_id != NULL_BLOCK)
    {
        _remove_from_block(game, player_color, piece_idx);
    }    

    if (verbose)
    {
        printf("%s moves piece %s from location %d to %s homepath %d in%sclockwise direction.\n", 
        piece->color_name,
        piece->name,
        current_location.value,
        piece->color_name,
        piece->location.dist_from_home,
        direction);
    }

}



void move_in_home_straight(Game *game, short player_color, short piece_idx, short steps)
{
    Player *player = &PLAYER_AT(game, player_color);
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    
    assert (piece->region == HOME);
    assert (piece->location.dist_from_home > 0);
    assert (piece->location.dist_from_home >= steps);

    piece->location.dist_from_home == CELLS_IN_HOME - steps;
    
}



void blocked(Game *game, short player_color, short piece_idx, short blocked_player_color, short block_idx, short init_destination, bool verbose)
{
    Player *player = &PLAYER_AT(game, player_color);
    Player *blocked_player = &PLAYER_AT(game, blocked_player_color);
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    Block *block = BLOCK_AT(game, block_idx);

    assert(piece->color != block->color);
    assert(piece->region == PATH);

    if(verbose)
    {
        printf("%s does not have other pieces in the board to move instead of the blocked piece.\n Ignoring the throw and moving on to the next player.\n", player->color_name);

    }
}



short append_block(Game *game, Block *block)
{

    for (size_t block_idx = 0; block_idx < MAX_BLOCK_COUNT; block_idx++)
    {
        if (BLOCK_AT(game, block_idx) == NULL)
        {
            BLOCK_AT(game, block_idx) = block;
            return block_idx;
        }
        
    }

    return NULL_BLOCK;
    
}



void create_block(Game *game, short player_color, short piece_idx, short other_piece_idx, bool remove_from_block, bool verbose)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    Piece *other_piece = &PIECE_AT(game, player_color, other_piece_idx);
    ModularInt destination = piece->location.location;

    if (piece->region == BASE)
    {
        bring_to_path(game, player_color, piece_idx, true);
    }else if (other_piece->region == BASE)
    {
        bring_to_path(game, player_color, other_piece_idx, true);
    }

    new_block(game, player_color, piece_idx, piece_idx);

    if (verbose)
    {
        printf("%s creates a block by moving %s piece and %s piece into location %d\n",
            piece->color_name,
            piece->name,
            other_piece->name,
            destination.value
        );
    }   
    
}



void add_to_block(Game *game, short player_color, short piece_idx, short block_idx, bool remove_from_block, bool verbose)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);

    if (piece->region == BASE)
    {
        bring_to_path(game, player_color, piece_idx, true);
    }

    append_to_block(game, player_color, piece_idx, block_idx);

    if(verbose)
    {
        printf("%s player's piece %s merge with %s block at location %d.\n",
            piece->color_name,
            piece->name,
            piece->color_name,
            piece->color_name,
            BLOCK_AT(game, block_idx)->location.value

        );
    }
}



void remove_from_block(Game *game, short player_color, short piece_idx, short block_idx, bool verbose)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    if (piece->block_id != NULL_BLOCK)
    {
        _remove_from_block(game, player_color, piece_idx);

        if (verbose)
        {
            printf("%s piece %s removed from block!");
        }
    }
    
}



void move_block(Game *game, short block_idx, short steps)
{
    Block *block = BLOCK_AT(game, block_idx);
    block->location = modular_add(block->location, steps*block->direction);
    for (short piece_idx = 0; piece_idx < TOTAL_PIECES_PLAYER; piece_idx++)
    {
        if (block->pieces[piece_idx])
        {
            move_path(game, block->color, piece_idx, steps, DO_NOT_CARE, false, false);
        }
        
    }
    
}



void capture_pieces_by_block(Game *game, short player_color, short block_idx, short target_player_color, short target_piece_idx)
{
    // Player *player = &PLAYER_AT(game, player_color);
    Player *target_player = &PLAYER_AT(game, target_player_color);
    Piece *target_piece = &PIECE_AT(game, player_color, target_piece_idx);
    Block *block = BLOCK_AT(game, block_idx);
    
    assert (block->color != target_piece->color);
    assert (target_piece->region == PATH);

    block->location = target_piece->location.location;
    
    for (short piece_idx = 0; piece_idx < TOTAL_PIECES_PLAYER; piece_idx++)
    {
        if (block->pieces[piece_idx])
        {
            Piece *piece = &PIECE_AT(game, player_color, piece_idx);
            piece->no_of_captures++;
        }
        
    }
    
    target_piece->no_of_captures++;

    target_piece->no_of_captures = 0;
    target_piece->approach = false;
    target_piece->direction = 1;
    target_piece->multiplier = 1;

    target_player->no_pieces_in_base++;
    target_player->no_pieces_in_path--;

}



void capture_block_by_block(Game *game, short block_idx, short target_block_idx, bool verbose)
{
    Block *block = BLOCK_AT(game, block_idx);
    Block *target_block = BLOCK_AT(game, target_block_idx);

    for (short piece_idx = 0; piece_idx < TOTAL_PIECES_PLAYER; piece_idx++)
    {
        if (target_block->pieces[piece_idx] == true)
        {
                Piece *target_piece = &PIECE_AT(game, target_block->color, piece_idx);
                target_piece->region = BASE;
                target_piece->block_id = NULL_BLOCK;
                target_piece->no_of_captures = 0;
                target_piece->approach = false;
                target_piece->direction = 1;
                target_piece->multiplier = 1;
                target_piece->location.location = modular_add(START_AT(target_piece->color), -1);

        }
        
    }
    
    for (short piece_idx = 0; piece_idx < TOTAL_PIECES_PLAYER; piece_idx++)
    {
        if (block->pieces[piece_idx] == true)
        {
                Piece *piece = &PIECE_AT(game, block->color, piece_idx);
                piece->no_of_captures++;
                piece->approach = true;

        }
    }  

    if (verbose)
    {
        Player *target_player = &PLAYER_AT(game, target_block->color);
        printf("%s player capture %s's block containing %d pieces.\n", PLAYER_AT(game, block->color).color_name, PLAYER_AT(game, target_block->color).color_name, target_block->no_of_pieces);
        printf("%s player now has %d/4 pieces on the board and %d/4 pieces on the base.\n", target_player->color_name, target_player->no_pieces_in_path, target_player->no_pieces_in_base);
    }
        

}



void break_block(Game *game, short block_id)
{

}



void merge_block(Game *game, short block_idx, short target_block_idx, bool verbose)
{
    Block *target_block = BLOCK_AT(game, target_block_idx);
    Block *block = BLOCK_AT(game, block_idx);

    assert(block->color == target_block->color);

    for (short piece_idx = 0; piece_idx < TOTAL_PIECES_PLAYER; piece_idx++)
    {
        if (target_block->pieces[piece_idx])
        {
            append_to_block(game, target_block->color, piece_idx, block_idx);
        }    

    }

    if (verbose)
    {
        printf("%s player merge a block with %d pieces with a block with %d pieces and created block with %d pieces at location %d.\n",
            PLAYER_AT(game, block->color).color_name,
            block->no_of_pieces - target_block->no_of_pieces, 
            target_block->no_of_pieces,
            block->no_of_pieces,
            target_block->location.value
        );
    }

    free(block);
    block = NULL;
    
}


void teleport_to_base(Game *game, short player_color, short piece_idx, bool remove_from_block, bool verbose)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);

    assert(piece->region == PATH);
    piece->region = BASE;
    piece->multiplier = 1;
    piece->location.location = modular_add(START_AT(player_color), -1);
    PLAYER_AT(game, piece->color).no_pieces_in_base++;
    PLAYER_AT(game, piece->color).no_pieces_in_path--;
    piece->no_of_captures = 0;

    if (remove_from_block && piece->block_id != NULL_BLOCK)
    {
        _remove_from_block(game, player_color, piece_idx);
    }

    if (verbose)
    {
        printf("%s piece %s teleported to Base\n",
            piece->color_name,
            piece->name
        );

        printf("%s player now has %d/4 pieces on the board and %d/4 pieces in the base.\n",
            piece->color_name,
            PLAYER_AT(game, piece->color).no_pieces_in_path,
            PLAYER_AT(game, piece->color).no_pieces_in_base
        );
    }   
}

void teleport_to_starting_cell(Game *game, short player_color, short piece_idx, bool remove_from_block, bool verbose)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);

    assert(piece->region == PATH);
    piece->location.location = START_AT(player_color);

    if (remove_from_block && piece->block_id != NULL_BLOCK)
    {
        _remove_from_block(game, player_color, piece_idx);
    }

    if (verbose)
    {
        printf("%s piece %s teleported to starting cell\n",
            piece->color_name,
            piece->name
        );
    }   
}

void teleport_to_approach_cell(Game *game, short player_color, short piece_idx, bool remove_from_block, bool verbose)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);

    assert(piece->region == PATH);
    piece->location.location = APPROACH_CELL(player_color);

    if (remove_from_block && piece->block_id != NULL_BLOCK)
    {
        _remove_from_block(game, player_color, piece_idx);
    }

    if (verbose)
    {
        printf("%s piece %s teleported to approach cell\n",
            piece->color_name,
            piece->name
        );
    }   
    
}

void teleport_to_bhawana(Game *game, short player_color, short piece_idx, bool remove_from_block, bool verbose)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);

    assert(piece->region == PATH);
    piece->location.location.value = BHAWANA;
    piece->multiplier = 0.5;

    if (rand()%2 == 0)
    {
        piece->multiplier = 2;
    }

    piece->rounds_affected = 4;

    if (remove_from_block && piece->block_id != NULL_BLOCK)
    {
        _remove_from_block(game, player_color, piece_idx);
    }

    if (verbose)
    {
        printf("%s piece %s teleported to bhawana\n",
            piece->color_name,
            piece->name
        );

        if (piece->multiplier > DEFAULT_MULTIPLIER)
        {
            printf("%s piece %s feels energized, and movement speed doubles.\n",
                        piece->color_name,
                        piece->name
            );
        }else{
            printf("%s piece %s feels sick, and movement speed halves.\n",
                        piece->color_name,
                        piece->name
            );
        }
        
    }   
    
}

void teleport_to_kotuwa(Game *game, short player_color, short piece_idx, bool remove_from_block, bool verbose)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);

    assert(piece->region == PATH);
    piece->location.location.value = KOTUWA;
    piece->multiplier = 0;
    piece->rounds_affected = 4;

    if (remove_from_block && piece->block_id != NULL_BLOCK)
    {
        _remove_from_block(game, player_color, piece_idx);
    }

    if (verbose)
    {
        printf("%s piece %s teleported to kotuwa\n",
            piece->color_name,
            piece->name
        );

        printf("%s piece %s attends briefing and cannot move for four rounds.\n",
            piece->color_name,
            piece->name
        );
        
    }   
    
}

void teleport_to_pitakotuwa(Game *game, short player_color, short piece_idx, bool remove_from_block, bool verbose)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);

    assert(piece->region == PATH);

    if (verbose)
    {
        printf("%s piece %s teleported to kotuwa\n",
            piece->color_name,
            piece->name
        );
        
    }   


    if (piece->direction == CLOCKWISE)
    {
        piece->direction = COUNTERCLOCKWISE;

        printf("%s piece %s, which was moving clockwise, has changed to moving counter-clockwise.\n",
            piece->color_name,
            piece->name);
    }else{
        teleport_to_kotuwa(game, player_color, piece_idx, remove_from_block, true);
        printf("%s piece %s, is moving in a counterclockwise direction. Teleporting to Kotuwa from Pita-Kotuwa.\n",
            piece->color_name,
            piece->name);        
    }
    
}

void land_on_mystery_cell(Game *game, short player_color, short piece_idx, bool remove_from_block, bool verbose)
{
    short teleport_to = rand()%6;
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    Action action;

    if (piece->region == BASE)
    {
        bring_to_path(game, player_color, piece_idx, true);
    }
    
    switch (teleport_to)
    {
    case 0:
        piece->location.location.value = BHAWANA;
        piece->location.location = modular_add(piece->location.location, -1);
        action = explore_actionspace_of_pieces(game, player_color, piece_idx, 1);

        if (action.action != NULL_ACTION)
        {
            if (action.action != MOVE_PATH)
            {
                perform(game, player_color, action);   
            }
            teleport_to_bhawana(game, player_color, piece_idx, remove_from_block, verbose);

        }
        break;
    case 1:
        piece->location.location.value = KOTUWA;
        piece->location.location = modular_add(piece->location.location, -1);
        action = explore_actionspace_of_pieces(game, player_color, piece_idx, 1);
        // printf("action: %d\n", action.action);
        if (action.action != NULL_ACTION)
        {
            if (action.action != MOVE_PATH)
            {
                perform(game, player_color, action);   
            }
            teleport_to_kotuwa(game, player_color, piece_idx, remove_from_block, verbose);
        }    
        break;
    case 2:
        piece->location.location.value = PITAKOTUWA;
        piece->location.location = modular_add(piece->location.location, -1);
        action = explore_actionspace_of_pieces(game, player_color, piece_idx, 1);
        // printf("action: %d\n", action.action);
        if (action.action != NULL_ACTION)
        {
            if (action.action != MOVE_PATH)
            {
                perform(game, player_color, action);   
            }
            teleport_to_pitakotuwa(game, player_color, piece_idx, remove_from_block, verbose);
        }      
        break;
    case 3:
        piece->location.location = APPROACH_CELL(player_color);
        piece->location.location = modular_add(piece->location.location, -1);
        action = explore_actionspace_of_pieces(game, player_color, piece_idx, 1);
        // printf("action: %d\n", action.action);
        if (action.action != NULL_ACTION)
        {
            if (action.action != MOVE_PATH)
            {
                perform(game, player_color, action);   
            }
            teleport_to_approach_cell(game, player_color, piece_idx, remove_from_block, verbose);

        }          
        break;
    case 4:
        piece->location.location = START_AT(player_color);
        piece->location.location = modular_add(piece->location.location, -1);
        action = explore_actionspace_of_pieces(game, player_color, piece_idx, 1);
        // printf("action: %d\n", action.action);
        if (action.action != NULL_ACTION)
        {
            if (action.action != MOVE_PATH)
            {
                perform(game, player_color, action);   
            }
            teleport_to_starting_cell(game, player_color, piece_idx, remove_from_block, verbose);

        }    
        break;
    case 5:
        teleport_to_base(game, player_color, piece_idx, remove_from_block, verbose);
        break;                                    
    default:
        printf("invalid address!");
        break;
    }
    }


void perform(Game *game, short player_color, Action action)
{    
    switch (action.action)
    {
    case BRING_TO_PATH:
        bring_to_path(game, player_color, action.input1, true);
        break;
    
    case CAPTURE:
        // capture(game, player_color, action.input1, action.input2, true, true);
        break;

    case MOVE_PATH:
        move_path(game, player_color, action.input1, action.input2, action.input3, true, true);
        break;

    case MOVE_TO_HOME_S:
        move_to_home_straight(game, player_color, action.input1, action.input2, true, true);
        break;

    case MOVE_IN_HOME_S:
        move_in_home_straight(game, player_color, action.input1, action.input2);
        break;

    case BLOCKED:
        
        short blocked_player_color = BLOCK_AT(game, action.input2)->color;
        blocked(game, player_color, action.input1, blocked_player_color, action.input2, action.input3, true);
        break;

    case CREAT_BLOCK:
        create_block(game, player_color, action.input1, action.input2, true, true);
        break;

    case ADD_TO_BLOCK:
        add_to_block(game, player_color, action.input1, action.input2, true, true);
        break;

    case REMOVE_FROM_BLOCK:
        remove_from_block(game, player_color, action.input1, action.input2, true);
        break;

    case MOVE_BLOCK:
        move_block(game, action.input1, action.input2);
        break;

    case CAPTURE_PIECE_BY_BLOCK:
        capture_pieces_by_block(game, player_color, action.input1, action.input2, action.input3);
        break;

    case CAPTURE_BLOCK_BY_BLOCK:
        capture_block_by_block(game, action.input1, action.input2, true);
        break;

    case BREAK_BLOCK:
        break_block(game, action.input1);
        break;

    case MERGE_BLOCK:
        merge_block(game, action.input1, action.input2, true);
        break;

    case LAND_ON_MYSTORY_CELL:
        land_on_mystery_cell(game, player_color, action.input1, true, true);
        break;

    case NULL_ACTION:
        break;
    default:
        printf("Invalid Action!");
    }
}


bool have_any_block(Game *game, short player_color)
{
    for (short piece_idx = 0; piece_idx < TOTAL_PIECES_PLAYER; piece_idx++)
    {
        if (PIECE_AT(game, player_color, piece_idx).block_id != NULL_BLOCK)
        {
            return false;
        }
    }
    return true;
    
}

bool roll_logic(Game *game, short current_player_id, short rolled)
{
    Player *player = &PLAYER_AT(game, current_player_id);

    if ((rolled == 6))
    {
        if((player->same_rolls == 3) && (player->prev_rolled == 6))
        {
            if (!have_any_block(game, current_player_id))
            {    
                player->same_rolls  = 0;
                return false;
            }

            return true;

        }
            player->same_rolls++;
            player_pointer_update(game, current_player_id);
            return true;
        
    }
    return true;
}


ActionSpace action_space_filter(Game *game, ActionSpace action_space, short player_color, short rolled)
{
    if ((rolled == 6) && !have_any_block(game, player_color))
    {
        ActionSpace action_space = new_actionspace();
        for (short block_idx = 0; block_idx < MAX_BLOCK_COUNT; block_idx++)
        {
            if (BLOCK_AT(game, block_idx) == NULL)
            {
                continue;
            }
            
            Block *block = BLOCK_AT(game, block_idx);
            // printf("block id: %d\n", block_idx);
            if (block->color == player_color)
            {
                Action action = {BREAK_BLOCK, block_idx};
                append_action(&action_space,  action);
            }
            
        }
    }
    return action_space;
}



void play(Game *game, short *player_pointer)
{
    short current_player_id = *player_pointer;
    Player *player = &PLAYER_AT(game, current_player_id);

    player_pointer_next(game);

    short rolled = roll_die(player, true);
    bool allow_play = roll_logic(game, current_player_id, rolled);

    // printf("%d\n", allow_play);

    if (!allow_play)
    {
        // printf("\n\tLEAVE!\n");
        return;
    }

    ActionSpace action_space = generate_action_space(game, current_player_id, rolled);
    ActionSpace constrained_action_space = action_space_filter(game, action_space, player->color, rolled);

    // print_action_space_(&constrained_action_space);

    if (constrained_action_space.length == 0)
    {
        return;
    }

    Action selected_action = player->perform_strategy(player, action_space);

    

    // action_logic(game, selected_action);
    perform(game, player->color, selected_action);

    // printf("Played!\n\n");

 
    
}

void play_round(Game *game)
{
    static short round_number = 0;
    assert (*game->player_pointer == *game->starting_player);
    short played_count  = 0;
    short prev_player = -1;
    do
    {
        prev_player = *game->player_pointer;

        play(game, game->player_pointer);
        // printf("start %s; current %s\n", PLAYER_AT(game, *game->starting_player).color_name, PLAYER_AT(game, *game->player_pointer).color_name);

        if (*game->player_pointer != prev_player)
        {
            played_count++;
        }
    } while (played_count < 4);

    
    
    for(short i=0; i < TOTAL_PLAYERS; i++)
    {
        Player *player = &PLAYER_AT(game, i);

        for (short player_color = 0; player_color < TOTAL_PIECES_PLAYER; player_color++)
        {
            for (short piece_idx = 0; piece_idx < TOTAL_PIECES_PLAYER; piece_idx++)
            {
                Piece *piece = &PIECE_AT(game, player_color, piece_idx);
                
                if (piece->rounds_affected == NOT_ACTIVATED)
                {
                    continue;
                }
                
                piece->rounds_affected--;
                if (piece->rounds_affected == NOT_ACTIVATED)
                {
                    piece->multiplier = DEFAULT_MULTIPLIER;    
                }
                
                
            }
            
        }

        printf("%s player now has %d/4 pieces on the board and %d/4 pieces on base.\n", player->color_name, player->no_pieces_in_path, player->no_pieces_in_base);
        printf("\n====================================================================================\n");
        printf("Location of pieces %s\n", player->color_name);
        printf("\n====================================================================================\n");
        for (short j = 0; j < TOTAL_PIECES_PLAYER; j++)
        {
            char piece_name[3];
            strcpy(piece_name, player->pieces[j].name);

            switch (player->pieces[j].region)
            {
            case PATH:
                short piece_location = player->pieces[j].location.location.value;
                printf("Piece %s -> %d\n", player->pieces[j].name, piece_location);
                break;


            case BASE:
                printf("Piece %s -> BASE\n", piece_name);
                break;

            case HOME:
                // short piece_location = player->pieces[i].location.location.value;
                printf("Piece %s -> HOME\n", piece_name);
                break;

            default:
                break;
            }
        }

    }
    
    round_number++;
    ModularInt new_mystry_location;
    if (round_number%4 == 0)
    {

        while (true)
        {
            new_mystry_location = modular_new(rand(), 52);
            if (get_piece(game, new_mystry_location) != NULL_PIECE)
            {
                *game->mystery_cell = new_mystry_location;
                break;
            }
        }
        printf("Mystery cell new location %d!\n", new_mystry_location.value);            
    }else{
        printf("Mystery cell still at same location %d!\n", new_mystry_location.value);
    }
    
}

//player's stratagies

Action random_player(Player *player, ActionSpace action_space)
{
    short action_idx = RAND(0, action_space.length);
    return action_space.action_space[action_idx];
}