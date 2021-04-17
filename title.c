#include "dcircus.h"

/* Title screen: */

int title_highlightx, title_highlighty, title_highlightw, title_highlighth, old_title_highlightx,
     	old_title_highlighty, old_title_highlightw, old_title_highlighth;
int title_x[NUM_TITLE_BALLOONS], title_y[NUM_TITLE_BALLOONS], title_xm[NUM_TITLE_BALLOONS],
   	title_ym[NUM_TITLE_BALLOONS], title_color[NUM_TITLE_BALLOONS], title_bumped[NUM_TITLE_BALLOONS];
int title_text_x, title_text_xm, title_text_img, title_text_time;
int title_done, title_quit, title_frame, title_case;

static inline void title_init(void)
{

   int i;
     
  // Draw background

  SDL_BlitSurface(images[IMG_TITLE], NULL, screen, NULL);
  SDL_Flip(screen);
    
  // Reset highlight info
  
  title_highlightx = -1;
  title_highlighty = -1;
  title_highlightw = -1;
  title_highlighth = -1;

  old_title_highlightx = -1;
  old_title_highlighty = -1;
  old_title_highlightw = -1;
  old_title_highlighth = -1;
  
  
  // Set balloon positions, directions and colors
  
  for (i = 0; i < NUM_TITLE_BALLOONS; i++)
    {
      title_x[i] = getRand() % (640 - 32);
      title_y[i] = getRand() % (480 - 32);
      
      title_xm[i] = (getRand() % 5) + 1;
      
      if ((getRand() % 2) == 0)
	title_xm[i] = -title_xm[i];
      
      title_ym[i] = (getRand() % 5) + 1;

      if ((getRand() % 2) == 0)
	title_ym[i] = -title_ym[i];
      
      title_color[i] = ((getRand() % 8) * 6) + IMG_BALLOON_RED_LEFT_0;
      title_bumped[i] = 0;
    }
  
  highscore_effect = 0;
  show_highscores = 0;
  title_text_x = -640;
  title_text_xm = 36;
  title_text_img = 0;
  title_text_time = 0;
  title_done=0;
  title_quit=0;
  title_frame=0;
  title_case=0;
  title_highlightx = (16 / DIVISOR);
  title_highlighty = (283 / DIVISOR);
  title_highlightw = (238 / DIVISOR);
  title_highlighth = (27 / DIVISOR);

  clear_events();
}

static inline void title_handle(void)
{
   SDL_Event title_event;
   int left=0, right=0, up=0, down=0, hit=0, hat=0;
   
      // Handle title_events
      while (SDL_PollEvent(&title_event))
	{
	  if (title_event.type == SDL_QUIT)
	    {
	      // Quit title_event!
	      
	      title_quit = 1;
	    }
#ifndef JOY_YES
	  else if (title_event.type == SDL_KEYDOWN)
	    {
	      // A keypress!
                            
              switch(title_event.key.keysym.sym)
              {
	      
                 case SDLK_ESCAPE:
		  // Escape: title_done
		  title_quit = 1;
                  break;
                 case SDLK_UP:
                    up=1;
                    break;
                 case SDLK_DOWN:
                    down=1;
                    break;
                 case SDLK_RIGHT:
                    right=1;
                    break;
                 case SDLK_LEFT:
                    left=1;
                    break;
                 case SDLK_RETURN:
                    hit=1;
                    break;
              }
	    }
#else
          else if (title_event.type == SDL_JOYHATMOTION)
            {
#ifdef DREAMCAST
   		hat=15^(SDL_JoystickGetHat(js,title_event.jhat.hat));
#else
   		hat=SDL_JoystickGetHat(js,title_event.jhat.hat);
#endif
                if (hat & SDL_HAT_UP)
                   up=1;
                else if (hat & SDL_HAT_DOWN)
                   down=1;
                else if (hat & SDL_HAT_LEFT)
                   left=1;
                else if (hat & SDL_HAT_RIGHT)
                   right=1;
            }
          else if (title_event.type == SDL_JOYBUTTONDOWN)
            {
               hit=1;
            }
#endif
          switch (title_case)
          {
             case 0:
                if (hit)
                {
                   num_players = 1;
                   title_done = 1;
                } else if (up)
                   title_case=5;
                else if (down)
                {
                   title_case=1;
                }
                else if ((left)|(right))
                {
                   title_case=6;
                }
                title_highlightx = (16 / DIVISOR);
                title_highlighty = (283 / DIVISOR);
                title_highlightw = (238 / DIVISOR);
                title_highlighth = (27 / DIVISOR);
                break;
             case 1:
                if (hit)
                {
		  num_players = 2;
		  coop = 0;
		  title_done = 1;
                } else if (up)
                   title_case=0;
                else if (down)
                   title_case=2;
                else if ((left)||(right))
                   title_case=6;
                title_highlightx = (16 / DIVISOR);
                title_highlighty = (310 / DIVISOR);
                title_highlightw = (264 / DIVISOR);
                title_highlighth = (27 / DIVISOR);
                break;
             case 2:
                if (hit)
                {
		  num_players = 2;
		  coop = 1;
		  title_done = 1;
                } else if (up)
                   title_case=1;
                else if (down)
                   title_case=3;
                else if ((left)||(right))
                   title_case=7;
                title_highlightx = (16 / DIVISOR);
                title_highlighty = (337 / DIVISOR);
                title_highlightw = (356 / DIVISOR);
                title_highlighth = (27 / DIVISOR);
                break;
             case 3:
                if (hit)
                {
                   barriers = !barriers;
#ifdef SOUND
                   playsound(SND_TEETER2 - barriers);
#endif
                } else if (up)
                   title_case=2;
                else if (down)
                   title_case=4;
                else if ((left)||(right))
                   title_case=8;
                title_highlightx = 0;
                title_highlighty = (371 / DIVISOR);
                title_highlightw = (207 / DIVISOR);
                title_highlighth = (27 / DIVISOR);
                break;
             case 4:
                if (hit)
                {
		  bouncy = !bouncy;
#ifdef SOUND
		  playsound(SND_TEETER2 - bouncy);
#endif
                } else if (up)
                   title_case=3;
                else if (down)
                   title_case=5;
                else if ((left)||(right))
                   title_case=8;
                title_highlightx = 0;
		title_highlighty = (398 / DIVISOR);
		title_highlightw = (374 / DIVISOR);
		title_highlighth = (27 / DIVISOR);
                break;
             case 5:
                if (hit)
                {
		  clearall = !clearall;
#ifdef SOUND
		  playsound(SND_TEETER2 - clearall);
#endif
                } else if (up)
                   title_case=4;
                else if (down)
                   title_case=0;
                else if ((left)||(right))
                   title_case=9;
                title_highlightx = 0;
                title_highlighty = (425 / DIVISOR);
                title_highlightw = (234 / DIVISOR);
                title_highlighth = (27 / DIVISOR);
                break;
             case 6:
                if (hit)
                {
		  sfx_vol = (sfx_vol + 1) % 4;
#ifdef SOUND		  
		  Mix_Volume(-1, (sfx_vol * MIX_MAX_VOLUME) / 3);
		  
		  playsound(SND_POP);
#endif
                } else if (up)
                   title_case=9;
                else if (down)
                   title_case=7;
                else if ((left)||(right))
                   title_case=0;
                title_highlightx = (559 / DIVISOR);
                title_highlighty = (284 / DIVISOR);
                title_highlightw = (73 / DIVISOR);
                title_highlighth = (52 / DIVISOR);
                break;
             case 7:
                if (hit)
                {
                  music_vol = (music_vol + 1) % 4;
#ifdef SOUND		  
		  Mix_VolumeMusic((music_vol * MIX_MAX_VOLUME) / 3);
		  
		  playsound(SND_POP);
#endif
                } else if (up)
                   title_case=6;
                else if (down)
                   title_case=8;
                else if ((left)||(right))
                   title_case=2;
                title_highlightx = (512 / DIVISOR);
                title_highlighty = (336 / DIVISOR);
                title_highlightw = (121 / DIVISOR);
                title_highlighth = (52 / DIVISOR);
                break;
             case 8:
                if (hit)
                {
#ifdef SOUND		  
		  playsound(SND_HIGHSCORE);
#endif		  
		  title_done = 1;
		  show_highscores = 1;
                } else if (up)
                   title_case=7;
                else if (down)
                   title_case=9;
                else if ((left)||(right))
                   title_case=4;
                title_highlightx = (440 / DIVISOR);
                title_highlighty = (398 / DIVISOR);
                title_highlightw = (195 / DIVISOR);
                title_highlighth = (29 / DIVISOR);
                break;
             case 9:
                if (hit)
                {
		  title_quit = 1;
                } else if (up)
                   title_case=8;
                else if (down)
                   title_case=6;
                else if ((left)||(right))
                   title_case=5;
                title_highlightx = (535 / DIVISOR);
                title_highlighty = (429 / DIVISOR);
                title_highlightw = (100 / DIVISOR);
                title_highlighth = (29 / DIVISOR);
                break;
                
          }
	}
}

static inline void title_draw(void)
{
   SDL_Rect src, dest;
   int i;
   
      // Draw highlight
      
      if (title_highlightx != -1)
	{
           src.x = title_highlightx;
           src.y = title_highlighty - (275 / DIVISOR);
           src.w = title_highlightw;
           src.h = title_highlighth;
	  
           dest.x = title_highlightx;
           dest.y = title_highlighty;
           dest.w = title_highlightw;
           dest.h = title_highlighth;
	  SDL_BlitSurface(images[IMG_TITLE_HIGHLIGHTS], &src, screen, &dest);
	}
      
      
      // Draw whether options are on: */
      
      if (barriers)
	draw(0, 376, IMG_LIGHT_ON);
      else
	draw(0, 376, IMG_LIGHT_OFF);
      
      if (bouncy)
	draw(0, 404, IMG_LIGHT_ON);
      else
	draw(0, 404, IMG_LIGHT_OFF);

      if (clearall)
	draw(0, 429, IMG_LIGHT_ON);
      else
	draw(0, 429, IMG_LIGHT_OFF);
      
      for (i = 0; i < 3; i++)
	{
	  if (sfx_vol > i)
	    draw(583 + i * 16, 311, IMG_LIGHT_ON);
	  else
	    draw(583 + i * 16, 311, IMG_LIGHT_OFF);

	  if (music_vol > i)
	    draw(583 + i * 16, 363, IMG_LIGHT_ON);
	  else
	    draw(583 + i * 16, 363, IMG_LIGHT_OFF);	
	}
      
      
      // Draw the balloons
      
	  for (i = 0; i < NUM_TITLE_BALLOONS; i++)
	    {
	      if (title_xm[i] > 0)
		draw(title_x[i], title_y[i], title_color[i] + 2 + (getRand() % 2));
	      else
		draw(title_x[i], title_y[i], title_color[i] + (getRand() % 2));
	    }
      
      // Draw the credits
      
      draw(title_text_x, 252, IMG_PROGRAMMER + title_text_img);
      
      
      // Update the screen
      SDL_Flip(screen);

}

static inline void title_move(void)
{
   int i, j, tmp;

      // Move the balloons
      for (i = 0; i < NUM_TITLE_BALLOONS; i++)
	{
	  // Move the balloon
	  
	  title_x[i] = title_x[i] + title_xm[i];
	  title_y[i] = title_y[i] + title_ym[i];
	  

	  // Apply gravity
	  
	  if ((title_frame % 3) == 0)
	    {
	      title_ym[i] = title_ym[i] + 1;
	      
	      if (title_ym[i] > 16)
		title_ym[i] = 16;
	    }
	  
	  
	  // Reset 'bumped' flag
	  
	  title_bumped[i] = 0;
	}

      
      // Make balloons bounce into each other
      
      for (i = 0; i < NUM_TITLE_BALLOONS; i++)
	{
	  for (j = 0; j < NUM_TITLE_BALLOONS; j++)
	    {
	      if (j != i && title_bumped[j] == 0 && title_bumped[i] == 0)
		{
		  if (title_x[i] > title_x[j] - 32 &&
		      title_x[i] < title_x[j] + 32 &&
		      title_y[i] > title_y[j] - 32 &&
		      title_y[i] < title_y[j] + 32)
		    {
		      title_x[i] = title_x[i] - (title_xm[i] * 2) / 3;
		      title_y[i] = title_y[i] - (title_ym[i] * 2) / 3;
		      
		      tmp = title_xm[i];
		      title_xm[i] = title_xm[j];
		      title_xm[j] = tmp;

		      tmp = title_ym[i];
		      title_ym[i] = title_ym[j];
		      title_ym[j] = tmp;
		      
		      title_bumped[i] = 1;
		      title_bumped[j] = 1;
		    }
		}
	    }
	}


      // Keep balloons in bounds
      
      for (i = 0; i < NUM_TITLE_BALLOONS; i++)
	{
	  if (title_x[i] <= 0)
	    {
	      title_xm[i] = (getRand() % 5) + 1;
	      title_x[i] = 0;
	    }
	  else if (title_x[i] >= 640 - 32)
	    {
	      title_xm[i] = -((getRand() % 5) + 1);
	      title_x[i] = 640 - 32;
	    }
	  
	  if (title_y[i] <= 0)
	    {
	      title_ym[i] = (getRand() % 5) + 1;
	      title_y[i] = 0;
	    }
	  else if (title_y[i] >= 480 - 32)
	    {
	      title_ym[i] = -(title_ym[i]);
	      title_y[i] = 480 - 32;
	    }
	}
      
      
      // Handle credits
      
      if (title_text_x < 0)
	{
	  title_text_x = title_text_x + title_text_xm;
	  
	  if (title_text_x >= 0)
	    {
	      title_text_x = 0;
	      title_text_xm = 0;
	    }
	  else
	    title_text_xm--;
	}
      else if (title_text_x == 0 && title_text_time < 100)
	{
	  title_text_time++;
	  title_text_xm = 0;
	}
      else if (title_text_x < 640)
	{
	  title_text_x = title_text_x + title_text_xm;
	  title_text_xm++;
	}
      else
	{
	  title_text_img = (title_text_img + 1) % 3;
	  title_text_x = -640;
	  title_text_xm = 36;
	  title_text_time = 0;
	}
}
  
int title(void)
{
  // --- MAIN TITLE LOOP ---
     title_init();
  do
    {
      title_frame++;     
      title_handle();
      title_move();
      if (if_frame)
      {
      	    SDL_BlitSurface(images[IMG_TITLE], NULL, screen, NULL);
            title_draw();
      }
      if_frame=wait_fps();

#ifdef MUSIC      
      // Keep playing music
      
	  if (!Mix_PlayingMusic())
	    {
	      Mix_PlayMusic(mus_title, 0);
	      Mix_VolumeMusic((music_vol * MIX_MAX_VOLUME) / 3);
	    }
#endif
    }
  while (title_done == 0 && title_quit == 0);  
  
#ifdef SOUND
  // Play a pop noise
  
  playsound(SND_POP);
  
  
  // Stop title music
  
  Mix_HaltMusic();
#endif
  SDL_Delay(300);

  return(title_quit);
}

