#!/bin/bash

INSTALL_DIR="$HOME/.local/bin"
AUTOSTART_FILE="$HOME/.config/autostart/spotify-pauser.desktop"

# === UNINSTALL FLAG ===
# ./install.sh --uninstall
if [[ "$1" == "--uninstall" ]]; then
    echo "[*] Uninstalling Spotify Pauser..."

    # Kill running spotify-pauser if it's active
    if pgrep -x "spotify-pauser" > /dev/null; then
        echo "[*] Stopping running instance..."
        pkill -x "spotify-pauser"
    else
        echo "[*] No running instance found."
    fi

    # Remove installed binary and autostart entry
    rm -f "$INSTALL_DIR/spotify-pauser"
    rm -f "$AUTOSTART_FILE"

    echo "[✓] Uninstalled."
    exit 0
fi

# === CONFIGURABLE OPTIONS ===
DEFAULT_POLLING_RATE=100     # in milliseconds
DEFAULT_SWIPE_CORNER=0       # 0 = top-left, 1 = top-right, etc.

# === Step 1: Install dependencies ===
echo "[+] Installing dependencies..."
sudo pacman -S --noconfirm base-devel libx11

# === Step 2: Clone the repo if not already in it ===
REPO_NAME="spotify-pauser"
if [ ! -f "spotify-pauser.cpp" ]; then
    echo "[+] Cloning repository..."
    git clone https://github.com/keswel/spotifypauser.git
    cd $REPO_NAME || exit 1
fi

# === Step 3: Customize settings ===
echo "[*] Setting up configuration..."
sed -i "s/^int pollingRateMs = .*;/int pollingRateMs = $DEFAULT_POLLING_RATE;/" spotify-pauser.cpp
sed -i "s/^int swipeCorner = .*;/int swipeCorner = $DEFAULT_SWIPE_CORNER;/" spotify-pauser.cpp

# === Step 4: Compile ===
echo "[+] Compiling spotify-pauser.cpp..."
g++ spotify-pauser.cpp -o spotify-pauser -lX11

# === Step 5: Install binary ===
INSTALL_DIR="$HOME/.local/bin"
mkdir -p "$INSTALL_DIR"
cp spotify-pauser "$INSTALL_DIR"

# === Step 6: Add to PATH if not already ===
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
    echo "export PATH=\$PATH:$INSTALL_DIR" >> ~/.bashrc
    echo "[+] Added $INSTALL_DIR to PATH in .bashrc"
fi

# === Step 7: Create autostart entry ===
AUTOSTART_DIR="$HOME/.config/autostart"
mkdir -p "$AUTOSTART_DIR"

cat > "$AUTOSTART_DIR/spotify-pauser.desktop" <<EOL
[Desktop Entry]
Type=Application
Exec=$INSTALL_DIR/spotify-pauser
Hidden=false
NoDisplay=false
X-GNOME-Autostart-enabled=true
Name=Spotify Pauser
Comment=Pause Spotify on gesture
EOL

echo "[✓] Installed and added to startup!"

