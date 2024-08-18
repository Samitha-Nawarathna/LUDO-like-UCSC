#include "types.h"

ModularInt modular_new(short value, short max)
{
    ModularInt mod_int = {value%max, max};
    return mod_int;
}

void modular_assign(ModularInt *a, short value)
{
    short temp = value%a->max;

    if (temp < 0)
    {
        a->value = (a->max + temp);
    }else
    {
        a->value = temp;
    }
}

ModularInt modular_add(ModularInt a, short b)
{
    // assert (a.max == b.max);
    ModularInt c = modular_new(0, a.max);

    modular_assign(&c, a.value + b);
    return c;
}

bool modular_is_between(ModularInt lower, ModularInt upper, ModularInt c) {

    assert ((lower.max == upper.max) && (lower.max == c.max));

    if (lower.value < upper.value) {
        return lower.value < c.value && c.value < upper.value;
    } else {
        return lower.value < c.value || c.value < upper.value;
    }
}

bool modular_is_between_directional(ModularInt lower, ModularInt upper, ModularInt c, bool direction)
{
    if (direction == 1)
    {
        return modular_is_between(lower, upper, c);
    }else{
        return modular_is_between(upper, lower, c);
    }
    
}

ModularInt MODULAR_SUB(ModularInt a, ModularInt b, short direction)
{
    return ((direction) == 1) ? modular_add((a), -1*(b).value): modular_add((b), -1*(a).value);
}

// short mod_value(ModularInt a)
// {
//     short temp = a.value%a.max;

//     if (temp < 0)
//     {
//         return a.max + temp;
//     }

//     return temp;
// }

void new_game(Game *game)
{   
    game->board = (Board *)malloc(sizeof(Board));
    game->playerArr = (Player *)malloc(sizeof(Player)*TOTAL_PLAYERS);
    game->pieceArr = (Piece *)malloc(sizeof(Piece)*TOTAL_PIECES);
    game->player_pointer = (short *)malloc(sizeof(short));
    game->starting_player = (short *)malloc(sizeof(short));
    game->winner = (short *)malloc(sizeof(short));
    game->blockArr = (Block **)malloc(sizeof(Block *)*MAX_BLOCK_COUNT);
    game->mystery_cell = (ModularInt *)malloc(sizeof(ModularInt));

    *game->board = new_board();
    *game->winner = -1;
    *game->player_pointer = -1;
    // *game->mystery_cell = NOT_ON_BOARD;
    char player_names[4][6] = {"Yello", "Blue", "Red", "Green"};
    char piece_names[16][3] = {"Y1", "Y2", "Y3", "Y4", "B1", "B2", "B3", "B4", "R1", "R2", "R3", "R4", "G1", "G2", "G3", "G4"};   

    for (short block_idx = 0; block_idx < MAX_BLOCK_COUNT; block_idx++)
    {
        BLOCK_AT(game, block_idx) = NULL;
        
    }
    

    for (short i = 0; i < TOTAL_PLAYERS; i++)
    {
        for (short j = 0; j < TOTAL_PIECES_PLAYER; j++)
        {
            PIECE_AT(game, i, j) = new_piece(i);
            strcpy(PIECE_AT(game, i, j).name, piece_names[i*TOTAL_PIECES_PLAYER + j]);
            strcpy(PIECE_AT(game, i, j).color_name, player_names[i]);
        }
        
        PLAYER_AT(game, i) = new_player(i, &PIECE_AT(game, i, 0), random_player);
        strcpy(PLAYER_AT(game, i).color_name, player_names[i]);
    }

    
}

Piece new_piece(Color color)
{
    Piece piece;
    piece.no_of_captures = 0;
    piece.multiplier = 1;
    piece.region = BASE;
    piece.location.location = modular_add(START_AT(color), -1);
    piece.start_location.max = TOTAL_CELLs;
    modular_assign(&piece.start_location, color*13);
    piece.approach = false;
    piece.color = color;
    piece.direction = 1;
    piece.block_id = NULL_BLOCK;
    // piece.parent_block = -1;

    return piece; 
}

Player new_player(Color color, Piece *pieces, Action (* perform_strategy)(Player *self, ActionSpace action))
{
    Player player;
    player.color = color;
    player.no_pieces_in_home = 0;
    player.no_pieces_in_base = 4;
    player.no_pieces_in_path = 0;
    player.pieces = pieces;
    player.perform_strategy = perform_strategy;
    player.same_rolls = 0;

    return player; 
}

short new_block(Game *game, Color player_color, short piece1_idx, short piece2_idx)
{
    Piece *piece1 = &PIECE_AT(game, player_color, piece1_idx); // the block at the cell that block will be created!
    Piece *piece2 = &PIECE_AT(game, player_color, piece2_idx);
    
    assert(piece1->region == PATH);
    assert(piece1->color == piece2->color);

    piece2->location = piece1->location;
    piece2->region = PATH; 

    Block* block = (Block *)malloc(sizeof(Block));
    block->pieces = (bool *)calloc(TOTAL_PIECES_PLAYER, sizeof(bool));    
    
    block->color = player_color;
    block->no_of_pieces = 2;
    block->pieces[piece1_idx] = true;
    block->pieces[piece2_idx] = true;
    block->direction = piece1->direction;
    block->movable = false;
    block->multiplier = 1;
    block->location = piece1->location.location;

    if (piece1->direction != piece2->direction)
    {
        block->movable = true;

        // short block_dist = MODULAR_SUB(block->location, APPROACH_CELL(block->color), block->direction).value; 
        // short piece2_dist = MODULAR_SUB(piece2->location.location, APPROACH_CELL(piece2->color), piece2->direction).value; 

        // if (piece2_dist > block_dist)
        // {
        //     block->direction = piece2->direction;
        // }

        if (block->location.value < TOTAL_CELLs/52)
        {
            block->direction = COUNTERCLOCKWISE;
        }
        
        
    }
    
    for (short block_idx = 0; block_idx < MAX_BLOCK_COUNT; block_idx++)
    {
        if (BLOCK_AT(game, block_idx) == NULL)
        {
            piece1->block_id = block_idx;
            piece2->block_id = block_idx;

            BLOCK_AT(game, block_idx) = block;

            printf("BLOCK!\n");
            return block_idx;
        }
        
    }


}

void append_to_block(Game *game, short player_color, short piece_idx, short block_idx)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);
    Block *block = BLOCK_AT(game, block_idx);
    assert(piece->color == block->color);

    piece->block_id = block_idx;
    piece->multiplier = block->multiplier;
    piece->location.location = block->location;
 
    block->pieces[piece_idx] = true;
    block->no_of_pieces++;

    if (block->movable == false)
    {
        if (block->direction != piece->direction)
        {
            block->movable = true;

            // short block_dist = MODULAR_SUB(block->location, APPROACH_CELL(block->color), block->direction).value;
            // short piece_dist = MODULAR_SUB(piece->location.location, APPROACH_CELL(piece->color), piece->direction).value; 
            
            // if (piece_dist > block_dist)
            // {
            //     block->direction = piece->direction;
            // }
            if (block->location.value < TOTAL_CELLs/52)
            {
                block->direction = COUNTERCLOCKWISE;
            }
        
        }
        
    }
    



}
void _remove_from_block(Game *game, short player_color, short piece_idx)
{
    Piece *piece = &PIECE_AT(game, player_color, piece_idx);

    assert(piece->block_id != NULL_BLOCK);

    Block *block = BLOCK_AT(game, piece->block_id);

    block->no_of_pieces--;
    piece->block_id = NULL_BLOCK;
    block->pieces[piece_idx] = false;

    if (block->no_of_pieces == 0)
    {
        game->blockArr[piece->block_id] = NULL;
        free(block);
    }

    if (block->movable)
    {
        bool same_dirs = true;
        short curr_direction = 0;

        for (short player_idx = 0; player_idx < TOTAL_PIECES_PLAYER; player_idx++)
        {
            if (block->pieces[player_idx])
            {
                if (!curr_direction)
                {
                    curr_direction = PIECE_AT(game, player_color, piece_idx).direction;
                }else{
                    if(curr_direction != PIECE_AT(game, player_color, piece_idx).direction)
                    {
                        same_dirs = false;
                        break;
                    }
                }
                
            }
                    
        }

        if (same_dirs)
        {
            block->movable = false;
            block->direction = curr_direction;
        }
    }
       
}


ActionSpace new_actionspace()
{
    ActionSpace actionspace;
    actionspace.action_space = (Action *)malloc(sizeof(Action)*6);
    actionspace.length = 0;

    return actionspace;
}

// Block *new_block(Piece *piece1, Piece *piece2)
// {
//     assert (piece1->color == piece2->color);

//     Block *block = (Block *)malloc(sizeof(Block));
//     block->color = piece1->color;
//     block->location = piece2->location.location;
//     piece1->location = piece2->location;
//     block->no_of_pieces = 2;

//     append_piece(block, piece1);
//     append_piece(block, piece2);

//     if (piece1->direction == piece2->direction)
//     {
//         block->movable = false;
//         return block;
//     }

//     block->movable = true;
//     short dist1 = MODULAR_SUB(piece1->location.location, APPROACH_CELL(piece1->color), piece1->direction).value;
//     short dist2 = MODULAR_SUB(piece2->location.location, APPROACH_CELL(piece2->color), piece2->direction).value;

//     if (dist1 > dist2)
//     {
//         block->direction = piece1->direction;
//         return block;
//     }
    
//     block->direction = piece2->direction;
//     return block; 

    
// }

Board new_board()
{
    Board board;
    board.std_path = (bool *)malloc(sizeof(bool)*TOTAL_PIECES);

    return board;
}
