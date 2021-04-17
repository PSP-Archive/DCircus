#include "dcircus.h"
#include <pspsdk.h>
#include <pspdebug.h>

PSP_MODULE_INFO("dcircus_psp", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();

	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", (void *) exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}


/* Global variables: */
int use_sound, use_fullscreen, use_low, use_joystick, use_joy_analog,
	use_paddle, num_rects, num_players, coop, barriers, bouncy, clearall, sfx_vol, music_vol,
	has_highscore, highscore_index, show_highscores, highscore_effect;

SDL_Surface * screen;
SDL_Surface * images[NUM_IMAGES];
SDL_Rect rects[MAX_RECTS];

SDL_Joystick * js;

int flying_active, flying_splat, flying_dir,
  flying_x, flying_y, flying_xm, flying_ym,
  flying_left_arm, flying_right_arm, flying_left_leg, flying_right_leg;

Uint8 balloons[2][NUM_ROWS][20];
int balloon_colors[2][NUM_ROWS];
int score[2], lives[2];
int highscore[8];
char highscorer[8][4];
char username_initials[3];
int mouse_grabbed;

Uint64 next_time;
int if_frame;

#ifdef SOUND
Mix_Chunk * sounds[NUM_SOUNDS];
Mix_Music * mus_title, * mus_game, * mus_gameover, * mus_hiscore, * mus_hiscreen;
#endif

int main(int argc, char * argv[])
{
   	int done=0;

	  pspDebugScreenInit();
		SetupCallbacks();

#ifdef GP32
        x_gp32_SetCPUSpeed_100();
#endif
        if (!init())
        {    
  /* --- MAIN FUNCTION LOOP: --- */
                intro();
     		done = 0;
                do
                {
                   show_highscores = 0;
                   done = title();
      
                   if (!done)
                   {
	                  if (show_highscores == 0)
	                  	done = game();
   			  else
	                  	done = highscorescreen();

   		   }
                }while (!done);       

   /* Save options: */
  
//  save_options();
   	}
   	quit();
        return 0;

}

int init(void)
{
   	int i;
        sfx_vol = 3;
        music_vol = 3;

        for (i = 0; i < 8; i++)
        {
           highscore[i] = 100;
           strcpy(highscorer[i], "TUX");
        }
	read_score();

  /* Set gameplay defaults: */
     	if (!setup())
        {
             	num_players = 1;
                coop = 0;
                barriers = 0;
                bouncy = 0;
                clearall = 0;
                next_time=SDL_GetTicks();
                if_frame=-1;
                return 0;
        }
        return -1;
}

void quit(void)
{
   	SDL_Quit();
#ifdef GP32
	asm("mov        r3,#0; bx       r3;");
#endif
        exit(0);
}


void intro(void)
{
  int n,i,j;
  SDL_Rect dest;


  for (n = 0; n < 20; n++)
    {

  	for (i = 0; i < (640 / DIVISOR) ; i+=images[IMG_BICON]->w)
		for (j = 0; j< (480 / DIVISOR); j+=images[IMG_BICON]->h)
		{
			dest.x=i;
			dest.y=j;
			dest.h=images[IMG_BICON]->h;
			dest.w=images[IMG_BICON]->w;
			SDL_BlitSurface(images[IMG_BICON],NULL,screen,&dest);
		}
  	for (i = 0; i < 4; i++)
	  for (j = 0; j<4; j++)
		  draw(i*160,j*120,IMG_FUZZ);
  
	if (n>15)
		drawtext(192, 360, "PRESENTS");
	if (n>10)
	  	drawtext(32, 280 , "NEW BREED SOFTWARE");
	if (n>5)
		drawtext(120,160, "BY CPASJUSTE");
#ifdef DREAMCAST
	drawtext(80,80,"DREAMCAST PORT");
#else
	drawtext(200,80,"PSP PORT");
#endif
	SDL_Flip(screen);
      	SDL_Delay(100);
    }
}

