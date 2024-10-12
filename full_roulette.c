#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

// Create the required types.

typedef struct Bet {
    int amount;
    char type[3];
} Bet;

// Calculates the winnings based off the result.
int payout() {
      
}
// Handles the setting up of the bet- the amount and the type of bet
Bet createBet() {
    Bet newBet;
    char betTypes[13][2][20] = {{"su", "Straight Up"},
                               {"sp", "Split"},
                               {"st", "Street"},
                               {"cb", "Corner Bet"},
                               {"fn", "Five Numbers"},
                               {"ln", "Line"},
                               {"dz", "Dozen"},
                               {"cl", "Column"},
                               {"su", "Street"},
                               {"fh", "First Half"},
                               {"sh", "Second Half"},
                               {"cl", "Colours"},
                               {"oe", "Odd/Even"}};

    printf("\nPlease enter your bet amount (in dollars): ");
    scanf("%d", newBet.amount);
    printf("\nYou're betting $%d. Please enter your bet type. You can choose from: ", betAmount);
    for (int i=0; i< 13; i++) {
        char betTypeName[20];
        char betTypeAb[20];
        strcpy(betTypeName, betTypes[i][1]);
        strcpy(betTypeAb, betTypes[i][0]);
        printf("\n%d. ", i+1);
        printf("%s:", betTypeName);
        printf(" (%s),", betTypeAb);
    }
    printf("Enter your choice code: ");
    char betType[3];
    scanf("%2s", newBet.type);
    return newBet;
}
int game() {
    Bet current_bet = createBet();
}
// Main func
int main() {
    printf("Welcome to Roulette!");
    game();
    
    return 0;
}
