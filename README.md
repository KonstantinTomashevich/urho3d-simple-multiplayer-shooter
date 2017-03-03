## Urho3D Simple Multiplayer Shooter
It's simple deathmatch shooter game on Urho3D game engine, which supports multiplayer and custom AIs.

## Features:
* Client-server multiplayer support.
* Custom scriptable AI support.
* Server leaderboard.

## Screenshots:
![1](https://s30.postimg.org/hxgjgbs8h/image.png)
![2](https://s7.postimg.org/tyiiums4r/image.png)
![3](https://s30.postimg.org/bl1e6hp69/image.png)
![4](https://s30.postimg.org/uen73hne9/image.png)
![5](https://s30.postimg.org/tqeckjooh/image.png)

## How to build
Step 1. Download Urho3D SDK binary release from [SourceForge](https://sourceforge.net/projects/urho3d/files/Urho3D/). Recommended version -- **1.6**. Also you can build Urho3D from source, for more information visit [Urho3D Site](https://urho3d.github.io).

Step 2. Set `URHO3D_HOME` environment var equal to the unpacked sdk dir.

Step 3. Clone repository:
```bash
git clone https://github.com/KonstantinTomashevich/urho3d-simple-multiplayer-shooter.git
```
Step 4. Generate build directory via CMake:
```bash
cd urho3d-simple-multiplayer-shooter && mkdir build && cd build && cmake ..
```
Step 5. Build.
```bash
make
```
Step 6. Executables will be in ${BINARY_DIR}/bin folder.
