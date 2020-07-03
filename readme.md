# Charon

This is an experiment and utility for Diablo 2. It is intended for single player and TCP/IP game use, so connections to Battle.net are disabled. It's main features include:

    Map Reveal
    Display Monsters, Missiles, and Items (superior and above) on Automap
    /players X uncap (16-20 is reasonable, but very difficult)
    Improved experience scaling for /players X by typing: /xp
    Weather toggle (press insert key)
    Always in Direct 3D Mode
    Loading additional mpq via command line parameter: -mpq "filename.mpq"

A note about 'Always in Direct 3D Mode':
    This feature causes the game to ignore command line flags that control the video, such as `-w`. The game now starts windowed every time, and you can toggle full screen mode on and off by holding Alt and pressing Enter (Alt+Enter). We're currently trying to understand how the game uses the rendering API, and we'll be looking to update the renderer to newer APIs, since the game currently uses DirectDraw v7 and Direct3D v3.

To use Charon you'll need to compile it with Visual Studio 2019, or grab the most recent version from the releases tab. Once you've compiled or downloaded Charon you must load Charon.dll into the game using [DLL Loader](https://github.com/Nishimura-Katsuo/DLLLoader). Loading it with traditional injection techniques may work, but will most likely crash or not function properly, since this is written to operate completely on Diablo 2's main thread for stability.

A lot of hard work is being put in by us at [Blizzhackers](https://github.com/blizzhackers) to bring you this tool with love.

>I see him there at the oars of his little boat in the lake, the ferryman of the dead, Charon, with his hand upon the oar and he calls me now. ~Alcestis (from Alcestis by Euripides)
