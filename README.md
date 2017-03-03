## Urho3D Simple Multiplayer Shooter
It's simple deathmatch shooter game on Urho3D game engine, which supports multiplayer and custom AIs.

## How to build
Step 1. Download Urho3D SDK binary release from [SourceForge](https://sourceforge.net/projects/urho3d/files/Urho3D/). Recommended version -- **1.6**. Also you can build it from source, for more information see [Urho3D Site](https://urho3d.github.io).

Step 2. Set `URHO3D_HOME` environment var to the unpacked sdk dir.

Step 3. Clone repository:
```bash
git clone https://github.com/KonstantinTomashevich/urho3d-simple-multiplayer-shooter.git
```
Step 4. Generate build directory using CMake:
```bash
cd urho3d-simple-multiplayer-shooter && mkdir build && cd build && cmake ..
```
Step 5. Build.
```bash
make
```
Step 6. Executables will be in ${BINARY_DIR}/bin folder.
