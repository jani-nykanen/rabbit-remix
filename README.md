![](https://img.itch.zone/aW1nLzIzNTAwOTQuZ2lm/original/x5u%2FUs.gif)

[Play on itch.io] (https://jani-nykanen.itch.io/rabbit-remix?secret=9XMsmLw0pr3Oat1uqNEdzNIV2gw)

------

## Building

On Linux, write
```
make install all DEFINES=-DKEY=insert_key_here
```
The DEFINES=... part is not necessary, you wouldn't know the secret key required to "authenticate" scores sent to the server anyway.

On Windows... do not even try. Yet. The Windows binary was built using `makefile_win32` included in the repo. (TODO: merge this makefile with the linux makefile). If you have mingw-w64 cross compiler installed on Linux, rename the file `makefile` and type the command mentioned above. Something might happend


-------

(c) 2019 Jani Nykänen
