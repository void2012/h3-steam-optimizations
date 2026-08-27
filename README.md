This is a small simple mod for the Steam release of Heroes of Might and Magic III designed to optimize the game. 
1. It uses rpmalloc instead of the original Visual C++ 6.0 heap (2-8x speedups)
2. It uses a more efficient win32 message pump mechanism (CPU usage drops from 8-100% to almost zero on modern systems)

Installation instruction:

1. Legally purchase the game via Steam
2. Copy `heroes3.exe` (and replace the original `heroes3.exe`) and `h3-steam-optimizations.dll` from Releases to your game installation (Usually it's `C:\Program Files (x86)\Steam\steamapps\common\Heroes Of Might And Magic III`);
3. Run the game
4. Enjoy the game!
