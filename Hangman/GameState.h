#pragma once
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <string>
#include <vector>

struct GameState {
    std::string secret;
    std::string current;
    std::vector<char> tried;
    std::vector<std::string> triedWords;
    int wrong = 0;
    int maxWrong = 8;
    bool finished = false;
    bool win = false;
};

class GameManager {
public:
    GameManager();
    
    void resetGame();
    void applyGuess(char c);
    void tryWholeWord(const std::string& word);
    
    bool alreadyTried(char c) const;
    bool alreadyTriedWord(const std::string& w) const;
    
    void loadBestScore();
    void saveBestScore();
    void calculateScore();
    
    GameState& getState() { return state; }
    const GameState& getState() const { return state; }
    
    int getCurrentScore() const { return currentScore; }
    int getBestScore() const { return bestScore; }
    
    void incrementCorrectLetters(int count) { correctLetters += count; }
    void incrementIncorrectLetters() { incorrectLetters++; }
    void incrementIncorrectWords() { incorrectWords++; }
    void addScore(int points) { currentScore += points; }

private:
    GameState state;
    int currentScore;
    int bestScore;
    int correctLetters;
    int incorrectLetters;
    int incorrectWords;
    
    static const char* BEST_SCORE_FILE;
    static const std::vector<std::string> WORDS;
};

#endif
