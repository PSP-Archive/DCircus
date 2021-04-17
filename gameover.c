#include "dcircus.h"


/* Game Over */

static inline void gameover_draw(int len, int frame)
{
	  SDL_Rect dest;

          // Blank screen: 
#ifndef GP32
          SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
#else
	  int i;
	  void *buf=screen->pixels;
	  for(i=0;i<240;i+=16)
	  {
		memset(buf+(i*320),0,320*16);
		if (i%32)
			SDL_Delay(1);
	  }
#endif
      
      
          // Draw "Final Score(s)" Text: 
      
          if (num_players == 1)
             drawtext(144, 144, "FINAL SCORE");
          else
             drawtext(128, 144, "FINAL SCORES");
      
      
          // Show game_players' scores: 
      
          if (num_players == 1)
          {
             drawnumber(224, 272, score[0], IMG_NUMBERS_0);
	  
             if (has_highscore == 0)
             {
                drawtext(208, 0, "HISCORE");
                draw(207, 64, IMG_ENTER_INITIALS);
             }
          }
          else
          {
             // Show game_player 1's score:
	  
             drawtext(0, 208, "PLAYER");
             drawtext(208, 208, "ONE");
             drawnumber(0, 272, score[0], IMG_NUMBERS_0);
	  
             if (has_highscore == 0)
             {
                drawtext(0, 0, "HISCORE");
                draw(0, 64, IMG_ENTER_INITIALS);
             }
	  
	  
             // Show game_player 2's score:
	  
             drawtext(336, 208, "PLAYER");
             drawtext(544, 208, "TWO");
             drawnumber(336, 272, score[1], IMG_NUMBERS_1);
	  
             if (has_highscore == 1)
             {
                drawtext(416, 0, "HISCORE");
                draw(415, 64, IMG_ENTER_INITIALS);
             }
          }
          // Draw text:
		      
          if (has_highscore == 0)
          {
             if (num_players == 1)
             {
                if ((frame%10)>5)
                {
                   dest.x=(272+(len<<5)) / DIVISOR;
                   dest.y=56 / DIVISOR;
                   dest.w=32 / DIVISOR;
                   dest.h=8 / DIVISOR;
                   SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 128, 128, 255));
                }
                drawtext(272, 32, highscorer[highscore_index]);
             }
             else
             {
                if ((frame%10)>5)
                {
                   dest.x=(len<<5) / DIVISOR;
                   dest.y=56 / DIVISOR;
                   dest.w=32 / DIVISOR;
                   dest.h=8 / DIVISOR;
                   SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 128, 128, 255));
                }
                drawtext(0, 32, highscorer[highscore_index]);
             }
          }
	  // Draw sad clown:
	  
	  draw(512, 320, IMG_SADCLOWN_0 + (frame / 5) % 3);
          
	  // Flip & Pause:
	  SDL_Flip(screen);
#ifndef GP32
	  SDL_Delay(30);
#endif
}


static inline int gameover_event(int l)
{
   SDL_Event event;
   SDLKey key;
   Uint8 hat;
   int left=0,right=0,up=0,down=0,hit=0,len=l;
   
	  while (SDL_PollEvent(&event) > 0)
	    {
	      if (event.type == SDL_QUIT)
		{
		  /* Quit request: game_done and game_quit! */
		  
                  hit = 1;
		  game_quit = 1;
		}
	      else if (event.type == SDL_KEYDOWN)
		{
		  /* A keypress! */
		  
		  key = event.key.keysym.sym;
                  
                  if ((key == SDLK_RETURN)||(key == SDLK_ESCAPE))
                     hit = 1;		      
                    
		  /* Does someone have high score? Accept input! */
		  
		  if (has_highscore != -1)
		    {
                      if (key == SDLK_LEFT)
                         left = 1;
                      else
                         if (key == SDLK_RIGHT)
                            right = 1;
                         else
                            if (key == SDLK_UP)
                               up = 1;
                            else
                               if (key == SDLK_DOWN)
                                  down = 1;
		    }
		}
              else if (event.type == SDL_MOUSEBUTTONDOWN
#ifdef JOY_YES
              		|| event.type == SDL_JOYBUTTONDOWN
#endif
                        )
		{
		  /* Click - game_done: */
                     hit = 1;
		}
#ifdef JOY_YES              
              else if ((event.type == SDL_JOYHATMOTION)&&(has_highscore != -1))
              {
#ifdef DREAMCAST
                 hat=15^(SDL_JoystickGetHat(js,event.jhat.hat));
#else
                 hat=SDL_JoystickGetHat(js,event.jhat.hat);
#endif
                 if (hat & SDL_HAT_LEFT)
                    left = 1;
                 else if (hat & SDL_HAT_RIGHT)
                    right = 1;
                 else if (hat & SDL_HAT_UP)
                    up = 1;
                 else if (hat & SDL_HAT_DOWN)
                    down = 1;
              }
#endif
	    }
            
            if (hit)
            {
               game_done=1;
#ifdef SOUND
               if (has_highscore != -1)
               	playsound(SND_HIGHSCORE);
#endif
            }
            else
               if ((left)&&(len>0))
               {
                  len--;
#ifdef SOUND
		  playsound(SND_POP);
#endif
               }
               else
                  if ((right)&&(len<2))
                  {
                     len++;
#ifdef SOUND
                     playsound(SND_POP);
#endif                     
                  }
                  else
                     if (up)
                     {
                        highscorer[highscore_index][len] = ((highscorer[highscore_index][len]-'A'+1)%('Z'-'A'+1))+'A';
#ifdef SOUND
                        playsound(SND_KEYPRESS);
#endif                     
                     }
                     else
                        if (down)
                        {
                           if (highscorer[highscore_index][len] == 'A')
                              highscorer[highscore_index][len] = 'Z';
                           else
                              highscorer[highscore_index][len]--;
#ifdef SOUND
   			   playsound(SND_KEYPRESS);
#endif                     
                        }
            return len;
}




int gameover(void)
{
  int frame=0, len=0;
  
  /* Stop music and sounds: */
  
#ifdef SOUND
      Mix_HaltMusic();
      Mix_HaltChannel(-1);
#endif
  

  /* Show scores: */
  
  if (game_quit == 0)
    {
      /* Darken screen: */
      
      SDL_Delay(300);
      
      game_done = 0;
      
      /* Wait for something: */

      do
	{
	  frame++;
          len=gameover_event(len);
          	  
	  /* Play game-over or high-score music: */
	  
#ifdef MUSIC
	      if (!Mix_PlayingMusic())
		{
		  if (has_highscore == -1)
		    Mix_PlayMusic(mus_gameover, 0);
		  else
		    Mix_PlayMusic(mus_hiscore, 0);
		  
		  Mix_VolumeMusic((music_vol * MIX_MAX_VOLUME) / 3);
		}
#endif
          gameover_draw(len,frame);
	  
	}
      while (game_done == 0);
      
      /* Silence music: */
      
#ifdef SOUND
	Mix_HaltMusic();
#endif
    }
  
  write_score();  
  /* Back to main!: */
  
  return(game_quit);
}

