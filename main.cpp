#include "Tile.h"
#include <chrono>
#include <fstream>
#include <string>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>
using namespace std;

void setText(sf::Text &text, float x, float y);
void createBoard(vector<vector<Tile*>> &board, int cols, int rows, int mines);
void destroyBoard(vector<vector<Tile*>> &board);
void placeMines(vector<vector<Tile*>> &board, int cols, int rows, int mines);
void toggleDebug(vector<vector<Tile*>> &board, bool debug);
void togglePause(vector<vector<Tile*>> &board, bool pause);
bool checkWin(vector<vector<Tile*>> &board);
void setWin(vector<vector<Tile*>> &board);
void setLose(vector<vector<Tile*>> &board);
void updateLeaderContent(sf::String &content);
void updateLeaderTxt(double &time, sf::String &name, sf::String &content);

int main()
{
    sf::Texture debug;
    debug.loadFromFile("../files/images/debug.png");
    sf::Texture digits;
    digits.loadFromFile("../files/images/digits.png");
    sf::Texture happy;
    happy.loadFromFile("../files/images/face_happy.png");
    sf::Texture lose;
    lose.loadFromFile("../files/images/face_lose.png");
    sf::Texture win;
    win.loadFromFile("../files/images/face_win.png");
    sf::Texture flag;
    flag.loadFromFile("../files/images/flag.png");
    sf::Texture leaderboard;
    leaderboard.loadFromFile("../files/images/leaderboard.png");
    sf::Texture mine;
    mine.loadFromFile("../files/images/mine.png");
    sf::Texture one;
    one.loadFromFile("../files/images/number_1.png");
    sf::Texture two;
    two.loadFromFile("../files/images/number_2.png");
    sf::Texture three;
    three.loadFromFile("../files/images/number_3.png");
    sf::Texture four;
    four.loadFromFile("../files/images/number_4.png");
    sf::Texture five;
    five.loadFromFile("../files/images/number_5.png");
    sf::Texture six;
    six.loadFromFile("../files/images/number_6.png");
    sf::Texture seven;
    seven.loadFromFile("../files/images/number_7.png");
    sf::Texture eight;
    eight.loadFromFile("../files/images/number_8.png");
    sf::Texture pause;
    pause.loadFromFile("../files/images/play.png");
    sf::Texture play;
    play.loadFromFile("../files/images/pause.png");
    sf::Texture hidden;
    hidden.loadFromFile("../files/images/tile_hidden.png");
    sf::Texture revealed;
    revealed.loadFromFile("../files/images/tile_revealed.png");

    string line;
    ifstream config("../files/config.cfg");
    int cols, rows, mines;
    getline(config, line);
    cols = stoi(line);
    getline(config, line);
    rows = stoi(line);
    getline(config, line);
    mines = stoi(line);
    config.close();
    bool running = true;
    bool newGame = true;
    bool debugged = false;
    bool paused = false;
    bool winGame = false;
    bool loseGame = false;
    bool started = false;
    bool beginTime = true;
    bool leaderOpen = false;
    int flags = mines;

    float width = (float) cols * 32;
    float height = (float) rows * 32 + 100;
    sf::RenderWindow welcome(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);

    sf::Font font;
    font.loadFromFile("../files/font.ttf");

    sf::Text welcomeText("WELCOME TO MINESWEEPER!", font, 24);
    welcomeText.setFillColor(sf::Color::White);
    welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(welcomeText, width/2, height/2 - 150);

    sf::Text enterText("Enter your name:", font, 20);
    enterText.setFillColor(sf::Color::White);
    enterText.setStyle(sf::Text::Bold);
    setText(enterText, width/2, height/2 - 75);

    sf::String name = "|";
    sf::Text nameText(name, font, 18);
    nameText.setFillColor(sf::Color::Yellow);
    nameText.setStyle(sf::Text::Bold);
    setText(nameText, width/2, height/2 - 45);

    sf::Text leaderText("LEADERBOARD", font, 20);
    leaderText.setFillColor(sf::Color::White);
    leaderText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(leaderText, (float) (cols * 16)/2, (float) (rows * 16 + 50)/2 - 120);

    sf::String leaderContent = "";
    sf::Text contentText(leaderContent, font, 18);
    contentText.setFillColor(sf::Color::White);
    contentText.setStyle(sf::Text::Bold);
    setText(contentText, (float) (cols * 16)/2, (float) (rows * 16 + 50)/2 + 20);

    while (running) {
        while (welcome.isOpen()) {
            sf::Event event;
            while (welcome.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    welcome.close();
                    return 0;
                }
                if (event.type == sf::Event::TextEntered) {
                    if ((event.text.unicode > 64 && event.text.unicode < 91) || (event.text.unicode > 96 && event.text.unicode < 123)) {
                        if (name.getSize() < 11) {
                            char newChar = tolower((char)event.text.unicode);
                            name.insert(name.getSize() - 1, newChar);
                            if (name.getSize() > 1) {
                                name[0] = toupper((char)name[0]);
                            }
                            nameText.setString(name);
                            setText(nameText, width/2, height/2 - 45);
                        }
                    }
                }
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::BackSpace) {
                        if (name.getSize() > 1) {
                            name.erase(name.getSize() - 2, 1);
                            nameText.setString(name);
                            setText(nameText, width/2, height/2 - 45);
                        }
                    }
                }
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        if (name.getSize() > 1) {
                            welcome.close();
                        }
                    }
                }
            }
            welcome.clear(sf::Color::Blue);
            welcome.draw(welcomeText);
            welcome.draw(enterText);
            welcome.draw(nameText);
            welcome.display();
        }

        sf::RenderWindow game(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);
        vector<vector<Tile*>> gameBoard;
        vector<vector<bool>> revealedBoard;
        sf::Sprite faceButton;
        faceButton.setTexture(happy);
        faceButton.setPosition((cols / 2.0) * 32 - 32, 32 * (rows + .5));
        sf::Sprite debugButton;
        debugButton.setTexture(debug);
        debugButton.setPosition(cols * 32 - 304, 32 * (rows + .5));
        sf::Sprite pausePlayButton;
        pausePlayButton.setTexture(play);
        pausePlayButton.setPosition(cols * 32 - 240, 32 * (rows + .5));
        sf::Sprite leaderButton;
        leaderButton.setTexture(leaderboard);
        leaderButton.setPosition(cols * 32 - 176, 32 * (rows + .5));
        chrono::time_point<std::chrono::system_clock> start, end;
        double elapsed = 0;
        double pausedElapsed = 0;
        while (game.isOpen()) {
            if (newGame) {
                if (!gameBoard.empty()) {
                    destroyBoard(gameBoard);
                    started = true;;
                    start = chrono::system_clock::now();
                }
                gameBoard.clear();
                createBoard(gameBoard, cols, rows, mines);
                faceButton.setTexture(happy);
                paused = false;
                debugged = false;
                newGame = false;
                winGame = false;
                loseGame = false;
                flags = mines;
                elapsed = 0;
                pausedElapsed = 0;
            }
            sf::Event event;
            while (game.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    game.close();
                    destroyBoard(gameBoard);
                    running = false;
                }
                if (event.type == sf::Event::MouseButtonPressed && !leaderOpen) {
                    if (beginTime) {
                        started = true;
                        beginTime = false;
                        start = chrono::system_clock::now();
                    }
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (!winGame && !loseGame) {
                            if ((event.mouseButton.x > (cols * 32 - 240) &&
                                 event.mouseButton.x < (cols * 32 + 64 - 240)) &&
                                event.mouseButton.y > (32 * (rows + .5)) &&
                                event.mouseButton.y < (32 * (rows + .5) + 64)) {
                                paused = !paused;
                            }
                        }
                        if ((event.mouseButton.x > (cols / 2.0 * 32 - 32) &&
                             event.mouseButton.x < (cols / 2.0 * 32 - 32 + 64)) &&
                            event.mouseButton.y > (32 * (rows + .5)) &&
                            event.mouseButton.y < (32 * (rows + .5) + 64)) {
                            newGame = true;
                            debugged = false;
                            toggleDebug(gameBoard, debugged);
                        }
                        if ((event.mouseButton.x > (cols * 32 - 176) &&
                             event.mouseButton.x < (cols * 32 - 176 + 64)) &&
                            event.mouseButton.y > (32 * (rows + .5)) &&
                            event.mouseButton.y < (32 * (rows + .5) + 64)) {
                            sf::RenderWindow leader(sf::VideoMode(cols * 16, rows * 16 + 50), "Minesweeper", sf::Style::Close);
                            updateLeaderContent(leaderContent);
                            contentText.setString(leaderContent);
                            pausePlayButton.setTexture(pause);
                            game.draw(pausePlayButton);
                            setText(contentText, (float) (cols * 16)/2, (float) (rows * 16 + 50)/2 + 20);
                            for (int i = 0; i < rows; ++i) {
                                for (int j = 0; j < cols; ++j) {
                                    sf::Sprite sprite;
                                    sprite.setTexture(revealed);
                                    sprite.setPosition((float) j * 32, (float) i * 32);
                                    game.draw(sprite);
                                }
                            }
                            game.display();
                            leaderOpen = true;
                            while (leader.isOpen()) {
                                while (leader.pollEvent(event)) {
                                    if (event.type == sf::Event::Closed) {
                                        leader.close();
                                    }
                                }
                                end = chrono::system_clock::now();
                                pausedElapsed = (chrono::duration<double>(started ? end - start : chrono::duration<double>::zero())).count() - elapsed;
                                leader.clear(sf::Color::Blue);
                                leader.draw(leaderText);
                                leader.draw(contentText);
                                leader.display();
                            }
                            goto continueLeader;
                        }
                        if (!paused && !winGame && !loseGame) {
                            if ((event.mouseButton.x > (cols * 32 - 304) &&
                                 event.mouseButton.x < (cols * 32 + 64 - 304)) &&
                                event.mouseButton.y > (32 * (rows + .5)) &&
                                event.mouseButton.y < (32 * (rows + .5) + 64)) {
                                debugged = !debugged;
                                toggleDebug(gameBoard, debugged);
                            }
                            if (event.mouseButton.x < (int) width && event.mouseButton.y < (int) (height - 100)) {
                                int row = (int) event.mouseButton.y / 32;
                                int col = (int) event.mouseButton.x / 32;
                                if (!gameBoard[row][col]->getRevealed() && !gameBoard[row][col]->getFlagged()) {
                                    gameBoard[row][col]->setRevealed(true);
                                    if (!gameBoard[row][col]->getMine()) {
                                        gameBoard[row][col]->revealAdjacent();
                                        if (checkWin(gameBoard)) {
                                            winGame = true;
                                            updateLeaderTxt(elapsed, name, leaderContent);
                                            faceButton.setTexture(win);
                                        }
                                    } else {
                                        debugged = true;
                                        toggleDebug(gameBoard, debugged);
                                        faceButton.setTexture(lose);
                                        loseGame = true;
                                    }
                                }
                            }
                        }
                    }
                    if (!paused && !winGame && !loseGame) {
                        if (event.mouseButton.button == sf::Mouse::Right) {
                            if (event.mouseButton.x < (int) width && event.mouseButton.y < (int) (height - 100)) {
                                int row = (int) event.mouseButton.y / 32;
                                int col = (int) event.mouseButton.x / 32;
                                if (!gameBoard[row][col]->getRevealed()) {
                                    bool changeFlag = !gameBoard[row][col]->getFlagged();
                                    gameBoard[row][col]->setFlagged(changeFlag);
                                    changeFlag ? --flags : ++flags;
                                }
                            }
                        }
                    }
                }
            }
            leaderOpen = false;
            continueLeader:;
            game.clear(sf::Color::White);
            if (winGame) {
                setWin(gameBoard);
            }
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    sf::Sprite sprite;
                    if (!gameBoard[i][j]->getRevealed() && !paused) {
                        sprite.setTexture(hidden);
                        sprite.setPosition((float) j * 32, (float) i * 32);
                        game.draw(sprite);
                    } else {
                        sprite.setTexture(revealed);
                        sprite.setPosition((float) j * 32, (float) i * 32);
                        game.draw(sprite);
                    }
                    if (!paused) {
                        if (gameBoard[i][j]->getRevealed() && !gameBoard[i][j]->getMine()) {
                            switch (gameBoard[i][j]->getCount()) {
                                case 0:
                                    sprite.setTexture(revealed);
                                    sprite.setPosition((float) j * 32, (float) i * 32);
                                    break;
                                case 1:
                                    sprite.setTexture(one);
                                    sprite.setPosition((float) j * 32, (float) i * 32);
                                    break;
                                case 2:
                                    sprite.setTexture(two);
                                    sprite.setPosition((float) j * 32, (float) i * 32);
                                    break;
                                case 3:
                                    sprite.setTexture(three);
                                    sprite.setPosition((float) j * 32, (float) i * 32);
                                    break;
                                case 4:
                                    sprite.setTexture(four);
                                    sprite.setPosition((float) j * 32, (float) i * 32);
                                    break;
                                case 5:
                                    sprite.setTexture(five);
                                    sprite.setPosition((float) j * 32, (float) i * 32);
                                    break;
                                case 6:
                                    sprite.setTexture(six);
                                    sprite.setPosition((float) j * 32, (float) i * 32);
                                    break;
                                case 7:
                                    sprite.setTexture(seven);
                                    sprite.setPosition((float) j * 32, (float) i * 32);
                                    break;
                                case 8:
                                    sprite.setTexture(eight);
                                    sprite.setPosition((float) j * 32, (float) i * 32);
                                    break;
                            }
                            game.draw(sprite);
                        }
                        if ((!gameBoard[i][j]->getRevealed() || loseGame) && gameBoard[i][j]->getFlagged()) {
                            sprite.setTexture(flag);
                            sprite.setPosition((float) j * 32, (float) i * 32);
                            game.draw(sprite);
                        }
                        if (gameBoard[i][j]->getRevealed() && gameBoard[i][j]->getMine()) {
                            sprite.setTexture(mine);
                            sprite.setPosition((float) j * 32, (float) i * 32);
                            game.draw(sprite);
                        }
                    }
                }
            }
            string counterString = to_string(flags);
            sf::Sprite sprite;
            sprite.setTexture(digits);
            if (!paused) {
                pausePlayButton.setTexture(play);
            } else {
                pausePlayButton.setTexture(pause);
            }
            if (counterString[0] == '-') {
                sprite.setTextureRect(sf::IntRect(10 * 21, 0, 21, 32));
                sprite.setPosition(12, (float) (32 * (rows + .5) + 16));
                counterString = counterString.substr(1, counterString.size() - 1);
                game.draw(sprite);
            }
            for (int i = 0; i < 3; ++i) {
                if ((int) (counterString.size()) - 3 + i < 0) {
                    sprite.setTextureRect(sf::IntRect(0, 0, 21, 32));
                    sprite.setPosition((float) (33 + (21 * i)), (float) (32 * (rows + .5) + 16));
                } else {
                    sprite.setTextureRect(sf::IntRect((int) (counterString[(int) (counterString.size()) - 3 + i] - '0') * 21, 0, 21, 32));
                    sprite.setPosition((float) (33 + (21 * i)), (float) (32 * (rows + .5) + 16));
                }
                game.draw(sprite);
            }
            if (started) {
                end = chrono::system_clock::now();
            }
            if (!paused && !winGame && !loseGame) {
                elapsed = (chrono::duration<double>(started ? end - start : chrono::duration<double>::zero())).count() - pausedElapsed;
            } else {
                pausedElapsed = (chrono::duration<double>(started ? end - start : chrono::duration<double>::zero())).count() - elapsed;
            }
            sprite.setTexture(digits);
            int minutes = (int) (elapsed / 60);
            int secondsLeft = (int) elapsed - (minutes * 60);
            sprite.setTextureRect(sf::IntRect(started ? (int) minutes / 10 * 21 : 0, 0, 21, 32));
            sprite.setPosition((float) cols * 32 - 97, (float) (32 * (rows + .5) + 16));
            game.draw(sprite);
            sprite.setTextureRect(sf::IntRect(started ? (int) minutes % 10 * 21 : 0, 0, 21, 32));
            sprite.setPosition((float) cols * 32 - 97 + 21, (float) (32 * (rows + .5) + 16));
            game.draw(sprite);
            sprite.setTextureRect(sf::IntRect(started ? (int) secondsLeft / 10 * 21 : 0, 0, 21, 32));
            sprite.setPosition((float) cols * 32 - 54, (float) (32 * (rows + .5) + 16));
            game.draw(sprite);
            sprite.setTextureRect(sf::IntRect(started ? (int) secondsLeft % 10 * 21 : 0, 0, 21, 32));
            sprite.setPosition((float) cols * 32 - 54 + 21, (float) (32 * (rows + .5) + 16));
            game.draw(sprite);
            game.draw(faceButton);
            game.draw(debugButton);
            game.draw(pausePlayButton);
            game.draw(leaderButton);
            game.display();
        }
    }
}

void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

void createBoard(vector<vector<Tile*>> &board, int cols, int rows, int mines) {
    for (int i = 0; i < rows; ++i) {
        vector<Tile*> row;
        board.push_back(row);
        for (int j = 0; j < cols; ++j) {
            board[i].push_back(nullptr);
        }
    }
    placeMines(board, cols, rows, mines);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (board[i][j] == nullptr) {
                board[i][j] = new Tile(false, false, false, i, j);
            }
        }
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (!board[i][j]->getMine()) {
                board[i][j]->loadAdjacent(board);
            }
        }
    }
}

void destroyBoard(vector<vector<Tile*>> &board) {
    for (int i = 0; i < board.size() - 1; ++i) {
        for (int j = 0; j < board[i].size() - 1; ++j) {
            delete board[i][j];
        }
    }
}

void placeMines(vector<vector<Tile*>> &board, int cols, int rows, int mines) {
    int col;
    int row;
    if (mines == 0) {
        return;
    } else {
        col = rand() % cols;
        row = rand() % rows;
        if (board[row][col] == nullptr) {
            board[row][col] = new Tile(false, true, false, row, col);
            placeMines(board, cols, rows, --mines);
        } else {
            placeMines(board, cols, rows, mines);
        }
    }
}

void toggleDebug(vector<vector<Tile*>> &board, bool debug) {
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            if (board[i][j]->getMine()) {
                board[i][j]->setRevealed(debug);
            }
        }
    }
}

bool checkWin(vector<vector<Tile*>> &board) {
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            if (!board[i][j]->getRevealed() && !board[i][j]->getMine()) {
                return false;
            }
        }
    }
    return true;
}

void setWin(vector<vector<Tile*>> &board) {
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            if (board[i][j]->getMine()) {
                board[i][j]->setFlagged(true);
            }
        }
    }
}

void updateLeaderContent(sf::String &content) {
    content.clear();
    ifstream file("../files/leaderboard.txt");
    string line;
    int i = 1;
    while (getline(file, line) && i < 6) {
        content += sf::String(to_string(i) + '\t' + line.substr(0, 5) + '\t');
        content += sf::String(line.substr(6) + "\n\n");
        ++i;
    }
}

void updateLeaderTxt(double &time, sf::String &name, sf::String &content) {
    ifstream fileIn("../files/leaderboard.txt");
    double minutes;
    double seconds;
    bool timeAdded = false;
    sf::String timeStr;
    sf::String newTime;
    minutes = time / 60;
    seconds = (int) time % 60;
    newTime = to_string((int) minutes / 10) + to_string((int) minutes % 10) + ":" + to_string((int) seconds / 10) +
              to_string((int) seconds % 10);
    string line, newLine, outStr;
    int i = 0;
    while (getline(fileIn, line) && i < 5) {
        timeStr = line.substr(0, 5);
        if (newTime < timeStr && !timeAdded) {
            timeAdded = true;
            newLine = newTime.toAnsiString() + ", " + name.substring(0, name.getSize() - 1).toAnsiString() + "*";
            outStr += newLine + "\n";
            i++;
        } else {
            if (line[line.size() - 1] == '*') {
                line = line.substr(0, line.size() - 1);
            }
        }
        outStr += line + "\n";
        i++;
        line.clear();
    }
    ofstream fileOut("../files/leaderboard.txt");
    fileOut << outStr;
    updateLeaderContent(content);
}