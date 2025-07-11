# SpotifyPauser
Lightweight C++ tool that utilizes xdotool and wmctrl to quickly pause spotify when cursor is swiped to corner of screen.


https://github.com/user-attachments/assets/1afc6bb7-cb4e-4ce3-acff-9c6a1cbba294



# How to Install
1. Clone repository.
2. Configure any desired settings (polling rate, desired swipe corner).
3. 
   ```bash
   chmod +x install.sh
   ./install.sh
   ```
4. Reboot computer.
5. Done!

# How to Uninstall
1. Navigate to the program's folder (/spotifypauser).
2. Run ./install.sh --uninstall
3. Reboot computer.
4. Done!.
   
# Customizability
 1. You can choose the corner that will run the pause command. (located in Main() {}) 
   ![image](https://github.com/user-attachments/assets/773d2083-578a-4e03-bc6f-51134007a9a0)

 2. You can also change the rate that the program checks whether cursor is in desired corner (can help reduce resources but current rate is extremely light weight + responsive).
   ![image](https://github.com/user-attachments/assets/075c7d1b-c91d-4ed3-bf36-eb661120a50f)

# Requirements
C++
xdotool
x11
wmctrl

# Compatibility
Compatible with Linux and Windows (see windows-env branch).

# Future
A simple GUI will be available in the future to help configure settings.

Install setup will be automated for any system.

