
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
int TILESIZE;//global variable

//The screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The frame rate
const int FRAMES_PER_SECOND = 20;

//The dimensions of the dot
const int DOT_WIDTH = 41;
const int DOT_HEIGHT = 47;

//The surfaces
SDL_Surface *dot = NULL;
SDL_Surface *screen = NULL;


//The event structure
SDL_Event event;

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
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "Move the Dot", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the dot image
    dot = load_image( "man.png" );

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

void DrawMap (vector < vector <int> > map);

int main( int argc, char* args[] )
{
    //Declare Variables
    ifstream ifs;
    ofstream fout;
    int i;
    vector <vector <int> > map;//Sets up a 2D array for the map
    int MaxTileX;
    int MaxTileY;

    int x;
    int y = 10;
    int xVel =0;
    int yVel = 0;

    TILESIZE = 16;

    //Opens the name of the text file
    ifs.open ("Test.map",ifstream::in);
    fout.open("err.log");
    //Dummy proof for when file is doesn't exist or not in right directory
    if (!ifs.is_open())
    {
        cout << "No File" << endl;
        return (-1);
    }

 ifs >> MaxTileY >> MaxTileX;//reads in 5,5

//Creates the rows of the map
 map.resize(MaxTileX);
for(int x = 0; x < MaxTileX; x++)
    {
        map[x].resize(MaxTileY);
    }

//Reads in properties of grid
for (int y = 0; y < MaxTileY; y++)

{
    //Creates the coloumns
    for(int x = 0; x < MaxTileX; x++)
    {
        ifs >> map[x][y];//puts the value from the file into the array, creating the properties of the map
    }
 }


    ifs.close();
for (int y = 0; y < MaxTileY; y++)

{
    //Creates the coloumns
    for(int x = 0; x < MaxTileX; x++)
    {
        fout << map[x][y] << " ";//puts the value from the file into the array, creating the properties of the map
    }
    fout << endl;
 }
 fout.close();
    //Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );
    freopen( "CON", "w", stdout );
    freopen( "CON", "w", stderr );

    //Set up screen
    screen = SDL_SetVideoMode( 440, 480, 32, SDL_SWSURFACE );//width, hight,

    //Set the window caption
    SDL_WM_SetCaption( "Game", NULL );

    //Update Screen
    SDL_Flip( screen );


    //Quit flag
    bool quit = false;

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

    //While the user hasn't quit
    while( quit == false )
    {
        //While there's events to handle
        while( SDL_PollEvent( &event ) )
        {

            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }

        //places map on screen
        DrawMap(map);

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
        }
    }
}
