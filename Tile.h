#pragma once
#include <vector>
#include <iostream>
using namespace std;

class Tile {
    vector<Tile*> adjacent;
    bool revealed;
    bool mine;
    bool flagged;
    int count = 0;
    int row;
    int col;
public:
    Tile(bool revealed, bool mine, bool flagged, int row, int col) {
        this->revealed = revealed;
        this->mine = mine;
        this->flagged = flagged;
        this->row = row;
        this->col = col;
    }
    void loadAdjacent(vector<vector<Tile*>> &tiles) {
        int lowerRow = (this->row - 1) >= 0 ? (this->row - 1) : 0;
        int upperRow = (this->row + 1) < tiles.size() ? (this->row + 1) : (int) tiles.size() - 1;
        int lowerCol = (this->col - 1) >= 0 ? (this->col - 1) : 0;
        int upperCol = (this->col + 1) != tiles[0].size() ? (this->col + 1) : (int) tiles[0].size() - 1;
        for (int i = lowerRow; i <= upperRow; ++i) {
            for (int j = lowerCol; j <= upperCol; ++j) {
                if (tiles[i][j] != this) {
                    this->adjacent.push_back(tiles[i][j]);
                }
            }
        }
        for (auto &i : adjacent) {
            if (i->mine) {
                ++this->count;
            }
        }
    }
    void revealAdjacent() {
        if (this->count == 0) {
            for (int i = 0; i < this->adjacent.size(); ++i) {
                if (!this->adjacent[i]->revealed) {
                    if (!this->adjacent[i]->flagged) {
                        this->adjacent[i]->revealed = true;
                    }
                    this->adjacent[i]->revealAdjacent();
                }
            }
        }
    }
    bool getRevealed() const {
        return this->revealed;
    }
    bool getMine() const {
        return this->mine;
    }
    bool getFlagged() const {
        return this->flagged;
    }
    int getCount() const {
        return this->count;
    }
    void setRevealed(bool newRevealed) {
        this->revealed = newRevealed;
    }
    void setFlagged(bool newFlagged) {
        this->flagged = newFlagged;
    }
    vector<Tile*> getAdjacent() {
        return this->adjacent;
    }
};