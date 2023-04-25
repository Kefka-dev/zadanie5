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
int getRestNames(const char* dbFileName, FILE_NAME **storeHere)
{
    FILE *db, *tmp;
    db = fopen(dbFileName, "r");

    char buffer[MAX_NAME];
    char path[MAX_NAME+PATH_LEN]; 
    strcpy(path, PATH);
    
    int SizeRestaurantFiles= INITIAL_MALLOC_SIZE;
    (*storeHere) = (FILE_NAME *)malloc(SizeRestaurantFiles*sizeof(FILE_NAME));
    int restCount=0;
    
    while (fgets(buffer, MAX_NAME, db) != NULL)
    {
        if(buffer[strlen(buffer)-1] == '\n')  
            buffer[strlen(buffer)-1] = '\0';
        strcat(path, buffer);
        //printf("%s\n", path);
        tmp = fopen(path, "r"); 
        //
        if ( tmp == NULL)
        {
            //printf("file cislo %2d sa nepodarilo otvorit\n", restCount);
            fprintf(stderr, "E1 %s\n", buffer);
            path[PATH_LEN-1] = '\0';
            continue;
        }
        else
        {
            if (restCount == SizeRestaurantFiles)
            {
                SizeRestaurantFiles = SizeRestaurantFiles + 16;
                (*storeHere) = (FILE_NAME*)realloc((*storeHere), SizeRestaurantFiles*sizeof(FILE_NAME));
            }
            
            strcpy((*storeHere)[restCount].name, path);
            //printf("file cislo %2d otvoreny\n", restCount);
            fclose(tmp);
            path[PATH_LEN-1] = '\0';
        }
        restCount++;
    }
    return restCount;
}

    
    return 0;
}
