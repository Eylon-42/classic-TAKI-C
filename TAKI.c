//
// Created by Eylon Fayena
//
#include "stdio.h"
#include "stdlib.h" // in order to use the "rand" and "srand" functions
#include "time.h" // in order to use "time" function
#include "stdbool.h"

#define MAX_CHARS_NAME 20
#define START_MAX_CARDS_FOR_PLAYER 4
#define CARDS_TYPES 6
#define MAX_TYPE_NAME 7
#define CARDS_COLORS_TYPES 4
#define MAX_CARDS_TYPES 14
#define MAX_REGULAR_CARDS_NUM 9
#define SPECIAL_COLOR_CARD 10
#define SPECIAL_TAKI_CARD 11
#define SPECIAL_STOP_CARD 12
#define SPECIAL_PLUS_CARD 13
#define SPECIAL_ARROW_CARD 14
#define NO_COLOR -1
#define PRINT_CARD_LINE 6
#define PRINT_CARD_COLUMN 9
#define NEW_CARD_FROM_DECK 0
#define ALREADY_HANDLED -1
//printing colors
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_RESET "\x1b[0m"

// types of the special cards on the game
const char typeOfSpecialCards[CARDS_TYPES][MAX_TYPE_NAME] = {"COLOR", "TAKI", "STOP", "+", "<->"};
// types of the cards colors in the game
const char cardColors[CARDS_COLORS_TYPES] = {'Y', 'R', 'B', 'G'};

typedef struct statistics{
    int cardNumberIndex;
    int counter;
} GAME_STATS;

typedef struct cardData{
    int colorNum;
    char colorChar;
    int number;
}CARD;

typedef struct players{
    char playerName[MAX_CHARS_NAME];
    CARD *holdingCards; // the cards each player has
    int physHandSize; // physical size
    int logicHandSize; // logic size
}PLAYER;

void gameProcess(PLAYER *gamePlayers, int numOfPlayers);
void printWelcome();
PLAYER *getNumOfPlayers(int *numberOfPlayers);
int genRandomNumber(int maxNumber);
CARD *reAllocate(CARD *card, int *logicSize,int *physicalSize, int newSize);
void startGameDeal(PLAYER *gamePlayers, int numberOfPlayers, GAME_STATS gameStats[]);
void dealACard(PLAYER *gamePlayers,int playerIndex,int cardIndex, GAME_STATS gameStats[]);
void genNewCardFromDeck(CARD *card, int maxCardMixNumber, GAME_STATS gameStats[]);
void updatePrintUpperCard(CARD *tableUpperCard, const int *upCardNumber, const char *upColorChar,const int *upColorNum);
char colorNumberToChar(int colorNumber);
void printCardToTable(CARD *card);
void printSpecialCard(CARD *card);
void printPlayerHand(PLAYER *currentPlayer);
int playersChoiceValid(PLAYER *currentPlayer, CARD *upperCard, GAME_STATS gameStats[]);
bool validInput(CARD playerCard,int holdingCardsLogSize, int playerChoice, CARD *tableUpperCard);
void playerGetsNewCard(PLAYER *currentPlayer, GAME_STATS gameStats[]);
char chooseSpecialColor(CARD *chosenCard);
void chooseTakiWaterFall(PLAYER *currentPlayer, CARD *tableUpperCard, int currPlayerChoice);
void swapCardToEnd(PLAYER *currentPlayer, int chosenCardIndex);
void playerGameMove(PLAYER *currentPlayer, CARD *chosenCard, CARD *tableUpperCard, int numOfPlayers,
                    int currPlayerChoice, int *playerIndex, bool *playerTurnDirection,bool beenUpdated, GAME_STATS gameStats[]);
void sortGameStatistics(GAME_STATS gameStats[], int size);
void mergeSortGame(GAME_STATS gameStats[], int size);
GAME_STATS *merge(GAME_STATS arr1[], int size1, GAME_STATS arr2[], int size2);
void copyArr(GAME_STATS dest[], GAME_STATS src[], int size);
void printGameStatistics(GAME_STATS gameStats[]);
void endGame(PLAYER *gamePlayers, int numOfPlayers);

int main() {
    srand(time(NULL));
    CARD card = card; // each card in the game
    PLAYER *gamePlayers; // the players of the game and their data
    int numOfPlayers=0; // initate the number of players

    printWelcome();
    gamePlayers = getNumOfPlayers(&numOfPlayers);
    gameProcess(gamePlayers, numOfPlayers);
    endGame(gamePlayers, numOfPlayers);
}

///functions
void printWelcome(){
/*
 * Function:  printWelcome
 * --------------------
 * printing welcome message to the game
 */
    printf("*********** Welcome to ");
    printf(ANSI_GREEN"T"ANSI_RED"A"ANSI_YELLOW"K"ANSI_BLUE"I"ANSI_RESET);
    printf(" game !!!***********\n");
}
void gameProcess(PLAYER *gamePlayers, int numOfPlayers) {
/*
 * Function:  gameProcess
 * --------------------
 * this function is the function who managing the game process
 */
    CARD tableUpperCard = tableUpperCard; // the card that set on the table
    CARD chosenCard = chosenCard; // the chosen card from the hand of the player
    // the game statistics array
    GAME_STATS gameStats[MAX_CARDS_TYPES] = {1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0, 11, 0, 12, 0,
                                             13, 0, 14, 0};
    tableUpperCard.number = 0, tableUpperCard.colorNum = 0, tableUpperCard.colorChar = '0'; // initiate card on front of table
    int playerTurn = 0, currPlayerChoice;
    bool noWinner = true, playerTurnDirection = true; // true is right direction, false is left direction(not normal)
    bool beenUpdated; // if a card has already been update has taken from the hand: true, otherwise: false.

    // initiate the game - dealing each player 4 cards and open an upper card to the table
    startGameDeal(gamePlayers, numOfPlayers, gameStats);
    genNewCardFromDeck(&tableUpperCard, MAX_REGULAR_CARDS_NUM, gameStats);
    updatePrintUpperCard(&tableUpperCard, &tableUpperCard.number, &tableUpperCard.colorChar, &tableUpperCard.colorNum);

    while (noWinner) {
        beenUpdated=false;
        printf("%s's turn:\n", gamePlayers[playerTurn].playerName);
        printPlayerHand(&gamePlayers[playerTurn]);

        currPlayerChoice = playersChoiceValid(&gamePlayers[playerTurn], &tableUpperCard, gameStats);
        chosenCard = gamePlayers[playerTurn].holdingCards[currPlayerChoice - 1];
        if (currPlayerChoice != NEW_CARD_FROM_DECK) {
            playerGameMove(&gamePlayers[playerTurn], &chosenCard, &tableUpperCard,
                           numOfPlayers, currPlayerChoice, &playerTurn, &playerTurnDirection,beenUpdated, gameStats);
            if (gamePlayers[playerTurn].logicHandSize == 0 && playerTurn>=0 && playerTurn<=numOfPlayers-1) {
                printf("The winner is... "ANSI_MAGENTA"%s"ANSI_RESET"! Congratulations!\n",
                       gamePlayers[playerTurn].playerName);
                noWinner = false;
            }
        } else { // in case of choice = 0 ->  NEW_CARD_FROM_DECK
            updatePrintUpperCard(&tableUpperCard, &tableUpperCard.number, &tableUpperCard.colorChar,&tableUpperCard.colorNum);
        }
        // handle the special cards with switching the turns
        if (playerTurnDirection) { // normal direction
            if (playerTurn == numOfPlayers){
                playerTurn = 1;
            }
            else if (playerTurn == numOfPlayers - 1) {
                playerTurn = 0;
            } else {
                playerTurn++;
            }
        } else { // left direction
            if (playerTurn <= 0) {
                playerTurn = numOfPlayers - 1;
            } else {
                playerTurn--;
            }
        }
    }
    // sorting and printing the game statistics
    sortGameStatistics(gameStats, MAX_CARDS_TYPES);
    printGameStatistics(gameStats);
}
CARD *reAllocate(CARD *card, int *logicSize,int *physicalSize, int newSize){
/*
 * Function:  reAllocate
 * --------------------
 * this reallocates the memory allocate of the player hand, if needed
 * return - CARD type - of player hands, after resizing to a bigger physical size, times 2.
 */
    CARD *newPlayerHand = NULL;
    int i;
    newPlayerHand = (CARD *) malloc(newSize * sizeof(CARD));

    if (newPlayerHand != NULL){
        // copy to new array of cards
        for (i = 0; i < *logicSize && i < newSize; i++) {
            newPlayerHand[i] = card[i];
        }
        // new physical size and logic size
        *physicalSize = newSize;
        *logicSize = *logicSize;
        // free the allocated memory
        free(card);
    }
    return newPlayerHand;
}
int genRandomNumber(int maxNumber) {
/*
 * Function:  genRandomNumber
 * --------------------
 * srand(x) function call (for an integer x) is used to set the starting value (seed)
 * for generating a sequence of random integer values.
 */
    int num;
    // random number between 1 and max number (included):
    num = 1 + rand() % maxNumber;
    return num;
}
PLAYER *getNumOfPlayers(int *numberOfPlayers){
/*
 * Function:  getNumOfPlayers
 * --------------------
 * this function takes a number from the user, to set number of players playing.
 * the function allocate memory according to the number of players to type PLAYER
 * return - PLAYER - the function return the array of players of the game with their names
 */
    int p;
    PLAYER *players = NULL;
    printf("Please enter the number of players: \n");
    scanf("%d", numberOfPlayers);
    while (*numberOfPlayers <= 0){
        printf("Please enter the number of players: \n");
        scanf("%d", numberOfPlayers);
    }
    players = (PLAYER*) malloc(*numberOfPlayers * sizeof (PLAYER));
    if (players == NULL){
        printf("allocation failed!");
        exit(1);
    }
    for (p=0;p < *numberOfPlayers;p++) {
        printf("Please enter the first name of player #%d: \n", p + 1);
        scanf("%s", (players+p)->playerName);
    }
    return players;
}
void startGameDeal(PLAYER *gamePlayers, int numberOfPlayers, GAME_STATS *gameStats) {
/*
 * Function:  startGameDeal
 * --------------------
 * this function go to every player and allocate memory to his holding cards, at the beginning as 4 cards each
 * after that, each card slot of the player generate a random card of random number and random color
 * then, updating the physical and logical size of each player's cards
 */
    int p, card;
    for (p = 0; p < numberOfPlayers; p++) {
        gamePlayers[p].holdingCards = (CARD *) malloc(START_MAX_CARDS_FOR_PLAYER * sizeof(CARD)); // allocating memory of 4 cards for each player
        if (gamePlayers[p].holdingCards != NULL){
            for (card = 0; card < START_MAX_CARDS_FOR_PLAYER; card++) {
                dealACard(gamePlayers, p, card, gameStats);
            }
            gamePlayers[p].physHandSize=START_MAX_CARDS_FOR_PLAYER;
            gamePlayers[p].logicHandSize=START_MAX_CARDS_FOR_PLAYER;
        } else{
            exit(1);
        }
    }
}
void dealACard(PLAYER *gamePlayers,int playerIndex,int cardIndex, GAME_STATS gameStats[]) {
/*
 * Function:  dealACard
 * --------------------
 * this function generates a new card for a specific player. each card gets a random card number and a random card color
 * the values are being saved to the card in the holding cards of each player
 */
    int randCardNum,randColorNum;
    char colorChar;
    randCardNum = genRandomNumber(MAX_CARDS_TYPES); // random card number generator
    gameStats[randCardNum].counter++; // the card saved in the game statistics
    gamePlayers[playerIndex].holdingCards[cardIndex].number = randCardNum;
    if(randCardNum==SPECIAL_COLOR_CARD){  // COLOR card number - if so, the player will have to choose the color
        gamePlayers[playerIndex].holdingCards[cardIndex].colorNum=NO_COLOR;
        colorChar = colorNumberToChar(NO_COLOR);
        gamePlayers[playerIndex].holdingCards[cardIndex].colorChar=colorChar;
    } else {
        randColorNum = genRandomNumber(CARDS_COLORS_TYPES); // random card color generator
        gamePlayers[playerIndex].holdingCards[cardIndex].colorNum = randColorNum;
        colorChar = colorNumberToChar(randColorNum);
        gamePlayers[playerIndex].holdingCards[cardIndex].colorChar = colorChar;
    }
}
void genNewCardFromDeck(CARD *card, int maxCardMixNumber, GAME_STATS gameStats[]){
/*
 * Function:  genNewCardFromDeck
 * --------------------
 * this function generates a new card to the table. each card gets a random card number and a random card color
 * the values are being saved to the card in the holding cards of each player
 */
    card->number = genRandomNumber(maxCardMixNumber);
    gameStats[card->number].counter++; // the card saved in the game statistics
    if(card->number == SPECIAL_COLOR_CARD) // has no color from the start
        card->colorNum = -1;
    else
        card->colorNum = genRandomNumber(CARDS_COLORS_TYPES);
    card->colorChar = colorNumberToChar(card->colorNum);
}
void printPlayerHand(PLAYER *currentPlayer){
/*
 * Function:  printPlayerHand
 * --------------------
 * this function prints out the cards that the current player is holding
 */
    int card;
    for (card=0;card<currentPlayer->logicHandSize; card++){
        printf("Card #%d",card+1);
        printCardToTable(&currentPlayer->holdingCards[card]);
    }
}
int playersChoiceValid(PLAYER *currentPlayer, CARD *upperCard, GAME_STATS gameStats[]){
/*
 * Function:  playersChoiceValid
 * --------------------
 * this function asking from the current playing player, to choose a card from his hand
 * it checks his validation with the help of validInput function
 * if the player choice is 0, the player get new card from deck, else, return the choice number
 * return - int - the choice number
 */
    int playerChoice;
    printf("Please enter 0 if you want to take a card from the deck\nor 1-%d if you want to put one of your cards in"
           " the middle:\n", currentPlayer->logicHandSize);
    scanf("%d", &playerChoice);
    // as long as the input is not as the rules
    while (validInput(currentPlayer->holdingCards[playerChoice-1],currentPlayer->logicHandSize,
                      playerChoice, upperCard) && playerChoice !=0){
        printf("Invalid card! Try again.\n");
        printf("Please enter 0 if you want to finish your turn\nor 1-%d if you want to put one of your cards in"
               " the middle:\n", currentPlayer->logicHandSize);
        scanf("%d", &playerChoice);
    }
    if (playerChoice == NEW_CARD_FROM_DECK){
        playerGetsNewCard(currentPlayer, gameStats);
    }
    return playerChoice;
}
bool validInput(CARD playerCard,int holdingCardsLogSize, int playerChoice, CARD *tableUpperCard){
/*
 * Function:  validInput
 * --------------------
 * this function checks if the player choice is valid according to the game's rules
 * return - bool - true if the input isn't valid, and false otherwise
 */
    bool inValidInput = true;
    if (playerChoice >=0 && playerChoice<=holdingCardsLogSize) { // if the choice in between the options
        inValidInput = false;
        if (tableUpperCard->colorNum == playerCard.colorNum || tableUpperCard->number == playerCard.number ||
            playerCard.number == SPECIAL_COLOR_CARD) {
            inValidInput = false;
        } else {
            inValidInput = true;
        }
    }
    return inValidInput;
}
void playerGetsNewCard(PLAYER *currentPlayer, GAME_STATS gameStats[]){
/*
 * Function:  playerGetsNewCard
 * --------------------
 * this function generating new card to a player. first, the function checks if there is any more memory space
 * for a new card in the player hand, if not, it reallocates the memory dedicated to the cards.
 * after that, updating the player logic size
 */
// add new card and reallocate times 2
    if(currentPlayer->logicHandSize == currentPlayer->physHandSize){ // if there is not enough space to get another card from deck
        currentPlayer->holdingCards = reAllocate(currentPlayer->holdingCards, &currentPlayer->logicHandSize,
                                                 &currentPlayer->physHandSize, (currentPlayer->physHandSize)*2);
    }
    // if there is enough space to get another card from deck, just generate new one
    genNewCardFromDeck(&currentPlayer->holdingCards[currentPlayer->logicHandSize],MAX_CARDS_TYPES, gameStats);
    currentPlayer->logicHandSize++; // increasing the logic size by one card
}
void playerGameMove(PLAYER *currentPlayer, CARD *chosenCard, CARD *tableUpperCard, int numOfPlayers,
                    int currPlayerChoice, int *playerIndex, bool *playerTurnDirection,bool beenUpdated, GAME_STATS gameStats[]){
/*
 * Function:  playerGameMove
 * --------------------
 * this function handles each player move, according to his card choice of play.
 * the function act according to the chosen card, and the game rules
 * the function is also handling the updating and printing to the table the card the has been chosen,
 * and removing that card from the players hand
 */

    switch (chosenCard->number) {
        case SPECIAL_COLOR_CARD:
            chosenCard->colorChar = chooseSpecialColor(chosenCard);
            tableUpperCard->colorNum = chosenCard->colorNum;
            break;
        case SPECIAL_STOP_CARD:
            if (currentPlayer->logicHandSize==1 && numOfPlayers==2) {  // if that was the player's last card and 2 game players
                playerGetsNewCard(currentPlayer, gameStats);
            }
            if (*playerTurnDirection == true) // if the direction is normal
                (*playerIndex)++;
            else
                (*playerIndex)--;
            break;
        case SPECIAL_PLUS_CARD:
            if (currentPlayer->logicHandSize==1){ // if that was the player's last card
                playerGetsNewCard(currentPlayer, gameStats);
            }
            if (*playerTurnDirection == true) // if the direction is normal
                (*playerIndex)--;
            else
                (*playerIndex)++;
            break;
        case SPECIAL_ARROW_CARD:
            (*playerTurnDirection) = !(*playerTurnDirection);
            break;
        case SPECIAL_TAKI_CARD:
            chooseTakiWaterFall(currentPlayer, tableUpperCard, currPlayerChoice);
            break;
    }
    if(chosenCard->number !=SPECIAL_TAKI_CARD){ // if the card is not TAKI - needs to be printed out and update the hand
        updatePrintUpperCard(tableUpperCard,&chosenCard->number, &chosenCard->colorChar,&chosenCard->colorNum);
        if(beenUpdated==false) // if the player's hand isn't already been updated -> needs to be updated
            swapCardToEnd(currentPlayer, currPlayerChoice-1);
    } else{ // if the chosen card is type TAKI, the hand of the player and the card on the table already been updated
        if (tableUpperCard->number==SPECIAL_TAKI_CARD){ // in case the player put a TAKI card and exit without putting any more cards on top
            updatePrintUpperCard(tableUpperCard,&chosenCard->number, &chosenCard->colorChar,&chosenCard->colorNum);
        } else{ // handle the last card on taki
            beenUpdated=true;
            playerGameMove(currentPlayer, tableUpperCard, tableUpperCard, numOfPlayers, currPlayerChoice,playerIndex,playerTurnDirection,beenUpdated,gameStats);
        }
    }
}
void swapCardToEnd(PLAYER *currentPlayer, int chosenCardIndex){
/*
 * Function:  swapCardToEnd
 * --------------------
 * this function swap the chosen card to the end of the array of the player's cards in hand,
 * and by that, the game process won't refer to that specific card, so the card is "removed"
 * if the card index his equal to -1 : that means the card "remove" has already happened and no need to do it again
 */
    if (chosenCardIndex != ALREADY_HANDLED){
        currentPlayer->holdingCards[chosenCardIndex] = currentPlayer->holdingCards[currentPlayer->logicHandSize-1];
        currentPlayer->logicHandSize--; // after player place one card to the table
    }
}
char chooseSpecialColor(CARD *chosenCard){
/*
 * Function:  chooseSpecialColor
 * --------------------
 * this function handle the case that the player has chosen to play with the change COLOR card.
 * in that case the player will have to choose the color himself.
 * the function also updating the color number of the color that the player chose
 * return - char - the color char of the color that the player chose
 */
    int colorChoice;
    char colorChar;
    printf("Please enter your color choice: \n1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
    scanf("%d", &colorChoice);
    while (colorChoice <1 || colorChoice >4){
        printf("Invalid card! Try again.\n");
        printf("Please enter your color choice: \n1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
        scanf("%d", &colorChoice);
    }
    chosenCard->colorNum = colorChoice; // updating the color number
    colorChar = colorNumberToChar(colorChoice);
    return colorChar;
}
void chooseTakiWaterFall(PLAYER *currentPlayer, CARD *tableUpperCard, int currPlayerChoice){
/*
 * Function:  chooseTakiWaterFall
 * --------------------
 * this function handle the case that the player has chosen to play with the TAKI card
 * according to the rules, as long as the player has not input the number 0 or the player is out of cards, or the next
 * card the player chose is actually a COLOR card, he can drop cards, as long as they the same color as the TAKI.
 * the function will update the table card and the player's hand
 */
    while (currPlayerChoice != 0 && currentPlayer->logicHandSize != 0 && currentPlayer->holdingCards[currPlayerChoice-1].number !=SPECIAL_COLOR_CARD){
        // update upper card on the table and print it
        updatePrintUpperCard(tableUpperCard, &currentPlayer->holdingCards[currPlayerChoice - 1].number,
                             &currentPlayer->holdingCards[currPlayerChoice - 1].colorChar,
                             &currentPlayer->holdingCards[currPlayerChoice - 1].colorNum);
        // update the player's hand
        swapCardToEnd(currentPlayer, currPlayerChoice-1);

        if(currentPlayer->logicHandSize != 0){ // in case of last card is TAKI
            printf("%s's turn:\n", currentPlayer->playerName);
            printPlayerHand(currentPlayer);
            printf("Please enter 0 if you want to finish your turn\nor 1-%d if you want to put one of your cards in"
                   " the middle:\n", currentPlayer->logicHandSize);
            scanf("%d", &currPlayerChoice);
            while (currentPlayer->holdingCards[currPlayerChoice-1].colorNum != tableUpperCard->colorNum && currPlayerChoice != 0){
                printf("Invalid card! Try again.\n");
                printf("Please enter 0 if you want to finish your turn\nor 1-%d if you want to put one of your cards in"
                       " the middle:\n", currentPlayer->logicHandSize);
                scanf("%d", &currPlayerChoice);
            }
        }
    }
}
void updatePrintUpperCard(CARD *tableUpperCard, const int *upCardNumber, const char *upColorChar,const int *upColorNum){
/*
 * Function:  updatePrintUpperCard
 * --------------------
 * this function updates the upper card and prints it out to the table
 */
    printf("Upper card:");
    tableUpperCard->number = *upCardNumber;
    tableUpperCard->colorChar = *upColorChar;
    tableUpperCard->colorNum = *upColorNum;
    printCardToTable(tableUpperCard);
}
char colorNumberToChar(int colorNumber){
/*
 * Function:  colorNumberToChar
 * --------------------
 * this function switch from a color random number to his equivalent char
 * return - char - the color char
 */
    switch (colorNumber) {
        case 1:
            return cardColors[0];
        case 2:
            return cardColors[1];
        case 3:
            return cardColors[2];
        case 4:
            return cardColors[3];
        default:
            return ' ';
    }
}
void printCardToTable(CARD *card){
/*
 * Function:  printCardToTable
 * --------------------
 * this function prints out the cards to the table, with his color :-)
 * if it's a special card, its need a special print, so the function calls printSpecialCard
 */
    switch (card->colorNum) {
        case 1:
            printf(ANSI_YELLOW);
            break;
        case 2:
            printf(ANSI_RED);
            break;
        case 3:
            printf(ANSI_BLUE);
            break;
        case 4:
            printf(ANSI_GREEN);
            break;
        default:
            printf(ANSI_RESET);
            break;
    }
    printf("\n*********\n");
    printf("*\t\t*\n*");
    if(card->number >=1 && card->number<=9){
        printf("   %d\t*\n", card->number);
        printf("*   %c\t*\n",card->colorChar);
        printf("*\t\t*\n");
    } else{
        printSpecialCard(card);
    }
    printf("*********\n");
    printf(ANSI_RESET);
}
void printSpecialCard(CARD *card){
/*
 * Function:  printSpecialCard
 * --------------------
 * this function prints out the special cards to the table, with his color :-)
 * with the help of the printCardToTable function
 */
    switch (card->number) {
        case SPECIAL_COLOR_CARD: //COLOR
            printf(" %s *\n",typeOfSpecialCards[0]);
            printf("*   %c\t*\n",card->colorChar);
            printf("*\t\t*\n");
            break;
        case SPECIAL_TAKI_CARD: //TAKI
            printf("  %s *\n",typeOfSpecialCards[1]);
            printf("*   %c\t*\n",card->colorChar);
            printf("*\t\t*\n");
            break;
        case SPECIAL_STOP_CARD: //STOP
            printf("  %s *\n",typeOfSpecialCards[2]);
            printf("*   %c\t*\n",card->colorChar);
            printf("*\t\t*\n");
            break;
        case SPECIAL_PLUS_CARD: //+
            printf("   %s \t*\n",typeOfSpecialCards[3]);
            printf("*   %c\t*\n",card->colorChar);
            printf("*\t\t*\n");
            break;
        case SPECIAL_ARROW_CARD: //<->
            printf("  %s  *\n",typeOfSpecialCards[4]);
            printf("*   %c\t*\n",card->colorChar);
            printf("*\t\t*\n");
            break;
        default:
            printf("*\t\t*\n");
            printf("*\t\t*\n");
            break;
    }
}
void sortGameStatistics(GAME_STATS gameStats[], int size){
/*
 * Function:  sortGameStatistics
 * --------------------
 * this function is responsible for sorting the game statistics array with a MergeSort algorithm
 */
    mergeSortGame(gameStats, size);
}
void mergeSortGame(GAME_STATS gameStats[], int size){
/*
 * Function:  mergeSortGame - Recursion function
 * --------------------
 * this function is responsible for sorting the game statistics array with a MergeSort algorithm
 * using a dynamic array to copy the cards each time of the sort
 */
    GAME_STATS *tmp = NULL;
    if (size <= 1)
        return;

    mergeSortGame(gameStats, size / 2);
    mergeSortGame(gameStats + size / 2, size - size / 2);
    tmp = merge(gameStats, size / 2 , gameStats + size / 2, size - size / 2);

    if (tmp) {
        copyArr(gameStats, tmp, size);
        free(tmp);
    } else {
        printf("Memory allocation failed!!!\n");
        exit(1);
    }
}
GAME_STATS *merge(GAME_STATS arr1[], int size1, GAME_STATS arr2[], int size2){
/*
 * Function:  merge
 * --------------------
 * this function is part of the MergeSort algorithm.
 * it's allocating memory to the result array, and then sorting by the counter of each card
 * return - GAME_STATS - the sorted array
 */
    int index1, index2, resInd;
    GAME_STATS* res = (GAME_STATS*)malloc((size1+size2) * sizeof(GAME_STATS));
    if (res) {
        index1 = index2 = resInd = 0;
        while ((index1<size1) && (index2<size2)) {
            if (arr1[index1].counter >= arr2[index2].counter) {
                res[resInd] = arr1[index1];
                index1++;
            } else {
                res[resInd] = arr2[index2];
                index2++;
            }
            resInd++;
        }

        while (index1 < size1) {
            res[resInd] = arr1[index1];
            index1++;
            resInd++;
        }
        while (index2 < size2) {
            res[resInd] = arr2[index2];
            index2++;
            resInd++;
        }
    }
    return res;
}
void copyArr(GAME_STATS dest[], GAME_STATS src[], int size) {
/*
 * Function:  copyArr
 * --------------------
 * this function is part of the MergeSort algorithm.
 * it's copying the source array to the destination array by pointers
 */
    int i;
    for (i=0; i < size; i++) {
        dest[i] = src[i];
    }
}
void printGameStatistics(GAME_STATS gameStats[]){
/*
 * Function:  printGameStatistics
 * --------------------
 * this function prints out the game statistics array after Merge sorting it
 */
    int cardNumber;
    printf("************** Game Statistics **************\n");
    printf("Card # | Frequency\n__________________\n");
    for (cardNumber=0; cardNumber<MAX_CARDS_TYPES;cardNumber++){
        if(gameStats[cardNumber].cardNumberIndex >=1 && gameStats[cardNumber].cardNumberIndex<=9){
            printf("   %d\t|\t%d\t\n", gameStats[cardNumber].cardNumberIndex, gameStats[cardNumber].counter);
        } else{
            switch (gameStats[cardNumber].cardNumberIndex) {
                case SPECIAL_COLOR_CARD:
                    printf(" %s\t|\t%d\t\n",typeOfSpecialCards[0],gameStats[cardNumber].counter);
                    break;
                case SPECIAL_TAKI_CARD:
                    printf(" %s\t|\t%d\t\n",typeOfSpecialCards[1],gameStats[cardNumber].counter);
                    break;
                case SPECIAL_STOP_CARD:
                    printf(" %s\t|\t%d\t\n",typeOfSpecialCards[2],gameStats[cardNumber].counter);
                    break;
                case SPECIAL_PLUS_CARD:
                    printf("   %s\t|\t%d\t\n",typeOfSpecialCards[3],gameStats[cardNumber].counter);
                    break;
                case SPECIAL_ARROW_CARD:
                    printf("  %s\t|\t%d\t\n",typeOfSpecialCards[4],gameStats[cardNumber].counter);
                    break;
                default:
                    printf("   \t|\t\t\n");
                    break;
            }
        }
    }
}
void endGame(PLAYER *gamePlayers, int numOfPlayers){
/*
 * Function:  endGame
 * --------------------
 * the function is responsible to free all the memory that has been allocated during the game using malloc
 */
    int p;
    for (p = 0; p < numOfPlayers; p++) {
        free(gamePlayers[p].holdingCards); // free each player cards in the hand
    }
    free(gamePlayers); // free every player
}
