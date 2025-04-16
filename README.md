# spotifypauser
Simple and lightweight C++ tool that utilizes xdotool to pause spotify by swiping the cursor in corner of screen.

# how to install
1. clone repository
2. configure any desired settings (polling rate, desired swipe corner)
3. compile c++ file using 
   ```
   g++ spotify-pauser.cpp -o spotify-pauser -lX11
4. 

# how to install
1. clone repository.
2. run the install.sh file (coming soon!)
3. restart computer.

# customizability
You can choose the corner that will run the pause command. (located in Main() {}) 

   ![image](https://github.com/user-attachments/assets/773d2083-578a-4e03-bc6f-51134007a9a0)

You can also change the rate that the program checks whether cursor is in desired corner (can help reduce resources but current rate is extremely light weight + responsive).

   ![image](https://github.com/user-attachments/assets/075c7d1b-c91d-4ed3-bf36-eb661120a50f)

# requirements
C++
xdotool
x11

# compatibility
Currently only Linux is supported. BUT, Windows functionality will be implemented soon.

# future
A simple GUI will be available in the future to help configure settings.
Install setup will be automated for any system.
