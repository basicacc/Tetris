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
    int end=0, littlepiece,startofpiece,endofpiece;
    for(int i=height;i>0;i--){
        for (int j=0;j<WIDTH;j++){
            grid[i][j]=grid[i-1][j];
        }
    }
    displayboard();
    usleep(100000);
    while(!end){
        end=1;
        for(int i=HEIGHT-1;i>=0;i--){
            startofpiece=-1; endofpiece=-1;
            for (int j=0;j<WIDTH;j++){
                if(startofpiece!=1 && endofpiece!=-1){
                    for(int k=startofpiece;k<=endofpiece;k++){
                        for(int z=i;z<HEIGHT && grid[z+1][k]==" ";z++){
                            grid[z+1][k]=grid[z][k];
                            grid[z][k]=" ";
                        }
                    }
                    displayboard();
                    usleep(100000);
                    startofpiece=-1;
                    endofpiece=-1;
                    end=0;
                }
                if((grid[i][j-1]==" " || j-1<0) && (grid[i][j+1]==" " || j+1>=WIDTH) && grid[i+1][j]==" " && i+1<HEIGHT && grid[i][j]!=" "){
                    for(int k=1;k<HEIGHT-1;k++){
                        if(grid[i+k][j]==" " && i+k<HEIGHT){
                            grid[i+k][j]=grid[i+k-1][j];
                            grid[i+k-1][j]=" ";
                            end=0;
                        }
                    }
                }
                else if(startofpiece==-1 && (grid[i][j-1]==" " || j-1<0) && (grid[i][j+1]!=" " || j+1>=WIDTH) && grid[i+1][j]==" " && i+1<HEIGHT && grid[i][j]!=" "){
                    endofpiece=-1;
                    startofpiece=j;
                }
                else if(startofpiece!=-1 && (grid[i][j]==" " || grid[i+1][j]!=" ")){
                    startofpiece=-1;
                }
                else if(startofpiece!=-1 && (grid[i][j+1]==" " || j+1>=WIDTH)){
                    endofpiece=j;
                }
            }
            if(startofpiece!=-1 && endofpiece!=-1){
               for(int k=startofpiece;k<=endofpiece;k++){
                    grid[i+1][k]=grid[i][k];
                    grid[i][k]=" ";
                    end=0;
                } 
                displayboard();
                usleep(300000);
            }
        }
        displayboard();
        usleep(300000);
    }
}

void checkforlines(){   //if there is a empty line
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

void boomit(struct coord* lastone){ //bomb destroys vertical and horizontal
    totalpoints+=10;
    for(int i=0;i<HEIGHT;i++){
        if(grid[i][lastone->X]!=" ") totalpoints+=5;
        grid[i][lastone->X]=" ";
    }
    movelinesdown(lastone->Y);
}