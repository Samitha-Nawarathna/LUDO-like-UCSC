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

// ModularInt get_destination(Piece *piece, short die_value)
// {
//     assert (piece->region == PATH);
//     short move_by = (short)die_value*piece->direction*piece->multiplier;
//     // printf("move_by = %d\n", move_by);
//     return modular_add(piece->location.location, move_by);
// }

// Piece *get_piece(Game *game, ModularInt destination)
// {
//     for (short i = 0; i < TOTAL_PIECES; i++)
//     {
//         if (game->pieceArr[i].region != PATH) continue;
        
//         if (MODULAR_IS_EQL(game->pieceArr[i].location.location, destination))
//         {
//             return &game->pieceArr[i];
//         }
//         return NULL;
//     } 
    
// }

// ActionSpace generate_action_space(Game *game, short player_id, short rolled)
// {
//     // short *rolled_ptr = (short *)malloc(sizeof(short)); //move rolled value to heap temporary so we can access value even from outside of this function
//     // *rolled_ptr = rolled;   //and must be deallocated after perform a action!!!

//     ActionSpace action_space;
//     action_space.action_space = (Action *)malloc(sizeof(Action)*TOTAL_PIECES_PLAYER);
//     action_space.length = 0;
//     Player *player = &PLAYER_AT(game, player_id);

//     for (short i = 0; i < TOTAL_PIECES_PLAYER; i++)
//     {
//         Piece *piece = &PIECE_AT(game, player_id, i);

//         if ((piece->region == HOME) && (piece->location.dist_from_home == 0)) continue;

//         //bring to path

//         if (piece->region == BASE)
//         {
//             if (rolled == 6)
//             {
//                 Operand operand1;
//                 operand1.piece = piece;

//                 Action action = {BRING_TO_PATH, player, piece, 0, NULL, NULL};
//                 action_space.action_space[action_space.length] = action;
//                 action_space.length++;
//                 continue;
//             }
//             continue;
//         }        

//         //moving in home straight

//         if ((piece->region == HOME))
//         {
//             short to_move = piece->multiplier*rolled;
//             if (to_move && (piece->location.dist_from_home >= to_move))
//             {
//                 int steps = rolled + piece->multiplier*rolled;
//                 Action action = {MOVE_IN_HOME_S, player, piece, steps, NULL, NULL};
//                 action_space.action_space[action_space.length] = action;
//                 action_space.length++;
//                 continue;            
//             }

//             continue;            
//         }

//         ModularInt current_location = piece->location.location;
//         ModularInt destination = get_destination(piece, rolled);

//         if (MODULAR_IS_EQL(current_location, destination)) continue;        

//         //move to home staight
//         if (piece->approach)
//         {
//             ModularInt piece_approach = modular_add(piece->start_location, APPROACH_CELL);
//             if (piece->direction == 1 && modular_is_between(current_location, destination, piece_approach))
//             {
//                 short steps_in_home_straight = modular_add(destination, -piece_approach.value).value;

//                 Action action = {MOVE_TO_HOME_S, player, piece, steps_in_home_straight, NULL, NULL};
//                 action_space.action_space[action_space.length] = action;
//                 action_space.length++;
//                 continue;                
//             }
//             else if(piece->direction == -1 && modular_is_between(destination, current_location, piece_approach))
//             {
                
//                 short steps_in_home_straight = modular_add(piece_approach, -destination.value).value;

//                 Action action = {MOVE_TO_HOME_S, player, piece, steps_in_home_straight, NULL, NULL};
//                 action_space.action_space[action_space.length] = action;
//                 action_space.length++;
//                 continue;                 
//             }
//         }
//         //capture a cell

//         Piece *target_piece = get_piece(game, destination);
        
//         if ((target_piece != NULL) && (target_piece->color != piece->color))
//         {
            
//             Operand operand1;
//             operand1.piece = target_piece;

//             Operand operand2;
//             operand2.player = &PLAYER_AT(game, target_piece->color);

//             Action action = {CAPTURE, player, piece, 0, operand1, operand2};            

//             action_space.action_space[action_space.length] = action;
//             action_space.length++;
//             continue;    

//         }
            

//         //move in standard path

            
//             Operand operand1;
//             operand1.standard_steps = modular_add(destination, -piece->location.location.value);

//             Action action = {MOVE_PATH, player, piece, 0, operand1, NULL};
//             action_space.action_space[action_space.length] = action;
//             action_space.length++;
        
//     }

//     return action_space;
     
// }
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
    printf("steps can be move: %d\n", steps);
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
            printf("a = %d\n", piece->location.location.value);
            printf("b = %d\n", destination.value);
            printf("dir = %d\n", APPROACH_CELL(piece->color).value);
    }else if(piece->direction == -1)
    {
        if (!modular_is_between(destination, piece->location.location, APPROACH_CELL(piece->color)))
        {
            return 0;
        }
    }
    printf("\nA\n");
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

        return nearset_block_id;
        
        
    }
    
}

short get_piece(Game *game, ModularInt destination)
{
    for (short piece_idx = 0; piece_idx < TOTAL_PIECES; piece_idx++)
    {
        Piece piece = game->pieceArr[piece_idx];
        if (MODULAR_IS_EQL(destination, piece.location.location))
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

    short block_idx = find_nearset_block(game, player_color, current_location, modular_add(destination, 1), piece.direction);

    if (block_idx != NULL_BLOCK)
    {
        printf("Block found at %d\n", block_idx);
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
    printf("where piece going to land:  %d\n", destination.value);

    if (target_piece_idx == NULL_PIECE)
    {
        Action action = {MOVE_PATH, piece_idx, steps, DO_NOT_CARE}; 
        return action;
    }

    Piece target_piece = game->pieceArr[target_piece_idx];

//write a function to return subtraction of mod ints        
//////////////HERE//////////////////////////////////////////////////////////////////////////////////
// update the step number according to locations of block and here is general case;
    
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

// Action explore_actionspace_of_blocks(Game *game, short block_idx, int steps)
// {
//     short target_block_idx = find_nearset_block(game, player_color, piece_idx, steps);
//     Piece piece = PIECE_AT(game, player_color, piece_idx);
    
//     ModularInt destination = modular_add(piece.location.location, steps);

//     if (block_idx != NULL_BLOCK)
//     {
//         Block *block = BLOCK_AT(game, block_idx);

//         if (MODULAR_IS_EQL(piece.location.location, modular_add(block->location, -1)))
//         {
//             Action action = {NULL_ACTION, DO_NOT_CARE, DO_NOT_CARE, DO_NOT_CARE}; 
//             return action;
//         }

//         if (MODULAR_IS_EQL(destination, block->location))
//         {
//             Action action = {BLOCKED, piece_idx, block_idx, destination.value};
//             return action;
//         }
//         short steps = MODULAR_SUB(piece.location.location, block->location, piece.direction).value;
        
//     }
    
//     destination = modular_add(piece.location.location, steps);
//     short target_piece_idx = get_piece(game, destination);
//     printf("%d\n", destination.value);

//     if (target_piece_idx == NULL_PIECE)
//     {
//         Action action = {MOVE_PATH, piece_idx, steps, DO_NOT_CARE}; 
//         return action;
//     }

//     Piece target_piece = game->pieceArr[target_piece_idx];

// //write a function to return subtraction of mod ints        
// //////////////HERE//////////////////////////////////////////////////////////////////////////////////
// // update the step number according to locations of block and here is general case;
    
//     if (target_piece.color == piece.color)
//     {
//         if (target_piece.block_id == NULL_BLOCK)
//         {
//             Action action = {CREAT_BLOCK, piece_idx, target_piece_idx, DO_NOT_CARE};
//             return action;
//         }else{

//             Action action = {ADD_TO_BLOCK, piece_idx, block_idx, DO_NOT_CARE};
//             return action;

//         }
        
//     }else{

//         Action action = {CAPTURE, piece_idx, target_piece_idx, DO_NOT_CARE};
//         return action;
//     }
    

// }
// ModularInt get_destination(Piece *piece, short die_value);

//still have to handle!!!!!!!!!!!
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
            continue;;
        }
        


        if (region == HOME)
        {   
            int distance_to_home = piece.location.dist_from_home;
            if (distance_to_home == 0)
            {
                continue;

            }else{
                
                int steps = die_value*piece.multiplier;
                if (distance_to_home < steps)
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
            printf("steps_in_home %d\n", steps_in_home);
            if (steps_in_home)
            {
                steps = steps - steps_in_home;
                Action action = explore_actionspace_of_pieces(game, player_color, piece_idx, steps);

                if (action.action == NULL_ACTION)
                {
                    continue;
                }
                if ((action.action == BLOCKED) || ((action.action == MOVE_PATH) && (MODULAR_IS_EQL(modular_add(piece.location.location, piece.direction*steps), modular_add(APPROACH_CELL(piece.color), 0)))))
                {
                    action.action = MOVE_TO_HOME_S;
                    action.input1 = piece_idx;
                    action.input2 = steps_in_home;
                    
                }

                append_action(&action_space, action);


            }else{
                
                Action action = explore_actionspace_of_pieces(game, player_color, piece_idx, steps);
                if (action.action == NULL_ACTION)
                {
                    continue;
                }
                // if (action.action == BLOCKED)
                // {
                //     action.action = MOVE_PATH;
                //     action.input1 = piece_idx;
                //     action.input2 = steps;
                    
                // }

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

                

                Action action = {MERGE_BLOCK, piece_at_destination->block_id, block_id};
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

    // for (short block_idx = 0; block_idx < MAX_BLOCK_COUNT; block_idx++)
    // {
    //     if (BLOCK_AT(game, block_idx) != NULL)
    //     {
    //         Block *block = BLOCK_AT(game, block_idx);
    //         short steps = block->direction*block->multiplier*block->direction;

    //         for (short piece_idx = 0; piece_idx < TOTAL_PIECES_PLAYER; piece_idx++)
    //         {
    //             Piece piece = new_piece(block->color);
    //             piece.region = PATH;
    //             piece.direction = block->direction;
    //             piece.location.location = block->location;
    //             piece.multiplier = block->multiplier;

    //             Action action = explore_actionspace_of_pieces(game, )
                
    //         }

            
    //     }
        
    // }
    
    
    
}

// void append(ActionSpace *actionspace, Action action)
// {
//     actionspace->action_space[actionspace->length] = action;
//     actionspace->length++;

// }

// ActionSpace generate_action_space(Game *game, int player_id, int rolled)
// {
//     Player curr_player = PLAYER_AT(game, player_id);
//     ActionSpace actionspace = new_actionspace();

//     for (short i = 0; i < TOTAL_PIECES_PLAYER; i++)
//     {
//         Piece piece = PIECE_AT(game, player_id, i);
//         int steps = rolled*piece.multiplier;

//         if(piece.region == HOME)
//         {
//             if (piece.location.dist_from_home == 0)
//             {
//                 continue;
//             }
            
//             if(piece.location.dist_from_home > 0)
//             {
//                 if(piece.location.dist_from_home >= steps)
//                 {
//                     Action action;
//                     action = {0, MOVE_IN_HOME_S, 0, 0, player_id, NULL};
//                     append(&actionspace, action);

//                     continue;
//                 }
//             }

//             if (piece.region == BASE)
//             {
//                 if (rolled == 6)
//                 {
//                     Piece dest_piece = *get_piece(player_id, piece.start_location);

//                     if (dest_piece.parent_block)
//                     {
//                         /* code */
//                     }
                    
//                     Action action;
//                 }
                
//             }
            
//         }
    

//     if (curr_player.)
//     {
//         /* code */
//     }
    
// }

// void bring_to_path(Player *player, Piece *piece, bool verbose)
// {
//     assert (piece->region == BASE);
//     piece->region = PATH; 

//     piece->location.location = piece->start_location;
//     piece->multiplier = 1;
//     player->no_pieces_in_base--;
//     player->no_pieces_in_path++;

//     //update_map
//     //log
//     if(verbose)
//     {
//         printf("%s player moves piece %s to starting point.\n\n", piece->color_name, piece->name, piece->color);
//     }
      
// }

// void move(Player *player, Piece *piece, ModularInt dest, bool verbose)
// {
//     assert (piece->region == PATH);

//     ModularInt current_location = piece->location.location; 
//     piece->location.location = dest;
//     int steps = modular_add(current_location, dest.value).value;
//     //update_map;
//     //log
//     char direction[18] = " ";
    

//     if (piece->direction == -1)
//     {
//         strcpy(direction, " counter-");
//     }

//     if (verbose)
//     {
//         printf("%s moves piece %s from location %d to %d by %d units in%sclockwise direction\n\n", piece->color_name, piece->name, current_location.value, piece->location.location.value, steps, direction);   
//     }
    
// }

// void move_to_home_straight(Player *player, Piece *piece, int steps, bool verbose)
// {
//     assert (piece->region == PATH);

//     piece->region = HOME;
//     ModularInt current_location = piece->location.location; 
//     piece->location.dist_from_home = CELLS_IN_HOME - steps;
    
//     if( !piece->location.dist_from_home)
//     {
//         player->no_pieces_in_path--;
//         player->no_pieces_in_home++;
//     }
    

//     //update_map
//     //log
//     if (verbose)
//     { 
//         printf("%s moves piece %s from location %d in standard path to %s homepath %d by %d units in home straight\n\n", piece->color_name, piece->name, current_location.value, piece->color_name, (6 - piece->location.dist_from_home), steps);
//     }
// }

// void move_in_home_straight(Player *player, Piece *piece, int steps, bool verbose)
// {
//     assert (piece->region == HOME);

//     short current_distance = piece->location.dist_from_home;
//     piece->location.dist_from_home = current_distance - steps;

//     //update_map
//     //log 
//     // printf("rolled = %d", piece->location.dist_from_home);

//     if (verbose)
//     {
//         printf("%s moves piece %s from location %s homepath %d to %s homepath %d by %d units in home straight\n\n", piece->color_name, piece->name, piece->color_name, (6 - current_distance), piece->color_name, (6 - piece->location.dist_from_home), steps);
//     }
// }

// void capture(Player *player, Piece *piece, Player *target_player, Piece *target_piece, bool verbose)
// {
//     assert (piece->region = PATH && target_piece->region == PATH);

//     ModularInt current_location = target_piece->location.location;
//     piece->location = target_piece->location;
//     target_piece->location.dist_from_home = TOO_FAR;
//     target_piece->region = BASE;
    
//     target_player->no_pieces_in_base++;
//     target_player->no_pieces_in_path--;

//     piece->no_of_captures += 1;

//     target_piece->no_of_captures = 0;
//     target_piece->multiplier = 0;


//     if (verbose)
//     {
//         printf("%s piece %s lands on square %d, captures %s piece %s, and return it to the base\n", piece->color_name, piece->name, current_location.value, target_piece->color_name, target_piece->name);
//         printf("%s player now has %d/4 pieces on the board and %d/4 pieces on the base\n\n",target_piece->color_name, player->no_pieces_in_path, player->no_pieces_in_base);
//     }
// }

// void blocked_by(Player *player, Piece *piece, Block *blocked_piece, int rolled)
// {
//     ModularInt current_location = piece->location.location;
//     int real_steps;
//     ModularInt expected_
//     move(piece, rolled, false);

//     printf("%s piece %s is blocked from moving from %d to %d by %s piece %s", player->color_name, piece->name, current_location.value, piece->location.location.value, piece->color_name, piece->name);

// }

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

void capture(Game *game, short player_color, short piece_idx, short target_piece_idx, bool verbose)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    Player *player = &PLAYER_AT(game, player_color);
    Piece *target_piece = game->pieceArr + target_piece_idx;
    Player *target_player = &PLAYER_AT(game, target_piece->color);

    if (piece->region == BASE)
    {
        bring_to_path(game, player_color, piece_idx, true);
    }else if (target_piece == BASE)
    {
        bring_to_path(game, player_color, target_piece_idx, true);
    }
    
    
    
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

    target_player->no_pieces_in_base++;
    target_player->no_pieces_in_path--;

    if(verbose)
    {
        printf("%s piece %s lands on squre %d, captures %s piece %s, and returns to the base.\n", player->color_name, piece->name, piece->location.location.value, target_piece->name, target_player->color_name);
        printf("%s player now has %d/4 pieces on the board and %d/4 pieces on the base.\n", target_piece->color_name, target_player->no_pieces_in_path, target_player->no_pieces_in_base);

    }


}
void move_path(Game *game, short player_color, short piece_idx, short steps, short real_destination_value, bool verbose)
{
    Player *player = &PLAYER_AT(game, player_color);
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);

    ModularInt destination = modular_add(piece->location.location, piece->direction*steps);
    ModularInt current_location = piece->location.location;
    char direction[8] = " ";
    switch (piece->direction)
    {
    case CLOCKWISE:
        strcpy(direction , " clockwise-");
        break;
    }


    piece->location.location = destination;
    piece->region == PATH;

    // if (real_destination_value != DO_NOT_CARE)
    // {
    //     blocked(game, piece_idx,  )
    // }
    

    if (verbose)
    {
        printf("%s moves piece %s from location %d to %d by %d units in%sclockwise direction.\n", 
        piece->color_name,
        piece->name,
        current_location.value,
        destination.value,
        steps,
        direction);
    }

}
void move_to_home_straight(Game *game, short player_color, short piece_idx, short steps, bool verbose)
{
    Player *player = &PLAYER_AT(game, player_color);
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    
    assert (piece->region == PATH);
    assert (piece->approach == true);
    assert (steps < 7);

    ModularInt current_location = piece->location.location;

    piece->location.dist_from_home = CELLS_IN_HOME - steps;
    piece->region == HOME;
    char direction[8] = " ";
    switch (piece->direction)
    {
    case CLOCKWISE:
        strcpy(direction , " clockwise-");
        break;
    }    

    if (verbose)
    {
        printf("%s moves piece %s from location %d to %shomepath%d in%sclockwise direction.\n", 
        piece->color_name,
        piece->name,
        piece->color_name,
        current_location.value,
        steps,
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
        printf("%s dose not have other pieces in the board to move instead of the blocked piece.\n Ignoring the throw and moving on to the next player.\n", player->color_name);

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

void create_block(Game *game, short player_color, short piece_idx, short other_piece_idx)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    Piece *other_piece = &PIECE_AT(game, player_color, other_piece_idx);

    if (piece->region == BASE)
    {
        bring_to_path(game, player_color, piece_idx, true);
    }else if (other_piece->region == BASE)
    {
        bring_to_path(game, player_color, other_piece_idx, true);
    }

    new_block(game, player_color, piece_idx, piece_idx);
}

void add_to_block(Game *game, short player_color, short piece_idx, short block_idx)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);

    if (piece->region == BASE)
    {
        bring_to_path(game, player_color, piece_idx, true);
    }

    append_to_block(game, player_color, piece_idx, block_idx);
}

void remove_from_block(Game *game, short player_color, short piece_idx, short block_idx)
{
    _remove_from_block(game, player_color, piece_idx, block_idx);
}

void move_block(Game *game, short block_idx, short steps)
{
    Block *block = BLOCK_AT(game, block_idx);
    block->location = modular_add(block->location, steps*block->direction);

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
void capture_block_by_block(Game *game, short block_idx, short target_block_idx)
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

}
void break_block(Game *game, short block_id)
{

}

void merge_block(Game *game, short block_idx, short target_block_idx)
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

    free(block);
    block = NULL;
    
}
void land_on_mystery_cell(Game *game, short player_color, short piece_idx)
{

}

// void blocked_msg(Game *game, short player_color, short piece_idx, short blocked_player_color, short init_destination)
// {
//     printf("%s piece %s is blocked moving from %d to %d by %s\n.", player_color, piece->name, piece->location.location.value, init_destination, blocked_player_color);
// }




void perform(Game *game, short player_color, Action action)
{    
    switch (action.action)
    {
    case BRING_TO_PATH:
        bring_to_path(game, player_color, action.input1, true);
        break;
    
    case CAPTURE:
        capture(game, player_color, action.input1, action.input2, true);
        break;

    case MOVE_PATH:
        move_path(game, player_color, action.input1, action.input2, action.input3, true);
        break;

    case MOVE_TO_HOME_S:
        move_to_home_straight(game, player_color, action.input1, action.input2, true);
        break;

    case MOVE_IN_HOME_S:
        move_in_home_straight(game, player_color, action.input1, action.input2);
        break;

    case BLOCKED:
        
        short blocked_player_color = BLOCK_AT(game, action.input2)->color;
        blocked(game, player_color, action.input1, blocked_player_color, action.input2, action.input3, true);
        break;

    case CREAT_BLOCK:
        create_block(game, player_color, action.input1, action.input2);
        break;

    case ADD_TO_BLOCK:
        add_to_block(game, player_color, action.input1, action.input2);
        break;

    case REMOVE_FROM_BLOCK:
        remove_from_block(game, player_color, action.input1, action.input2);
        break;

    case MOVE_BLOCK:
        move_block(game, action.input1, action.input2);
        break;

    case CAPTURE_PIECE_BY_BLOCK:
        capture_pieces_by_block(game, player_color, action.input1, action.input2, action.input3);
        break;

    case CAPTURE_BLOCK_BY_BLOCK:
        capture_block_by_block(game, action.input1, action.input2);
        break;

    case BREAK_BLOCK:
        break_block(game, action.input1);
        break;

    case MERGE_BLOCK:
        merge_block(game, action.input1, action.input2);
        break;

    case LAND_ON_MYSTORY_CELL:
        land_on_mystery_cell(game, player_color, action.input1);
        break;

    default:
        printf("Invalid Action!");
    }
}

bool roll_logic(Game *game, short current_player_id, short rolled)
{
    Player *player = &PLAYER_AT(game, current_player_id);

    if ((rolled == 6))
    {
        if((player->same_rolls == 3) && (player->prev_rolled == 6))
        {
            player->same_rolls  = 0;
            return false;
        }
        else
        {
            player->same_rolls++;
            // *game->player_pointer = current_player_id;
            player_pointer_update(game, current_player_id);
            return true;
        }
        
    }
    return true;
}

// void action_logic(Game *game, Action selected_action)
// {
//     if (selected_action.action == CAPTURE)
//     {
//         player_pointer_update(game, selected_action.piece->color);
//         // *game->player_pointer = selected_action.operand1.piece->color;
//     }
    
// }

ActionSpace action_space_filter(ActionSpace action_space, Player *player, short rolled)
{
    return action_space;
}

// void print_action_space_(ActionSpace *action_space)
// {
//     for (short i = 0; i < action_space->length; i++)
//     {
//         // printf("i = %d\n", ((Piece *)action_space->action_space[i].operand1)->color);
//         Action action = action_space->action_space[i];
//         printf("{%d} {%s}\n", action.action, action.piece->name);
//     }
    
// }

// void play(Game *game, short *player_pointer)
// {
//     short current_player_id = *player_pointer;
//     Player *player = &PLAYER_AT(game, current_player_id);

//     player_pointer_next(game);

//     short rolled = roll_die(player, true);
//     bool allow_play = roll_logic(game, current_player_id, rolled);

//     // printf("%d\n", allow_play);

//     if (!allow_play)
//     {
//         printf("\n\tLEAVE!\n");
//         return;
//     }

//     ActionSpace action_space = generate_action_space(game, current_player_id, rolled);
//     ActionSpace constrained_action_space = action_space_filter(action_space, player, rolled);

//     // print_action_space_(&constrained_action_space);

//     if (constrained_action_space.length == 0)
//     {
//         return;
//     }

//     Action selected_action = player->perform_strategy(player, action_space);

    

//     action_logic(game, selected_action);
//     perform(player, rolled, selected_action);

//     printf("Played!\n\n");

 
    
// }

// void play_round(Game *game)
// {
//     assert (*game->player_pointer == *game->starting_player);
//     short played_count  = 0;
//     short prev_player = -1;
//     do
//     {
//         prev_player = *game->player_pointer;

//         play(game, game->player_pointer);
//         printf("start %s; current %s\n", PLAYER_AT(game, *game->starting_player).color_name, PLAYER_AT(game, *game->player_pointer).color_name);

//         if (*game->player_pointer != prev_player)
//         {
//             played_count++;
//         }
//     } while (played_count < 4);

    
    
//     for(short i=0; i < TOTAL_PLAYERS; i++)
//     {
//         Player *player = &PLAYER_AT(game, i);

//         printf("%s player now has %d/4 pieces on the board and %d/4 pieces on base.\n", player->color_name, player->no_pieces_in_path, player->no_pieces_in_base);
//         printf("\n====================================================================================\n");
//         printf("Location of pieces %s\n", player->color_name);
//         printf("\n====================================================================================\n");
//         for (short j = 0; j < TOTAL_PIECES_PLAYER; j++)
//         {
//             char piece_name[3];
//             strcpy(piece_name, player->pieces[j].name);

//             switch (player->pieces[j].region)
//             {
//             case PATH:
//                 short piece_location = player->pieces[j].location.location.value;
//                 printf("Piece %s -> %d\n", player->pieces[j].name, piece_location);
//                 break;


//             case BASE:
//                 printf("Piece %s -> BASE\n", piece_name, BASE);
//                 break;

//             case HOME:
//                 // short piece_location = player->pieces[i].location.location.value;
//                 printf("Piece %s -> HOME\n", piece_name);
//                 break;

//             default:
//                 break;
//             }
//         }

//         printf("Mystery cell not yet unlocked!\n");
//     }
    
// }

//player's stratagies

Action random_player(Player *player, ActionSpace action_space)
{
    short action_idx = RAND(0, action_space.length);
    return action_space.action_space[action_idx];
}