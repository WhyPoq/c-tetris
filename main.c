/*
    Program description: Tetris game in console
    Author: Konstantin Skliar
    Date:
        24.09.2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>


//inner width and height (width and height of gaming area)
#define W 13
#define H 14

typedef struct Frame{
    int sX;
    int eX;
} Frame;

typedef struct Piece{
    //all possible rotations of a shape
    int (*shapes)[4][4];
    int rotationsCount;
    //actual dimensions of shapes rotations
    Frame* frames;
} Piece;

typedef struct GamePiece{
    int pieceIdx;
    int x;
    int y;
    int curRotation;
    Frame curFrame;
} GamePiece;


/*
    Functions declaration
*/
void init();
void initPieces();
void hidecursor();
char getInput();
Piece createPiece(int (*shapes)[4][4], int rotationsCount);
Frame getShapeFrame(int (*shape)[4]);
void initScreenBuffer();
void setBuffer(int x, int y, char val);
char getBuffer(int x, int y);
void setBufferInGameField(int x, int y, char val);
void outputScreenBuffer();
void drawWalls();
void initGameField();
void setGameField();
int getGameField();
int inGameField();
void drawGameField();
void spawnPiece();
void drawGamePiece();
void moveGamePiece();
int checkCollision();
int checkNextCollision();
void dropPiece();
int checkLose();
void movePiece(int dir);
int calcRotationShift();
void rotate();
void moveDown();
void destroyFullRows();
void fastDown();
void addScore(int destroyedRows);

/*
    Global variables
*/
HANDLE outputHandle;

Piece pieces[7];
char screenBuffer[H + 1][W + 3];

GamePiece gamePiece;
int gameField[H][W];
const int spawnAreaHeight = 4;

int score = 0;


int main(){
    printf("init\n");
    init();

    spawnPiece();

    int gameEnd = 0;
    //game loop
    while(!gameEnd)
    {
        for(int i = 0; i < 20; i++){
            char ch = getInput();

            if(ch == 'a'){
                movePiece(-1);
            }
            if(ch == 'd'){
                movePiece(1); 
            }
            if(ch == 'w'){
                rotate();
            }
            if(ch == 's'){
                fastDown();
            }

            drawWalls();
            drawGameField();

            drawGamePiece();

            outputScreenBuffer();

            Sleep(5);
        }

        if(checkNextCollision()){
            dropPiece();
            if(checkLose()){
                gameEnd = 1;
            }
            else{
                destroyFullRows();
                spawnPiece();
            }
        }
        else{
            moveGamePiece();
        }
    }

    printf("Game end\n");
    getchar();
    return 0;
}

void init(){
    initPieces();
    initScreenBuffer();
    initGameField();

    time_t t;
    srand((unsigned) time(&t));

    outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    hidecursor();
}

//defining all pieces in the game
void initPieces(){
    int TShapeLayout[4][4][4] = {
        {
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    };

    int LShapeLayout[4][4][4] = {
        {
            {1, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        }
    };

    int LMirrorShapeLayout[4][4][4] = {
        {
            {0, 0, 1, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        }
    };

    int lineShapeLayout[2][4][4] = {
        {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0}
        }
    };

    int squareShapeLayout[1][4][4] = {
        {
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        }
    };

    int zShapeLayout[4][4][4] = {
        {
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    };

    int zMirrorShapeLayout[4][4][4] = {
        {
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    };

    pieces[0] = createPiece(TShapeLayout, 4);
    pieces[1] = createPiece(LShapeLayout, 4);
    pieces[2] = createPiece(LMirrorShapeLayout, 4);
    pieces[3] = createPiece(lineShapeLayout, 2);
    pieces[4] = createPiece(squareShapeLayout, 2);
    pieces[5] = createPiece(zShapeLayout, 4);
    pieces[6] = createPiece(zMirrorShapeLayout, 4);
}

Piece createPiece(int (*shapes)[4][4], int rotationsCount){
    Piece piece;
    piece.shapes = malloc(4 * 4 * 4 * sizeof(int));
    memcpy(piece.shapes, shapes, 4 * 4 * 4 * sizeof(int));
    piece.rotationsCount = rotationsCount;
    
    piece.frames = malloc(piece.rotationsCount * sizeof(Frame));
    for(int i = 0; i < piece.rotationsCount; i++){
        piece.frames[i] = getShapeFrame(shapes[i]);
    }

    return piece;
}

Frame getShapeFrame(int (*shape)[4]){
    Frame frame;

    frame.sX = 4;
    frame.eX = 4;

    for(int x = 0; x < 4; x++){
        int freeColumn = 1;
        for(int y = 0; y < 4; y++){
            if(shape[y][x] == 1){
                freeColumn = 0;
                break;
            }
        }

        if(!freeColumn){
            frame.sX = x;
            break;
        }
    }

    for(int x = 4 - 1; x >= 0; x--){
        int freeColumn = 1;
        for(int y = 0; y < 4; y++){
            if(shape[y][x] == 1){
                freeColumn = 0;
                break;
            }
        }

        if(!freeColumn){
            frame.eX = x;
            break;
        }
    }

    return frame;
}

void hidecursor()
{
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(outputHandle, &info);
}


char getInput(){
    char ch = 0;
    if(kbhit()){
        ch = getch();
    }
    return ch;
}

void initScreenBuffer(){
    for(int y = 0; y < H + 1; y++){
        for(int x = 0; x < W + 2; x++){
            screenBuffer[y][x] = '.';
        }
    }

    for(int y = 0; y < H + 1; y++){
        screenBuffer[y][W + 2] = '\0';
    }
}

void setBuffer(int x, int y, char val){
    screenBuffer[y][x] = val;
}

char getBuffer(int x, int y){
    return screenBuffer[y][x];
}

//before setting to buffer transforms position relative to game field to global position
void setBufferInGameField(int x, int y, char val){
    screenBuffer[y][x + 1] = val;
}

void outputScreenBuffer(){
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    SetConsoleCursorPosition(outputHandle, coord);

    char offset[] = "        ";

    printf("%sScore: %d\n\n", offset, score); //tmp

    for(int y = 0; y < H + 1; y++){
        printf("%s%s\n", offset, screenBuffer[y]);
    }
    fflush(stdout);
}

void drawWalls(){
    for(int i = 0; i < H + 1; i++){
        setBuffer(0, i, '@');
        setBuffer(W + 1, i, '@');
    }

    for(int i = 1; i < W + 1; i++){
        setBuffer(i, H, '@');
    }
}

void initGameField(){
    for(int y = 0; y < H; y++){
        for(int x = 0; x < W; x++){
            gameField[y][x] = 0;
        }
    }
}

void setGameField(int x, int y, int val){
    if(inGameField(x, y)){
        gameField[y][x] = val;
    }
}

int getGameField(int x, int y){
    return gameField[y][x];
}

int inGameField(int x, int y){
    return (x >= 0 && x < W && y >= 0 && y < H);
}

void drawGameField(){
    for(int y = 0; y < H; y++){
        for(int x = 0; x < W; x++){
            if(getGameField(x, y) == 1){
                setBufferInGameField(x, y, '#');
            }
            else{
                if(y == spawnAreaHeight - 1){
                    setBufferInGameField(x, y, '=');
                }
                else{
                    setBufferInGameField(x, y, '.');
                }
            }
        }
    }
}

void spawnPiece(){
    gamePiece.pieceIdx = rand() % (sizeof(pieces) / sizeof(Piece));
    gamePiece.curRotation = 0;
    gamePiece.curFrame = pieces[gamePiece.pieceIdx].frames[gamePiece.curRotation];

    gamePiece.x = W / 2 - (gamePiece.curFrame.eX - gamePiece.curFrame.sX) / 2;
    gamePiece.y = 0;
}

void drawGamePiece(){
    Piece* curPiece = &pieces[gamePiece.pieceIdx];
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            if(curPiece->shapes[gamePiece.curRotation][y][x] == 1){
                setBufferInGameField(gamePiece.x + x, gamePiece.y + y, '+');
            }
        }
    }
}

void moveGamePiece(){
    gamePiece.y++;
}

int checkCollision(){
   Piece* curPiece = &pieces[gamePiece.pieceIdx];
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            if(curPiece->shapes[gamePiece.curRotation][y][x] == 1){
                if(!inGameField(gamePiece.x + x, gamePiece.y + y) || getGameField(gamePiece.x + x, gamePiece.y + y) == 1){
                    return 1;
                }
            }
        }
    } 
    return 0;
}

//chech if the piece will collide after moving down by 1
int checkNextCollision(){
    gamePiece.y++;
    int res = checkCollision();
    gamePiece.y--;
    return res;
}

void dropPiece(){
    Piece* curPiece = &pieces[gamePiece.pieceIdx];
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            if(curPiece->shapes[gamePiece.curRotation][y][x] == 1){
                setGameField(gamePiece.x + x, gamePiece.y + y, 1);
            }
        }
    } 
}

int checkLose(){
    Piece* curPiece = &pieces[gamePiece.pieceIdx];
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            if(curPiece->shapes[gamePiece.curRotation][y][x] == 1){
                if(gamePiece.y + y < spawnAreaHeight){
                    return 1;
                }
            }
        }
    } 
    return 0;
}

//move piece left if dir==-1 and right if dir==1
void movePiece(int dir){
    gamePiece.x += dir;

    //not able to move in this direction
    if(checkCollision()){
        gamePiece.x -= dir;
    }
}

//tells how to shift the piece to rotate; returns 0 if there is no way to shift to prevent collision
int calcRotationShift(){
    Piece* curPiece = &pieces[gamePiece.pieceIdx];

    if(gamePiece.x + gamePiece.curFrame.sX < 0){
        return -(gamePiece.x + gamePiece.curFrame.sX);
    }
    else if(gamePiece.x + gamePiece.curFrame.eX > W - 1){
        return W - 1 - (gamePiece.x + gamePiece.curFrame.eX);
    }

    return 0;
}

//rotate the current game piece if possible
void rotate(){
    int prevRotation = gamePiece.curRotation;
    int prevX = gamePiece.x;
    gamePiece.curRotation = (gamePiece.curRotation + 1) % pieces[gamePiece.pieceIdx].rotationsCount;
    gamePiece.curFrame = pieces[gamePiece.pieceIdx].frames[gamePiece.curRotation];
    int shift = calcRotationShift();

    gamePiece.x += shift;
    if(checkCollision()){
        gamePiece.curRotation = prevRotation;
        gamePiece.curFrame = pieces[gamePiece.pieceIdx].frames[gamePiece.curRotation];
        gamePiece.x = prevX;
    }
}

//move piece as down as it possible
void fastDown(){
    while(!checkNextCollision()){
        moveGamePiece();
    }
}

void destroyFullRows(){
    int destroyedRows = 0;
    int nextRow = H - 1;

    for(int y = H - 1; y >= 0; y--){
        int fullRow = 1;
        for(int x = 0; x < W; x++){
            if(getGameField(x, y) == 0){
                fullRow = 0;
                break;
            }
        }
        if(!fullRow){
            for(int x = 0; x < W; x++){
                setGameField(x, nextRow, getGameField(x, y));
            }
            nextRow--;
        }
        else{
            destroyedRows++;
        }
    }

    addScore(destroyedRows);
}

void addScore(int destroyedRows){
    if(destroyedRows == 0)
        return;

    if(destroyedRows == 1){
        score += 100;
    }
    else if(destroyedRows == 2){
        score += 250;
    }
    else if(destroyedRows == 3){
        score += 500;
    }
    else{
        score += 1000;
    }
}