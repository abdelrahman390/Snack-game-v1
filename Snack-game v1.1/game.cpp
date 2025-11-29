#include <iostream>
#include <cstdlib>
#include <ctime> 
#include <windows.h>
#include <conio.h>
#include <vector>

using namespace std;

void hideCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;   // hide cursor
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void gotoXY(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int generateRandomNum()
{
    return rand();
}

bool stopGame = 0;
bool looser = 0;

class Player
{
private:
    char dirs[4] = { 't', 'd', 'r', 'l' };
    char dir; // top: t, down: d, right: r, left: l
    int score = 0;
    vector<pair<int, int>> snakeBody;

    void changePlayerPositionBy(int x, int y)
    {
		snakeBody[0].first += x;
        snakeBody[0].second += y;
    }

public:
    Player(int x, int y)
    {
		snakeBody.push_back({ x, y });
        // generate randome number from 0 to 3;
        dir = dirs[generateRandomNum() % 4];
    }

    void changePlayerScore(int n) {
        this->score += n;
    }

    void movePlayer()
    {
		pair <int, int> prevPos = snakeBody[0];
        for (int i = 0; i < snakeBody.size(); i++)
        {
            if (i == 0) {
                switch (dir)
                {
                case 't':
                    changePlayerPositionBy(0, -1);
                    break;
                case 'd':
                    changePlayerPositionBy(0, 1);
                    break;
                case 'r':
                    changePlayerPositionBy(1, 0);
                    break;
                case 'l':
                    changePlayerPositionBy(-1, 0);
                    break;
                }
            } else {
                pair<int, int> temp = snakeBody[i];
                snakeBody[i] = prevPos;
                prevPos = temp;
            }
        }

    }

    void increamentSnakeBody() {
        snakeBody.push_back({0, 0});
	}

    void checkSnakeBodyColl() {
		pair<int, int> headPos = snakeBody[0];
        for (int i = 1; i < snakeBody.size(); i++)
        {
            if(headPos.first == snakeBody[i].first &&
               headPos.second == snakeBody[i].second) {
                looser = 1;
			}
        }
    }

    void setPlayerDir(char c) {
        dir = c;
    }

    int getPlayerXPos()
    {
        return snakeBody[0].first;
    }

    int getPlayerYPos()
    {
        return snakeBody[0].second;
    }

    int getPlayerScore()
    {
        return this->score;
    }

    char getPlayerDir()
    {
        return this->dir;
    }

    vector<pair<int, int>> getSnakeBody()
    {
        return this->snakeBody;
	}

};

void input(Player* p)
{
    if (_kbhit())
    {                      // check if a key was pressed
        int ch = _getch(); // get the key code

        if (ch == 27)
            stopGame = 1; // ESC key to exit

        // Arrow keys send TWO codes: first 0 or 224
        if (ch == 0 || ch == 224)
        {
            int arrow = _getch(); // get the second code

            char c;
            switch (arrow)
            {
            case 72: // up arrow
				if (p->getPlayerDir() != 'd') // prevent reverse direction
                p->setPlayerDir('t');
                break;
            case 80: // down arrow
                if (p->getPlayerDir() != 't') // prevent reverse direction
                p->setPlayerDir('d');
                break;
            case 75: // left arrow
                if (p->getPlayerDir() != 'r') // prevent reverse direction
                p->setPlayerDir('l');
                break;
            case 77: // right arrow
                if (p->getPlayerDir() != 'l') // prevent reverse direction
                p->setPlayerDir('r');
                break;
            }
        }
    }
}

class Screen
{
private:
    int height;
    int width;
    int fruitXPos;
    int fruitYPos;
    Player* player;

    void frameDraw()
    {
        int width = this->width;
        int hight = this->height;

        for (int i = 0; i < hight; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (i == 0 || i == hight - 1)
                {
                    cout << '-';
                }
                else
                {
                    if (j == 0 || j == width - 1)
                    {
                        cout << '|';
                    }
                    else
                    {
                        cout << " ";
                    }
                }
            }
            cout << endl;
        }
        cout << "Score: " << player->getPlayerScore() << endl;
        cout << "Click ESC button to exit." << endl;
    }

    void drawFruit() {
        gotoXY(fruitXPos, fruitYPos);
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED); // Set text color to red
        cout << '*';
        SetConsoleTextAttribute(hConsole, 7);
    }

    void drawPlayer() {
		vector<pair<int, int>> snakeBody = player->getSnakeBody();
        for (int i = 0; i < snakeBody.size(); i++)
        {
            gotoXY(snakeBody[i].first, snakeBody[i].second);
            cout << "O"; // draw player
        }
    }

    void checkFruit() {
        // check if the player eat the fruit
        if (player->getPlayerXPos() == fruitXPos &&
            player->getPlayerYPos() == fruitYPos) {
            player->changePlayerScore(1);
			player->increamentSnakeBody();
            // remove previous fruit
            gotoXY(fruitXPos, fruitYPos);
            cout << " ";   // erase fruit
            // update new values
            fruitXPos = 1 + generateRandomNum() % (width - 2);
            fruitYPos = 1 + generateRandomNum() % (height - 2);
            drawFruit();
        }
    }

    void checkBorders() {
        if (player->getPlayerXPos() <= 0 || player->getPlayerXPos() >= width - 1 ||
            player->getPlayerYPos() <= 0 || player->getPlayerYPos() >= height - 1) {
            looser = 1;
        }
    }

    void updateScore() {
        gotoXY(0, height);
        cout << "Score: " << player->getPlayerScore() << "     ";
    }

    void deletePlayer() {
        vector<pair<int, int>> snakeBody = player->getSnakeBody();
        for (int i = 0; i < snakeBody.size(); i++)
        {
            gotoXY(snakeBody[i].first, snakeBody[i].second);
            cout << " ";   // erase
        }
    }

    void looserMessage() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED); // Set text color to red
        cout << "Looser" << endl;
        SetConsoleTextAttribute(hConsole, 7);
    }

    void gameEnded() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED); // Set text color to red
        cout << "Game Ended" << endl;
        SetConsoleTextAttribute(hConsole, 7);
    }

public:
    Screen(int h, int w, Player* p)
    {
        this->height = h;
        this->width = w;
        player = p;

        // init fruit Positions
        int fX = 1 + generateRandomNum() % (w - 2);
        int fY = 1 + generateRandomNum() % (h - 2);
        while (fX == p->getPlayerXPos() && fY == p->getPlayerYPos())
        {
            fX = 1 + generateRandomNum() % (w - 2);
            fY = 1 + generateRandomNum() % (h - 2);
        }
        this->fruitXPos = fX;
        this->fruitYPos = fY;
    }

    void draw()
    {
        frameDraw();
        drawFruit();
        for (int i = 0; i < 300; i++)
        {
            // system("cls"); // very slow
            if (stopGame) break;
            checkFruit();
            checkBorders();
            player->checkSnakeBodyColl();
            if (looser) break;
            input(player);
            drawPlayer();
            // next step
            Sleep(200);
            updateScore();
            deletePlayer();
            player->movePlayer();
        }
        // move cursor to bottom
        gotoXY(0, height + 2);
        if (looser) {
            looserMessage();
        }
        else {
            gameEnded();
        }
    }
};

int main() {
    hideCursor();
    srand(time(0));

    // screen size
    int h, w;
    h = 15;
    w = 50;

    // player
    int px = w / 2;
    int py = h / 2;
    Player player1(px, py);

    // screen
    Screen mainScreen(h, w, &player1);
    mainScreen.draw();

    system("pause");
    return 0;
}