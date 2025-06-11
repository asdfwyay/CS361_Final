/**
 * @mainpage Four-of-a-Kind Game
 * @section Description
 * 
 * Simulates a card game where players must obtain four-of-a-kind by drawing and discarding cards from adjacent piles.
 */

/**
 * @file main.cpp
 * @author Naufal Ahmad <na867@drexel.edu>
 * @date June 11, 2025
 * @brief Simulates a card game where players must obtain four-of-a-kind by drawing and discarding cards from adjacent piles.
 * 
 * @section Description
 * Simulates a card game where players must obtain four-of-a-kind by drawing and discarding cards from adjacent piles.
 */

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "t_stack.h"

#define PRINT_STATES true

std::vector<int> cards = {1,1,1,1,
                          2,2,2,2,
                          3,3,3,3,
                          4,4,4,4,
                          5,5,5,5,
                          6,6,6,6};
t_stack<int> cardPiles[4];
std::mutex cardMutex;
std::mutex ioMutex;
std::condition_variable cv;

bool gameOver = false;


/**
 * playerThread
 * 
 * A thread representing a player in the game. Each player has
 * a hand, where four cards may be stored. For the player to win,
 * the cards must all match.
 * 
 * @param drawPile The pile the player draws cards from.
 * @param discardPile The pile the player discards cards to.
 */
void playerThread(int drawPile, int discardPile) {
    int hand[4];

    // Draw initial 4 cards
    for (int i = 0; i < 4; i++) {
        const std::lock_guard<std::mutex> lk(cardMutex);
        int chosenCardPos = rand() % cards.size();
        hand[i] = cards[chosenCardPos];
        cards.erase(cards.begin() + chosenCardPos);
    }

    while (!gameOver) {
        // Check if I have won
        if (std::all_of(hand, hand+4, [hand](int x){ return x == hand[0]; })) {
            {
                const std::lock_guard<std::mutex> lk(ioMutex);
                if (gameOver) break;
                std::cout << "Player " << drawPile + 1 << " has won!" << std::endl;
                gameOver = true;
            }
            cv.notify_all();
            break;
        }

        // Discard randomly chosen card from hand to left pile
        int slotReplace = rand() % 4;
        cardPiles[discardPile].push(hand[slotReplace]);
        cv.notify_one();
        if (PRINT_STATES) {
            const std::lock_guard<std::mutex> lk(ioMutex);
            if (gameOver) break;
            std::cout << "P" << drawPile + 1 << ": DISCARD [";
            for (int i = 0; i < 4; i++) {
                if (i == slotReplace) std::cout << "*";
                std::cout << hand[i];
                if (i != 3) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
        }

        // Draw card from right pile when able
        {
            std::unique_lock<std::mutex> lk(cardMutex);
            cv.wait(lk, [drawPile]{ return !cardPiles[drawPile].empty() || gameOver; });
            if (gameOver) break;
            hand[slotReplace] = cardPiles[drawPile].pop();
        }
        if (PRINT_STATES) {
            const std::lock_guard<std::mutex> lk(ioMutex);
            if (gameOver) break;
            std::cout << "P" << drawPile + 1 << ": DRAW [";
            for (int i = 0; i < 4; i++) {
                if (i == slotReplace) std::cout << "*";
                std::cout << hand[i];
                if (i != 3) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
        }
    }
}

/**
 * main
 * 
 * The program takes no arguments.
 * 
 * @param argc The number of arguments passed to the program.
 * @param argv The arguments passed to the program.
 * @return 0 on success, 1 on failure.
 */
int main(int argc, char **argv) {
    std::vector<std::thread> threads;
    srand(time(NULL));

    // Generate piles
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            int chosenCardPos = rand() % cards.size();
            cardPiles[i].push(cards[chosenCardPos]);
            cards.erase(cards.begin() + chosenCardPos);
        }
    }

    // Run game
    for (int i = 0; i < 4; i++) {
        threads.push_back(std::thread(playerThread, i % 4, (i+1) % 4));
    }
    for (auto& t: threads) {
        if (t.joinable()) t.join();
    }

    return 0;
}