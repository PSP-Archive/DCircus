#include "dcircus.h"

int highscorescreen(void)
{
  SDL_Event event;
  int done, quit, i, y=(images[IMG_HIGHSCORE_TOP]->h)*DIVISOR;
  
  // Clear screen
  
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
  
  draw(0, 0, IMG_HIGHSCORE_TOP);
  draw(0, y, IMG_HIGHSCORE_LEFT);
  
  
  // Draw scores
  
  for (i = 0; i < 8; i++)
    {
      // Score
      
      drawnumber(32, y + (i * 32) + 16,
		 highscore[i], IMG_NUMBERS_0 + (i % 2));
      
      
      // Initials
      
      drawtext(224, y + (i * 32) + 16,
	       highscorer[i]);
      
      
      // Modes

      // (Barriers)
      
      if (i >= 4)
	{
	  draw(336, y + (i * 32) + 24,
	       IMG_LIGHT_ON);
	}
      else
	{
	  draw(336, y + (i * 32) + 24,
	       IMG_LIGHT_OFF);
	}
      
      
      // (Bouncy Balloons)
      
      if (i == 2 || i == 3 || i == 6 || i == 7)
	{
	  draw(444, y + (i * 32) + 24,
	       IMG_LIGHT_ON);
	}
      else
	{
	  draw(444, y + (i * 32) + 24,
	       IMG_LIGHT_OFF);
	}


      // (Clear All)
      
      if ((i % 2) == 1)
	{
	  draw(564, y + (i * 32) + 24,
	       IMG_LIGHT_ON);
	}
      else
	{
	  draw(564, y + (i * 32) + 24,
	       IMG_LIGHT_OFF);
	}
    }
 
  SDL_Flip(screen);  
  while (SDL_PollEvent(&event))
     SDL_Delay(50);

  // Wait for input
  
  done = 0;
  quit = 0;
  
  do
    {
      while (SDL_PollEvent(&event))
	{
          switch (event.type)
          {
             case SDL_QUIT:
                	quit = 1;
                        break;
             case SDL_KEYDOWN:
             case SDL_JOYHATMOTION:
             case SDL_MOUSEBUTTONDOWN:
             case SDL_JOYBUTTONDOWN:
                	done = 1;
                        break;
          }
	}
      
      SDL_Delay(30);
      
      
      // Keep playing music
      
#ifdef MUSIC
	  if (!Mix_PlayingMusic())
	    {
	      Mix_PlayMusic(mus_hiscreen, 0);
	      Mix_VolumeMusic((music_vol * MIX_MAX_VOLUME) / 3);
	    }
#endif

    }
  while (!done && !quit);


  // Stop music
  
#ifdef SOUND
    Mix_HaltMusic();
#endif

  return(quit);
}


void write_score(void)
{
#ifdef DREAMCAST
	vmu_pkg_t       pkg;
	uint8           *pkg_out;
	int             pkg_size;
	int file_size=0;
	uint8 *data;
	file_t fd;

	fd=fs_open("/ram/tempfile",O_WRONLY);
	fs_write(fd,&highscore,32);
	fs_write(fd,&highscorer,32);
	fs_close(fd);

	fd=fs_open("/ram/tempfile",O_RDONLY);
	file_size=fs_total(fd);
	data=malloc(file_size);
	fs_read(fd,data,file_size);
	fs_close(fd);

	strcpy(pkg.desc_short, "DCIRCUS");
	strcpy(pkg.desc_long, "High Score");
	strcpy(pkg.app_id, "KOS");
	pkg.icon_cnt = 0;
	pkg.icon_anim_speed = 0;
	pkg.eyecatch_type = VMUPKG_EC_NONE;
	pkg.data_len = file_size;
	pkg.data = data;
	vmu_pkg_build(&pkg, &pkg_out, &pkg_size);

	fs_unlink(SCORE_FILE);
	fd = fs_open(SCORE_FILE, O_WRONLY);
	if (fd)
	{
		fs_write(fd,pkg_out,pkg_size);
		fs_close(fd);
	}
	free(pkg_out);
	free(data);
#else
	FILE *f;
	f=fopen(SCORE_FILE,"w");
	if (f!=NULL)
	{
		fwrite((void*)&highscore,8,sizeof(int),f);
		fwrite((void*)&highscorer,8,sizeof(char[4]),f);
		fclose(f);
	}
#endif
}

void read_score(void)
{
	FILE *f=fopen(SCORE_FILE,"r");
	if (f!=NULL)
	{
#ifdef DREAMCAST
		fseek(f,128,SEEK_SET);
#endif
		fread((void*)&highscore,8,sizeof(int),f);
		fread((void*)&highscorer,8,sizeof(char[4]),f);
		fclose(f);
	}
}
