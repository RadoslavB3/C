#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int validateInput(char card[2], char player1[7][2], char player2[7][2])
{
    const char *validValues = "23456789TJQKA";
    const char *validColors = "hdsc";
    if (strchr(validValues, card[0]) == NULL || strchr(validColors, card[1]) == NULL) {
        fprintf(stderr, "Invalid card.\n");
        return 1;
    }
    for (int i = 0; i <= 6; i++) {
        if (card[0] == player1[i][0] && card[1] == player1[i][1]) {
            fprintf(stderr, "Duplicate card.\n");
            return 1;
        }
        if (card[0] == player2[i][0] && card[1] == player2[i][1]) {
            fprintf(stderr, "Duplicate card.\n");
            return 1;
        }
    }
    return 0;
}

int convertChar(char card)
{
    if (card == 'T') {
        return 10;
    }
    if (card == 'J') {
        return 11;
    }
    if (card == 'Q') {
        return 12;
    }
    if (card == 'K') {
        return 13;
    }
    if (card == 'A') {
        return 14;
    }
    if (card == 'h') {
        return 1;
    }
    if (card == 'd') {
        return 2;
    }
    if (card == 's') {
        return 3;
    }
    if (card == 'c') {
        return 4;
    }
    return card - '0';
}

int convertArray(char player1[7][2], char player2[7][2], int player1_converted[7][2], int player2_converted[7][2])
{
    for (int i = 0; i <= 6; i++) {
        for (int j = 0; j <= 1; j++) {
            player1_converted[i][j] = convertChar(player1[i][j]);
            player2_converted[i][j] = convertChar(player2[i][j]);
        }
    }
    return 0;
}

int inputCards(char player1[7][2], char player2[7][2])
{
    char card[2] = { 0 };
    char control = 0;
    int count = 0;
    int succesCards = 0;
    for (int i = 0; i <= 8; i++) {
        count = (scanf(" %c%c%c", &card[0], &card[1], &control));
        if (count != 3) {
            break;
        }
        if (validateInput(card, player1, player2) != 0) {
            return 1;
        }
        if (!isspace(control)) {
            fprintf(stderr, "Invalid card set.\n");
            return 1;
        }
        if ((i == 1 || i == 3 || i == 8) && control != '\n') {
            fprintf(stderr, "Invalid card set.\n");
            return 1;
        }
        if (i <= 1) {
            player1[i][0] = card[0];
            player1[i][1] = card[1];

        } else if (i <= 3) {
            player2[i - 2][0] = card[0];
            player2[i - 2][1] = card[1];

        } else {
            player1[i - 2][0] = card[0];
            player1[i - 2][1] = card[1];
            player2[i - 2][0] = card[0];
            player2[i - 2][1] = card[1];
        }
        succesCards++;
        count = 0;
    }
    if (count == -1 && succesCards == 0) {
        return 1;
    }
    if (count == -1 && succesCards != 0) {
        fprintf(stderr, "Invalid number of input cards.\n");
        return 1;
    }
    if (count != 0) {
        fprintf(stderr, "Invalid input format.\n");
        return 1;
    }
    return 0;
}

void sortArray(int array[7][2])
{
    for (int xcord = 0; xcord <= 6; xcord++) {
        for (int ycord = xcord + 1; ycord <= 6; ycord++) {
            if (array[xcord][0] > array[ycord][0]) {

                int temp = array[xcord][1];
                int temp2 = array[xcord][0];
                array[xcord][0] = array[ycord][0];
                array[xcord][1] = array[ycord][1];
                array[ycord][0] = temp2;
                array[ycord][1] = temp;
            }
        }
    }
}

void countCards(int player1[7][2], int player2[7][2], int player1_count[15], int player2_count[15], int colors1[5], int colors2[5])
{
    for (int i = 0; i <= 6; i++) {
        int card1 = player1[i][0];
        int card2 = player2[i][0];
        int color1 = player1[i][1];
        int color2 = player2[i][1];
        colors1[color1] += 1;
        colors2[color2] += 1;
        player1_count[card1] += 1;
        player2_count[card2] += 1;
    }
}

void checkPairs(int countCard, int card, int pairs[2], int *triple, int *quadra)
{
    if (countCard == 2) {
        if (pairs[0] == 0) {
            pairs[0] = card;

        } else if (pairs[0] != 0 && pairs[1] != 0) {
            int aux = pairs[1];
            pairs[1] = card;
            pairs[0] = aux;

        } else {
            pairs[1] = card;
        }
    }
    if (countCard == 3) {
        if (*triple != 0) {
            pairs[0] = *triple;
        }
        *triple = card;
    }
    if (countCard == 4) {
        *quadra = card;
    }

}

void valueOfCombination(int *player_value, int triple, int pairs[2], int quadra)
{
    if (quadra != 0) {
        *player_value = 8;

    } else if (pairs[0] != 0 && triple != 0) {
        *player_value = 7;

    } else if (triple != 0) {
        *player_value = 4;

    } else if (pairs[0] != 0 && pairs[1] != 0) {
        *player_value = 3;

    } else if (pairs[0] != 0) {
        *player_value = 2;
    }
}

int maxCards(int player1[15], int player2[15], int usedCards_p1[2], int usedCards_p2[2], int value)
{
    int maxCards_p1[5] = { 0 };
    int maxCards_p2[5] = { 0 };
    int pos1 = 0;
    int pos2 = 0;
    for (int i = 14; i >= 2; i--) {
        if (player1[i] != 0 && i != usedCards_p1[0] && i != usedCards_p1[1] && pos1 < 5) {
            maxCards_p1[pos1] = i;
            pos1++;
        }
        if (player2[i] != 0 && i != usedCards_p2[0] && i != usedCards_p2[1] && pos2 < 5) {
            maxCards_p2[pos2] = i;
            pos2++;
        }
    }
    if (value == 2) {
        for (int i = 0; i <= 2; i++) {
            if (maxCards_p1[i] > maxCards_p2[i]) {
                return 1;
            }
            if (maxCards_p1[i] < maxCards_p2[i]) {
                return 2;
            }
        }
    }
    if (value == 3) {
        if (maxCards_p1[0] > maxCards_p2[0]) {
            return 1;
        }
        if (maxCards_p1[0] < maxCards_p2[0]) {
            return 2;
        }
    }
    if (value == 4) {
        for (int i = 0; i <= 1; i++) {
            if (maxCards_p1[i] > maxCards_p2[i]) {
                return 1;
            }
            if (maxCards_p1[i] < maxCards_p2[i]) {
                return 2;
            }
        }
    }
    if (value == 8) {
        if (maxCards_p1[0] > maxCards_p2[0]) {
            return 1;
        }
        if (maxCards_p1[0] < maxCards_p2[0]) {
            return 2;
        }
    }
    if (value == 0) {
        for (int i = 0; i <= 4; i++) {
            if (maxCards_p1[i] > maxCards_p2[i]) {
                return 1;
            }
            if (maxCards_p2[i] > maxCards_p1[i]) {
                return 2;
            }
        }
    }
    return 3;
}


int checkStraigtFlush(int player[7][2], int color)
{
    int count = 1;
    int highest = 0;
    for (int i = 6; i >= 0; i--) {
        if (highest == 0 && player[i][1] == color) {
            highest = player[i][0];

        } else if (highest - player[i][0] == count && player[i][1] == color) {
            count++;

        } else if (player[i][1] == color) {
            highest = player[i][0];
            count = 1;
        }
        if (count == 5) {
            return highest;
        }
    }
    if (count == 4 && highest == 5 && player[6][0] == 14) {
        for (int i = 6; i >= 3; i--) {
            if (player[i][0] == 14 && player[i][1] == color) {
                return 1;
            }
        }
    }
    return 0;
}

int checkStraight(int player[7][2])
{
    int count = 1;
    int highest = player[6][0];
    for (int i = 5; i >= 0; i--) {
        if (player[i][0] == player[i + 1][0]) {
            continue;
        }
        if (highest - player[i][0] == count) {
            count++;

        } else {
            highest = player[i][0];
            count = 1;
        }
        if (count == 5) {
            return highest;
        }
    }
    if (count == 4 && highest == 5 && player[6][0] == 14) {
        return highest;
    }
    return 0;
}

void checkFlush(int player[7][2], int color, int flash_cards[5])
{
    int pos = 0;
    for (int i = 6; i >= 0; i--) {
        if (player[i][1] == color && pos < 5) {
            flash_cards[pos] = player[i][0];
            pos++;
        }
    }
}

void valueOfStraights(int player1[7][2], int player2[7][2], int color1, int color2, int *straights1, int *straights2)
{
    int straight_flush1 = 0;
    int flush_cards1[5] = { 0 };
    int flush_cards2[5] = { 0 };
    int straight_flush2 = 0;
    int plain_straight1 = 0;
    int plain_straight2 = 0;
    if (color1 != 0) {
        straight_flush1 = checkStraigtFlush(player1, color1);
        if (straight_flush1 == 0) {
            checkFlush(player1, color1, flush_cards1);
            *straights1 = 6;

        } else {
            *straights1 = 9;
        }

    } else {
        plain_straight1 = checkStraight(player1);
        if (plain_straight1 != 0) {
            *straights1 = 5;
        }
    }
    if (color2 != 0) {
        straight_flush2 = checkStraigtFlush(player2, color2);
        if (straight_flush2 == 0) {
            checkFlush(player2, color2, flush_cards2);
            *straights2 = 6;

        } else {
            *straights2 = 9;
        }

    } else {
        plain_straight2 = checkStraight(player2);
        if (plain_straight2 != 0) {
            *straights2 = 5;
        }
    }
    if (*straights1 == *straights2 && *straights1 == 9) {
        if (straight_flush1 > straight_flush2) {
            *straights2 = 0;
        }
        if (straight_flush2 > straight_flush1) {
            *straights1 = 0;
        }
    }
    if (*straights1 == *straights2 && *straights1 == 6) {
        for (int i = 0; i <= 4; i++) {
            if (flush_cards1[i] > flush_cards2[i]) {
                *straights2 = 0;
                break;
            }
            if (flush_cards2[i] > flush_cards1[i]) {
                *straights1 = 0;
                break;
            }
        }
    }
    if (*straights1 == *straights2 && *straights1 == 5) {
        if (plain_straight1 > plain_straight2) {
            *straights2 = 0;
        }
        if (plain_straight2 > plain_straight1) {
            *straights1 = 0;
        }
    }
}

int checkCombinations(int player1[15], int player2[15], int player1_converted[7][2], int player2_converted[7][2], int color1, int color2)
{
    int pairs_p1[2] = { 0 };
    int triple_p1 = 0;
    int quadra_p1 = 0;
    int pairs_p2[2] = { 0 };
    int triple_p2 = 0;
    int quadra_p2 = 0;
    int player1_value = 0;
    int player2_value = 0;
    int straights1 = 0;
    int straights2 = 0;
    int high_pair1 = 0;
    int high_pair2 = 0;
    int usedCards_p1[2] = { 0 };
    int usedCards_p2[2] = { 0 };
    for (int i = 2; i <= 14; i++) {
        checkPairs(player1[i], i, pairs_p1, &triple_p1, &quadra_p1);
        checkPairs(player2[i], i, pairs_p2, &triple_p2, &quadra_p2);
    }
    valueOfStraights(player1_converted, player2_converted, color1, color2, &straights1, &straights2);
    valueOfCombination(&player1_value, triple_p1, pairs_p1, quadra_p1);
    valueOfCombination(&player2_value, triple_p2, pairs_p2, quadra_p2);
    if (straights1 > player1_value) {
        player1_value = straights1;
    }
    if (straights2 > player2_value) {
        player2_value = straights2;
    }
    if (pairs_p1[1] == 0) {
        high_pair1 = pairs_p1[0];

    } else {
        high_pair1 = pairs_p1[1];
    }
    if (pairs_p2[1] == 0) {
        high_pair2 = pairs_p2[0];

    } else {
        high_pair2 = pairs_p2[1];
    }
    if (player1_value == 0 && player2_value == 0) {
        return maxCards(player1, player2, usedCards_p1, usedCards_p2, 0);
    }
    if (player1_value > player2_value) {
        return 1;
    }
    if (player2_value > player1_value) {
        return 2;
    }
    if (player1_value == 7) {
        if (triple_p1 > triple_p2) {
            return 1;
        }
        if (triple_p2 > triple_p1) {
            return 2;
        }
        if (high_pair1 > high_pair2) {
            return 1;
        }
        if (high_pair2 > high_pair1) {
            return 2;
        }
    }
    if (player1_value == 8) {
        if (quadra_p1 > quadra_p2) {
            return 1;
        }
        if (quadra_p2 > quadra_p1) {
            return 2;
        }
        usedCards_p1[0] = quadra_p1;
        usedCards_p2[0] = quadra_p2;
        return maxCards(player1, player2, usedCards_p1, usedCards_p2, 8);
    }
    if (player1_value == 4) {
        if (triple_p1 > triple_p2) {
            return 1;
        }
        if (triple_p2 > triple_p1) {
            return 2;
        }
        usedCards_p1[0] = triple_p1;
        usedCards_p2[0] = triple_p2;
        return maxCards(player1, player2, usedCards_p1, usedCards_p2, 4);
    }
    if (player1_value == 3) {
        for (int i = 1; i >= 0; i--) {
            if (pairs_p1[i] > pairs_p2[i]) {
                return 1;
            }
            if (pairs_p2[i] > pairs_p1[i]) {
                return 2;
            }
        }
        return maxCards(player1, player2, pairs_p1, pairs_p2, 3);
    }
    if (player1_value == 2) {
        if (pairs_p1[0] > pairs_p2[0]) {
            return 1;
        }
        if (pairs_p2[0] > pairs_p1[0]) {
            return 2;
        }
        return maxCards(player1, player2, pairs_p1, pairs_p2, 2);
    }
    return 3;
}

void anulArray(char player1[7][2], char player2[7][2], int p1_conv[7][2], int p2_conv[7][2], int count1[15], int count2[15], int c1[5], int c2[5])
{
    for (int i = 0; i <= 6; i++) {
        for (int j = 0; j <= 1; j++) {
            player1[i][j] = 0;
            player2[i][j] = 0;
            p1_conv[i][j] = 0;
            p2_conv[i][j] = 0;
        }
    }
    for (int i = 0; i <= 14; i++) {
        count1[i] = 0;
        count2[i] = 0;
    }
    for (int i = 0; i <= 4; i++) {
        c1[i] = 0;
        c2[i] = 0;
    }
}


int main(void)
{
    char player1[7][2] = { { 0 } };
    char player2[7][2] = { { 0 } };
    int player1_converted[7][2] = { { 0 } };
    int player2_converted[7][2] = { { 0 } };
    int colors_p1[5] = { 0 };
    int colors_p2[5] = { 0 };
    int highest_color1 = 0;
    int highest_color2 = 0;
    int player1_count[15] = { 0 };
    int player2_count[15] = { 0 };
    int winner = 0;
    while (inputCards(player1, player2) != 1) {
        convertArray(player1, player2, player1_converted, player2_converted);
        sortArray(player1_converted);
        sortArray(player2_converted);
        countCards(player1_converted, player2_converted, player1_count, player2_count, colors_p1, colors_p2);
        highest_color1 = 0;
        highest_color2 = 0;
        for (int i = 1; i <= 4; i++) {
            if (colors_p1[i] >= 5) {
                highest_color1 = i;
            }
            if (colors_p2[i] >= 5) {
                highest_color2 = i;
            }
        }
        winner = checkCombinations(player1_count, player2_count, player1_converted, player2_converted, highest_color1, highest_color2);
        if (winner == 1) {
            printf("W\n");

        } else if (winner == 2) {
            printf("L\n");

        } else {
            printf("D\n");
        }
        anulArray(player1, player2, player1_converted, player2_converted, player1_count, player2_count, colors_p1, colors_p2);
    }
    return 0;
}
