//Sanket Joshi
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
int TILESIZE = 16;//global variable

//The screen attributes
const int SCREEN_WIDTH = 620;
const int SCREEN_HEIGHT = 780;
const int SCREEN_BPP = 32;

vector <vector <int> > map;//Sets up a 2D array for the map

//The frame rate
const int FRAMES_PER_SECOND = 20;

//The dimensions of the dot
const int DOT_WIDTH = 12;
const int DOT_HEIGHT = 16;
const int DOT_VEL = 64;


//The surfaces
SDL_Surface *dot = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *tiles[2];


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
    void showUnder();
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
    void unpause();

    //Gets the timer's time
    int get_ticks();

    //Checks the status of the timer
    bool is_started();
    bool is_paused();
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
    dot = load_image( "man.png" );
    tiles[0] = load_image("sky.png");
    tiles[1] = load_image("block.png");
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
    x = 0;
    y = 0;

    //Initialize the velocity
    xVel = 0;
    yVel = 0;
}
void Dot::handle_input()
{
//If a key was pressed
    if( event.type == SDL_KEYDOWN )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel = -512; break;
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
            case SDLK_LEFT: xVel += DOT_VEL*2; break;
            case SDLK_RIGHT: xVel -= DOT_VEL*2; break;
        }
    }

}

void Dot::move( Uint32 deltaTicks )
{
    //Move the dot left or right
    float CurrXVel = xVel * ( deltaTicks / 1000.f );

    //Move the dot up or down
    float CurrYVel = yVel * ( deltaTicks / 1000.f );

    //Not colliding
    if ((map[(x + CurrXVel)/TILESIZE][y/TILESIZE] == 0) && (map [(x + DOT_WIDTH + CurrXVel) / TILESIZE][y/TILESIZE] == 0) && (map [(x + DOT_WIDTH +  CurrXVel)/ TILESIZE] [(y + DOT_HEIGHT) / TILESIZE] == 0)  && (map [(x + CurrXVel)/ TILESIZE] [(y + DOT_HEIGHT) / TILESIZE] == 0))
    {
        x += CurrXVel;//allows player to keep moving if they are not colliding
    }
    if ((map[(x)/TILESIZE][(y + CurrYVel)/TILESIZE] == 0) && (map [(x + DOT_WIDTH) / TILESIZE][(y + CurrYVel)/TILESIZE] == 0) && (map [(x + DOT_WIDTH)/ TILESIZE] [(y + DOT_HEIGHT+ CurrYVel) / TILESIZE] == 0)  && (map [(x)/ TILESIZE] [(y + DOT_HEIGHT+ CurrYVel) / TILESIZE] == 0))
    {
        y += CurrYVel;
    }
    else
    {
        yVel = 0;
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
        yVel=0;
    }
}

void Dot::showUnder()
{
    int my = (y + DOT_HEIGHT)/TILESIZE;
    int mmy = y/TILESIZE;
    int mx = (x + DOT_WIDTH)/TILESIZE;
    int mmx = x/TILESIZE;
    apply_surface( mmx * TILESIZE, mmy * TILESIZE, tiles[map [mmx][mmy]], screen );
    apply_surface( mx * TILESIZE, mmy * TILESIZE, tiles[map [mx][mmy]], screen );
    apply_surface( mx * TILESIZE, my * TILESIZE, tiles[map [mx][my]], screen );
    apply_surface( mmx * TILESIZE, my * TILESIZE, tiles[map [mmx][my]], screen );
}

void Dot::show() {
    //apply_surface( x, y, tiles[map [x][y]], screen );
    //apply_surface( x, y, tiles[map [x][y]], screen );
    //apply_surface( x, y, tiles[map [x][y]], screen );
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

void Timer::stop()
{
    //Stop the timer
    started = false;

    //Unpause the timer
    paused = false;
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

void Timer::unpause()
{
    //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;

        //Reset the starting ticks
        startTicks = SDL_GetTicks() - pausedTicks;

        //Reset the paused ticks
        pausedTicks = 0;
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

bool Timer::is_started()
{
    return started;
}


bool Timer::is_paused()
{
    return paused;
}


void DrawMap (vector < vector <int> > map);

int main( int argc, char* args[] )
{
    //Declare Variables
    ifstream ifs;
    int i;
    int MaxTileX;
    int MaxTileY;
    int x;
    int y = 10;
    int xVel =0;
    int yVel = 0;
    float CurrX;
    float CurrY;

    TILESIZE = 16;


    //Opens the name of the text file
    ifs.open ("TestMap.txt",ifstream::in);
    //Dummy proof for when file is doesn't exist or not in right directory
    if (!ifs.is_open())
    {
        cout << "No File" << endl;
        return (-1);
    }

 ifs >> MaxTileY >> MaxTileX;//reads in 5,5


 map.resize(MaxTileX);//Creates the rows of the map
for(int x = 0; x < MaxTileX; x++)
    {
        map[x].resize(MaxTileY);//columns created
    }

//Reads in properties of grid
for (int y = 0; y < MaxTileY; y++)

{
    for(int x = 0; x < MaxTileX; x++)
    {
        ifs >> map[x][y];//puts the value from the file into the array, creating the properties of the map
    }
 }

    ifs.close();

    //Set up screen
    screen = SDL_SetVideoMode( 440, 480, 32, SDL_SWSURFACE );//width, hight,

    //Set the window caption
    SDL_WM_SetCaption( "Game", NULL );

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
    DrawMap(map);
    //While the user hasn't quit
    while( quit == false )
    {
        if (map [CurrX][CurrY] == 0) //If character is off the ground
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
        }
        myDot.showUnder();
        //Move the dot
        myDot.move(delta.get_ticks());

        //places map on screen


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

//FUNCTION THAT MAKES THE MAP
void DrawMap (vector < vector <int> > map)
{
    for(int x = 0; x < map.size(); x++)
    {
        for (int y = 0; y < map[x].size(); y++)
        {
            SDL_Rect rect;//variable
            Uint32 colour;

            rect.w = TILESIZE;
            rect.h = TILESIZE;
            rect.x = x * TILESIZE; //+ 45;//whatever adds to x is what you move to the right, scrolling
            rect.y = y * TILESIZE; //+ 150;//add to y is up and down scrolling

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

            //draw a tile for map at x and y
            SDL_FillRect (screen, &rect, colour);
            apply_surface( rect.x, rect.y, tiles[map [x][y]], screen );
        }
    }
}
