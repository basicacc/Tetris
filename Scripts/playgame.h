int startinggame(){
    char ignorekey=-2;
    int gameactive=1,hitground=0,defaultspeed;

    struct coord lastone;
    printf("\033[?25l\033[2J"); //hide cursor and clear screen
    initboard();    //clear board
    init_terminal();    //raw mode
    set_nonblocking();  //so SIGINT dont cause issue

    nextTetromino = rand() % 11;
    

    while(gameactive){
        srand(time(NULL));

        ignorekey = -2;
        while(setup_and_read() != -1); //to fix input issue


        int randomTetromino = nextTetromino;

        while (1) { //not same block in a row
            nextTetromino = rand() % 11;
            if (randomTetromino == nextTetromino) {
                continue;
            } else {
                break;
            }
        }

        lastone.X=WIDTH/2-(shapesleftrightheight[randomTetromino][1]/2);
        lastone.Y=0;
        lastone.type=randomTetromino;
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                lastone.shapeofit[i][j]=shapes[randomTetromino][i][j];
            }
        }
        for(int i=0;i<3;i++){
            lastone.edges[i]=shapesleftrightheight[randomTetromino][i];
        }

        if(placeshapes(randomTetromino)==-1){   //game end
            return -1;
        }
        displayboard();
        usleep(300000);
        hitground=0;

        while(!hitground){
            defaultspeed=300000;
            char current=setup_and_read();

            while(current==ignorekey){
                current=setup_and_read();
            }

            if(tolower(current)=='d'){
                defaultspeed=150000;
                if(moveright(&lastone)==-1){
                    ignorekey='d';
                    defaultspeed=0;
                }
            }
            else if(tolower(current)=='a'){
                defaultspeed=150000;
                if(moveleft(&lastone)==-1){
                    ignorekey='a';
                    defaultspeed=0;
                }
            }
            else if(tolower(current)=='r'){
                defaultspeed=50000;
                if(rotate(&lastone)==-1){
                    ignorekey='r';
                    defaultspeed=0;
                }
            }
            else if(tolower(current)=='s'){
                //increase falling speed
                defaultspeed=50000;
                if(movedown(&lastone)==-1){
                    hitground=1;
                } else {
                    defaultspeed=0;
                }
            }
            else if(current==3){ //Ctrl+C
                printf("\033[?25h");
                restore_terminal();
                return -1;
            }
            else{
                ignorekey=(current!=-1 ? current : -2);
                if(movedown(&lastone)==-1){
                    hitground=1;
                    defaultspeed=50000;
                }
            }
            displayboard();
            usleep(defaultspeed);
        }
        if(lastone.type==10) boomit(&lastone);
        
        if(lastone.Y==0){
            return -1;
        }
        checkforlines();
    }
}