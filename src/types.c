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


    *game->board = new_board();
    *game->winner = -1;
    *game->player_pointer = -1;
    char player_names[4][6] = {"Yello", "Blue", "Red", "Green"};
    char piece_names[16][3] = {"Y1", "Y2", "Y3", "Y4", "B1", "B2", "B3", "B4", "R1", "R2", "R3", "R4", "G1", "G2", "G3", "G4"};   

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
    piece.multiplier = 0;
    piece.region = BASE;
    piece.location.far_from_home = 255;
    piece.start_location.max = TOTAL_CELLs;
    modular_assign(&piece.start_location, color*13);
    piece.approach = false;
    piece.color = color;
    piece.direction = 1;

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

Board new_board()
{
    Board board;
    board.std_path = (bool *)malloc(sizeof(bool)*TOTAL_PIECES);

    return board;
}
