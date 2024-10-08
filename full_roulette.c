#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
// Handles the game logic
int game() {
    int chip = createBet();
}
// Calculates the winnings based off the result.
int payout() {
    int betAmount;
    char betType[13][2][20] = {{"su", "Straight Up"},
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

    printf("\nPlease enter your bet amount: ");
    scanf("%d", betAmount);
    printf("\nYou're betting $%d. Please enter your bet type. You can choose from: ", betAmount);
    


}
// Handles the setting up of the bet- the amount and the type of bet
char createBet() {

}
// Main func
int main() {
    printf("Welcome to Roulette!");
}
