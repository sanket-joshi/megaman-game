//Sanket Joshi
//(Work Done By us Will be Labelled with Multiple Astericks [*******] the rest is taken from Different Lessons on Lazyfoo.net)
//The headers
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>

using namespace std;
void Player (int x, int y);
void DrawMap (vector < vector <int> > map);
int TILESIZE = 30;//global variable

//The screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

vector <vector <int> > map;//Sets up a 2D array for the map

//The frame rate
const int FRAMES_PER_SECOND = 20;

//The dimensions of the dot
const int DOT_WIDTH = 20;
const int DOT_HEIGHT = 23;
const int DOT_VEL = 200;
bool grounded = false;//used for preventing multiple jumps


//The surfaces
SDL_Surface *dot = NULL;
SDL_Surface *screen = NULL;


//The event structure
SDL_Event event;

//The dot that will move around on the screen
class Dot
{
    private:

    public:
    //Initializes the variables
    Dot();

    //Takes key presses and adjusts the dot's velocity
    void handle_input();

    //Moves the dot
    void move(Uint32 deltaTicks);

    //The X and Y offsets of the dot
    float x, y;

    //The velocity of the dot
    int xVel, yVel;

    //Shows the dot on the screen
    void show();
};

//The timer
class Timer
{
    private:
    //The clock time when the timer started
    int startTicks;

    //The ticks stored when the timer was paused
    int pausedTicks;

    //The timer status
    bool paused;
    bool started;

    public:
    //Initializes variables
    Timer();

    //The various clock actions
    void start();
    void stop();
    void pause();


    //Gets the timer's time
    int get_ticks();
};

SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Map the color key
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 255, 255, 255 );//colcour keys the transparancy to be white (255.255,255)

            //Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_FULLSCREEN );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption("Game", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the dot image
    dot = load_image( "guy.png" );

    //If there was a problem in loading the dot
    if( dot == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surface
    SDL_FreeSurface( dot );

    //Quit SDL
    SDL_Quit();
}

Dot::Dot()
{
    //Initialize the offsets
    x = 30;
    y = 400;

    //Initialize the velocity
    xVel = 0;
    yVel = 0;
}
/******* Movement / Character Physics *******/
void Dot::handle_input()
{
    Dot myDot;
    float CurrX = x/TILESIZE;
    float CurrY = y/TILESIZE;
//If a key was pressed
    if( event.type == SDL_KEYDOWN )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            //Cases for keys that are pressed to move the character
            //If the up key was pressed, then adjust the vertical velocity, unless the area 1 pixel below his current position is a 0, which is sky.
            case SDLK_UP:if (grounded) {yVel = -500; grounded = false; }break;//grounded prevents the character from multiple jumps
            case SDLK_LEFT: xVel -= DOT_VEL*2; break;
            case SDLK_RIGHT: xVel += DOT_VEL*2; break;
        }
    }

    //If a key was released
    else if( event.type == SDL_KEYUP )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            //Stop the sideways movement by changing the velocity back to zero when a key is realeased.
            case SDLK_LEFT: xVel += DOT_VEL*2; break;
            case SDLK_RIGHT: xVel -= DOT_VEL*2; break;
        }
    }

}

void Dot::move( Uint32 deltaTicks )
{

    /***************************** COLLISIONS DETECTION ************************************************/
    Dot myDot;
    float CurrX = x / TILESIZE;//used to put character into position in grid of our map
    float CurrY = y / TILESIZE;// /TILESIZE is used to take where character is in pixels (x,y) and convert it into where he is in grid (row,column)

    //Move the dot left or right
    float CurrXVel = xVel * ( deltaTicks / 1000.f );//Current x velocity of the character. Deltaticks is used for framrate

    //Move the dot up or down
    float CurrYVel = yVel * ( deltaTicks / 1000.f );//Current Y velocity

    /* What we do for collision is to check if the Character is NOT colliding and if he is colliding, we check
    check if the character can move up, down, left or right, if he can move in one of those directions, then we allow the character to move by his current velocity. */

    //Check to see id character can move in X Direction
    if ((map[(x + CurrXVel)/TILESIZE][y/TILESIZE] == 0) &&//Top Left Corner
        (map [(x + DOT_WIDTH + CurrXVel) / TILESIZE][y/TILESIZE] == 0) && //Bottom Right Corner
        (map [(x + DOT_WIDTH +  CurrXVel)/ TILESIZE] [(y + DOT_HEIGHT) / TILESIZE] == 0 )  &&//Top Right Corner
        (map [(x + CurrXVel)/ TILESIZE] [(y + DOT_HEIGHT) / TILESIZE] == 0))//Bottom Left Corner
    {
        x += CurrXVel;//allows player to keep moving if they are not colliding. The player's x position will increase by the x velocity
    }

    //Check to see if Character can move in Y Direction
    if ((map[x/TILESIZE][(y + CurrYVel)/TILESIZE] == 0) &&//Top Left Corner
        (map [(x + DOT_WIDTH) / TILESIZE][(y + CurrYVel)/TILESIZE] == 0) &&//Bottom Left Corner
        (map [(x + DOT_WIDTH)/ TILESIZE] [(y + DOT_HEIGHT + CurrYVel) / TILESIZE] == 0 )  && //Top Right Corner
        (map [x / TILESIZE] [(y + DOT_HEIGHT + CurrYVel) / TILESIZE] == 0))//Bottom Left Corner
    {
        y += CurrYVel;
        //grounded=false;
    }
    else
    {
        if (yVel>0) grounded = true;
        yVel = 0;//when he is colliding set his y velocity to zero
    }

    //If the dot went too far to the left
    if( x < 0 )
    {
        //Move back
        x = SCREEN_WIDTH-DOT_WIDTH;
    }
    //or the right
    else if( x + DOT_WIDTH > SCREEN_WIDTH )
    {
        //Move back
        x = 0;
    }

    //If the dot went too far up
    if( y < 0 )
    {
        //Move back
        y = 0;
    }

    //or down
    else if( y + DOT_HEIGHT > SCREEN_HEIGHT )
    {
        //Move back
        y = SCREEN_HEIGHT-DOT_HEIGHT;
        //yVel=0;
    }

    if ( y == SCREEN_HEIGHT-DOT_HEIGHT )
    {
        y=5;
        x=SCREEN_WIDTH-DOT_WIDTH-145;
    }

    if (map [CurrX][CurrY+1] == 2) //If character is on a red tile
        {
            x=480;//teleport
            y=426;
        }

    if (map [CurrX][CurrY+1] == 3) //Spring board
    {
        yVel-=100; //Launch character
    }

    if (map [CurrX] [CurrY+1] == 4)//Purple Teleporter
    {
        x=300;
        y=126;
    }

    if (map [CurrX] [CurrY+1] == 5)//Finishing Block
    {

    }
}

void Dot::show()
{
    //Show the dot
    apply_surface( x, y, dot, screen );
}

Timer::Timer()
{
    //Initialize the variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    //Start the timer
    started = true;

    //Unpause the timer
    paused = false;

    //Get the current clock time
    startTicks = SDL_GetTicks();
}


void Timer::pause()
{
    //If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        //Pause the timer
        paused = true;

        //Calculate the paused ticks
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}


int Timer::get_ticks()
{
    //If the timer is running
    if( started == true )
    {
        //If the timer is paused
        if( paused == true )
        {
            //Return the number of ticks when the timer was paused
            return pausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
        }
    }

    //If the timer isn't running
    return 0;
}

void DrawMap (vector < vector <int> > map);

int main( int argc, char* args[] )
{
    //Declare Variables
    ifstream ifs;
    int MaxTileX;
    int MaxTileY;
    int x;
    int y = 10;
    int xVel =0;
    int yVel = 0;
    float CurrX;
    float CurrY;

    //TILESIZE = 25;

    //Opens the name of the text file
    ifs.open ("map.map",ifstream::in);
    //Dummy proof for when file is doesn't exist or not in right directory
    if (!ifs.is_open())
    {
        cout << "No File" << endl;
        return (-1);
    }


/**************** MAP LOADING ***************************/

 ifs >> MaxTileY >> MaxTileX;//reads in the x and y of the map from the text file. The text file has two initial numbers which represent the intial properties of the first vector, which is the rows and columns of the map, which is what max tile x and y are
 map.resize(MaxTileX);//Creates the rows of the map. It does this by reading the first number in the text file and resizing the size of the vector to that number
for(int x = 0; x < MaxTileX; x++)
    {
        map[x].resize(MaxTileY);//Creates the columns by reading the second number in the text file and rezies that segment of the vector to that number, to create the properties of the columns for that vector
    }

//Reads in properties of grid
for (int y = 0; y < MaxTileY; y++)//This first loop will create the columns of the map, which is inside the first vector that has the intital properties of how big the columns and rows must be. In the text file is a map created out of 0,1,2,3,4,5

{
    for(int x = 0; x < MaxTileX; x++)//This second loop will read in the properties
    {
        ifs >> map[x][y];//puts the value from the file into the array, creating the properties of the map. This is similar to a push back and a cin because the properties from ifs (txt file) are put into the vector map to create the grid for the map
    }
 }

    ifs.close();//Closes text file

    //Set up screen
    screen = SDL_SetVideoMode( 440, 480, 32, SDL_FULLSCREEN );//width, hight,

    //Update Screen
    SDL_Flip( screen );

    //Quit flag
    bool quit = false;

    //The dot that will be used
    Dot myDot;

    //Keeps track of time since last rendering
    Timer delta;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

    CurrX = myDot.x/TILESIZE;
    CurrY = myDot.y/TILESIZE;

    //freopen( "CON", "w", stdout );
    //freopen( "CON", "w", stderr );

    //While the user hasn't quit
    while( quit == false )
    {
        if (map [CurrX][CurrY+1] == 0) //If character is off the ground
        {
            myDot.yVel +=2;//Apply Gravity
        }

        //While there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            //Handle events for the dot
            myDot.handle_input();

            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                quit = true;
            }
        }

        //Move the dot
        myDot.move(delta.get_ticks());

        //places map on screen
        DrawMap(map);

        //Show the dot on the screen
        myDot.show();

        //Restart delta timer
        delta.start();

        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }

    }

    //Clean up
    clean_up();

    return 0;
}
/************************** DRAWING THE MAP *************************************/
//FUNCTION THAT MAKES THE MAP
void DrawMap (vector < vector <int> > map)
{
    for(int x = 0; x < map.size(); x++)//This first loop will be used to draw the rows of the map where the loop can't excede the actual size of the map
    {
        for (int y = 0; y < map[x].size(); y++)//This second loop is used to draw the columns with the columns having to be less then the size of the map in terms of the rows of the maps size
        {
            SDL_Rect rect;//variable
            Uint32 colour;

            //This section of code is to create the actual blocks that will be displayed in the game. TILESIZE is a global variable that sets every dimension of the block to be the same, creating a square block
            rect.w = TILESIZE;
            rect.h = TILESIZE;
            rect.x = x * TILESIZE;
            rect.y = y * TILESIZE;

            //These are where colours for the blocks are created
            if (map [x][y] == 0)
                {
                    //Blue
                    colour = SDL_MapRGB (screen->format , 0, 255, 255);
                }

                else  if (map [x][y] == 1)
                {
                    //Green
                    colour = SDL_MapRGB (screen->format , 0, 255, 0);
                }

                else if (map [x] [y] == 2)
                {
                    //Red (teleport)
                    colour = SDL_MapRGB (screen->format , 255, 0, 0);
                }

                else if (map [x] [y] == 3)
                {
                    //Orange (Spring box)
                    colour = SDL_MapRGB (screen->format , 255, 128, 0);

                }

                else if (map [x] [y] == 4)
                {
                    //Purple (Teleporter)
                    colour = SDL_MapRGB (screen->format , 147, 56, 186);

                }

                else if (map [x] [y] == 5)
                {
                    //Golden (Finish)
                    colour = SDL_MapRGB (screen->format , 255, 255, 0);

                }

            //draw a tile for map at x and y, fills in each rectangle with the respective dimensions and colours
            SDL_FillRect (screen, &rect, colour);
        }
    }
}
