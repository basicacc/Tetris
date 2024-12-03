int startinggame(){
    char ignorekey=-2;
    int gameactive=1,hitground=0,defaultspeed;

    struct coord lastone;
    printf("\033[?25l\033[2J");
    initboard();
    set_nonblocking();
    init_terminal();

    nextTetromino = rand() % 11;
    

    while(gameactive){
        srand(time(NULL));

        ignorekey = -2;
        while(setup_and_read() != -1);


        int randomTetromino = nextTetromino;

        while (1) {
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

        if(placeshapes(randomTetromino)==-1){
            return -1;
        }
        displayboard();
        usleep(300000);
        hitground=0;

        while(!hitground){
            defaultspeed=300000;
            char current=setup_and_read();

            if(current == ignorekey){
                ignorekey = -2;
            }

            while(current==ignorekey){
                current=setup_and_read();
                ignorekey=-2;
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
                // Soft drop - increase falling speed
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