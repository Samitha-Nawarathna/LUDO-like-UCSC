#include "logic.h"
#include <stdlib.h>

void start_game(Game *game)
{
    int value[4];
    int player_ids[4] = {0, 1, 2, 3};

    for (short i = 0; i < TOTAL_PLAYERS; i++)
    {
        value[i] = roll_die(&PLAYER_AT(game, i));
        // printf("%d -> %d\n",i, value[i]);
    }

    for (short i = 0; i < 2; i++)
    {
        for (short j = 0; j < TOTAL_PLAYERS - 1; j++)
        {
            if (value[j] > value[j+1])
            {
                int temp = player_ids[j];
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


int roll_die(Player *player)
{
    return rand()%6 + 1;
}

void player_pointer_next(Game *game)
{
    *game->player_pointer += 1;
    *game->player_pointer = *game->player_pointer%4;
}

ModularInt get_destination(Piece *piece, int rolled)
{
    assert (piece->region == PATH);
    return modular_add(piece->location.location, rolled);
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

ActionSpace generate_action_space(Game *game, int player_id, int rolled)
{
    // int *rolled_ptr = (int *)malloc(sizeof(int)); //move rolled value to heap temporary so we can access value even from outside of this function
    // *rolled_ptr = rolled;   //and must be deallocated after perform a action!!!

    ActionSpace action_space;
    action_space.action_space = (Action *)malloc(sizeof(Action)*TOTAL_PIECES_PLAYER);
    action_space.length = 0;

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

                Action action = {BRING_TO_PATH, operand1, NULL};
                action_space.action_space[action_space.length] = action;
                action_space.length++;
                continue;
            }
            continue;
        }        

        //moving in home straight

        if ((piece->region == HOME))
        {
            int to_move = piece->multiplier*rolled;
            if (to_move && (piece->location.far_from_home >= to_move))
            {
                Operand operand1;
                operand1.piece = piece;
                
                Operand operand2;
                operand2.rolled = rolled;

                Action action = {MOVE_IN_HOME_S, operand1, operand2};
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
                int to_move = modular_add(destination, -piece_approach.value).value;

                Operand operand1;
                operand1.piece = piece;
                
                Operand operand2;
                operand2.rolled = to_move;

                Action action = {MOVE_TO_HOME_S, operand1, operand2};
                action_space.action_space[action_space.length] = action;
                action_space.length++;
                continue;                
            }
            else if(piece->direction == -1 && modular_is_between(destination, current_location, piece_approach))
            {
                int to_move = modular_add(piece_approach, -destination.value).value;
                Operand operand1;
                operand1.piece = piece;
                
                Operand operand2;
                operand2.rolled = to_move;

                Action action = {MOVE_TO_HOME_S, operand1, operand2};
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
            operand1.piece = piece;
            
            Operand operand2;
            operand2.piece = target_piece;

            Action action = {CAPTURE, operand1, operand2};            

            action_space.action_space[action_space.length] = action;
            action_space.length++;
            continue;    

        }
            

        //move in standard path
            Operand operand1;
            operand1.piece = piece;
            
            Operand operand2;
            operand2.rolled = rolled;

            Action action = {MOVE_PATH, operand1, operand2};
            action_space.action_space[action_space.length] = action;
            action_space.length++;
        
    }

    return action_space;
     
}

void bring_to_path(Piece *piece)
{
    assert (piece->region == BASE);
    piece->region = PATH; 

    piece->location.location = piece->start_location;

    //update_map
    //log
    printf("%s player moves piece %s to starting point.\n\n", piece->color_name, piece->name, piece->color);  
}

void move(Piece *piece, int rolled)
{
    assert (piece->region == PATH);

    ModularInt current_location = piece->location.location; 
    piece->location.location = modular_add(piece->location.location, rolled);

    //update_map;
    //log
    char direction[18] = " ";

    if (piece->direction == -1)
    {
        strcpy(direction, " counter-");
    }
    printf("%s moves piece %s from location %d to %d by %d units in%sclockwise direction\n\n", piece->color_name, piece->name, current_location.value, piece->location.location.value, rolled, direction);
}

void move_to_home_straight(Piece *piece, int rolled)
{
    assert (piece->region == PATH);

    piece->region = HOME;
    ModularInt current_location = piece->location.location; 
    piece->location.far_from_home = CELLS_IN_HOME - rolled;

    //update_map
    //log 
    printf("%s moves piece %s from location %d in standard path to %s homepath %d by %d units in home straight\n\n", piece->color_name, piece->name, current_location.value, piece->color_name, (6 - piece->location.far_from_home), rolled);}

void move_in_home_straight(Piece *piece, int rolled)
{
    assert (piece->region == HOME);

    int current_distance = piece->location.far_from_home;
    piece->location.far_from_home = current_distance - rolled;

    //update_map
    //log 
    // printf("rolled = %d", piece->location.far_from_home);

    printf("%s moves piece %s from location %s homepath %d to %s homepath %d by %d units in home straight\n\n", piece->color_name, piece->name, piece->color_name, (6 - current_distance), piece->color_name, (6 - piece->location.far_from_home), rolled);

}

void capture(Piece *piece, Piece *target_piece)
{
    assert (piece->region = PATH && target_piece->region == PATH);
    ModularInt current_location = target_piece->location.location;
    piece->location = target_piece->location;
    target_piece->location.far_from_home = TOO_FAR;

    piece->no_of_captures += 1;

    target_piece->no_of_captures = 0;
    target_piece->multiplier = 0;

    printf("%s piece %s lands on square %d, captures %s piece %s, and return it to the base\n", piece->color_name, piece->name, current_location.value, target_piece->color_name, target_piece->name);
    printf("%s player now has %d/4 pieces on the board and %d/4 pieces on the base\n\n",target_piece->color_name, 2, TOTAL_PIECES_PLAYER - 2);

}

void perform(Action action)
{    
    switch (action.action)
    {
    case BRING_TO_PATH:
        bring_to_path(action.operand1.piece);
        break;
    
    case MOVE_PATH:
        move(action.operand1.piece, action.operand2.rolled);
        break;

    case MOVE_TO_HOME_S:
        move_to_home_straight(action.operand1.piece, action.operand2.rolled);
        break;

    case MOVE_IN_HOME_S:
        move_in_home_straight(action.operand1.piece, action.operand2.rolled);
        break;

    case CAPTURE:
        capture(action.operand1.piece, action.operand2.piece);
        break;

    default:
        printf("invalid Action!");
        break;
    }
}