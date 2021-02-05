# Installation guide

## Dependencies
- `SFML` library, version `2.5.1`
- `Boost` library, version `1.75.0`

- download `SFML` library, version `2.5.1`
    - __possibility 1:__ install it system-wide and add relevant binaries to the `PATH` variable
    - __possibility 2:__download non-OS-specific sources from [the official web site](https://www.sfml-dev.org/files/SFML-2.5.1-sources.zip) and extract it in the same directory as `main.cpp`, then build it using the same configuration as the sources for this project
        
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

### Actual issues
- bug in client connecting - after unsuccessfull duplicate connection, another
    connection is marked as duplicate even if it isn't