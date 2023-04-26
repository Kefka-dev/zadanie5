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
        path[strlen(path)-1] = '\0';
        //printf("%s\n", path);
        tmp = fopen(path, "r"); 
        if ( tmp == NULL)
        {
            //printf("file cislo %2d sa nepodarilo otvorit\n", restCount);
            fprintf(stderr, "E1 %s\n", buffer);
            path[PATH_LEN] = '\0';
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
            (*storeHere)[restCount].valid = TRUE;
            fclose(tmp);
            path[PATH_LEN] = '\0';
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

int extractJedloACenu(char* jedloString, char *p_menoJedla, int*p_price)
{
    char *token1, *token2;
    char buffer[MAX_NAME];
    token1 = strtok(jedloString, MEAL_PRICE_DELIM);
    token2 = strtok(NULL, MEAL_PRICE_DELIM);

    if(token1 == NULL || token2 == NULL)
	{
		return 1;
	}

    if (isNumber(token2) == FALSE)
	{
		return 1;
	}

    //sscanf(token1, "%s", p_menoJedla);
    strcpy(p_menoJedla,token1);
    sscanf(token2, "%d", p_price);
    return 0;
}

int main(int argc, const char* argv[])
{
    FILE_NAME *restaurantFiles;
    
    int numberOfRestFiles;
    numberOfRestFiles = getRestNames(argv[1],&restaurantFiles);

    ///////////
    FILE *restRecord;
    char readBuffer[BUFFER_SIZE];
    int lineCount, errCheck, jedloCount =0;
    RESTAURANT db[numberOfRestFiles];
    
    int menuSize = INITIAL_MALLOC_SIZE;
    //going through each restRecord
    for (int i = 0; i < numberOfRestFiles; i++)
    {
        restRecord = fopen(restaurantFiles[i].name, "r");
        //initial menu allocation
        db[i].menu = (MEAL*)malloc(menuSize*sizeof(MEAL));
        if (restRecord != NULL)
        {
            // printf("SUCCESS\n");
            //reading each line of a restRecord file
            lineCount = 1;
            jedloCount = 0;
            while (strcmp(fgets(readBuffer, BUFFER_SIZE, restRecord), "\n"))
            {
                readBuffer[strlen(readBuffer)-1] = '\0';
                //printf("i:%d %s\n", i, readBuffer);
                if (lineCount == 1)
                {
                    strcpy(db[i].name, readBuffer);
                }
                //overenie ci je pozicia vo validnom formate,
                //ak nie tak sa vypise chyba a nastavi flag ze file cislo i nie je valid file
                else if (lineCount == 2)
                {
                    if(extractPosition(readBuffer, &db[i].pos.x, &db[i].pos.y) == FAILED)
                    {
                        fprintf(stderr, "E2 %s\n", (restaurantFiles[i].name)+PATH_LEN);
                        restaurantFiles[i].valid = FALSE;
                        break;
                    }
                }
                else
                {
                    if(jedloCount == menuSize)
                    {
                        menuSize = menuSize + 16;
                        db[i].menu = (MEAL*)realloc(db[i].menu, menuSize*sizeof(MEAL));
                    }
                    if(extractJedloACenu(readBuffer,db[i].menu[jedloCount].name, &db[i].menu[jedloCount].price) == FAILED)
                    {
                        fprintf(stderr, "E2 %s\n", (restaurantFiles[i].name)+PATH_LEN);
                        restaurantFiles[i].valid = FALSE;
                        break;
                    }
                    jedloCount++;
                }
                lineCount++;
            }
            db[i].n = jedloCount;
        }
        else
        {
            printf("FILE SA NEOTVORIL\n");
            continue;
        }
        

        fclose(restRecord);
    }

    printf("{\n%s\"Restaurants\":\n%s[\n", JSON_OFFSET, JSON_OFFSET);
    int invalidAmount=0;
    for (int i = 0; i < numberOfRestFiles; i++)
    {
        if (restaurantFiles[i].valid != TRUE)
        {
            invalidAmount++;
        }
        else
        {
            printf("%s%s{\n",JSON_OFFSET, JSON_OFFSET);
            printf("%s%s%s\"Name\": \"%s\",\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET,db[i].name);
            printf("%s%s%s\"Position\":\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET);
            printf("%s%s%s{\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET);
            printf("%s%s%s%s\"X\": %d,\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET, JSON_OFFSET, db[i].pos.x);
            printf("%s%s%s%s\"Y\": %d\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET, JSON_OFFSET, db[i].pos.y);
            printf("%s%s%s},\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET);
            printf("%s%s%s\"Menu\":\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET);
            printf("%s%s%s[\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET);
            for (int j = 0; j < db[i].n; j++)
            {
                printf("%s%s%s%s{\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET,JSON_OFFSET);
                printf("%s%s%s%s%s\"Name\": \"%s\",\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET,JSON_OFFSET,JSON_OFFSET, db[i].menu[j].name);
                printf("%s%s%s%s%s\"Price\": %d\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET,JSON_OFFSET,JSON_OFFSET, db[i].menu[j].price);
                if (j==(db[i].n)-1)
                {
                    printf("%s%s%s%s}\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET,JSON_OFFSET);
                    continue;
                }
                printf("%s%s%s%s},\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET,JSON_OFFSET);
            }
            printf("%s%s%s]\n", JSON_OFFSET,JSON_OFFSET,JSON_OFFSET);
            //printf("i=%d counter =%d\n",i, numberOfRestFiles-invalidAmount+1);
            if (i == numberOfRestFiles-invalidAmount+1)
            {
                printf("%s%s}\n",JSON_OFFSET, JSON_OFFSET);
                continue;
            }
            printf("%s%s},\n",JSON_OFFSET, JSON_OFFSET);

        }
    }
    printf("%s]\n}\n", JSON_OFFSET);
    

    for (int i = 0; i < numberOfRestFiles; i++)
    {
        free(db[i].menu);
    }
    free(restaurantFiles);
    return 0;
}
