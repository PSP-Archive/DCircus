
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>

#define SOUND 1
#define DEBUG 1
#define JOY_YES 1
#define MUSIC 1


#ifdef SOUND
#include <SDL/SDL_mixer.h>
#endif
#include <SDL/SDL_image.h>

#define DATA_PREFIX "dcircus/"

#ifndef DATA_PREFIX
#error No DATA_PREFIX defined!
#endif

#define DIVISOR 2

/* Constraints: */

#define GET_RAND_MAX 16384
#define NUM_TITLE_BALLOONS 16
#define STARTING_LIVES 5
#define MAX_RECTS 512
#define FLYING_START_Y 192
#define FLYING_START_YM -8
#define BOUNCER_TIME 6
#define GRAVITY 1
#define FLYING_SPLAT_TIME 50
#define NUM_ROWS 3
#define NUM_BARRIERS 3
#define FASTEST_YM_OFF_BALLOON 8
#define MAX_YM 32
#define LIMB_ANIMATION_TIME 8
#define FPS (1000 / 16)
#define SHOW_PLAYER_TIME 100
#define FPS2 (FPS / 2)
#define SOUND_BUFFER 512


/* Definitions: */

#define LEFT  0
#define RIGHT 1

#define GONE 0
#define NORMAL 1
#define POPPING0 2
#define POPPING1 2


/* Image enumerations: */

enum {
  IMG_ICON,
  IMG_LETTERS,
  IMG_BICON,
  IMG_TITLE,
  IMG_TITLE_HIGHLIGHTS,
  IMG_LIGHT_ON,
  IMG_LIGHT_OFF,
  IMG_PROGRAMMER,
  IMG_GRAPHICS,
  IMG_MUSIC,
  
  IMG_BACKGROUND_0,
  IMG_BACKGROUND_1,
  
  IMG_BALLOON_RED_LEFT_0,
  IMG_BALLOON_RED_LEFT_1,
  IMG_BALLOON_RED_RIGHT_0,
  IMG_BALLOON_RED_RIGHT_1,
  IMG_BALLOON_RED_DIE_0,
  IMG_BALLOON_RED_DIE_1,

  IMG_BALLOON_ORANGE_LEFT_0,
  IMG_BALLOON_ORANGE_LEFT_1,
  IMG_BALLOON_ORANGE_RIGHT_0,
  IMG_BALLOON_ORANGE_RIGHT_1,
  IMG_BALLOON_ORANGE_DIE_0,
  IMG_BALLOON_ORANGE_DIE_1,

  IMG_BALLOON_YELLOW_LEFT_0,
  IMG_BALLOON_YELLOW_LEFT_1,
  IMG_BALLOON_YELLOW_RIGHT_0,
  IMG_BALLOON_YELLOW_RIGHT_1,
  IMG_BALLOON_YELLOW_DIE_0,
  IMG_BALLOON_YELLOW_DIE_1,

  IMG_BALLOON_GREEN_LEFT_0,
  IMG_BALLOON_GREEN_LEFT_1,
  IMG_BALLOON_GREEN_RIGHT_0,
  IMG_BALLOON_GREEN_RIGHT_1,
  IMG_BALLOON_GREEN_DIE_0,
  IMG_BALLOON_GREEN_DIE_1,

  IMG_BALLOON_CYAN_LEFT_0,
  IMG_BALLOON_CYAN_LEFT_1,
  IMG_BALLOON_CYAN_RIGHT_0,
  IMG_BALLOON_CYAN_RIGHT_1,
  IMG_BALLOON_CYAN_DIE_0,
  IMG_BALLOON_CYAN_DIE_1,

  IMG_BALLOON_BLUE_LEFT_0,
  IMG_BALLOON_BLUE_LEFT_1,
  IMG_BALLOON_BLUE_RIGHT_0,
  IMG_BALLOON_BLUE_RIGHT_1,
  IMG_BALLOON_BLUE_DIE_0,
  IMG_BALLOON_BLUE_DIE_1,

  IMG_BALLOON_PURPLE_LEFT_0,
  IMG_BALLOON_PURPLE_LEFT_1,
  IMG_BALLOON_PURPLE_RIGHT_0,
  IMG_BALLOON_PURPLE_RIGHT_1,
  IMG_BALLOON_PURPLE_DIE_0,
  IMG_BALLOON_PURPLE_DIE_1,

  IMG_BALLOON_WHITE_LEFT_0,
  IMG_BALLOON_WHITE_LEFT_1,
  IMG_BALLOON_WHITE_RIGHT_0,
  IMG_BALLOON_WHITE_RIGHT_1,
  IMG_BALLOON_WHITE_DIE_0,
  IMG_BALLOON_WHITE_DIE_1,

  IMG_CLOWN_BODY_LEFT,
  IMG_CLOWN_BODY_RIGHT,
  
  IMG_CLOWN_BODY_UPSIDE_DOWN,
  
  IMG_CLOWN_LEFT_ARM_0,
  IMG_CLOWN_LEFT_ARM_1,
  IMG_CLOWN_LEFT_ARM_2,

  IMG_CLOWN_RIGHT_ARM_0,
  IMG_CLOWN_RIGHT_ARM_1,
  IMG_CLOWN_RIGHT_ARM_2,

  IMG_CLOWN_LEFT_LEG_0,
  IMG_CLOWN_LEFT_LEG_1,
  
  IMG_CLOWN_LEFT_LEG_0_UPSIDE_DOWN,
  IMG_CLOWN_LEFT_LEG_1_UPSIDE_DOWN,

  IMG_CLOWN_RIGHT_LEG_0,
  IMG_CLOWN_RIGHT_LEG_1,
  
  IMG_CLOWN_RIGHT_LEG_0_UPSIDE_DOWN,
  IMG_CLOWN_RIGHT_LEG_1_UPSIDE_DOWN,
  
  IMG_TEETER_TOTTER_LEFT_0,
  IMG_TEETER_TOTTER_LEFT_1,
  IMG_TEETER_TOTTER_LEFT_2,
  IMG_TEETER_TOTTER_LEFT_3,

  IMG_TEETER_TOTTER_RIGHT_0,
  IMG_TEETER_TOTTER_RIGHT_1,
  IMG_TEETER_TOTTER_RIGHT_2,
  IMG_TEETER_TOTTER_RIGHT_3,
  
  IMG_BOUNCER_0,
  IMG_BOUNCER_1,
  IMG_BARRIER,
  
  IMG_TIMES,
  IMG_NUMBERS_0,
  IMG_NUMBERS_1,
  IMG_FUZZ,
  IMG_CLOWN_HEAD,
  IMG_CLOWN_HEAD_OH,
  IMG_SADCLOWN_0,
  IMG_SADCLOWN_1,
  IMG_SADCLOWN_2,
  IMG_ENTER_INITIALS,
  
  IMG_HIGHSCORE_TOP,
  IMG_HIGHSCORE_LEFT,
  
  IMG_SEAL_0,
  IMG_SEAL_1,
  
  IMG_BEACHBALL_0,
  IMG_BEACHBALL_1,
  IMG_BEACHBALL_2,
  
  IMG_BEAR_RIGHT_0,
  IMG_BEAR_RIGHT_1,
  
  IMG_BEAR_LEFT_0,
  IMG_BEAR_LEFT_1,
  
  NUM_IMAGES
};

/* Highscore filename */
#ifdef DREAMCAST
#define SCORE_FILE "/vmu/a1/dcircus.dat"
#else
#define SCORE_FILE DATA_PREFIX "dcircus.dat"
#endif

#ifndef FILE_SEPARATOR
#define FILE_SEPARATOR "/"
#endif

/* Image fileames: */
#define DATA_IMAGES DATA_PREFIX "images" FILE_SEPARATOR
#define DATA_IMAGE_TITLE DATA_IMAGES "title" FILE_SEPARATOR
#define DATA_IMAGE_BACKGRN DATA_IMAGES "backgrn" FILE_SEPARATOR
#define DATA_IMAGE_BALL DATA_IMAGES "ball" FILE_SEPARATOR
#define DATA_IMAGE_CLOWN DATA_IMAGES "clown" FILE_SEPARATOR
#define DATA_IMAGE_STATUS DATA_IMAGES "status" FILE_SEPARATOR
#define DATA_IMAGE_TEET DATA_IMAGES "teet" FILE_SEPARATOR
#define DATA_IMAGE_BOUNC DATA_IMAGES "bounc" FILE_SEPARATOR
#define DATA_IMAGE_HIGH DATA_IMAGES "high" FILE_SEPARATOR
#define DATA_IMAGE_ACTS DATA_IMAGES "acts" FILE_SEPARATOR

static const char * image_names[NUM_IMAGES] = {
   DATA_IMAGES "icon.pcx",
   DATA_IMAGE_STATUS "letters.pcx",
   DATA_IMAGES "bicon.pcx",
   DATA_IMAGE_TITLE "title.pcx",
   DATA_IMAGE_TITLE "titleh.pcx",
   DATA_IMAGE_TITLE "lighton.pcx",
   DATA_IMAGE_TITLE "lightoff.pcx",
   DATA_IMAGE_TITLE "program.pcx",
   DATA_IMAGE_TITLE "graphics.pcx",
   DATA_IMAGE_TITLE "music.pcx",
  
   DATA_IMAGE_BACKGRN "backgrn0.pcx",
   DATA_IMAGE_BACKGRN "backgrn1.pcx",
  
   DATA_IMAGE_BALL "rleft0.pcx",
   DATA_IMAGE_BALL "rleft1.pcx",
   DATA_IMAGE_BALL "rright0.pcx",
   DATA_IMAGE_BALL "rright1.pcx",
   DATA_IMAGE_BALL "rdie0.pcx",
   DATA_IMAGE_BALL "rdie1.pcx",

   DATA_IMAGE_BALL "oleft0.pcx",
   DATA_IMAGE_BALL "oleft1.pcx",
   DATA_IMAGE_BALL "oright0.pcx",
   DATA_IMAGE_BALL "oright1.pcx",
   DATA_IMAGE_BALL "odie0.pcx",
   DATA_IMAGE_BALL "odie1.pcx",

   DATA_IMAGE_BALL "yleft0.pcx",
   DATA_IMAGE_BALL "yleft1.pcx",
   DATA_IMAGE_BALL "yright0.pcx",
   DATA_IMAGE_BALL "yright1.pcx",
   DATA_IMAGE_BALL "ydie0.pcx",
   DATA_IMAGE_BALL "ydie1.pcx",

   DATA_IMAGE_BALL "gleft0.pcx",
   DATA_IMAGE_BALL "gleft1.pcx",
   DATA_IMAGE_BALL "gright0.pcx",
   DATA_IMAGE_BALL "gright1.pcx",
   DATA_IMAGE_BALL "gdie0.pcx",
   DATA_IMAGE_BALL "gdie1.pcx",

   DATA_IMAGE_BALL "cleft0.pcx",
   DATA_IMAGE_BALL "cleft1.pcx",
   DATA_IMAGE_BALL "cright0.pcx",
   DATA_IMAGE_BALL "cright1.pcx",
   DATA_IMAGE_BALL "cdie0.pcx",
   DATA_IMAGE_BALL "cdie1.pcx",

   DATA_IMAGE_BALL "bleft0.pcx",
   DATA_IMAGE_BALL "bleft1.pcx",
   DATA_IMAGE_BALL "bright0.pcx",
   DATA_IMAGE_BALL "bright1.pcx",
   DATA_IMAGE_BALL "bdie0.pcx",
   DATA_IMAGE_BALL "bdie1.pcx",

   DATA_IMAGE_BALL "pleft0.pcx",
   DATA_IMAGE_BALL "pleft1.pcx",
   DATA_IMAGE_BALL "pright0.pcx",
   DATA_IMAGE_BALL "pright1.pcx",
   DATA_IMAGE_BALL "pdie0.pcx",
   DATA_IMAGE_BALL "pdie1.pcx",

   DATA_IMAGE_BALL "wleft0.pcx",
   DATA_IMAGE_BALL "wleft1.pcx",
   DATA_IMAGE_BALL "wright0.pcx",
   DATA_IMAGE_BALL "wright1.pcx",
   DATA_IMAGE_BALL "wdie0.pcx",
   DATA_IMAGE_BALL "wdie1.pcx",
  
   DATA_IMAGE_CLOWN "bleft.pcx",
   DATA_IMAGE_CLOWN "bright.pcx",

   DATA_IMAGE_CLOWN "budown.pcx",
  
   DATA_IMAGE_CLOWN "larm0.pcx",
   DATA_IMAGE_CLOWN "larm1.pcx",
   DATA_IMAGE_CLOWN "larm2.pcx",
  
   DATA_IMAGE_CLOWN "rarm0.pcx",
   DATA_IMAGE_CLOWN "rarm1.pcx",
   DATA_IMAGE_CLOWN "rarm2.pcx",
  
   DATA_IMAGE_CLOWN "lleg0.pcx",
   DATA_IMAGE_CLOWN "lleg1.pcx",
  
   DATA_IMAGE_CLOWN "lleg0ud.pcx",
   DATA_IMAGE_CLOWN "lleg1ud.pcx",
  
   DATA_IMAGE_CLOWN "rleg0.pcx",
   DATA_IMAGE_CLOWN "rleg1.pcx",

   DATA_IMAGE_CLOWN "rleg0ud.pcx",
   DATA_IMAGE_CLOWN "rleg1ud.pcx",
  
   DATA_IMAGE_TEET "left0.pcx",
   DATA_IMAGE_TEET "left1.pcx",
   DATA_IMAGE_TEET "left2.pcx",
   DATA_IMAGE_TEET "left3.pcx",

   DATA_IMAGE_TEET "right0.pcx",
   DATA_IMAGE_TEET "right1.pcx",
   DATA_IMAGE_TEET "right2.pcx",
   DATA_IMAGE_TEET "right3.pcx",

   DATA_IMAGE_BOUNC "bounc0.pcx",
   DATA_IMAGE_BOUNC "bounc1.pcx",
   DATA_IMAGE_BOUNC "barrier.pcx",
  
   DATA_IMAGE_STATUS "times.pcx",
   DATA_IMAGE_STATUS "numbers0.pcx",
   DATA_IMAGE_STATUS "numbers1.pcx",
   DATA_IMAGE_STATUS "fuzz.pcx",
   DATA_IMAGE_STATUS "chead.pcx",
   DATA_IMAGE_STATUS "cheadoh.pcx",
   DATA_IMAGE_STATUS "sadc0.pcx",
   DATA_IMAGE_STATUS "sadc1.pcx",
   DATA_IMAGE_STATUS "sadc2.pcx",
   DATA_IMAGE_STATUS "enter.pcx",
  
   DATA_IMAGE_HIGH "top.pcx",
   DATA_IMAGE_HIGH "left.pcx",

   DATA_IMAGE_ACTS "seal0.pcx",
   DATA_IMAGE_ACTS "seal1.pcx",
  
   DATA_IMAGE_ACTS "beachb0.pcx",
   DATA_IMAGE_ACTS "beachb1.pcx",
   DATA_IMAGE_ACTS "beachb2.pcx",

   DATA_IMAGE_ACTS "bearr0.pcx",
   DATA_IMAGE_ACTS "bearr1.pcx",
   DATA_IMAGE_ACTS "bearl0.pcx",
   DATA_IMAGE_ACTS "bearl1.pcx"
};


/* Bits of background that change: */

#define NUM_BACKGROUND_CHANGES 4


static const int background_change_rects[NUM_BACKGROUND_CHANGES][4] = {
  {424, 0, 88, 127},
  {256, 150, 153, 87},
  {26, 288, 57, 63},
  {580, 295, 44, 55}
};






/* Acts: */

enum {
  ACT_SEAL,
  ACT_BEAR,
  NUM_ACTS
};


#ifdef SOUND

/* Sound enumerations: */

enum {
  SND_POP,
  SND_BOUNCE,
  SND_TEETER1,
  SND_TEETER2,
  SND_SPLAT,
  SND_APPLAUSE,
  SND_CHEERING,
  SND_HIGHSCORE,
  SND_KEYPRESS,
  NUM_SOUNDS
};


/* Sound filenames: */
#define DATA_SOUNDS DATA_PREFIX "sounds" FILE_SEPARATOR

static const char * sound_names[NUM_SOUNDS] = {
   DATA_SOUNDS "pop.wav",
   DATA_SOUNDS "bounce.wav",
   DATA_SOUNDS "teeter1.wav",
   DATA_SOUNDS "teeter2.wav",
   DATA_SOUNDS "splat.wav",
   DATA_SOUNDS "applause.wav",
   DATA_SOUNDS "cheering.wav",
   DATA_SOUNDS "wahoo.wav",
   DATA_SOUNDS "keypress.wav"
};

/* Music: */
//#ifdef GP32

#define MUS_TITLE DATA_PREFIX "a.dat"
#define MUS_GAME DATA_PREFIX "b.dat"
#define MUS_GAMEOVER DATA_PREFIX "c.dat"
#define MUS_HISCORE DATA_PREFIX "d.dat"
#define MUS_HISCORESCREEN DATA_PREFIX "e.dat"

/*#else

#define DATA_MUSICS DATA_PREFIX "music" FILE_SEPARATOR

#define MUS_TITLE  DATA_MUSICS "finally.mod"
#define MUS_GAME  DATA_MUSICS "klovninarki.mod"
#define MUS_GAMEOVER  DATA_MUSICS "kaupunki.mod"
#define MUS_HISCORE  DATA_MUSICS "hiscore.mod"
#define MUS_HISCORESCREEN  DATA_MUSICS "hiscreen.mod"
#endif*/


#endif

#ifdef DREAMCAST
/* XPM */
static char * vmu_xpm = 
"++++++++++++++++++++.........+++++.+++++++++++++"
"++++++++++++++++++...++++++...........++++++++++"
"+++++++++++++++++...++++++++...........+++++++++"
"++++++++++++++++....+++++++++..........+++++++++"
"++++++++++++++++...++++++++++..........+++++++++"
"+++++++++++++++..++++++++++.+............+++++++"
"+++++++++++++.......++++++.+.+++..........++++++"
"+++++++++++...........+++++++++++.........++++++"
"++++++++.............++++++.+++.++.......+++++++"
"++++++++.............+++++...+...+....++++++++++"
"++++++++++...........++++++.+++.++..++++++++++++"
"++++++++++..........+++.++++++++++..++++++++++++"
"++++++++++......++++++...++++++++.....++++++++++"
"++++++++++......+..+++....+++++.........++++++++"
"+++++++++++........++++....+++++......++.+++++++"
"+++++++++++++......++++......++++..+.+.+.+++++++"
"++++++++++++++.....++++++......+++......++++++++"
"++++++++++++++++...++++++++...........++++++++++"
"++++++++++++++++.+...++++++++.......++++++++++++"
"++++++++++++++++.+++...+++.+.+.+..++++++++++++++"
"++++++++++++++++..++++++++....+.++++++++++++++++"
"++++++++++++++++.+.+++++...+++.+++++++++++++++++"
"++++++++++++++++..+++++...++++++++++++++++++++++"
"++++++++++++++++..+++++.++++++++++++++++++++++++"
"++++++++++++++++++..+...++++++++++++++++++++++++"
"+++++++++++++++++++...++++++++++++++++++++++++++"
"++++++++++++++++++++++++++++++++++++++++++++++++"
"+++++++.+.++++++++++++.+++.++.+++++++++++++.++++"
"++++++.++++..++.+.+.+.++++.++++..++.+.+.+.+.++++"
"++++++.++.+.++.++.+.++.+++.++.+.+.+.+.++.+++++++"
"+++++++.+.+.+++.++..+.++++..+.+.+.++..+.+.+.++++"
"++++++++++++++++++++++++++++++++++++++++++++++++";
#endif


/* Function prototypes: */

int init(void);
void quit(void);
void finalize(void);
int setup(void);
void intro(void);
int title(void);
int game(void);
int gameover(void);
FILE * open_option_file(char * mode);
int highscorescreen(void);
int pausescreen(void);
void read_score(void);
void write_score(void);
SDL_RWops * get_data(const char *name);


/* Global variables: */
extern int GET_RAND[];
extern int get_rand_index;
extern int num_rects, num_players, coop, barriers, bouncy, clearall, sfx_vol, music_vol,
	has_highscore, highscore_index, show_highscores, highscore_effect;

extern SDL_Surface * screen;
extern SDL_Surface * images[NUM_IMAGES];
extern SDL_Rect rects[MAX_RECTS];

extern SDL_Joystick * js;

extern int flying_active, flying_splat, flying_dir,
  flying_x, flying_y, flying_xm, flying_ym,
  flying_left_arm, flying_right_arm, flying_left_leg, flying_right_leg;

extern Uint8 balloons[2][NUM_ROWS][20];
extern int balloon_colors[2][NUM_ROWS];
extern int score[2], lives[2];
extern int highscore[8];
extern char highscorer[8][4];
extern char username_initials[3];
extern int mouse_grabbed;

#ifdef SOUND
extern Mix_Chunk * sounds[NUM_SOUNDS];
extern Mix_Music * mus_title, * mus_game, * mus_gameover, * mus_hiscore, * mus_hiscreen;
#endif

extern Uint64 next_time;
extern int if_frame;

extern int game_done, game_quit;


/* Draw an object: */

static inline void draw(int x, int y, int pict)
{
  SDL_Rect src, dest;
  
  src.x = 0;
  src.y = 0;
  src.w = images[pict] -> w;
  src.h = images[pict] -> h;
#if DIVISOR > 1  
  dest.x = (x / DIVISOR);
  dest.y = (y / DIVISOR);
#else
  dest.x = x;
  dest.y = y;
#endif
  dest.w = src.w;
  dest.h = src.h;

  SDL_BlitSurface(images[pict], &src, screen, &dest);
}

static inline void drawtext(int x, int y, char * str)
{
  int i;
  SDL_Rect src, dest;
  
  for (i = 0; i < strlen(str); i++)
    {
      if (str[i] >= 'A' && str[i] <= 'Z')
	{
	  src.x = (str[i] - 'A') * (32 / DIVISOR);
	  src.y = 0;
	  src.w = (32 / DIVISOR);
	  src.h = (32 / DIVISOR);
#if DIVISOR > 1
	  dest.x = (x / DIVISOR) + (i * (32 / DIVISOR));
	  dest.y = (y / DIVISOR);
#else
	  dest.x = x + (i * 32);
	  dest.y = y;
#endif
	  dest.w = (32 / DIVISOR);
	  dest.h = (32 / DIVISOR);
	  
	  SDL_BlitSurface(images[IMG_LETTERS], &src,
			  screen, &dest);

         }
    }
}


static inline void drawnumber(int x, int y, int v, int img)
{
   char str[16];
   int i;
   SDL_Rect src, dest;
   
   sprintf(str, "%d", v);
   
   for (i=0; i < strlen(str); i++)
   {
      src.x = (str[i] - '0') * (32 / DIVISOR);
      src.y = 0;
      src.w = (32 / DIVISOR);
      src.h = (32 / DIVISOR);
#if DIVISOR > 1      
      dest.x = (x/DIVISOR) + (i * (32 / DIVISOR));
      dest.y = (y/DIVISOR);
#else
      dest.x = x + (i * 32);
      dest.y = y;
#endif
      dest.w = (32 / DIVISOR);
      dest.h = (32 / DIVISOR);
      
      SDL_BlitSurface(images[img], &src, screen, &dest);
   }
}

#ifdef SOUND
/* Play a sound: */

static inline void playsound(int snd)
{
    Mix_PlayChannel(-1, sounds[snd], 0);
}
#endif

static inline int wait_fps()
{
   int ret=0;
   Uint64 now=SDL_GetTicks();
   long end=next_time-now;
   if ( end > -FPS2 )
   {
      if ((if_frame)&&(end>(FPS+FPS2)))
         SDL_Delay(end-FPS);
      ret=-1;
   }
   else
      ret=0;
   next_time+=FPS;
   return ret;
}

static inline int getRand()
{
	get_rand_index=((get_rand_index+1)%GET_RAND_MAX);
	return GET_RAND[get_rand_index];
}

static inline void clear_events(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
		SDL_Delay(10);
}

