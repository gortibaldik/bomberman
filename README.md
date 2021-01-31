# Installation guide

## Dependencies
- `SFML` library, version `2.5.1`
- `Boost` library, version `1.75.0`

- download `SFML` library, version `2.5.1`
    - __possibility 1:__ install it system-wide and add relevant binaries to the `PATH` variable
    - __possibility 2:__download non-OS-specific sources from [the official web site](https://www.sfml-dev.org/files/SFML-2.5.1-sources.zip) and extract it in the same directory as `main.cpp`, then build it using the same configuration as the sources for this project
    - for windows and mac this two steps suffice, for linux it is needed to install all the [dependencies of the library](https://www.sfml-dev.org/tutorials/2.5/start-linux.php)
- download `Boost` library, version `1.75.0` : [link](https://www.boost.org/users/download/)
    - __guide for linux:__
        ```shell
        # install all the dependencies of the library
        sudo apt install build-essential g++ python-dev autotools-dev libicu-dev libbz2-dev libboost-all-dev
        cd boost_1_75_0 # cd to wherever you've extracted the downloaded sources of boost
        # run the installation scripts
        ./bootstrap.sh --prefix=/usr/
        ./b2
        sudo ./b2 install
        ```
    - __guide for windows:__
        - currently, `CMakeLists.txt` are configured so that `cmake` looks to `C:/Program Files boost_1_75_0/` when trying to find `Boost` so I kindly ask you to extract the sources to `C:/Program Files/`
        - run _Visual studio 2019 developer command prompt_ __as administrator__ in the directory where you've extracted the sources
        ```shell
        bootstrap
        .\b2
        ```

        
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
