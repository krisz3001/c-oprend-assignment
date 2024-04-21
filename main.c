#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "poems.txt"
#define POEM_LENGTH 256

int getChoice(char *prompt)
{
    char line[POEM_LENGTH];

    int chosen = -1;
    int valid = -1;

    while (valid < 1)
    {
        printf("\n%s", prompt);
        char *res = fgets(line, sizeof(line), stdin);
        valid = sscanf(line, "%d", &chosen);
        clearerr(stdin);
    }
    return chosen;
}

int showMenu()
{
    printf("1. List poems\n");
    printf("2. Add a poem\n");
    printf("3. Delete a poem\n");
    printf("4. Edit a poem\n");
    printf("5. Exit\n");

    return getChoice("Choose an action: ");
}

void listPoems()
{
    system("clear");
    printf("--- Stored poems ---\n\n");
    FILE *f = fopen(FILENAME, "ab+");
    if (f == NULL)
    {
        printf("Error opening the file!\n");
        return;
    }
    char line[POEM_LENGTH];
    int lineCount = 0;
    while (fgets(line, sizeof(line), f))
    {
        printf("    %d. %s", lineCount + 1, line);
        lineCount++;
    }
    fclose(f);

    if (lineCount == 0)
    {
        printf("    (none)\n");
    }

    printf("\n");
}

void addPoem()
{
    system("clear");
    printf("--- Adding a poem ---\n\n");

    FILE *f = fopen(FILENAME, "a");
    if (f == NULL)
    {
        printf("Error opening the file!\n");
        return;
    }

    char line[POEM_LENGTH];
    do
    {
        printf("\nEnter the poem: ");
        clearerr(stdin);
    } while (fgets(line, sizeof(line), stdin) == NULL);

    fprintf(f, "%s", line);
    fclose(f);
}

void deletePoem()
{
    system("clear");
    printf("--- Deleting a poem ---\n\n");

    FILE *f = fopen(FILENAME, "ra");
    if (f == NULL)
    {
        printf("Error opening the file!\n");
        return;
    }

    int capacity = 2;
    char **lines = malloc(capacity * sizeof(char *));

    char line[POEM_LENGTH];
    int lineCount = 0;
    while (fgets(line, sizeof(line), f))
    {
        if (lineCount >= capacity)
        {
            capacity *= 2;
            lines = realloc(lines, capacity * sizeof(char *));
        }
        lines[lineCount] = malloc(sizeof(line));
        strcpy(lines[lineCount], line);
        printf("    %d. %s", lineCount + 1, line);
        lineCount++;
    }
    fclose(f);

    if (lineCount == 0)
    {
        printf("    No poems to delete!\n\n");
        return;
    }

    int chosen = getChoice("Choose a poem to delete: ");

    f = fopen(FILENAME, "w");
    if (f == NULL)
    {
        printf("Error opening the file!\n");
        return;
    }

    for (int i = 0; i < lineCount; i++)
    {
        if (i != chosen - 1)
        {
            fprintf(f, "%s", lines[i]);
        }
        else
        {
            system("clear");
            printf("Poem deleted!\n\n");
        }
    }

    fclose(f);

    for (int i = 0; i < lineCount; i++)
    {
        free(lines[i]);
    }
    free(lines);
}

void editPoem()
{
    system("clear");
    printf("--- Editing a poem ---\n\n");

    FILE *f = fopen(FILENAME, "ra");
    if (f == NULL)
    {
        printf("Error opening the file!\n");
        return;
    }

    int capacity = 2;
    char **lines = malloc(capacity * sizeof(char *));

    char line[POEM_LENGTH];
    int lineCount = 0;
    while (fgets(line, sizeof(line), f))
    {
        if (lineCount >= capacity)
        {
            capacity *= 2;
            lines = realloc(lines, capacity * sizeof(char *));
        }
        lines[lineCount] = malloc(sizeof(line));
        strcpy(lines[lineCount], line);
        printf("    %d. %s", lineCount + 1, line);
        lineCount++;
    }
    fclose(f);

    if (lineCount == 0)
    {
        printf("    No poems to edit!\n\n");
        return;
    }

    int chosen = getChoice("Choose a poem to edit: ");

    f = fopen(FILENAME, "w");
    if (f == NULL)
    {
        printf("Error opening the file!\n");
        return;
    }

    for (int i = 0; i < lineCount; i++)
    {
        if (i != chosen - 1)
        {
            fprintf(f, "%s", lines[i]);
        }
        else
        {
            do
            {
                printf("\nEnter the edited poem: ");
                clearerr(stdin);
            } while (fgets(line, sizeof(line), stdin) == NULL);

            fprintf(f, "%s", line);
            system("clear");
            printf("Poem edited!\n\n");
        }
    }

    fclose(f);

    for (int i = 0; i < lineCount; i++)
    {
        free(lines[i]);
    }
    free(lines);
}

int main()
{
    system("clear");
    int chosen = -1;
    do
    {
        chosen = showMenu();
        switch (chosen)
        {
        case 1:
            listPoems();
            break;
        case 2:
            addPoem();
            break;
        case 3:
            deletePoem();
            break;
        case 4:
            editPoem();
            break;
        case 5:
            return 0;
        default:
            system("clear");
            printf("Wrong input!\n\n");
            break;
        }
    } while (1);
}