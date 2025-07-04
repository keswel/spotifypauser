#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>


char* get_focused_window(char mode[]) {
  static char window_id[64]; 
  FILE* fp; 

  if (strcmp(mode, "window") == 0) {
    fp = popen("xdotool getactivewindow", "r");
  }else if (strcmp(mode, "class") == 0){
    
   // fp = popen("xdotool getactivewindow", "r");
    fp = popen("xdotool getwindowclassname $(xdotool getactivewindow)", "r");
  }else{
    printf("ERROR: mode not valid! (please use window or class)");
    pclose(fp);
    return NULL;
  }

  if (fp == NULL) {
    perror("ERROR: popen failed");
    return NULL;
  }

  if (fgets(window_id, sizeof(window_id), fp) == NULL) {
    perror("ERROR: fgets failed");
    pclose(fp);
    return NULL;
  }

  pclose(fp);

  // remove trailing newline, if any
  size_t len = strlen(window_id);
  if (len > 0 && window_id[len - 1] == '\n') {
    window_id[len - 1] = '\0';
  }
  return window_id;
}

// jumps to spotify wherever it is on your WM
void jump_to_spotify() {
  // TODO: use xdotool instead of wmctrl to reduce dependencies.
  system("wmctrl -x -a spotify.Spotify");
}

// returns to orgininal saved window.
void return_to_window(char str[]) {
  char command[512];

  // uses window id (from window stack) to create command
  snprintf(command, sizeof(command), "xdotool windowactivate %s", str); 
  // execute commmand
  system(command);
  printf("executed: xdotool windowactivate %s\n", str); 
}

// emulates play/pause button on keyboard.
void pause_spotify() {
  //system("xdotool key  XF8AudioPlay");
  system("xdotool key  space");
}

void assign_targets(int width, int height, char pause_location_option[], int *target_x, int *target_y) {
  // tells the program at what x & y it will pause spotify.
  if (strcmp(pause_location_option, "top left") == 0) 
  {
    *target_x = 0;
    *target_y = 0;
  } 
  else if (strcmp(pause_location_option, "top right") == 0)
  {
    *target_x = width - 1;
    *target_y = 0;
  }
  else if (strcmp(pause_location_option, "bottom left") == 0)
  {
    *target_x = 0;
    *target_y = height - 1;
  }
  else if (strcmp(pause_location_option, "bottom right") == 0)
  {
    *target_x = width - 1;
    *target_y = height - 1;
  }
  printf("\ntarget_x = %d\ntarget_y = %d\n", *target_x, *target_y);
}

int main() {
  // default polling rate: 50000 microseconds | eq. 50ms.
  int polling_rate = 50000;
  
  // location that you want to hover your cursor over to pause spotify
  // Options include (bottom left selected by default):
  //    top left
  //    top right
  //    bottom left
  //    bottom right
  char pause_location_option[] = "bottom left";

  char get_mode[8];

  // opens display
  Display* display = XOpenDisplay(nullptr);
  char *focused_window;

  Screen *screen = DefaultScreenOfDisplay(display);
  
  // if no display
  if (!display) return -1;
  
  Window root = DefaultRootWindow(display);
  Window returned_root, returned_child;
  
  // used for mouse tracking
  int root_x, root_y;
  int win_x, win_y;
  
  // finds width & height of screen for targetting
  int width = screen->width;
  int height = screen->height;
  int target_x, target_y;

  assign_targets(width, height, pause_location_option, &target_x, &target_y); 
  
  unsigned int mask;
  int time_at_corner = 0;
  
  // assumes spotify is already playing.
  bool is_spotify_paused = false; 

  while (true) 
  {
    if (!display) 
    {
      printf("ERROR: display is not found [null]!\n"); 
    }
    
    if (XQueryPointer(display, root, &returned_root, &returned_child, &root_x, &root_y, &win_x, &win_y, &mask)) 
    {
      // printf("mouse position: (%d, %d)\n", root_x, root_y);
     
      // increment time_at_corner for the amount of seconds cursor is on bottom left corner
      if ((root_x == target_x) && (root_y == target_y)) 
      {
        time_at_corner++;
      }else{
        time_at_corner == 0;
      }
      
      if (time_at_corner >= 2) 
        {
        // allows program to know when spotify is paused.
        is_spotify_paused = !is_spotify_paused;      
        
        // logs corresponding action 
        if (is_spotify_paused) {
          printf("pausing spotify! :0\n");
        }else{
          printf("unpausing spotify! :)\n");
        }

        // saves window to go back to after jumping
        strcpy(get_mode, "window");
        focused_window = strdup(get_focused_window(get_mode));
        printf("focused_window: %s\n", focused_window);

        // jump to spotify window 
        printf("jumping to spotify\n");
        jump_to_spotify();
        strcpy(get_mode, "class"); // check class of current window.
        usleep(90000);
        if (strcmp(get_focused_window(get_mode), "Spotify") != 0) {
          printf("ERROR: Spotify not found (may not be running).\n");
          free(focused_window);
          strcpy(get_mode, "NULL");
          time_at_corner = 0;
          continue;
        }

        // function that unpauses and pauses spotify.
        pause_spotify();

        // returns to orignial window
        printf("returning to: %s\n", focused_window);
        return_to_window(focused_window);

        // prevents forever loop
        time_at_corner = 0;
        
        // sleep for 500ms after pausing 
        usleep(500000);
        free(focused_window);
      }
    
      // sleep for polling_rate.
      usleep(polling_rate);
    }
  }
  
  // close display connection
  XCloseDisplay(display);
  
  return 0;
} 
