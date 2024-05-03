#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <wait.h>
#include <errno.h>

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

void freeLines(char **lines, int lineCount)
{
    for (int i = 0; i < lineCount; i++)
    {
        free(lines[i]);
    }
    free(lines);
}

void addUsedPoem(char ***poems, int *capacity, int *nextIndex, char *poem)
{
    printf("Capacity: %d, Next index: %d\n", *capacity, *nextIndex);
    if (*nextIndex >= *capacity)
    {
        *capacity *= 2;
        *poems = realloc(*poems, *capacity * sizeof(char *));
    }

    (*poems)[*nextIndex] = malloc(sizeof(poem));
    strcpy((*poems)[*nextIndex], poem);
    (*nextIndex)++;
}

int isPoemUsed(char ***poems, int *nextIndex, char *poem)
{
    for (int i = 0; i < *nextIndex; i++)
    {
        if (strcmp(poem, (*poems)[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int showMenu()
{
    printf("1. List poems\n");
    printf("2. Add a poem\n");
    printf("3. Delete a poem\n");
    printf("4. Edit a poem\n");
    printf("5. Sprinkle\n");
    printf("6. Exit\n");

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
        freeLines(lines, lineCount);
        return;
    }

    int chosen = getChoice("Choose a poem to delete: ");

    f = fopen(FILENAME, "w");
    if (f == NULL)
    {
        printf("Error opening the file!\n");
        freeLines(lines, lineCount);
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

    freeLines(lines, lineCount);
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
        freeLines(lines, lineCount);
        return;
    }

    int chosen = getChoice("Choose a poem to edit: ");

    f = fopen(FILENAME, "w");
    if (f == NULL)
    {
        printf("Error opening the file!\n");
        freeLines(lines, lineCount);
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

    freeLines(lines, lineCount);
}

void signalHandler(int signal)
{
    printf("Child arrived!\n");
}

struct Message
{
    long mtype;
    char mtext[POEM_LENGTH];
};

void sprinke(char ***usedPoems, int *storeCapacity, int *nextIndex)
{
    system("clear");
    signal(SIGUSR1, signalHandler);

    char *children[4] = {"Fiú1", "Fiú2", "Fiú3", "Fiú4"};
    int size = sizeof(children) / sizeof(char *);
    srand(time(NULL));
    int selected = rand() % size;
    printf("Selected child: %s\n", children[selected]);

    // PIPE
    int pipefd[2];
    char *msg = "";
    if (pipe(pipefd) == -1)
    {
        perror("Error opening the pipe!");
        exit(EXIT_FAILURE);
    }

    // MESSAGE QUEUE

    key_t key = ftok("nyuszimama", 3556);
    int mqueue = msgget(key, 0666 | IPC_CREAT);
    if (mqueue < 0)
    {
        perror("msgget");
        return;
    }
    else
    {
        printf("Message queue created, Key: %i\n", key);
    }

    pid_t pid = fork();

    if (pid > 0)
    {
        // Parent
        printf("Child sent to Barátfa!\n");
        close(pipefd[0]);
        pause();

        // Choosing 2 random poems

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
            lineCount++;
        }
        fclose(f);

        if (lineCount < 2 || lineCount - *nextIndex < 2)
        {
            printf("There are not enough poems to choose from!\n");
            freeLines(lines, lineCount);
            int stop = -1;
            write(pipefd[1], &stop, sizeof(int));
            close(pipefd[1]);
            return;
        }

        printf("%d poems found!\n", lineCount);

        // Select two different poems
        int first;
        int second;
        do
        {
            first = rand() % lineCount;
            second = rand() % lineCount;
        } while (second == first || isPoemUsed(usedPoems, nextIndex, lines[first]) || isPoemUsed(usedPoems, nextIndex, lines[second]));

        char *firstPoem = lines[first];
        char *secondPoem = lines[second];

        int length = strlen(firstPoem) + 1;
        write(pipefd[1], &length, sizeof(int));
        write(pipefd[1], firstPoem, strlen(firstPoem) + 1);

        length = strlen(secondPoem) + 1;
        write(pipefd[1], &length, sizeof(int));
        write(pipefd[1], secondPoem, strlen(secondPoem) + 1);

        close(pipefd[1]);

        wait(NULL);

        // Receive the chosen poem
        struct Message message;
        int status = msgrcv(mqueue, &message, sizeof(struct Message), 1, 0);
        if (status < 0)
        {
            perror("msgrcv");
        }
        else
        {
            printf("Chosen poem: %s\n", message.mtext);
            addUsedPoem(usedPoems, storeCapacity, nextIndex, message.mtext);
        }

        freeLines(lines, lineCount);
        printf("Sprinkling done!\n");
    }
    else
    {
        // Child
        kill(getppid(), SIGUSR1);
        sleep(1);
        close(pipefd[1]);

        char poems[2][POEM_LENGTH];

        int length = 0;
        read(pipefd[0], &length, sizeof(int));
        if (length == -1) // Will not get poems because there are no two unused
        {
            close(pipefd[0]);
            exit(0);
        }
        read(pipefd[0], poems[0], length);
        poems[0][length - 2] = '\0';
        printf("\nFirst poem: %s\n", poems[0]);

        read(pipefd[0], &length, sizeof(int));
        read(pipefd[0], poems[1], length);
        poems[1][length - 2] = '\0';
        printf("Second poem: %s\n", poems[1]);

        srand(time(NULL));
        int chosen = rand() % 2;

        close(pipefd[0]);

        // Send back the chosen poem

        struct Message message;
        message.mtype = 1;
        strcpy(message.mtext, poems[chosen]);
        msgsnd(mqueue, &message, sizeof(struct Message), 0);

        printf("%s\nSzabad-e locsolni?\n\n", poems[chosen]);

        exit(0);
    }
}

int main()
{
    system("clear");
    int chosen = -1;

    int capacity = 2;
    int nextIndex = 0;
    char **usedPoems = malloc(sizeof(char *) * capacity);

    for (int i = 0; i < nextIndex; i++)
    {
        printf("%s\n", usedPoems[i]);
    }

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
            sprinke(&usedPoems, &capacity, &nextIndex);
            printf("Capacity: %d, Next index: %d\n", capacity, nextIndex);
            for (int i = 0; i < nextIndex; i++)
            {
                printf("Stored: %s\n", usedPoems[i]);
            }

            break;
        case 6:
            return 0;
        default:
            system("clear");
            printf("Wrong input!\n\n");
            break;
        }
    } while (1);

    freeLines(usedPoems, nextIndex);
}