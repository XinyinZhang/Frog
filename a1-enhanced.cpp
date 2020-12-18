/*
CS 349 Assignment 1 enhanced: Multi-player support
// (Allow two players to play the game simultaneously, each controlling their own Frog 
using distinct controls. Player one control the left frog and player two control the right frog.
player one uses arrow key to control the movement of left frog
player two uses w:up, a:left, s:down, d:right to control the movement of right frog
the level goes up only until both frogs reaches the goal area and one of them press the 'n' button
if one of them collide the enemy block, both frogs go back to level 1)
*/
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <list>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <vector>
/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;

struct XInfo {
    Display*    display;
    int      screen;
    Window   window;
    GC       gc;
};

void error( string str ) {
    cerr << str << endl;
    exit(0);
}


// get microseconds
unsigned long now() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}



//initial level number
int levelNum = 1;

//An abstract class representing displayable things

class Displayable {
public:
    virtual void paint(XInfo& xinfo) = 0;
};


//A Text displayable
class Text : public Displayable {

public:
    virtual void paint(XInfo& xinfo) {
        XDrawImageString( xinfo.display, xinfo.window, xinfo.gc,
                          this->x, this->y, this->s.c_str(), this->s.length() );
    }
    Text(int x, int y, string s): x(x), y(y), s(s)  {}

    void addLevelNum(XInfo &xinfo){
        levelNum++;
    }


private:
    int x;
    int y;
    string s; // string to show
};



//a class to draw all the enemy blocks and frog
class Retrangle : public Displayable {
public:
    virtual void paint(XInfo& xinfo){

    XSetFillStyle(xinfo.display, xinfo.gc, FillSolid);
    XSetLineAttributes(xinfo.display, xinfo.gc,
                       1, LineSolid, CapButt, JoinRound);  
        

    //draw a filled 50px by 50px rectrangle
    XFillRectangle(xinfo.display, xinfo.window, xinfo.gc, x, y, width, height);

    }

    //function to control the event:
    //1: enemy block move to left or right automatically
    //2:  when a block reaches one side of the window, a new block comes out from the other side of the window.
    void moveEnemy(XInfo& xinfo, string movement, int speed, int width, int interval){
        if (movement == "right"){
           if (x >= interval + 850 - width){
            x = 0 - width;
           } //2
           
            x = x + speed;   //1         
        }
        else if (movement == "left"){

            if (x <= 0 - interval){
                x = 850; 
            } // 2

            x = x - speed; //1
        }
        

    }


    //control frog to move 50px in the direction when the arrow key are pressed
    void moveRetrangle(XInfo& xinfo, string movement) {
    
        
        if (movement == "up"){
            if (y <= 0){
                y = 0;
            }
            else {
                y = y - 50;
                

            }
        

        }

        else if (movement == "down"){
            if (y >= 200){
                y = 200;
            }

            else if (y <= 0){
                y = 0;
            }

            else {
                y = y + 50;

            }
            
        }




        else if (movement == "left"){
            if (x <= 0){
                x = 0;
            }
            else {
                x = x - 50;

            }
            
        }

        else if (movement == "right"){

            if (x >= 800){
                x = 800;
            }

            else {
                x = x + 50;

            }

        }

      /*  else if (movement == "n"){
            if (y <= 0){
               x = 350;
                y = 200;

            }
            

        }*/
    

    }

    //control frog 2 to move when 'w', 'a', 's', 'd' are pressed
     void moveRetrangle2(XInfo& xinfo, string movement) {
    
        
        if (movement == "w"){
            if (y <= 0){
                y = 0;
            }
            else {
                y = y - 50;
                

            }
        

        }

        else if (movement == "s"){
            if (y >= 200){
                y = 200;
            }

            else if (y <= 0){
                y = 0;
            }

            else {
                y = y + 50;

            }
            
        }




        else if (movement == "a"){
            if (x <= 0){
                x = 0;
            }
            else {
                x = x - 50;

            }
            
        }

        else if (movement == "d"){

            if (x >= 800){
                x = 800;
            }

            else {
                x = x + 50;

            }

        }

       /* else if (movement == "n"){
           if (y <= 0){
               x = 450;
                y = 200;

            }
            

       }*/
    

    }


    //get the x, y, or width field of the retrangle
    int getX(){return x;}
    int getY(){return y;}
    int getWidth(){return width;}

    void changeX(int newX) {x = newX;}
    void changeY(int newY) {y = newY;}


    // constructor for retrangle
    Retrangle(int x, int y, int width, int height): x(x), y(y), width(width), height(height) {}



private:
    int x;
    int y;
    int width;
    int height;
    

};





/*
 * Create a window
 */
void initX(int argc, char* argv[], XInfo& xinfo) {
    XSizeHints hints;
    hints.x = 100;
    hints.y = 100;
    hints.width = 850;
    hints.height = 250;
    hints.flags = PPosition | PSize;
    /*
    * Display opening uses the DISPLAY  environment variable.
    * It can go wrong if DISPLAY isn't set, or you don't have permission.
    */
    xinfo.display = XOpenDisplay( "" );
    if ( !xinfo.display )   {
        error( "Can't open display." );
    }
    /*
    * Find out some things about the display you're using.
    */
    // DefaultScreen is as macro to get default screen index
    xinfo.screen = DefaultScreen( xinfo.display ); 
    unsigned long white, black;
    white = XWhitePixel( xinfo.display, xinfo.screen ); 
    black = XBlackPixel( xinfo.display, xinfo.screen );
    xinfo.window = XCreateSimpleWindow(
        xinfo.display,              // display where window appears
        DefaultRootWindow( xinfo.display ), // window's parent in window tree
        hints.x, hints.y,           // upper left corner location
        hints.width, hints.height,  // size of the window
        10,                         // width of window's border
        black,                      // window border colour
        white );                        // window background colour
    // extra window properties like a window title
    XSetStandardProperties(
        xinfo.display,      // display containing the window
        xinfo.window,       // window whose properties are set
        "Frog",    // window's title
        "OW",               // icon's title
        None,               // pixmap for the icon
        argv, argc,         // applications command line args
        &hints);            // size hints for the window
    /*
     * Put the window on the screen.
     */
    XSelectInput( xinfo.display, xinfo.window,
                  ButtonPressMask | KeyPressMask | ButtonMotionMask );

    XMapRaised( xinfo.display, xinfo.window );
    XFlush(xinfo.display);

    //give server time to setup before sending drawing commands
    sleep(1);
}



// function to repaint a display list

void repaint( list<Displayable*> dList, XInfo& xinfo) {
  list<Displayable*>::const_iterator begin = dList.begin();
  list<Displayable*>::const_iterator end = dList.end();
  XClearWindow(xinfo.display, xinfo.window);
  while ( begin != end ) {
    Displayable* d = *begin;
    d->paint(xinfo);
    begin++;
  }
  XFlush(xinfo.display);
}





//function to detect collision between the blocks and Frog, 
//if the Frog and a block collide, reset the level to 1
//move the frog back to initial position

void checkCollision(vector <Retrangle*>& enemy, Retrangle* frog, Retrangle* frog2){
    int x = frog->getX();
    int y = frog->getY();

   

    for (int i = 0; i < enemy.size(); i++){
        int blockY = enemy[i]->getY();
        if (blockY == y){
            int blockX = enemy[i]->getX();
            int blockW = enemy[i]->getWidth();

            if ((x >= blockX - 50) && (x <= blockX + blockW)){
                levelNum = 1;
                frog->changeX(350);
                frog->changeY(200);

                frog2->changeX(450);
                frog2->changeY(200);
                break;

            }
        }
    }


}

//function to go to a new level
void goToNextLevel(Retrangle* frog1, Retrangle* frog2){
    int y_1 = frog1->getY();
    int y_2 = frog2->getY();

           if (y_1 <= 0 && y_2 <= 0){
                levelNum = levelNum + 1;

                frog1->changeX(450);
                frog1->changeY(200);

                frog2->changeX(350);
                frog2->changeY(200);

            } else {
                cout << "Both frogs need to reach the goal area!" << endl;
            }
            


}


int y = 0;
const int BufferSize = 10;
int FPS = 30;

string LevelNumber = "";

//The loop responding to events from the user

void eventloop(XInfo& xinfo) {
    XEvent event;
    KeySym key;
    char text[BufferSize];
    list<Displayable*> dList;   

    Retrangle* frog = new Retrangle(350, 200, 50, 50);
    Retrangle* frog2 = new Retrangle(450, 200, 50, 50);

    dList.push_back(frog);
    dList.push_back(frog2);
 

    // time of last window paint
    unsigned long lastRepaint = 0;

    //A vector of enemy blocks
     vector <Retrangle*> enemyList;

     //50 x 50 px blocks on the top of window

      Retrangle* topFirst = new Retrangle(0, 50, 50, 50);
      Retrangle* topSecond = new Retrangle(283, 50, 50, 50);
      Retrangle* topThird = new Retrangle(566, 50, 50, 50);
      Retrangle* topLast = new Retrangle(850, 50, 50, 50);

      //20 x 50 px blocks on the middle of window
      
      Retrangle* MiddleFirst = new Retrangle(0, 100, 20, 50);
      Retrangle* MiddleSecond = new Retrangle(212, 100, 20, 50);
      Retrangle* MiddleThird = new Retrangle(425, 100, 20, 50);
      Retrangle* MiddleForth = new Retrangle(637, 100, 20, 50);
      Retrangle* MiddleLast = new Retrangle(850, 100, 20, 50);

      //100 x 50 px blocks on the bottom of window
      
      Retrangle* BottomFirst = new Retrangle(0, 150, 100, 50);
      Retrangle* BottomSecond = new Retrangle(425, 150, 100, 50);
      Retrangle* BottomLast = new Retrangle(850, 150, 100, 50);


     enemyList.push_back(topFirst);
     enemyList.push_back(topSecond);
     enemyList.push_back(topThird);
     enemyList.push_back(topLast);

      enemyList.push_back(MiddleFirst);
      enemyList.push_back(MiddleSecond);
      enemyList.push_back(MiddleThird);
      enemyList.push_back(MiddleForth);
      enemyList.push_back(MiddleLast);
     
      enemyList.push_back(BottomFirst);
      enemyList.push_back(BottomSecond);
      enemyList.push_back(BottomLast);


     dList.push_back(topFirst);
     dList.push_back(topSecond);
     dList.push_back(topThird);
     dList.push_back(topLast);
    

      dList.push_back(MiddleFirst);
      dList.push_back(MiddleSecond);
      dList.push_back(MiddleThird);
      dList.push_back(MiddleForth);
      dList.push_back(MiddleLast);
     
      dList.push_back(BottomFirst);
     dList.push_back(BottomSecond);
     dList.push_back(BottomLast);

   
    
    while ( true ) {

        // Draw text "Level: N" at the top right of the window

        ostringstream nts;
        nts << levelNum;
        LevelNumber = nts.str();

        dList.push_back(new Text(730, 20, "Level: "+LevelNumber));

       



        if (XPending(xinfo.display) > 0) { 
            repaint(dList, xinfo);
            XNextEvent( xinfo.display, &event ); 

       
        switch ( event.type ) {
        
       
        case KeyPress:  
            int i = XLookupString( 
                (XKeyEvent*)&event, text, BufferSize, &key, 0 );
            // cout << "KeySym " << key
            //      << " text='" << text << "'"
            //      << " at " << event.xkey.time
            //      << endl;
            if ( i == 1 && text[0] == 'q' ) {   //exit when 'q' is typed
                cout << "Terminated normally." << endl;
                //XCloseDisplay(xinfo.display);
                return;
            }

             else if ( i == 1 && text[0] == 'n' ) { //go to next level when n is pressed

               
                goToNextLevel(frog, frog2);
                break; 
            }

            else if ( i == 1 && text[0] == 'w' ) { //control frog 2 to move up

               frog2->moveRetrangle2(xinfo, "w");
                break; 
            }

            else if ( i == 1 && text[0] == 's' ) { //control frog 2 to move down

               frog2->moveRetrangle2(xinfo, "s");
                break; 
            }

            else if ( i == 1 && text[0] == 'a' ) { //control frog 2 to move left

               frog2->moveRetrangle2(xinfo, "a");
                break; 
            }

            else if ( i == 1 && text[0] == 'd' ) { //control frog 2 to move right

               frog2->moveRetrangle2(xinfo, "d");
                break; 
            }

         

           
           switch(key){ //control the frog for player 1 to move by pressing arrow keys
                case XK_Up:
                   
                    frog->moveRetrangle(xinfo, "up");

                    break;

                 case XK_Down:
                   
                    frog->moveRetrangle(xinfo, "down");
                    break;    
               
                case XK_Left:
                   
                    frog->moveRetrangle(xinfo, "left");
                    break;

                case XK_Right:

                    frog->moveRetrangle(xinfo, "right");
                    break;
                }
            break;


        } 
    }

    
    //call moveEnemy to move the enemy blocks and at the moment when a block reaches one side of the window
    // a new block comes out from the other side of the window.
    topFirst->moveEnemy(xinfo, "right", levelNum, 50, 850/3);
    topSecond->moveEnemy(xinfo, "right", levelNum, 50, 850/3);
    topThird->moveEnemy(xinfo, "right", levelNum, 50, 850/3);
    topLast->moveEnemy(xinfo, "right", levelNum, 50, 850/3);
    

    MiddleFirst->moveEnemy(xinfo, "left", levelNum, 20, 850/4);
    MiddleSecond->moveEnemy(xinfo, "left", levelNum, 20, 850/4);
    MiddleThird->moveEnemy(xinfo, "left", levelNum, 20, 850/4);
    MiddleForth->moveEnemy(xinfo, "left", levelNum, 20, 850/4);
    MiddleLast->moveEnemy(xinfo, "left", levelNum, 20, 850/4);


    BottomFirst->moveEnemy(xinfo, "right", levelNum, 100, 850/2);
    BottomSecond->moveEnemy(xinfo, "right", levelNum, 100, 850/2);
    BottomLast->moveEnemy(xinfo, "right", levelNum, 100, 850/2);

    //detect collision between enemy blocks and frogs
    checkCollision(enemyList, frog, frog2);
    checkCollision(enemyList, frog2, frog);

    unsigned long end = now();


    if (end - lastRepaint > 1000000 / FPS) { //redraw window at frame rate 30FPS as a default setting
        repaint(dList, xinfo);



        XFlush(xinfo.display);
        lastRepaint = now();   

        }

        if (XPending(xinfo.display) == 0) {
            usleep(1000000 / FPS - (end - lastRepaint));
        }

    }
}






int main ( int argc, char* argv[] ) {
    if (argc > 1){
        stringstream sti(argv[1]);
        sti >> FPS;
    } //change FPS based on command-line parameter

    XInfo xinfo;
    initX(argc, argv, xinfo);

    GC gc = XCreateGC(xinfo.display, xinfo.window, 0, 0);


    XSetForeground(xinfo.display, gc, BlackPixel(xinfo.display, xinfo.screen));
    XSetBackground(xinfo.display, gc, WhitePixel(xinfo.display, xinfo.screen));
   
    xinfo.gc = gc;  

    XFlush(xinfo.display);
    eventloop(xinfo);
    XCloseDisplay(xinfo.display);
}



