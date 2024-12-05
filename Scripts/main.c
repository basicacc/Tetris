#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

char *tetrisascii=
    " /$$$$$$$$          /$$               /$$\n"
    "|__  $$__/         | $$              |__/\n"
    "   | $$  /$$$$$$  /$$$$$$    /$$$$$$  /$$  /$$$$$$$\n"
    "   | $$ /$$__  $$|_  $$_/   /$$__  $$| $$ /$$_____/\n"
    "   | $$| $$$$$$$$  | $$    | $$  \\__/| $$|  $$$$$$\n"
    "   | $$| $$_____/  | $$ /$$| $$      | $$ \\____  $$\n"
    "   | $$|  $$$$$$$  |  $$$$/| $$      | $$ /$$$$$$$/\n"
    "   |__/ \\_______/   \\____/ |__/      |__/|_______/\n\n\n";

#define WIDTH 10
#define HEIGHT 20

#define RED "\033[31m\u1401\033[0m"
#define GREEN "\033[32m\u1401\033[0m"
#define YELLOW "\033[33m\u1401\033[0m"
#define BLUE "\033[34m\u1401\033[0m"
#define MAGENTA "\033[35m\u1401\033[0m"
#define CYAN "\033[36m\u1401\033[0m"
#define BOMB "\u156B"
char *blockColors[] = {RED, GREEN, BLUE, YELLOW, BLUE, MAGENTA, CYAN};

char *grid[HEIGHT][WIDTH];
int shapes[11][4][4] = {
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}, 
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 
    {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}} //bomb
};
//default
int shapesleftrightheight[11][3] = { //left = how many zeros on left
    {1,2,4},                         //right = last and most 1s till end
    {0,3,2},                         //height = max amount of 1s on a line
    {0,3,2},
    {0,3,2},
    {0,2,2},
    {0,3,2},
    {0,3,2},
    {0,3,3},
    {0,3,1},
    {0,2,2},
    {0,1,1}
};
//To display points
int totalpoints=0; 
int highestpoint=0;

int nextTetromino = -1;

//To remember the latest properties of moving block
struct coord{
    int X;
    int Y;
    int type;
    int shapeofit[4][4];
    int edges[3];
};

void displayboard() {
    //Updating highestpoint everytime called
    highestpoint = (highestpoint > totalpoints ? highestpoint : totalpoints);
    
    printf("\033[2J\033[H"); //clear screen and put cursor on "Home" (top left)
    
    printf(tetrisascii); //Cool looking ascii art
    
    //Whole display of everything here:
    printf("\t\u250F");
    for (int i = 0; i < WIDTH * 2; i++) printf("\u2501");
    printf("\u2513\tPøIиτs: %d\t    ᕼIGᕼEᔕT: %d",totalpoints,highestpoint);
    printf("\n ");

    //top straight line
    for (int i = 0; i < HEIGHT; i++) {
        printf("\t\u2503"); 
        for (int j = 0; j < WIDTH; j++) {
            printf("%s ", grid[i][j]); 
        }
        printf("\u2503");
        
        //Showing the upcoming piece
        if (i == 3) printf("\t\t    Nҽxƚ");
        if (i == 4 || i == 9){
            printf("\t\t%s",(i==4 ? "\u2546" : "\u2544"));
            for(int k = 0; k < 10; k++) {
                printf("\u254D");
            }
            printf("%s", (i==4 ? "\u2545" : "\u2543"));
        }
        //to display next
        else if (i >= 5 && i < 9) {
            printf("\t\t\u2507");
            for(int k = 0; k < (4-shapesleftrightheight[nextTetromino][1])+(shapesleftrightheight[nextTetromino][1]%2); k++) putchar(' ');
            for (int k = 0; k < 4; k++) {

                printf("%s ", (i-((4-shapesleftrightheight[nextTetromino][2])/2)-5>=0 ? (shapes[nextTetromino][i-((4-shapesleftrightheight[nextTetromino][2])/2)-5][k] == 1 ? (nextTetromino==10 ? BOMB : blockColors[nextTetromino % 7]) : " ") : " "));
            }
            printf("%s\u2507", (nextTetromino==10 ? "\033[2D" : ""));
        }
        
        printf("\n ");
    }
    //down straight line
    printf("\t\u2517");
    for (int i = 0; i < WIDTH * 2; i++) printf("\u2501");
    printf("\u251A\nEnd game: Ctrl+C\n\n");
}

int placeshapes(int type) {
    //choose color (also default shape)
    char *color = blockColors[type % 7];
    if(type==10){ //check if it is a bomb
        color = BOMB;
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shapes[type][i][j] == 1) {
                if(grid[i][j+(WIDTH/2)-(shapesleftrightheight[type][1]/2)]!=" "){
                    displayboard();
                    return -1;  //If there is a shape blocking us return -1
                }
                grid[i][j+(WIDTH/2)-(shapesleftrightheight[type][1]/2)] = color; //place block
            }
        }
    }
    return 0;
}

void initboard() { //clear board
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = " "; 
        }
    }
}





extern int set_nonblocking();   //To be able to read without SIGINT causing problem
extern char setup_and_read();   //Select() and Read() used to read 1 character (timeout = 0.1)
extern int init_terminal();     //set terminal raw mode (non_canoncial and non_echo)
extern int restore_terminal();  //reset to default

#include "movements.h"
#include "playgame.h"

int customgetline(char input[]){    //better than default getline()
    char onechar;
    int lenfornow=0;
    do{
        onechar=getchar();
        input[lenfornow++]=onechar;
    }
    while(onechar!='\n' && onechar!=EOF);
    input[lenfornow--]='\0';
    return lenfornow;
}

void slowlyoutput(char *outputthis){    //to make cool dialog
    int ind=0;
    char onecharacter[1];
    while(outputthis[ind]!='\0' && outputthis[ind]!=-1){
        onecharacter[0]=outputthis[ind++];
        write(1,onecharacter,1);
        usleep(70000);
    }
    return;
}

void dialogforfun(){    //as named just a dialog for fun
    int lenfordialog;
    char inputfordialog[200];
    printf("\033[2J\033[H");
    printf(tetrisascii);

    slowlyoutput("\033[?25l>\033[36mAre you ready for a challenge?\033[0m\n");
    write(1,">\033[?25h",7);
    do{
        lenfordialog=customgetline(inputfordialog);
        if(lenfordialog==1 && inputfordialog[0]=='?'){
            write(1,"\n[\033[36mYES\033[0m] || [\033[36mNO\033[0m]\n",32);
            write(1,"\n\n>\033[?25h",9);
        }
        else if(lenfordialog==3 && tolower(inputfordialog[0])=='y' && tolower(inputfordialog[1])=='e' && tolower(inputfordialog[2])=='s'){
            slowlyoutput("\033[?25l>\033[36mAlrightt, Let's see what you got >:)\033[0m");
            sleep(1);
            return;
        }
        else if(lenfordialog==2 && tolower(inputfordialog[0])=='n' && tolower(inputfordialog[1])=='o'){
            slowlyoutput("\033[?25l>\033[36mI knew you are a coward!\033[0m\033[?25h");
            exit(0);
        }
        else{
            slowlyoutput("\033[?25l>\033[36mUhhh.. write \033[0m[\033[36m?\033[0m]\033[36m to see what you can write...\033[0m\n");
            write(1,">\033[?25h",8);
        }
    }
    while(1);
}

int main() {
    char input[200];
    int lengthofinput=-1;

    dialogforfun();

    while(startinggame()==-1){  //Till game is active (Always return -1)
        highestpoint=(totalpoints > highestpoint ? totalpoints : highestpoint);
        totalpoints=0;
        //check if player wants to play again.
        slowlyoutput(">\033[36mAs expected.. Another loser!\033[0m\n>\033[36mWanna try and lose again? \033[0m[\033[36mYES\033[0m/\033[36mNO\033[0m]\n>\033[?25h");
        restore_terminal(); //restoring so we can accept input in tradition way
        do{
            lengthofinput=customgetline(input);
            if(lengthofinput==2 && tolower(input[0])=='n' && tolower(input[1])=='o'){
                slowlyoutput("\033[?25l>\033[36mBye bye, see you later!!!\033[0m\033[?25h");
                return 0;
            }
            else if(lengthofinput==3 && tolower(input[0])=='y' && tolower(input[1])=='e' && tolower(input[2])=='s'){
                slowlyoutput("\033[?25l>\033[36mHmm interesting..\033[0m\n");
                sleep(1);
                break;
            }
            else{
                slowlyoutput("\033[?25l\n>\033[31mReally? Just write Yes\033[0m/\033[31mNo...\033[0m\n\n>\033[36mWanna try and lose again? \033[0m[\033[36mYES\033[0m/\033[36mNO\033[0m]\n>\033[?25h");
            }
        }
        while(1);
    }
    return 0;
}