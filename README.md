# SpotifyPauser
Simple and lightweight C++ tool that utilizes xdotool to pause spotify by swiping the cursor in corner of screen.

<<<<<<< HEAD
# how to install
1. clone repository.
2. navigate to the cloned files (cd).
3. run chmod to make installer executable.
=======
# How to Install
1. clone repository
2. configure any desired settings (polling rate, desired swipe corner)
3. compile c++ file using 
>>>>>>> 42a893c (cleaned up code a bit & updated readme)
   ```
   chmod +x install.sh
4. run ./install.sh to install
   ```
   ./install.sh
6. reboot computer
7. done!

<<<<<<< HEAD
# how to uninstall
1. navigate to the program's folder (/spotifypauser)
2. run ./install.sh --uninstall
3. reboot computer
4. done!
   
# customizability
You can choose the corner that will run the pause command. (located in Main() {}) 
=======
# Customizability
 1. You can choose the corner that will run the pause command. (located in Main() {}) 
>>>>>>> 42a893c (cleaned up code a bit & updated readme)

   ![image](https://github.com/user-attachments/assets/773d2083-578a-4e03-bc6f-51134007a9a0)

 2.You can also change the rate that the program checks whether cursor is in desired corner (can help reduce resources but current rate is extremely light weight + responsive).

   ![image](https://github.com/user-attachments/assets/075c7d1b-c91d-4ed3-bf36-eb661120a50f)

# Requirements
C++
xdotool
x11
wmctrl

# Compatibility
Currently only Linux is supported. BUT, Windows functionality will be implemented soon.

# Future
A simple GUI will be available in the future to help configure settings.
Install setup will be automated for any system.
