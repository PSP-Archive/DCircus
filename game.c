#include "dcircus.h"

/* Game: */

int game_teeter_x, game_teeter_xm, game_teeter_xmm, old_game_teeter_x, game_teeter_roll;
int game_teeter_side, game_teeter_sound;
int game_bouncers[2], game_barrier_x[3];
int game_done, game_quit;


int game_act, game_act_x, game_act_y, game_act_xm, game_act_ym;


/* Reset a row of balloons: */

static inline void resetballoons(int game_player, int row)
{
  int i;
  
  for (i = 0; i < 20; i++)
    balloons[game_player][row][i] = NORMAL;
}

/* Draw a balloon: */

static inline void drawballoon(int game_player, int x, int y, int off)
{
  SDL_Rect dest;
  int img;  

  img = -1;
  
  if (balloons[game_player][y][x] != GONE)
    {
      /* What color balloon? */
      
      img = IMG_BALLOON_RED_LEFT_0 + (balloon_colors[game_player][y] * 6);
      
      if (highscore_effect)
	img = IMG_BALLOON_RED_LEFT_0 + ((getRand() % 8) * 6);
      
      
      /* Is it popping? */
      
      if (balloons[game_player][y][x] != NORMAL)
	{
	  /* Yes: */
	  
	  img = img + 4;
	  
	  if (balloons[game_player][y][x] == POPPING0)
	    img++;
	}
      else
	{
	  /* No:  Wave string */
	  
	  img = img + (getRand() % 2);
	  
	  
	  /* What direction is it going? */
	  
	  img = img + ((y % 2) * 2);
	}
      
      
      /* Draw it! */
      
      dest.x = x * (32 / DIVISOR);
      
      /* (Shove it left or right 1/2 a space?) */
      
      if (off == 1)
	{
	  if ((y % 2) == 0)
	    dest.x = dest.x - (16 / DIVISOR);
	  else
	    dest.x = dest.x + (16 / DIVISOR);
	}
      
      dest.y = (y * (32 / DIVISOR)) + (32 / DIVISOR);
      dest.w = (32 / DIVISOR);
      dest.h = (32 / DIVISOR);
      
      SDL_BlitSurface(images[img], NULL,
		      screen, &dest);
    }
}



static inline void addscore(int game_player, int inc)
{
  score[game_player] = score[game_player] + inc;
  
  
  /* Is this a high score? */
  
  if (score[game_player] >= highscore[highscore_index])
    {
      /* Did they just get this high score? */
      
      if (has_highscore != game_player)
	{
	  has_highscore = game_player;
#ifdef SOUND
	  playsound(SND_HIGHSCORE);
#endif
	  highscore_effect = 50;
	}
      
      highscore[highscore_index] = score[game_player];
    }
}

/* Draw a clown: */

static inline void drawclown(int x, int y, int side,
	       int left_arm, int right_arm,
	       int left_leg, int right_leg)
{
  draw(x, y, IMG_CLOWN_BODY_LEFT + side);
  
  if (left_arm != -1)
    draw(x, y, IMG_CLOWN_LEFT_ARM_0 + left_arm);
  
  if (right_arm != -1)
    draw(x, y, IMG_CLOWN_RIGHT_ARM_0 + right_arm);
  
  draw(x, y, IMG_CLOWN_LEFT_LEG_0 + left_leg);
  draw(x, y, IMG_CLOWN_RIGHT_LEG_0 + right_leg);
}

/* Draw a darkened area: */

static inline void drawfuzz(int x, int y, int w, int h)
{
  int nx,ny,nw,nh;
  int xx, yy;
  SDL_Rect src, dest;
#if DIVISOR > 1
  nx=(x / DIVISOR);
  ny=(y / DIVISOR);
  nw=(w / DIVISOR);
  nh=(h / DIVISOR);
#else
  nx=x;
  ny=y;
  nw=w;
  nh=h;
#endif
  
  for (yy = ny; yy < ny + nh; yy = yy + (images[IMG_FUZZ] -> h))
    {
      for (xx = nx; xx < nx + nw; xx = xx + (images[IMG_FUZZ] -> w))
	{
	  src.x = 0;
	  src.y = 0;
	  src.w = images[IMG_FUZZ] -> w;
	  src.h = images[IMG_FUZZ] -> h;
	  
	  if (xx + src.w > nx + nw)
	    src.w = nx + nw - xx;
	  
	  if (yy + src.h > ny + nh)
	    src.h = ny + nh - yy;
	  
	  dest.x = xx;
	  dest.y = yy;
	  
	  dest.w = src.w;
	  dest.h = src.h;
	  
	  SDL_BlitSurface(images[IMG_FUZZ], &src,
			  screen, &dest);
	}
    }
  
}

/* Activate a clown: */

static inline void newclown(void)
{
  /* Activate the clown: */
  
  flying_active = 1;
  flying_splat = 0;
  
  
  /* Start somewhere: */
  
  flying_x = 608 * (getRand() % 2);
  flying_y = FLYING_START_Y;
  
  if (flying_x == 0)
    flying_dir = RIGHT;
  else
    flying_dir = LEFT;
  
  
  /* Speed: */
  
  flying_xm = 0;
  flying_ym = FLYING_START_YM;
  
  
  /* Set limbs getRandomly: */
  
  flying_left_arm = getRand() % 3;
  flying_right_arm = getRand() % 3;
  flying_left_leg = getRand() % 2;
  flying_right_leg = getRand() % 2;
}

static inline void game_init(void)
{
  int i,y;
  
  /* Init: */
    
  has_highscore = -1;
  highscore_effect = 0;
  
  game_teeter_side = LEFT;
  game_teeter_x = 0;
  game_teeter_xm = 0;
  game_teeter_xmm = 0;
  game_teeter_roll = 0;
  game_teeter_sound = 0;
  mouse_grabbed = 0;
  
  game_bouncers[0] = 0;
  game_bouncers[1] = 0;
  
  for (i = 0; i < 2; i++)
    {
      lives[i] = STARTING_LIVES;
      score[i] = 0;
      
      for (y = 0; y < NUM_ROWS; y++)
	resetballoons(i, y);
      
      for (y = 0; y < NUM_ROWS; y++)
	balloon_colors[i][y] = y * 2;
    }
  
  flying_active = 0;
  flying_splat = 0;
  highscore_index = (barriers * 4 + bouncy * 2 + clearall);
  
  for (i = 0; i < NUM_BARRIERS; i++)
    game_barrier_x[i] = i * 128;
  
  game_done = 0;
  game_quit = 0;
 
  clear_events();
 

  /* Draw entire background: */
  
  SDL_BlitSurface(images[IMG_BACKGROUND_0], NULL, screen, NULL);  
  SDL_Flip(screen);
}


static inline int game_event(int showp)
{

      /* Handle events: */


  int fire=0;
  int mouse_x=0, mouse_y=0;
  SDL_Event event;
  SDLKey key=SDLK_UNKNOWN;
  Uint8 * keystate;
  static Sint16 axis=0;
  Uint8 hat;
  int show_player=showp;
  
      while (SDL_PollEvent(&event) > 0)
	{
	  /* Handle digital controls: */
	  
	  keystate = SDL_GetKeyState(NULL);
	  
	  hat = 0;
#ifdef JOY_YES
          if (event.type == SDL_JOYHATMOTION)
#ifdef DREAMCAST
             hat=15^(SDL_JoystickGetHat(js,event.jhat.hat));
#else
             hat=SDL_JoystickGetHat(js,event.jhat.hat);
#endif
#endif
	  
	  if (keystate[SDLK_LEFT] || (hat & SDL_HAT_LEFT))
	    {
	      game_teeter_xmm = -2;
	      if (game_teeter_xm > 0)
		game_teeter_xm = 0;
	    }
	  else if (keystate[SDLK_RIGHT] || (hat & SDL_HAT_RIGHT))
	    {
	      game_teeter_xmm = 2;
	      if (game_teeter_xm < 0)
		game_teeter_xm = 0;
	    }
	  else
	    {
	      game_teeter_xmm = 0;
	      game_teeter_xm = 0;
	    }
	  
	  
	  /* Handle incoming events: */
	  
	  if (event.type == SDL_QUIT)
	    {
	      /* Quit request: */      
              
	      game_quit = 1;
	    }
          else if (event.type == SDL_KEYDOWN)
            {
              /* A keypress! */
              
              key = event.key.keysym.sym;
              
              if (key == SDLK_ESCAPE)
                {
                  /* Escape: Quit the game and return to main menu: */
                
                  game_done = 1;
                }
	      else if (key == SDLK_SPACE || key == SDLK_TAB ||
		       key == SDLK_p)
		{
		  /* SPACE, TAB or P: Pause! */
		  
		  game_done = pausescreen();
		  if (game_done == 2)
		    {
		      game_done = 0;
		      game_quit = 1;
		    }
		}
	      else if (key == SDLK_LALT || key == SDLK_RALT ||
		       key == SDLK_LSHIFT || key == SDLK_RSHIFT ||
		       key == SDLK_RETURN)
		{
		  /* Fire button! */
		  
		  fire = 1;
		}
	      else if (key == SDLK_g)
		{
		  /* Toggle mouse grabbing: */
		  
		  if (mouse_grabbed == 0)
		    {
		      SDL_WM_GrabInput(SDL_GRAB_ON);
                      mouse_grabbed = 1;
		    }
		  else
		    {
		      SDL_WM_GrabInput(SDL_GRAB_OFF);
		      mouse_grabbed = 0;
		    }
		}
	    }
	  else if (event.type == SDL_MOUSEMOTION)
	    {
	      game_teeter_x = event.motion.x - 48;
	      mouse_x = event.motion.x;
	      mouse_y = event.motion.y;
	      
	      if (game_teeter_x < 32)
		game_teeter_x = 32;
	      else if (game_teeter_x > 512)
		game_teeter_x = 512;
	    }

#ifdef JOY_YES

	  else  if (event.type == SDL_JOYAXISMOTION)
	    {
	      // Joystick motion
		  // Analog

		  if (event.jaxis.axis == 0)
		    {
                       axis = event.jaxis.value>>3;
                       if (axis>32)
                          axis=32;
                       else if (axis<-32)
                          axis=-32;
		    }
	    }
          else if (event.type == SDL_JOYBUTTONDOWN)
             {
                switch (event.jbutton.button)
                {
                   case 1:
                      game_done=1;
                      break;
                   case 3:
                      game_done = pausescreen();
                      if (game_done == 2)
                      {
                         game_done = 0;
                         game_quit = 1;
                      }
                      break;
                   default:
                      fire=1;
                }
             }
#endif
	  else if (event.type == SDL_MOUSEBUTTONDOWN)
	    {
	      fire = 1;
	    }
	}

      game_teeter_x += axis;
      if (game_teeter_x < 32)
	game_teeter_x = 32;
      else if (game_teeter_x > 512)
	game_teeter_x = 512;
      
      /* Handle fire button: */
      
      if (fire)
	{
	  /* Swap teeter-totter side: */
	  
	  if (flying_active)
	    game_teeter_side = 1 - game_teeter_side;
	  
	  
	  /* Activate a new clown: */
	  
	  if (flying_active == 0 && flying_splat == 0)
	    {
	      /* Add the new clown: */
	      
	      newclown();
            	  
	      /* Erase "Player X Ready" message, if any: */
	      
	      if (show_player > 0)
		show_player = 1;
	    }
	}
        return show_player;
}

static inline void game_handle(int player, int frame)
{
   int i, x, y, any, some;
   
      /* Handle highscore effect: */
      
      if (highscore_effect > 0)
	highscore_effect--;


      /* Handle the barrel: */
      
      if (game_teeter_x > old_game_teeter_x)
	{
	  game_teeter_roll = game_teeter_roll + 1;
	  if (game_teeter_roll > 3)
	    game_teeter_roll = 0;
	}
      else if (game_teeter_x < old_game_teeter_x)
	{
	  game_teeter_roll = game_teeter_roll - 1;
	  if (game_teeter_roll < 0)
	    game_teeter_roll = 3;
	}
      /* Handle bouncers: */
      
      for (i = 0; i < 2; i++)
	{
	  if (game_bouncers[i] > 0)
	    game_bouncers[i]--;
	}
      
      
      /* Handle barriers: */
      
      if (barriers)
	{
	  for (i = 0; i < NUM_BARRIERS; i++)
	    {
	      /* Move and wrap-around: */
	      
	      game_barrier_x[i] = game_barrier_x[i] + 8;
	      
	      if (game_barrier_x[i] >= 640)
		game_barrier_x[i] = -64;
	    }
	}
      /* Handle balloons: */
      
      any = 0;
      
      for (y = 0; y < NUM_ROWS; y++)
	{
	  /* Handle popping: */
	  
	  some = 0;
	  
	  for (x = 0; x < 20; x++)
	    {
	      if (balloons[player][y][x] == POPPING0)
		balloons[player][y][x] = GONE;
	      else if (balloons[player][y][x] > POPPING0)
		balloons[player][y][x]--;
	      
	      if (balloons[player][y][x] == NORMAL)
		{
                  some = 1;
		  any = 1;
		}
	    }
          	  
	  /* All balloons popped?  Add more? */
	  
	  if (some == 0 && flying_y > (NUM_ROWS * 32) + 64 &&
	      clearall == 0)
	    {
	      resetballoons(player, y);
	      balloon_colors[player][y]++;
	      
	      if (balloon_colors[player][y] > 7)
		balloon_colors[player][y] = 0;
	      
	      if (y == 0)
		{
		  addscore(player, 1000);
		  lives[player]++;
#ifdef SOUND
		  playsound(SND_CHEERING);
#endif
		}
	      else if (y == 1)
		{
		  addscore(player, 250);
#ifdef SOUND
		  playsound(SND_APPLAUSE);
#endif
		}
	      else if (y == 2)
		{
		  addscore(player, 100);
#ifdef SOUND
		  playsound(SND_APPLAUSE);
#endif
		}
	    }
	  
	  
	  /* Move balloons: */
	  
	  if ((frame % 4) == 0)
	    {
	      if ((y % 2) == 0)
		{
		  /* Left: */
		  
		  i = balloons[player][y][0];
		  
		  for (x = 0; x < 19; x++)
		    balloons[player][y][x] = balloons[player][y][x + 1];
		  
		  balloons[player][y][19] = i;
		}
	      else
		{
		  /* Right: */
		  
		  i = balloons[player][y][19];
		  
		  for (x = 18; x >= 0; x--)
		    balloons[player][y][x + 1] = balloons[player][y][x];
		  
		  balloons[player][y][0] = i;
		}
	    }
	}
      /* Reset all balloons? */
      
      if (clearall && !any && flying_y > (NUM_ROWS * 32) + 64)
	{
	  for (y = 0; y < 3; y++)
	    {
	      resetballoons(player, y);
	      balloon_colors[player][y]++;
	      
	      if (balloon_colors[player][y] > 7)
		balloon_colors[player][y] = 0;
	      
	      if (y == 0)
		{
		  addscore(player, 1000);
		  lives[player]++;
#ifdef SOUND
		  playsound(SND_CHEERING);
#endif
		}
	      else if (y == 1)
		{
		  addscore(player, 250);
#ifdef SOUND
		  playsound(SND_APPLAUSE);
#endif
		}
	      else if (y == 2)
		{
		  addscore(player, 100);
#ifdef SOUND
		  playsound(SND_APPLAUSE);
#endif
		}
	    }
	}
        
      /* Handle flying clown: */
      
      if (flying_active)
	{
	  /* Move the clown: */
	  
	  flying_x = flying_x + flying_xm;
	  flying_y = flying_y + flying_ym;
	  
	  
	  /* Bounce off top: */
	  
	  if (flying_y < 32)
	    {
	      flying_y = 32;
	      flying_ym = 0;
	    }
	  
	  
	  /* Bounce off bouncers: */
	  
	  if (flying_y > 416 && (flying_x < 32 || flying_x > 576))
	    {
	      flying_y = 416;
	      flying_ym = -abs(flying_ym);
	      
	      
	      /* Make bouncer squish and launch clown towards center: */
	      
	      if (flying_x < 32)
		{
		  game_bouncers[0] = BOUNCER_TIME;
		  flying_xm = 8;
		}
	      else
		{
		  game_bouncers[1] = BOUNCER_TIME;
		  flying_xm = -8;
		}
	      
	      
	      /* Give a point for bouncing: */
	      
	      addscore(player, 1);
	      
	      
	      /* Play bouncer sound: */
#ifdef SOUND      
	      playsound(SND_BOUNCE);
#endif
	    }
	  /* Bounce off barriers: */
	  
	  if (barriers)
	    {
	      for (i = 0; i < NUM_BARRIERS; i++)
		{
		  if (flying_y >= (NUM_ROWS * 32) &&
		      flying_y <= (NUM_ROWS * 32) + 64 &&
		      flying_x >= game_barrier_x[i] - 32 &&
		      flying_x <= game_barrier_x[i] + 64)
		    {
		      if (flying_y <= (NUM_ROWS * 32) + 32)
			{
			  flying_y = NUM_ROWS * 32;
			  flying_ym = -abs(flying_ym);
			}
		      else
			{
			  flying_y = (NUM_ROWS * 32) + 64;
			  flying_ym = abs(flying_ym);
			}
#ifdef SOUND
		      playsound(SND_BOUNCE);
#endif
		    }
		}
	    }
	  
	  
	  /* Bounce off teeter-totter or splat: */
	  
	  if (flying_y > 448)
	    {
	      flying_y = 448;
	      
	      
	      /* Did we hit the teeter-totter? */
	      
	      if ((game_teeter_side == RIGHT &&
		   (flying_x >= game_teeter_x && flying_x <= game_teeter_x + 96)) ||
		  (game_teeter_side == LEFT &&
		   (flying_x >= game_teeter_x - 32 && flying_x <= game_teeter_x + 64)))
		{
		  /* Yes!  Bounce other the clown: */
		  
		  flying_ym = -(abs(flying_x - (game_teeter_x + 48 - 16)) / 3) - 16;
		  flying_y = 432;
		  
		  
		  if (game_teeter_side == LEFT)
		    {
		      flying_x = game_teeter_x + 64;
		      flying_dir = LEFT;
		      game_teeter_side = RIGHT;
		    }
		  else
		    {
		      flying_x = game_teeter_x;
		      flying_dir = RIGHT;
		      game_teeter_side = LEFT;
		    }
		  
		  
		  /* Randomly pick a X direction: */
		  
		  if ((getRand() % 2) == 0)
		    {
		      if (flying_xm != 0)
			flying_xm = 0;
		      else
			flying_xm = -4;
		    }
		  
		  
		  /* Randomly change X direction (sign): */
		  
		  if ((getRand() % 2) == 0)
		    {
		      flying_xm = -flying_xm;
		    }
		  
		  
		  /* Give a point for bouncing: */
		  
		  addscore(player, 1);
		  
		  
		  /* Play teeter-totter bounce sound: */
		  
#ifdef SOUND
		  playsound(SND_TEETER1 + game_teeter_sound);
#endif
		  game_teeter_sound = !game_teeter_sound;
		}
	      else
		{
		  /* No!  Splat the flying clown! */
		  
		  flying_active = 0;
		  flying_splat = FLYING_SPLAT_TIME;
		  lives[player]--;
#ifdef SOUND
		  playsound(SND_SPLAT);
#endif
		}
	    }
	  
	  
	  /* Bounce off balloons: */
	  
	  x = (flying_x + 16) / 32;
	  y = (flying_y / 32) - 1;

	  if ((frame / 2) % 2)
	    {
	      if ((y % 2) == 0)
		x = (flying_x / 32);
	      else
		x = (flying_x / 32) + 1;
	    }
          
	  if (y >= 0 && y < NUM_ROWS)
	    {
	      if (balloons[player][y][x] == NORMAL)
		{
		  balloons[player][y][x] = POPPING1;
#ifdef SOUND
		  playsound(SND_POP);
#endif  
		  addscore(player, y + 1);
                  		  
		  /* Bounce horizontally: */
		  
		  if ((flying_x % 32) < 16)
		    flying_xm = -4;
		  else
		    flying_xm = 4;
		  
		  
		  /* Bounce vertically: */
		  
		  if (bouncy == 1)
		    {
		      flying_ym = -flying_ym;
		      
		      if (flying_ym > FASTEST_YM_OFF_BALLOON)
			flying_ym = FASTEST_YM_OFF_BALLOON;
		    }
		}
	    }
	  
	  
	  /* Bounce off sides: */
	  
	  if (flying_x < 0)
	    {
	      flying_x = 0;
	      flying_xm = abs(flying_xm);
	    }
	  else if (flying_x > 608)
	    {
	      flying_x = 608;
	      flying_xm = -abs(flying_xm);
	    }
	  
	  
	  /* Deal with gravity: */
	  
	  flying_ym = flying_ym + GRAVITY;
	  
	  if (flying_ym > MAX_YM)
	    flying_ym = MAX_YM;
	  if (flying_ym < -MAX_YM)
	    flying_ym = -MAX_YM;
	}
}


// GAME............................


static inline void game_game(void)
{
  int i, x, y;
  int frame=0, background_frame=0, player=0, show_player=SHOW_PLAYER_TIME;


  game_act=getRand() % NUM_ACTS;
  game_act_x=0;
  game_act_y=0;
  game_act_xm=0;
  game_act_ym=0;
  
  
  /* --- MAIN GAME LOOP --- */
  
  do
    {
      frame++;
      // Animate background

      if ((frame % 5) == 0 )
	  background_frame = (background_frame + 1) % 2;
      
      if (if_frame)
         SDL_BlitSurface(images[IMG_BACKGROUND_0 + background_frame], NULL, screen, NULL);
            
      // Keep track of old teeter-totter position
      
      old_game_teeter_x = game_teeter_x;   
      
      // Move teeter totter (digital joystick code)
      
      game_teeter_x = game_teeter_x + game_teeter_xm;
      game_teeter_xm = game_teeter_xm + game_teeter_xmm;
      
      if (game_teeter_xm > 32)
	game_teeter_xm = 32;
      else if (game_teeter_xm < -32)
	game_teeter_xm = -32;
      
      if (game_teeter_x < 32)
	game_teeter_x = 32;
      else if (game_teeter_x > 512)
	game_teeter_x = 512;

      game_event(show_player); 
      game_handle(player,frame);
      
      /* Count splats down: */
      
      if (flying_splat)
	{
	  flying_splat--;
	  
	  
	  /* If out of clowns, show game over while clown is splat: */
	  
	  if (lives[player] == 0)
	    {
	      if (num_players == 1)
		{
		  drawfuzz(176, 224, 288, 32);
		  drawtext(176, 224, "GAME OVER");
		}
	      else if (num_players == 2)
		{
		  drawfuzz(176, 192, 288, 96);
		  drawtext(224, 192, "PLAYER");
		  drawnumber(304, 224, player + 1, IMG_NUMBERS_0 + player);
		  drawtext(176, 256, "GAME OVER");
		}
	    }
	  
	  
	  if (flying_splat == 0)
	    {
	      /* Switch players: */
	      
	      if (num_players == 2)
		{
		  /* Copy balloons if in coop mode: */
		  
		  if (coop)
		    {
		      for (y = 0; y < NUM_ROWS; y++)
			{
			  for (x = 0; x < 20; x++)
			    {
			      balloons[1 - player][y][x] =
				balloons[player][y][x];
			    }
			}
		    }
		  
		  
		  /* Swap player: */
		  
		  player = 1 - player;
		  
		  if (lives[player] == 0)
		      player = 1 - player;
		}
              	      
	      /* Show which player is playing now: */

	      show_player = SHOW_PLAYER_TIME;


	      /* No more lives? */
	      
	      if (lives[player] == 0)
		{
		  game_done = 1;
		}
	    }
	}
      
      /* Change limb positions: */
	  
      if (flying_active || flying_splat)
	{
	  if ((frame % LIMB_ANIMATION_TIME) == 0)
	    {
	      flying_left_arm = getRand() % 3;
	      flying_right_arm = getRand() % 3;
	      flying_left_leg = getRand() % 2;
	      flying_right_leg = getRand() % 2;
	    }
	}
      /* Draw act: */
      
	  if (game_act == ACT_SEAL)
	    {
	      game_act_y = game_act_y + game_act_ym;
	      game_act_ym++;
	      
	      if (game_act_y >= 315)
		{
		  game_act_y = 315;
		  game_act_ym = -10;
		}
              if (if_frame)
              {
	             draw(148, game_act_y, IMG_BEACHBALL_0 + ((frame / 4) % 3));
                     draw(152, 347, IMG_SEAL_0 + ((frame / 4) % 2));
              }
	    }
	  else if (game_act == ACT_BEAR)
	    {
	      game_act_x = game_act_x + game_act_xm;
	      if (game_act_x <= 64)
		{
		  game_act_x = 64;
		  game_act_xm = 4;
		}
	      else if (game_act_x >= 524)
		{
		  game_act_x = 524;
		  game_act_xm = -4;
		}
	      if (if_frame)
              {
                 if (game_act_xm > 0)
                    draw(game_act_x, 340, IMG_BEAR_RIGHT_0 + ((frame / 4) % 2));
                 else
                    draw(game_act_x, 340, IMG_BEAR_LEFT_0 + ((frame / 4) % 2));
              }
	    }
      /* Draw balloons: */
      if (if_frame)
         for (y = 0; y < NUM_ROWS; y++)
         {
            for (x = 0; x < 20; x++)
	    {
	      drawballoon(player, x, y, ((frame / 2) % 2));
	    }
         }      
      
      /* Draw barriers: */
      
      if ((barriers)&&(if_frame))
	{
	  for (i = 0; i < 3; i++)
	    draw(game_barrier_x[i], (NUM_ROWS * 32) + 32, IMG_BARRIER);
      	}
      
      
      /* Draw teeter-totter: */
      
      if (if_frame)
      {
         if (game_teeter_side == LEFT)
            	draw(game_teeter_x, 448, IMG_TEETER_TOTTER_LEFT_0 + game_teeter_roll);
         else if (game_teeter_side == RIGHT)
               	draw(game_teeter_x, 448, IMG_TEETER_TOTTER_RIGHT_0 + game_teeter_roll);
      /* Draw clown on teeter-totter: */
      
         drawclown(game_teeter_x + 64 - (game_teeter_side * 64), 444, game_teeter_side,
		1, 1, 1, 1);
                
      /* Draw flying clown: */
      
         if (flying_active)
	 {
	  drawclown(flying_x, flying_y, flying_dir,
		    flying_left_arm, flying_right_arm,
		    flying_left_leg, flying_right_leg);	  
	}
        
      /* Draw splat clown: */
      
         if (flying_splat)
	 {
	  drawclown(flying_x, flying_y, 2,
		    -1, -1,
		    flying_left_leg + 2, flying_right_leg + 2);
	  
	 }
      
      /* Draw bouncers: */
      
         for (i = 0; i < 2; i++)
	 {
	  if (game_bouncers[i] == 0)
	    draw(608 * i, 448, IMG_BOUNCER_0);
	  else
	    draw(608 * i, 448, IMG_BOUNCER_1);
	  
	 }
      

      /* Draw lives status: */
      
         drawfuzz(512, 0, 128, 32);
         
         draw(512,0, IMG_CLOWN_HEAD + ( (frame/20 ) %2) );
               
         draw(544, 0, IMG_TIMES);
         
         drawnumber(576, 0, lives[player], IMG_NUMBERS_0 + player);
         /* Draw score status: */

         drawfuzz(0, 0, 192, 32);
         drawnumber(0, 0, score[player], IMG_NUMBERS_0 + player);
                  
      /* Draw "Player X Ready" message: */
      
         if (show_player > 0 && lives[player] > 0)
	 {
	  if (num_players == 1)
	    {
	      drawfuzz(240, 224, 160, 32);
	      drawtext(240, 224, "READY");
	    }
	  else if (num_players == 2)
	    {
	      drawfuzz(224, 192, 192, 96);
	      drawtext(224, 192, "PLAYER");
	      drawnumber(304, 224, player + 1, IMG_NUMBERS_0 + player);
	      drawtext(240, 256, "READY");
	    }

	  
	  /* Count timer down: */
	  
	  show_player--;
          }
      }
      
      
      /* Update the screen: */

      if (if_frame)
         SDL_Flip(screen);

      if_frame=wait_fps();

      
      /* Keep playing music: */
      
#ifdef MUSIC
	  if (!Mix_PlayingMusic())
	    {
	      Mix_PlayMusic(mus_game, 0);
	      Mix_VolumeMusic((music_vol * MIX_MAX_VOLUME) / 3);
	    }
#endif
    }
  while (game_done == 0 && game_quit == 0);
}  


/* Pause screen: */

int pausescreen(void)
{
  SDL_Event event;
  SDLKey key;
  int frame=0, background_frame=0;
  int game_done, game_quit;
  
  /* Stop music: */
  
#ifdef SOUND
      Mix_PauseMusic();
#endif

  clear_events();
  SDL_Delay(100);
  clear_events();


  /* Wait for keypress: */
  
  game_done = 0;
  game_quit = 0;
  
  do
    {
      if ((frame % 5) == 0 )
            background_frame = (background_frame + 1) % 2;
      if (if_frame)
       {
            SDL_BlitSurface(images[IMG_BACKGROUND_0 + background_frame], NULL, screen, NULL);
              /* Display "PAUSED" Message: */
            drawfuzz(224, 224, 192, 32);
            drawtext(224, 224, "PAUSED");
            SDL_Flip(screen);
       }
      while (SDL_PollEvent(&event))
	{
           frame++;
           
        // Animate background
        

	  if (event.type == SDL_QUIT)
	    {
	      /* Quit event!  */
	      
	      game_quit = 2;
	    }
	  else if (event.type == SDL_KEYDOWN)
	    {
	      /* A keypress! */

              key = event.key.keysym.sym;

	      if (key == SDLK_SPACE || key == SDLK_TAB ||
		  key == SDLK_p)
		{
		  /* SPACE, TAB or P: Pause! */
		  
		  game_done = 1;
		}
	      else if (key == SDLK_ESCAPE)
		{
		  /* ESCAPE: Quit! */
		  
		  game_quit = 1;
		}
	    }
            else if (event.type == SDL_JOYBUTTONDOWN)
            {
               game_done=1;
            }
	}
        SDL_Delay(10);
        if_frame=wait_fps();
    }
  while (game_quit == 0 && game_done == 0);
  
  /* Unpause music: */
  
#ifdef SOUND
      Mix_ResumeMusic();
#endif

  return(game_quit);
}


int game(void)
{
   game_init();
   game_game();
   return gameover();
}
