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

int shapesleftrightheight[11][3] = {
    {1,2,4},
    {0,3,2},
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

int totalpoints=0;
int highestpoint=0;
int nextTetromino = -1;

struct coord{
    int X;
    int Y;
    int type;
    int shapeofit[4][4];
    int edges[3];
};

void displayboard() {
    highestpoint = (highestpoint > totalpoints ? highestpoint : totalpoints);
    printf("\033[2J\033[H");
    printf(tetrisascii);
    printf("\t\u250F");
    for (int i = 0; i < WIDTH * 2; i++) printf("\u2501");
    printf("\u2513\tPøIиτs: %d\t    ᕼIGᕼEᔕT: %d",totalpoints,highestpoint);
    printf("\n ");

    for (int i = 0; i < HEIGHT; i++) {
        printf("\t\u2503"); 
        for (int j = 0; j < WIDTH; j++) {
            printf("%s ", grid[i][j]); 
        }
        printf("\u2503");
        
        // showing the upcoming lil piece
        if (i == 3) printf("\t\t    Nҽxƚ");
        if (i == 4 || i == 9){
            printf("\t\t%s",(i==4 ? "\u2546" : "\u2544"));
            for(int k = 0; k < 10; k++) {
                printf("\u254D");
            }
            printf("%s", (i==4 ? "\u2545" : "\u2543"));
        }
        else if (i >= 5 && i < 9) {
            printf("\t\t\u2507");
            for(int k = 0; k < (4-shapesleftrightheight[nextTetromino][1])+(shapesleftrightheight[nextTetromino][1]%2); k++) putchar(' ');
            for (int k = 0; k < 4; k++) {

                printf("%s ", (i-((4-shapesleftrightheight[nextTetromino][2])/2)-5>=0 ? (shapes[nextTetromino][i-((4-shapesleftrightheight[nextTetromino][2])/2)-5][k] == 1 ? (nextTetromino==10 ? BOMB : blockColors[nextTetromino % 7]) : " ") : " "));
            }
            printf("%s\u254F", (nextTetromino==10 ? "\033[2D" : ""));
        }
        
        printf("\n ");
    }
    printf("\t\u2517");
    for (int i = 0; i < WIDTH * 2; i++) printf("\u2501");
    printf("\u251A\n");
}

int placeshapes(int type) {
    char *color = blockColors[type % 7];
    if(type==10){
        color = BOMB;
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shapes[type][i][j] == 1) {
                if(grid[i][j+(WIDTH/2)-(shapesleftrightheight[type][1]/2)]!=" "){
                    displayboard();
                    return -1;
                }
                if (i < HEIGHT && j < WIDTH) { 
                    grid[i][j+(WIDTH/2)-(shapesleftrightheight[type][1]/2)] = color; 
                }
            }
        }
    }
    return 0;
}

void initboard() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = " "; 
        }
    }
}



int movedown(struct coord* lastone){
    char *color = blockColors[lastone->type % 7];
    if(lastone->type==10){
        color = BOMB;
    }
    if(lastone->edges[2]+lastone->Y==HEIGHT) return -1;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(lastone->shapeofit[i][j]==1 && (lastone->shapeofit[i+1][j]!=1 || i+1>=4)){
                if(grid[lastone->Y+i+1][lastone->X+j]!=" ") return -1;
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(lastone->shapeofit[i][j]==1){
                if(lastone->shapeofit[i-1][j]!=1 || i-1<0){
                    grid[lastone->Y+i][lastone->X+j]=" ";
                }
                grid[lastone->Y+i+1][lastone->X+j]=color;
            }
        }
    }
    lastone->Y++;
    return 0;
}

int moveleft(struct coord* lastone){
    char *color = blockColors[lastone->type % 7];
    if(lastone->type==10){
        color = BOMB;
    }
    if(lastone->edges[0]+lastone->X<=0) return -1;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(lastone->shapeofit[i][j]==1){
                if(grid[lastone->Y+i][lastone->X+j-1]!=" ") return -1;
                else break;
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(lastone->shapeofit[i][j]==1){
                if(lastone->shapeofit[i][j+1]!=1 || j+1>WIDTH){
                    grid[lastone->Y+i][lastone->X+j]=" ";
                }
                grid[lastone->Y+i][lastone->X+j-1]=color;
            }
        }
    }
    lastone->X--;
    return 0;
}

int moveright(struct coord* lastone){
    char *color = blockColors[lastone->type % 7];
    if(lastone->type==10){
        color = BOMB;
    }
    if(lastone->edges[1]+lastone->X>=WIDTH) return -1;
    for(int i=3;i>=0;i--){
        for(int j=3;j>=0;j--){
            if(lastone->shapeofit[i][j]==1){
                if(grid[lastone->Y+i][lastone->X+j+1]!=" ") return -1;
                else break;
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(lastone->shapeofit[i][j]==1){
                if(lastone->shapeofit[i][j-1]!=1 || j-1<0){
                    grid[lastone->Y+i][lastone->X+j]=" ";
                }
                grid[lastone->Y+i][lastone->X+j+1]=color;
            }
        }
    }
    lastone->X++;
    return 0;
}

void movelinesdown(int height){
    int end=0;
    for(int i=height;i>0;i--){
        for (int j=0;j<WIDTH;j++){
            grid[i][j]=grid[i-1][j];
        }
    }
    while(!end){
        end=1;
        for(int i=HEIGHT-2;i>=0;i--){
            for (int j=0;j<WIDTH;j++){
                if((grid[i][j-1]==" " || j-1<0) && (grid[i][j+1]==" " || j+1>=WIDTH) && grid[i+1][j]==" " && i+1<HEIGHT && grid[i][j]!=" "){
                    grid[i+1][j]=grid[i][j];
                    grid[i][j]=" ";
                    end=0;
                }
            }
        }
        displayboard();
        usleep(300000);
    }
}

void checkforlines(){
    int wrong=0,nonefound=0;
    while(!nonefound){
        nonefound=1;
        for (int i=HEIGHT-1;i>=0;i--){
            wrong=0;
            for(int j=0;j<WIDTH;j++){
                if(grid[i][j]==" "){
                    wrong=1;
                    break;
                }
            }
            if(!wrong){
                movelinesdown(i);
                nonefound=0;
                totalpoints+=10;
                break;
            }
        }
    }
}

int rotate(struct coord* lastone) {
    char *color = blockColors[lastone->type % 7];
    if(lastone->type==10){
        color = BOMB;
    }
    int extra[4][4] = {0}; 
    int left=4, right=-1, down=-1;
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            if(lastone->shapeofit[i][j] == 1) {
                grid[lastone->Y+i][lastone->X+j]=" ";
            }
        }
    }
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            extra[j][lastone->edges[2]-i-1] = lastone->shapeofit[i][j];
        }
    }
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            if(extra[i][j] == 1) {

                if(lastone->X + j >= WIDTH || lastone->X + j < 0 || 
                   lastone->Y + i >= HEIGHT || 
                   grid[lastone->Y + i][lastone->X + j] != " ") {
                    for(int y = 0; y < 4; y++) {
                        for(int x = 0; x < 4; x++) {
                            if(lastone->shapeofit[y][x] == 1) {
                                grid[lastone->Y + y][lastone->X + x] = color;
                            }
                        }
                    }
                    return -1;
                }
            }
        }
    }
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            lastone->shapeofit[i][j]=extra[i][j];
            if(extra[i][j] == 1) {
                if(j < left) left = j;
                if(j+1>right) right = j+1;
                if(i+1>down) down = i+1;
            }
        }
    }
    lastone->edges[0]=left;
    lastone->edges[1]=right;
    lastone->edges[2]=down;
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            if(lastone->shapeofit[i][j] == 1) {
                grid[lastone->Y+i][lastone->X+j] = color;
            }
        }
    }
    return 0;
}

void boomit(struct coord* lastone){
    totalpoints+=10;
    for(int i=0;i<HEIGHT;i++){
        if(grid[i][lastone->X]!=" ") totalpoints+=5;
        grid[i][lastone->X]=" ";
    }
    movelinesdown(lastone->Y);
}

extern int set_nonblocking();
extern char setup_and_read();
extern int init_terminal();
extern int restore_terminal();

#include "playgame.h"

int customgetline(char input[]){
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

void slowlyoutput(char *outputthis){
    int ind=0;
    char onecharacter[1];
    while(outputthis[ind]!='\0' && outputthis[ind]!=-1){
        onecharacter[0]=outputthis[ind++];
        write(1,onecharacter,1);
        usleep(70000);
    }
    return;
}

void dialogforfun(){
    int lenfordialog;
    char inputfordialog[200];
    printf("\033[2J\033[H");
    printf(tetrisascii);

    slowlyoutput(">\033[36mAre you ready for a challenge?\033[0m\n");
    write(1,">",1);
    do{
        lenfordialog=customgetline(inputfordialog);
        if(lenfordialog==1 && inputfordialog[0]=='?'){
            write(1,"\n[\033[36mYES\033[0m] || [\033[36mNO\033[0m]\n",32);
            write(1,"\n\n>",3);
        }
        else if(lenfordialog==3 && tolower(inputfordialog[0])=='y' && tolower(inputfordialog[1])=='e' && tolower(inputfordialog[2])=='s'){
            slowlyoutput(">\033[36mAlrightt, Let's see what you got >:)\033[0m\0");
            sleep(1);
            return;
        }
        else if(lenfordialog==2 && tolower(inputfordialog[0])=='n' && tolower(inputfordialog[1])=='o'){
            slowlyoutput(">\033[36mI knew you are a coward!\033[0m");
            exit(0);
        }
        else{
            slowlyoutput(">\033[36mUhhh.. write \033[0m[\033[36m?\033[0m]\033[36m to see what you can write...\033[0m\n");
            write(1,">",2);
        }
    }
    while(1);

}

int main() {
    char input[200];
    int lengthofinput=-1;

    dialogforfun();


    while(startinggame()==-1){
        highestpoint=(totalpoints > highestpoint ? totalpoints : highestpoint);
        totalpoints=0;
        printf("\033[?25h");
        restore_terminal();
        printf("Lil bro, you need to delete the game HAHAHAHAHA\nWanna lose again? [YES/NO]:");
        do{
            lengthofinput=customgetline(input);
            if(lengthofinput==2 && tolower(input[0])=='n' && tolower(input[1])=='o'){
                printf("Bye bye Lil bro!!!");
                return 0;
            }
            else if(lengthofinput==3 && tolower(input[0])=='y' && tolower(input[1])=='e' && tolower(input[2])=='s'){
                printf("Hmm interesting..\n");
                sleep(1);
                break;
            }
            else{
                printf("\033[31m\nYou are dumb for sure...\n\033[0m\nWanna lose again? [YES/NO]:");
            }
        }
        while(1);
    }
    return 0;
}