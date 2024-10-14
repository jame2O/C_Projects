// This is absolute arse and doesn't work

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <time.h>

void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
int setBet()
{
    int bet;
    printf("Please enter the amount you'd like to bet: ");
    scanf("%d", &bet);
    clearInputBuffer();
    printf("\nYou're betting £%d. Let's get started!\n", bet);
    return bet;
}

char *setColour()
{
    static char colour[6]; // Static array to hold the color string
    printf("Please enter your colour (red, black or green): ");
    scanf("%s", colour);
    clearInputBuffer();
    return colour; // Return the pointer to the colour array
}

int getWinnings(bool hasWon, int bet, char colour[])
{
    int winnings = 0;
    if (hasWon)
    {
        if (strcmp(colour, "green") == 0)
        {
            winnings = bet * 35;
        }
        else
        {
            winnings = bet * 2;
        }
        printf("Congratulations! You win! Your winnings are: £%d\n", winnings);
    }
    else
    {
        printf("You lose! You won nothing :(\n");
    }
    return winnings; // Return winnings for future use if needed
}

bool playAgain() {
    char playAgain[20];
    printf("\nWhat would you like to do next? (Let it ride! | I'm done): ");
    fgets(playAgain, sizeof(playAgain), stdin); // Use fgets to read input

    // Remove the newline character if present
    playAgain[strcspn(playAgain, "\n")] = '\0';
    if (strcmp(playAgain, "Let it ride!") == 0) {
        return true;
    }
    else
    {
        return false;
    }
    
}
int game(int bet) {
    // Seed the random number generator
    srand(time(NULL));

    // Declare and populate table
    char table[37][6];
    int i = 0; // Start from 0 for better indexing
    char(*tablep)[6] = table;
    strcpy(tablep[0], "green");
    while (i < 36)
    {
        if (i % 2 == 0)
        {
            strcpy(tablep[i + 1], "red");
        }
        else
        {
            strcpy(tablep[i + 1], "black");
        }
        i++;
    }
    int inputBet;
    if (bet == 0) {
        inputBet = setBet();
    } else {
        inputBet = bet;
    }
    char *colour = setColour(); // Correctly receive the color input
    int winnings;
    printf("\nGood luck! Spinning table...");
    Sleep(3000); // Sleep for 3 seconds for effect
    int resultIndex = rand() % 37;
    char *result = tablep[resultIndex];
    printf("\nThe result is: %s\n", result); // Show the result

    if (strcmp(colour, result) == 0)
    {
        winnings = getWinnings(true, inputBet, colour);
    }
    else
    {
        winnings = getWinnings(false, inputBet, colour);
    }
    return winnings;
}
int main()
{
    int totalWinnings = game(0);
    bool restart = true;
    while (restart) {
        restart = playAgain();
        totalWinnings = game(totalWinnings);
    }
    return 0;
}