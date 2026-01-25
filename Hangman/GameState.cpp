#include "GameState.h"
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>

const char* GameManager::BEST_SCORE_FILE = "bestscore.txt";

const std::vector<std::string> GameManager::WORDS = {
    "PROGRAM", "RACUNAR", "TASTATURA", "MONITOR", "PETLJA", "FAJL", 
    "KOMPAJLER", "OBJEKAT", "POKAZIVAC", "NIZ", "KLASA", "FUNKCIJA", 
    "MIS", "HARDVER", "SOFTVER", "MEMORIJA", "DISK", "MREZA", 
    "INTERNET", "PRETRAZIVAC"
};

GameManager::GameManager()
    : currentScore(0), bestScore(0), correctLetters(0), 
      incorrectLetters(0), incorrectWords(0) {
    loadBestScore();
    resetGame();
}

void GameManager::resetGame() {
    state.secret = WORDS[std::rand() % WORDS.size()];
    state.current.assign(state.secret.size(), '_');
    state.tried.clear();
    state.triedWords.clear();
    state.wrong = 0;
    state.finished = false;
    state.win = false;
    currentScore = 0;
}

bool GameManager::alreadyTried(char c) const {
    return std::find(state.tried.begin(), state.tried.end(), c) != state.tried.end();
}

bool GameManager::alreadyTriedWord(const std::string& w) const {
    return std::find(state.triedWords.begin(), state.triedWords.end(), w) != state.triedWords.end();
}

void GameManager::applyGuess(char c) {
    if (state.finished || alreadyTried(c))
        return;
    
    state.tried.push_back(c);
    bool hit = false;
    int lettersFound = 0;
    
    for (size_t i = 0; i < state.secret.size(); ++i) {
        if (state.secret[i] == c) {
            state.current[i] = c;
            hit = true;
            lettersFound++;
        }
    }
    
    if (!hit) {
        state.wrong++;
        incorrectLetters++;
        currentScore -= 80;
    } else {
        correctLetters += lettersFound;
        currentScore += 100 * lettersFound;
    }
    
    if (state.current == state.secret) {
        state.finished = true;
        state.win = true;
        calculateScore();
    } else if (state.wrong >= state.maxWrong) {
        state.finished = true;
        state.win = false;
    }
}

void GameManager::tryWholeWord(const std::string& word) {
    if (word.empty() || word.size() != state.secret.size())
        return;
    
    if (alreadyTriedWord(word))
        return;
    
    state.triedWords.push_back(word);
    
    if (word == state.secret) {
        int totalLetters = (int)state.secret.size();
        int unguessedLetters = 0;
        for (char c : state.current) {
            if (c == '_')
                unguessedLetters++;
        }
        
        state.current = state.secret;
        int wordBonus = (int)((1000.0f / totalLetters) * unguessedLetters);
        currentScore += wordBonus;
        
        state.finished = true;
        state.win = true;
    } else {
        state.wrong++;
        incorrectWords++;
        currentScore -= 125;
        if (state.wrong >= state.maxWrong) {
            state.finished = true;
            state.win = false;
        }
    }
}

void GameManager::loadBestScore() {
    std::ifstream in(BEST_SCORE_FILE);
    if (!in.is_open())
        return;
    int value = 0;
    if (in >> value)
        bestScore = value;
}

void GameManager::saveBestScore() {
    std::ofstream out(BEST_SCORE_FILE, std::ios::trunc);
    if (!out.is_open()) {
        std::cout << "Ne mozemo otvoriti bestscore.txt za pisanje" << std::endl;
        return;
    }
    out << bestScore;
}

void GameManager::calculateScore() {
    if (state.finished && state.win && currentScore > bestScore) {
        bestScore = currentScore;
        saveBestScore();
    }
}
