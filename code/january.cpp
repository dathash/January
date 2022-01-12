#include <stdint.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <windows.h>
#include <vector>

#define internal static
#define global_variable static 
#define local_persist static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

global_variable int GlobalScreenWidth = 800;
global_variable int GlobalScreenHeight = 800;
global_variable int Volume = 64;
global_variable SDL_Window *GlobalWindow;
global_variable SDL_Surface *GlobalScreenSurface;
global_variable SDL_Surface *GlobalImage;
global_variable SDL_Event GlobalEventHandler;
global_variable Mix_Music *GlobalMusic;
global_variable bool32 GlobalRunning;

/*
   TODO(alex):
   - Volume
   - Shuffle Mode
   
   Fun?
   - Sound Effects
   - FFW/FBW
   - GUI
*/

struct Album
{
    int Index;
    char *CoverPath;
    std::vector<char *> Songs;
};

struct RecordPlayer
{
    int Paused = 0;
    int AlbumIndex = 0;
    std::vector<Album *> Albums;
};

internal SDL_Surface *
LoadSurface(char *Path)
{
    SDL_Surface *LoadedSurface = SDL_LoadBMP(Path);
    if(!LoadedSurface)
    {
        printf("Unable to load image %s! SDL Error: %s\n", Path, SDL_GetError());
    }
    return LoadedSurface;
}

internal void
PlayFirstSong(RecordPlayer *RP)
{
    Mix_HaltMusic();
    RP->Albums[RP->AlbumIndex]->Index = 0;

    GlobalMusic = Mix_LoadMUS(RP->Albums[RP->AlbumIndex]->Songs[0]);

    Mix_PlayMusic(GlobalMusic, 1);
    GlobalImage = LoadSurface(RP->Albums[RP->AlbumIndex]->CoverPath);
}

internal void
PlayNextAlbum(RecordPlayer *RP)
{
    if(RP->AlbumIndex < RP->Albums.size() - 1)
    {
        RP->AlbumIndex++;
    }
    else
    {
        RP->AlbumIndex = 0;
    }
    PlayFirstSong(RP);
    GlobalImage = LoadSurface(RP->Albums[RP->AlbumIndex]->CoverPath);
}

internal void
PlayLastAlbum(RecordPlayer *RP)
{
    if(RP->AlbumIndex > 0)
    {
        RP->AlbumIndex--;
    }
    else
    {
        RP->AlbumIndex = RP->Albums.size() - 1;
    }
    PlayFirstSong(RP);
    GlobalImage = LoadSurface(RP->Albums[RP->AlbumIndex]->CoverPath);
}

internal void
PlayNextSong(RecordPlayer *RP)
{
    Mix_HaltMusic();
    Album *CurrentAlbum = RP->Albums[RP->AlbumIndex];
    if(CurrentAlbum->Index < CurrentAlbum->Songs.size() - 1)
    {
        CurrentAlbum->Index++;
        GlobalMusic = Mix_LoadMUS(CurrentAlbum->Songs[CurrentAlbum->Index]);

        Mix_PlayMusic(GlobalMusic, 1);
    }
    else
    {
        PlayNextAlbum(RP);
    }
}

internal void
PlayLastSong(RecordPlayer *RP)
{
    Mix_HaltMusic();
    Album *CurrentAlbum = RP->Albums[RP->AlbumIndex];
    if(CurrentAlbum->Index > 0)
    {
        CurrentAlbum->Index--;
    }
    else
    {
        CurrentAlbum->Index = CurrentAlbum->Songs.size() - 1;
    }

    GlobalMusic = Mix_LoadMUS(CurrentAlbum->Songs[CurrentAlbum->Index]);

    Mix_PlayMusic(GlobalMusic, 1);
}

internal void 
InitAlbums(Album *D4, Album *PL)
{
    D4->Index = 0;
    D4->CoverPath = "w:/january/data/image0.bmp";
    D4->Songs = {"w:/january/data/Frogs.wav", 
                 "w:/january/data/Cavern.wav", 
                 "w:/january/data/Audio Assault.wav", 
                 "w:/january/data/The Future.wav", 
                 "w:/january/data/CorporationCorp.wav"};

    PL->Index = 0;
    PL->CoverPath = "w:/january/data/Planet-Lleh.bmp";
    PL->Songs = {"w:/january/data/Train To Paradise.wav", 
                 "w:/january/data/Xanadu.wav", 
                 "w:/january/data/Escape.wav", 
                 "w:/january/data/New Life.wav"};

}

internal void
Init()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0)
    {
        if(Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) == 0)
        {
            GlobalWindow = SDL_CreateWindow("January", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, GlobalScreenWidth, GlobalScreenHeight, SDL_WINDOW_SHOWN);
            if(GlobalWindow)
            {
                GlobalScreenSurface = SDL_GetWindowSurface(GlobalWindow);
            }
            else
            {
                // Error: No Window.
            }
        }
        else
        {
            // Error: Sound Failed to Initialize.
        }
    }
    else
    {
        // Error: SDL Init.
    }
}

internal void
HandleEvent(RecordPlayer *RP)
{
    if(GlobalEventHandler.type == SDL_QUIT)
    {
        GlobalRunning = false;
    }
    else if(GlobalEventHandler.type == SDL_KEYDOWN)
    {
        switch(GlobalEventHandler.key.keysym.sym)
        {
            case SDLK_SPACE:
            {
                if(RP->Paused)
                {
                    Mix_ResumeMusic();
                    RP->Paused = false;
                }
                else
                {
                    Mix_PauseMusic();
                    RP->Paused = true;
                }
            } break;

            case SDLK_RIGHT:
            {
                PlayNextSong(RP);
            } break;

            case SDLK_LEFT:
            {
                PlayLastSong(RP);
            } break;

            case SDLK_n:
            {
                PlayNextAlbum(RP);
            } break;

            case SDLK_b:
            {
                PlayLastAlbum(RP);
            } break;

            case SDLK_DOWN:
            {
                Volume = Mix_VolumeMusic(Volume - 16);
            } break;

            case SDLK_UP:
            {
                Volume = Mix_VolumeMusic(Volume + 16);
            } break;

            default:
            {
            } break;
        }
    }
}

int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,
            int nShowCmd)
{
    Init();
    RecordPlayer RP;
    Album D4;
    Album PL;
    RP.Albums.push_back(&D4);
    RP.Albums.push_back(&PL);

    InitAlbums(&D4, &PL); 

    PlayFirstSong(&RP);

    Volume = Mix_Volume(-1, Volume);

    GlobalRunning = true;
    while(GlobalRunning)
    {
        while(SDL_PollEvent(&GlobalEventHandler))
        {
            HandleEvent(&RP);
        }

        SDL_Rect StretchRect;
        StretchRect.x = 0;
        StretchRect.y = 0;
        StretchRect.w = GlobalScreenWidth;
        StretchRect.h = GlobalScreenHeight;


        SDL_BlitScaled(GlobalImage, 0, GlobalScreenSurface, &StretchRect);

        SDL_UpdateWindowSurface(GlobalWindow);

        

        if(!Mix_PlayingMusic())
        {
            PlayNextSong(&RP);
        }
    }
    return 0;
}
