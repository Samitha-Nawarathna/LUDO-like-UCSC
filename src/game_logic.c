#include <stdio.h>
#include <stdlib.h>
#include "game_logic.h"

void init_game()
{
    Board board = *init_board();

    for (short i = 0; i < TOTAL_PLAYERS; i++)
    {
        for (short j = 0; j < TOTAL_PIECES_PLAYER; j++)
        {
            Piece *piece = init_piece(i);
            PICECE_AT(pieceArr, i, j) = piece;
            // pieceArr[i*TOTAL_PIECES_PLAYER + j] = piece;
        }
        
        Player *player = init_player(i, *(pieceArr + i*TOTAL_PIECES_PLAYER));
        playerArr[i] = player;
    }
    
}