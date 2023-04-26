#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 1000
#define BUFFER_SIZE 2048
#define INITIAL_MALLOC_SIZE 16
//path na miesto kde su ulozene subory s restauraciami
#define PATH "db/"
#define PATH_LEN 3
#define TRUE 1
#define FALSE 0
#define FAILED 1
#define POS_DELIM ","
#define MEAL_PRICE_DELIM ","
#define JSON_OFFSET "   "

typedef struct fileName {
    char name[MAX_NAME+PATH_LEN];
    int valid;
}FILE_NAME;

// Poloha na mape
typedef struct position {
    int x;
    int y;
} POSITION;

// Jedlo
typedef struct meal {
    char name[MAX_NAME]; // nazov
    int price; // cena v EUR
} MEAL;

// Restauracia
typedef struct restaurant {
    char name[MAX_NAME]; // nazov
    POSITION pos; // poloha
    int n; // aktualny pocet poloziek ponuky
    MEAL *menu; // databaza poloziek ponuky
} RESTAURANT;

//z db suboru zoberie nazvy suborov s restauraciami, vrati pocet existujucich suborov
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
//return 1 ak je string cislo, inac 0
int isNumber(char *string)
{
	for (int i = 0; i < strlen(string); i++)
	{
		if(isdigit(string[i]) == FALSE)
		{
			return 0;
		}
	}
	return 1;
}

//return 1 ak nastala chybova sutuacia
int extractPosition(char* positionString, int* posX, int* posY){
	char *token1, *token2;
	token1 = strtok(positionString, POS_DELIM);
	token2 = strtok(NULL, POS_DELIM);

	if(token1 == NULL || token2 == NULL)
	{
		return 1;
	}
	
	if (isNumber(token1) == FALSE || isNumber(token2) == FALSE)
	{
		return 1;
	}
	
	sscanf(token1, "%d", posX);
	sscanf(token2, "%d", posY);
	
	return 0;
}


int main(int argc, const char* argv[])
{
    FILE_NAME *restaurantFiles;
    
    int numberOfRestaurants;
    numberOfRestaurants = getRestNames(argv[1],&restaurantFiles);
    // for (int i = 0; i < numberOfRestaurants; i++)
    // {
    //     printf("%2d %20s\n", i, restaurantFiles[i].name);
    // }

    return 0;
}
