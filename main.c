#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include<ctype.h>
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
#define WIDTH 10
#define HEIGHT 20

#define RED '#'
#define GREEN '#'
#define BLUE '#'
#define YELLOW '#'

char blockColors[] = {RED, GREEN, BLUE, YELLOW};

char grid[HEIGHT][WIDTH];
int shapes[7][4][4] = {
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}, 
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 
    {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}
};

int shapesleftrightheight[7][3] = {
    {1,2,4},
    {0,3,2},
    {0,3,2},
    {0,3,2},
    {0,2,2},
    {0,3,2},
    {0,3,2}
};

struct coord{
    int X;
    int Y;
    int type;
};

void placeshapes(int type) {
    char color = blockColors[type % 4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shapes[type][i][j] == 1) {
                if (i < HEIGHT && j < WIDTH) { 
                    grid[i][j] = color; 
                }
            }
        }
    }
}

void initboard() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = ' '; 
        }
    }
}

void displayboard() {
    #ifdef _WIN32
    system("cls");
    #endif
    printf("\033[2J\033[H");
    printf("      TETRIS GAME\n ");
    
    for (int i = 0; i < WIDTH * 2 + 2; i++) printf("=");
    printf("\n ");

    for (int i = 0; i < HEIGHT; i++) {
        printf("|"); 
        for (int j = 0; j < WIDTH; j++) {
            printf("%c ", grid[i][j]); 
        }
        printf("|\n ");
    }

    for (int i = 0; i < WIDTH * 2 + 2; i++) printf("=");
    printf("\n");
}

int movedown(struct coord* lastone){
    usleep(300000);
    if(shapesleftrightheight[lastone->type][2]+lastone->Y==HEIGHT) return -1;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(shapes[lastone->type][i][j]==1 && shapes[lastone->type][i+1][j]!=1){
                if(grid[lastone->Y+i+1][lastone->X+j]=='#') return -1;
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(shapes[lastone->type][i][j]==1){
                if(shapes[lastone->type][i-1][j]!=1 || i-1<0){
                    grid[lastone->Y+i][lastone->X+j]=' ';
                }
                grid[lastone->Y+i+1][lastone->X+j]=blockColors[lastone->type % 4];
            }
        }
    }
    lastone->Y++;
    return 0;
}

int moveleft(struct coord* lastone){
    if(shapesleftrightheight[lastone->type][0]+lastone->X<=0) return -1;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(shapes[lastone->type][i][j]==1){
                if(grid[lastone->Y+i][lastone->X+j-1]=='#') return -1;
                else break;
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(shapes[lastone->type][i][j]==1){
                if(shapes[lastone->type][i][j+1]!=1 || j+1>WIDTH){
                    grid[lastone->Y+i][lastone->X+j]=' ';
                }
                grid[lastone->Y+i][lastone->X+j-1]=blockColors[lastone->type % 4];
            }
        }
    }
    lastone->X--;
}

int moveright(struct coord* lastone){
    if(shapesleftrightheight[lastone->type][1]+lastone->X>=WIDTH) return -1;
    for(int i=3;i>=0;i--){
        for(int j=3;j>=0;j--){
            if(shapes[lastone->type][i][j]==1){
                if(grid[lastone->Y+i][lastone->X+j+1]=='#') return -1;
                else break;
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(shapes[lastone->type][i][j]==1){
                if(shapes[lastone->type][i][j-1]!=1 || j-1<0){
                    grid[lastone->Y+i][lastone->X+j]=' ';
                }
                grid[lastone->Y+i][lastone->X+j+1]=blockColors[lastone->type % 4];
            }
        }
    }
    lastone->X++;
}

extern int set_nonblocking();
extern char setup_and_read();
extern int init_terminal();

int main() {
    int gameactive=1,hitground=0;
    struct coord lastone;
    printf("\033[2J");
    initboard();
    set_nonblocking();
    init_terminal();
    while(gameactive){
        srand(time(NULL));
        int randomTetromino = rand() % 7;
        lastone.X=0;
        lastone.Y=0;
        lastone.type=randomTetromino;
        placeshapes(randomTetromino); 
        displayboard();
        hitground=0;

        
        while(!hitground){
            char current=setup_and_read();
            if(current == -1){
                if(movedown(&lastone)==-1){
                    hitground=1;
                }
            }
            else if(tolower(current)=='d'){
                moveright(&lastone);
            }
            else if(tolower(current)=='a'){
                moveleft(&lastone);
            }
            displayboard();
        }
    }
    return 0;
}