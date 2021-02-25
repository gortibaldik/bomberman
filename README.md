# Network Bomberman
- just an usual bomberman game playable both in single player and in multiplayer mode
- uses udp sockets for networking, with application specific heartbeat protocol
![bombermanGame](media/bomberman.png)

# Compilation guide

## Dependencies
- `SFML` library, version `2.5.1`

- download `SFML` library, version `2.5.1`
    - __possibility 1:__ install it system-wide and add relevant binaries to the `PATH` variable
    - __possibility 2:__ download non-OS-specific sources from [the official web site](https://www.sfml-dev.org/files/SFML-2.5.1-sources.zip) and extract it in the same directory as `main.cpp`, then build it using the same configuration as the sources for this project
        
## Linux
- quick guide to run the executable
```shell
mkdir build
cd build
cmake ..
./Bomberman
```

## Windows
- run _visual studio 2019_ on folder with the sources using _open folder_
- everything should be configured in `.vs/launch.vs.json` and `CMakeLists.txt` and only running `Bomberman.exe` should suffice

### Notice
- developed on linux, tested both on windows and linux

# Acknowledgements
- [bomb party art](https://opengameart.org/content/bomb-party-the-complete-set)
- [small pixel hearts](https://opengameart.org/content/simple-small-pixel-hearts)
- [FFFTusj font](https://www.fontsquirrel.com/fonts/fff-tusj)
- [Raleway font](https://www.fontsquirrel.com/fonts/raleway)
- my girlfriend for all the power-ups drawings
- [Game development by example](https://www.packtpub.com/product/sfml-game-development-by-example/9781785287343)

