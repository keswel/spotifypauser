#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <chrono>
#include <thread>


// default polling rate: 50000 microseconds | eq. 50ms.
int polling_rate = 50000;
    
// location that you want to hover your cursor over to pause spotify
// Options include (bottom left selected by default):
//    top left
//    top right
//    bottom left
//    bottom right
char pause_location_option[] = "bottom left";

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

#ifdef __linux__
  // linux build
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  #include <termios.h>
  #include <unistd.h>
  
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
    system("wmctrl -x -a spotify.Spotify");
  }

  // returns to original saved window.
  void return_to_window(char str[]) {
    char command[512];

    // uses window id (from window stack) to create command
    snprintf(command, sizeof(command), "xdotool windowactivate %s", str); 
    // execute command
    system(command);
    printf("executed: xdotool windowactivate %s\n", str); 
  }

  // emulates play/pause button on keyboard.
  void pause_spotify() {
    //system("xdotool key  XF8AudioPlay");
    system("xdotool key  space");
  }


  int main() {

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
          jump_to_spotify();
          strcpy(get_mode, "class"); // check class of current window.
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
#elif defined(_WIN32)


  // windows build!
  #include <Windows.h>
  #include <TlHelp32.h>
  #include <string>
  #include <algorithm>
  #include <vector>
  #include <audiopolicy.h>

  // stores spotify data.
  struct SpotifyProcess{
    std::string name;
    DWORD pid;

    // constructor
    SpotifyProcess(const std::string& n, DWORD p) : name(n), pid(p) {}
  };

  HWND findMainSpotifyWindow(std::vector<SpotifyProcess> snapshots);
  void pauseSpotify(const std::vector<SpotifyProcess> &snapshots);

  // context struct for EnumWindows callback
  struct WindowMatchContext {
    DWORD pid;
    HWND hwnd = nullptr;
    std::string title;
  };

  // prints out all processes under "Spotify"
  void printSnapshotInfo(const std::vector<SpotifyProcess> snapshots)  {
    for (int i=0; i<snapshots.size(); i++) {
      std::cout << "Process #" << i << std::endl;
      std::cout << (snapshots[i]).name << " (PID: " << (snapshots[i]).pid << ")" << std::endl;
    }
  } 

  // grabs all processes under "Spotify"
  std::vector<SpotifyProcess> grabSpotifySnapshot() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create process snapshot.\n";
        return std::vector<SpotifyProcess>();
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // check if Process32First fails
    if (!Process32First(hSnapshot, &pe32)) {
        std::cerr << "Failed to get first process.\n";
        CloseHandle(hSnapshot);
        return std::vector<SpotifyProcess>();
    }

    std::vector<SpotifyProcess> spotifySnapshots;

    do {
        std::string exeName = pe32.szExeFile;
        std::string lowerName = exeName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (lowerName.find("spotify") != std::string::npos) {
            // append to vector
            spotifySnapshots.push_back(SpotifyProcess(exeName, pe32.th32ProcessID));

            // print out process info
            std::cout << "Found process: " << (spotifySnapshots.back()).name
                      << " (PID: " << (spotifySnapshots.back()).pid << ")\n";
        }
    } while (Process32Next(hSnapshot, &pe32));

    HWND hwnd = findMainSpotifyWindow(spotifySnapshots);
    if (hwnd) {
        std::cout << "Found main Spotify window: " << hwnd << std::endl;
    } else {
        std::cout << "No active Spotify window found." << std::endl;
    }

    CloseHandle(hSnapshot);
    return spotifySnapshots;
  }

  // checks if window belongs to a target PID and has a meaningful title
  BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
      WindowMatchContext* context = reinterpret_cast<WindowMatchContext*>(lParam);
      DWORD windowPID = 0;
      GetWindowThreadProcessId(hwnd, &windowPID);

      if (windowPID == context->pid && IsWindowVisible(hwnd)) {
          TCHAR buffer[256];
          GetWindowText(hwnd, buffer, sizeof(buffer) / sizeof(TCHAR));
          std::string title = std::string(buffer, strlen(buffer));

          if (!title.empty() && title != "Spotify") {
              context->hwnd = hwnd;
              context->title = title;
              return FALSE; // Found a window we care about
          }
      }

      return TRUE; // keep looking
  }

  // finds the main Spotify window (with title not equal to "Spotify")
  HWND findMainSpotifyWindow(const std::vector<SpotifyProcess> snapshots) {
      for (const auto& proc : snapshots) {
          WindowMatchContext context;
          context.pid = proc.pid;

          EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&context));

          if (context.hwnd != nullptr) {
              std::cout << "Matched window for PID " << proc.pid
                        << " with title: " << context.title << std::endl;
              return context.hwnd;
          }
      }

      return nullptr;
  }

  void pauseSpotify(const std::vector<SpotifyProcess> &snapshots) {
    // main Spotify window that is playing music
    HWND spotifyWindow = findMainSpotifyWindow(snapshots);

    if (spotifyWindow == nullptr) {
      std::cout << "Could not find Spotify window." << std::endl;
      return;
    }

    std::cout << "Targeting main Spotify window: " << spotifyWindow << std::endl;

    // bring the main Spotify window to the foreground
    ShowWindow(spotifyWindow, SW_RESTORE);  // restore if minimized
    SetForegroundWindow(spotifyWindow);     // bring to front

    // simulate spacebar key press
    keybd_event(VK_SPACE, 0, 0, 0);                  // Key down
    keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);    // Key up
    std::cout << "Sent spacebar to main Spotify window" << std::endl;

    Sleep(200);
  }

  void switchToWindow(HWND hwnd) {
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
  }

  int main() {
    POINT cursor_pos;
    
    // finds width & height of screen for targetting
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    int target_x, target_y;

    assign_targets(width, height, pause_location_option, &target_x, &target_y); 
    // if cursor_pos is found
    if (GetCursorPos(&cursor_pos)) {
      // assumes spotify is already playing.
      bool is_spotify_paused = false;
      int time_at_corner = 0;
      std::cout << "Cursor x: " << cursor_pos.x << ", Cursor Y: " << cursor_pos.y << std::endl;
      std::cout << "Polling rate: " << polling_rate << std::endl;
      std::cout << "Screen Size: " << width << " x " << height << std::endl;

      while (true) {
        // get cursor_pos.
        GetCursorPos(&cursor_pos);

        // std::cout << "Cursor x: " << cursor_pos.x << ", Cursor Y: " << cursor_pos.y << std::endl;

        // increment time_at_corner for the amount of seconds cursor is on bottom left corner
        if ((cursor_pos.x == target_x) && (cursor_pos.y == target_y)) 
        {
          time_at_corner++;
        }else{
          time_at_corner = 0;
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
          HWND foreground_window = GetForegroundWindow();
          std::cout << "Active window: " << foreground_window << std::endl;

          // save all instances of spotify (pid & name)
          std::vector<SpotifyProcess> spotifySnapshots = grabSpotifySnapshot();

          // if vector empty continue else >>
          if (!spotifySnapshots.empty()) {
            printSnapshotInfo(spotifySnapshots);
            pauseSpotify(spotifySnapshots);

            switchToWindow(foreground_window);
            Sleep(100);
          }

          // prevents forever loop
          time_at_corner = 0;
          
          // sleep for 500ms after pausing 
          Sleep(500);
        }
      
        // sleep for polling_rate.
        Sleep(polling_rate / 1000);
      }
    
    }
    return 0;
  }
#endif


