#include "dcircus.h"


int setup_joystick(void)
{
   SDL_JoystickEventState(SDL_ENABLE);
   js = SDL_JoystickOpen(0);
   return 0;
}

int setup_SDL(void)
{
  // Init SDL
#ifndef SOUND
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);
#else
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK|SDL_INIT_AUDIO);
  
	if (Mix_OpenAudio(22050, AUDIO_S16, 2, SOUND_BUFFER) < 0)
        {
#ifdef DEBUG
		printf("\nWarning: I could not set up audio for 44100 Hz "
			"16-bit stereo.\n"
                  	"The Simple DirectMedia error that occured was:\n"
                  	"%s\n\n", SDL_GetError());
#endif
		return 1;
        }
#ifdef DREAMCAST
	vmu_set_icon(vmu_xpm);
#endif
#endif
  
  	screen = SDL_SetVideoMode(640/DIVISOR, 480/DIVISOR, 0, 0);
        return 0;
}

static inline void loading(int i)
{
   int col;
   SDL_Rect dest;

      SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

      // Draw percentage bar
      
      dest.x = 0;
      dest.y = 450 /DIVISOR;
      dest.w = ((640 / DIVISOR) * i) / (NUM_IMAGES+NUM_SOUNDS+25);
      dest.h = (30 /DIVISOR);
      
      col = 127+((128 * i) / (NUM_IMAGES+NUM_SOUNDS+25));
      
      SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, col, col, 255));

      if (i>0)
	     draw(280,200,0);
      if (i>1)
	     drawtext(200,340,"LOADING");

      SDL_Flip(screen);

      SDL_Delay(1);
}

int setup_load_graphics()
{
  // Load graphics
  
   int i;
   SDL_Surface * image;

  for (i = 0; i < NUM_IMAGES; i++)
    {
      // Load image file
//#ifdef GP32
      image = IMG_Load_RW(get_data(image_names[i]),0);
//#else
//      image = IMG_Load(image_names[i]);
//#endif


      if (image == NULL)
        {
#ifdef DEBUG
          fprintf(stderr,
                  "\nError: I couldn't load a graphics file:\n"
                  "%s\n"
                  "The Simple DirectMedia error that occured was:\n"
                  "%s\n\n", image_names[i], SDL_GetError());
#endif
          return 1;
        }
      
      // Set transparency
      
      if ( (i!=IMG_TITLE)&&(i!=IMG_TITLE_HIGHLIGHTS)&&(i!=IMG_BACKGROUND_0)&&
           (i!=IMG_BACKGROUND_1)&&(i!=IMG_HIGHSCORE_TOP)&&(i!=IMG_HIGHSCORE_LEFT) &&
	   (i!=IMG_SADCLOWN_0)&&(i!=IMG_SADCLOWN_1)&&(i!=IMG_SADCLOWN_2)&&
	   (i!=IMG_ENTER_INITIALS)&&(i!=IMG_ICON)&&(i!=IMG_BICON)
         )
      {
#ifdef GP32
         if (SDL_SetColorKey(image, (SDL_SRCCOLORKEY | SDL_RLEACCEL) ,SDL_MapRGB(image -> format,0xFF, 0xFF, 0xFF)) == -1)
#else
         if (SDL_SetColorKey(image, SDL_SRCCOLORKEY ,SDL_MapRGB(image -> format,0xFF, 0xFF, 0xFF)) == -1)
#endif
         {
#ifdef DEBUG
            fprintf(stderr,
                  "\nError: I could not set the color key for the file:\n"
                  "%s\n"
                  "The Simple DirectMedia error that occured was:\n"
                  "%s\n\n", image_names[i], SDL_GetError());
#endif
            return 2;
         }
         images[i] = SDL_DisplayFormat(image);
         if (images[i] == NULL)
         {
#ifdef DEBUG
            fprintf(stderr,
                  "\nError: I couldn't convert a file to the display format:\n"
                  "%s\n"
                  "The Simple DirectMedia error that occured was:\n"
                  "%s\n\n", image_names[i], SDL_GetError());
#endif
            return 3;
        }
        SDL_FreeSurface(image);
      }
      else
      {
         images[i]=image;
      }

      loading(i);
      
    }
    return 0;
}

#ifdef SOUND
int setup_load_sounds(void)
{
  // Load sounds:
   int i;
      
      for (i = 0; i < NUM_SOUNDS; i++)
        {
//#ifdef GP32
          sounds[i] = Mix_LoadWAV_RW(get_data(sound_names[i]),0);
//#else
//          sounds[i] = Mix_LoadWAV(sound_names[i]);
//#endif
          if (sounds[i] == NULL)
            {
#ifdef DEBUG
              fprintf(stderr,
                      "\nError: I could not load the sound file:\n"
                      "%s\n"
                      "The Simple DirectMedia error that occured was:\n"
                      "%s\n\n", sound_names[i], SDL_GetError());
#endif
              return 4;
            }
	  loading(NUM_IMAGES+i);
        }
      return 0;
}

int setup_load_musics(void)
{      
      // Load musics
      
      // (title)
      mus_title = Mix_LoadMUS(MUS_TITLE);
      if (mus_title == NULL)
	{
#ifdef DEBUG
	  fprintf(stderr,
		  "\nError: I could not load the music file:\n"
		  "%s\n"
		  "The Simple DirectMedia error that occured was:\n"
		  "%s\n\n", MUS_TITLE, SDL_GetError());
#endif
	  return 5;
	}

      loading(NUM_IMAGES+NUM_SOUNDS+5);

      // (game)
      
      mus_game = Mix_LoadMUS(MUS_GAME);
      if (mus_game == NULL)
	{
#ifdef DEBUG
	  fprintf(stderr,
		  "\nError: I could not load the music file:\n"
		  "%s\n"
		  "The Simple DirectMedia error that occured was:\n"
		  "%s\n\n", MUS_GAME, SDL_GetError());
#endif
	  return 6;
	}

      loading(NUM_IMAGES+NUM_SOUNDS+10);

      // (gameover)
      
      mus_gameover = Mix_LoadMUS(MUS_GAMEOVER);
      if (mus_gameover == NULL)
	{
#ifdef DEBUG
	  fprintf(stderr,
		  "\nError: I could not load the music file:\n"
		  "%s\n"
		  "The Simple DirectMedia error that occured was:\n"
		  "%s\n\n", MUS_GAMEOVER, SDL_GetError());
#endif
	  return 7;
	}

      loading(NUM_IMAGES+NUM_SOUNDS+15);

      // (hiscore)

      mus_hiscore = Mix_LoadMUS(MUS_HISCORE);
      if (mus_hiscore == NULL)
	{
#ifdef DEBUG
	  fprintf(stderr,
		  "\nError: I could not load the music file:\n"
		  "%s\n"
		  "The Simple DirectMedia error that occured was:\n"
		  "%s\n\n", MUS_HISCORE, SDL_GetError());
#endif
	  return 8;
	}

      loading(NUM_IMAGES+NUM_SOUNDS+20);

      // (hiscore screen)

      mus_hiscreen = Mix_LoadMUS(MUS_HISCORESCREEN);
      if (mus_hiscreen == NULL)
	{
#ifdef DEBUG
	  fprintf(stderr,
		  "\nError: I could not load the music file:\n"
		  "%s\n"
		  "The Simple DirectMedia error that occured was:\n"
		  "%s\n\n", MUS_HISCORESCREEN, SDL_GetError());
#endif
	  return 9;
	}

      loading(NUM_IMAGES+NUM_SOUNDS+25);

      return 0;
}
#endif

int setup(void)
{
   	int ret;
        
        ret=setup_SDL();
        if (!ret)
        {
           ret=setup_joystick();
           if (!ret)
           {
              ret=setup_load_graphics();
              if (!ret)
              {
		 get_rand_index=(SDL_GetTicks()%GET_RAND_MAX);
#ifdef SOUND        
                 ret=setup_load_sounds();
#ifdef MUSIC
                 if (!ret)
                 {
                    ret=setup_load_musics();
		    SDL_Delay(123);
                 }
#endif
#endif
              }
           }
        }
	return ret;
}

