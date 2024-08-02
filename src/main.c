#include <stdio.h>
#include "game_logic.h"
// #include "types.h"



int main()
{
    init_game();

    for (short i = 0; i < TOTAL_PLAYERS; i++)
    {
       printf("player %d: color = %d\n", i, playerArr[i]->color);
       for (short j = 0; j < TOTAL_PIECES; j++)
       {
            printf("\t");
            printf("Player %d: piece %d: color = %d\n", i, j, PICECE_AT(pieceArr, i, j)->color);
       }
       
    }
    

    return 0;
}