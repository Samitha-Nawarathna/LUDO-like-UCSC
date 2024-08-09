#include "logic.h"
#include <stdlib.h>

#define RAND(min, max) rand()%(max) + (min)
#define NEWLINE printf("\n");


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

ModularInt get_destination(Piece *piece, short rolled)
{
    assert (piece->region == PATH);
    short move_by = (short)rolled*piece->direction*piece->multiplier;
    // printf("move_by = %d\n", move_by);
    return modular_add(piece->location.location, move_by);
}

Piece *get_piece(Game *game, ModularInt destination)
{
    for (short i = 0; i < TOTAL_PIECES; i++)
    {
        if (game->pieceArr[i].region != PATH) continue;
        
        if (MODULAR_IS_EQL(game->pieceArr[i].location.location, destination))
        {
            return &game->pieceArr[i];
        }
        return NULL;
    } 
    
}

ActionSpace generate_action_space(Game *game, short player_id, short rolled)
{
    // short *rolled_ptr = (short *)malloc(sizeof(short)); //move rolled value to heap temporary so we can access value even from outside of this function
    // *rolled_ptr = rolled;   //and must be deallocated after perform a action!!!

    ActionSpace action_space;
    action_space.action_space = (Action *)malloc(sizeof(Action)*TOTAL_PIECES_PLAYER);
    action_space.length = 0;
    Player *player = &PLAYER_AT(game, player_id);

    for (short i = 0; i < TOTAL_PIECES_PLAYER; i++)
    {
        Piece *piece = &PIECE_AT(game, player_id, i);

        if ((piece->region == HOME) && (piece->location.far_from_home == 0)) continue;

        //bring to path

        if (piece->region == BASE)
        {
            if (rolled == 6)
            {
                Operand operand1;
                operand1.piece = piece;

                Action action = {BRING_TO_PATH, player, piece, 0, NULL, NULL};
                action_space.action_space[action_space.length] = action;
                action_space.length++;
                continue;
            }
            continue;
        }        

        //moving in home straight

        if ((piece->region == HOME))
        {
            short to_move = piece->multiplier*rolled;
            if (to_move && (piece->location.far_from_home >= to_move))
            {
                int steps = rolled + piece->multiplier*rolled;
                Action action = {MOVE_IN_HOME_S, player, piece, steps, NULL, NULL};
                action_space.action_space[action_space.length] = action;
                action_space.length++;
                continue;            
            }

            continue;            
        }

        ModularInt current_location = piece->location.location;
        ModularInt destination = get_destination(piece, rolled);

        if (MODULAR_IS_EQL(current_location, destination)) continue;        

        //move to home staight
        if (piece->approach)
        {
            ModularInt piece_approach = modular_add(piece->start_location, APPROACH_CELL);
            if (piece->direction == 1 && modular_is_between(current_location, destination, piece_approach))
            {
                short steps_in_home_straight = modular_add(destination, -piece_approach.value).value;

                Action action = {MOVE_TO_HOME_S, player, piece, steps_in_home_straight, NULL, NULL};
                action_space.action_space[action_space.length] = action;
                action_space.length++;
                continue;                
            }
            else if(piece->direction == -1 && modular_is_between(destination, current_location, piece_approach))
            {
                
                short steps_in_home_straight = modular_add(piece_approach, -destination.value).value;

                Action action = {MOVE_TO_HOME_S, player, piece, steps_in_home_straight, NULL, NULL};
                action_space.action_space[action_space.length] = action;
                action_space.length++;
                continue;                 
            }
        }
        //capture a cell

        Piece *target_piece = get_piece(game, destination);
        
        if ((target_piece != NULL) && (target_piece->color != piece->color))
        {
            
            Operand operand1;
            operand1.piece = target_piece;

            Operand operand2;
            operand2.player = &PLAYER_AT(game, target_piece->color);

            Action action = {CAPTURE, player, piece, 0, operand1, operand2};            

            action_space.action_space[action_space.length] = action;
            action_space.length++;
            continue;    

        }
            

        //move in standard path

            
            Operand operand1;
            operand1.standard_steps = modular_add(destination, -piece->location.location.value);

            Action action = {MOVE_PATH, player, piece, 0, operand1, NULL};
            action_space.action_space[action_space.length] = action;
            action_space.length++;
        
    }

    return action_space;
     
}

void bring_to_path(Player *player, Piece *piece, bool verbose)
{
    assert (piece->region == BASE);
    piece->region = PATH; 

    piece->location.location = piece->start_location;
    piece->multiplier = 1;
    player->no_pieces_in_base--;
    player->no_pieces_in_path++;

    //update_map
    //log
    if(verbose)
    {
        printf("%s player moves piece %s to starting point.\n\n", piece->color_name, piece->name, piece->color);
    }
      
}

void move(Player *player, Piece *piece, ModularInt dest, bool verbose)
{
    assert (piece->region == PATH);

    ModularInt current_location = piece->location.location; 
    piece->location.location = dest;
    int steps = modular_add(current_location, dest.value).value;
    //update_map;
    //log
    char direction[18] = " ";
    

    if (piece->direction == -1)
    {
        strcpy(direction, " counter-");
    }

    if (verbose)
    {
        printf("%s moves piece %s from location %d to %d by %d units in%sclockwise direction\n\n", piece->color_name, piece->name, current_location.value, piece->location.location.value, steps, direction);   
    }
    
}

void move_to_home_straight(Player *player, Piece *piece, int steps, bool verbose)
{
    assert (piece->region == PATH);

    piece->region = HOME;
    ModularInt current_location = piece->location.location; 
    piece->location.far_from_home = CELLS_IN_HOME - steps;
    
    if( !piece->location.far_from_home)
    {
        player->no_pieces_in_path--;
        player->no_pieces_in_home++;
    }
    

    //update_map
    //log
    if (verbose)
    { 
        printf("%s moves piece %s from location %d in standard path to %s homepath %d by %d units in home straight\n\n", piece->color_name, piece->name, current_location.value, piece->color_name, (6 - piece->location.far_from_home), steps);
    }
}

void move_in_home_straight(Player *player, Piece *piece, int steps, bool verbose)
{
    assert (piece->region == HOME);

    short current_distance = piece->location.far_from_home;
    piece->location.far_from_home = current_distance - steps;

    //update_map
    //log 
    // printf("rolled = %d", piece->location.far_from_home);

    if (verbose)
    {
        printf("%s moves piece %s from location %s homepath %d to %s homepath %d by %d units in home straight\n\n", piece->color_name, piece->name, piece->color_name, (6 - current_distance), piece->color_name, (6 - piece->location.far_from_home), steps);
    }
}

void capture(Player *player, Piece *piece, Player *target_player, Piece *target_piece, bool verbose)
{
    assert (piece->region = PATH && target_piece->region == PATH);

    ModularInt current_location = target_piece->location.location;
    piece->location = target_piece->location;
    target_piece->location.far_from_home = TOO_FAR;
    target_piece->region = BASE;
    
    target_player->no_pieces_in_base++;
    target_player->no_pieces_in_path--;

    piece->no_of_captures += 1;

    target_piece->no_of_captures = 0;
    target_piece->multiplier = 0;


    if (verbose)
    {
        printf("%s piece %s lands on square %d, captures %s piece %s, and return it to the base\n", piece->color_name, piece->name, current_location.value, target_piece->color_name, target_piece->name);
        printf("%s player now has %d/4 pieces on the board and %d/4 pieces on the base\n\n",target_piece->color_name, player->no_pieces_in_path, player->no_pieces_in_base);
    }
}

// void blocked_by(Player *player, Piece *piece, Block *blocked_piece, int rolled)
// {
//     ModularInt current_location = piece->location.location;
//     int real_steps;
//     ModularInt expected_
//     move(piece, rolled, false);

//     printf("%s piece %s is blocked from moving from %d to %d by %s piece %s", player->color_name, piece->name, current_location.value, piece->location.location.value, piece->color_name, piece->name);

// }

void perform(Player *player, int rolled, Action action)
{    
    switch (action.action)
    {
    case BRING_TO_PATH:
        bring_to_path(player, action.piece,  true);
        break;
    
    case MOVE_PATH:
        move(player, action.piece, action.operand1.standard_steps, true);
        break;

    case MOVE_TO_HOME_S:
        move_to_home_straight(player, action.piece, action.steps, true);
        break;

    case MOVE_IN_HOME_S:
        move_in_home_straight(player, action.piece, action.steps, true);
        break;

    case CAPTURE:
        capture(player, action.piece, action.operand2.player, action.operand1.piece, true);
        break;

    default:
        printf("invalid Action!");
        break;
    }
}

bool roll_logic(Game *game, short current_player_id, short rolled)
{
    Player *player = &PLAYER_AT(game, current_player_id);

    if ((rolled == 6))
    {
        if((player->same_rolls == 2) && (player->prev_rolled == 6))
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

void action_logic(Game *game, Action selected_action)
{
    if (selected_action.action == CAPTURE)
    {
        player_pointer_update(game, selected_action.piece->color);
        // *game->player_pointer = selected_action.operand1.piece->color;
    }
    
}

ActionSpace action_space_filter(ActionSpace action_space, Player *player, short rolled)
{
    return action_space;
}

void print_action_space_(ActionSpace *action_space)
{
    for (short i = 0; i < action_space->length; i++)
    {
        // printf("i = %d\n", ((Piece *)action_space->action_space[i].operand1)->color);
        Action action = action_space->action_space[i];
        printf("{%d} {%s}\n", action.action, action.piece->name);
    }
    
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
        printf("\n\tLEAVE!\n");
        return;
    }

    ActionSpace action_space = generate_action_space(game, current_player_id, rolled);
    ActionSpace constrained_action_space = action_space_filter(action_space, player, rolled);

    print_action_space_(&constrained_action_space);

    if (constrained_action_space.length == 0)
    {
        return;
    }

    Action selected_action = player->perform_strategy(player, action_space);

    

    action_logic(game, selected_action);
    perform(player, rolled, selected_action);

    printf("Played!\n\n");

 
    
}

void play_round(Game *game)
{
    assert (*game->player_pointer == *game->starting_player);
    short played_count  = 0;
    short prev_player = -1;
    do
    {
        prev_player = *game->player_pointer;

        play(game, game->player_pointer);
        printf("start %s; current %s\n", PLAYER_AT(game, *game->starting_player).color_name, PLAYER_AT(game, *game->player_pointer).color_name);

        if (*game->player_pointer != prev_player)
        {
            played_count++;
        }
    } while (played_count < 4);

    
    
    for(short i=0; i < TOTAL_PLAYERS; i++)
    {
        Player *player = &PLAYER_AT(game, i);

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
                printf("Piece %s -> BASE\n", piece_name, BASE);
                break;

            case HOME:
                // short piece_location = player->pieces[i].location.location.value;
                printf("Piece %s -> HOME\n", piece_name);
                break;

            default:
                break;
            }
        }

        printf("Mystery cell not yet unlocked!\n");
    }
    
}

//player's stratagies

Action random_player(Player *player, ActionSpace action_space)
{
    short action_idx = RAND(0, action_space.length);
    return action_space.action_space[action_idx];
}