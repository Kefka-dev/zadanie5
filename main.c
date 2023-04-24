#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 1000
// DATOVE TYPY
// Poloha na mape
typedef struct position {
    int x;
    int y;
} POSITION;

// Jedlo
typedef struct meal {
    char name[MAX_NAME + 1]; // nazov
    int price; // cena v EUR
} MEAL;

// Restauracia
typedef struct restaurant {
    char name[MAX_NAME + 1]; // nazov
    POSITION pos; // poloha
    int n; // aktualny pocet poloziek ponuky
    MEAL *menu; // databaza poloziek ponuky
} RESTAURANT;

void initMenu(RESTAURANT *restaurant)
{
    restaurant->menu = (MEAL*)malloc(16*sizeof(MEAL));
    restaurant->n = 0;
}

int main(int argc, char* argv[])
{
    RESTAURANT *db;

    printf("%s\n", argv[1]);
    for (int i = 0; i < strlen(argv[1]); i++)
    {
        printf("%c\n", argv[1][i]);
    }
    
    return 0;
}
