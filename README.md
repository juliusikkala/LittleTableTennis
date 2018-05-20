# LittleTableTennis

![Screenshot of the game](screenshot.png)

This is an example project for
[Littleton](https://github.com/juliusikkala/Littleton). It's a clone of the
famous "Pong" -game. Even though Littleton is licensed under LGPLv3, this
project itself is under the MIT license (see the LICENSE file for more details.)

This means that Littleton should be linked dynamically with the project. Both
the Meson and Visual Studio build files do this. Do note that due to the nature
of Littleton, you should treat it as if was statically linked: do not replace
liblittleton.so (or littleton.dll on Windows) with a different version that the
executable was not built against.

## Building

Please clone the repo using

```console
$ git clone --recurse-submodules git@github.com:juliusikkala/LittleTableTennis.git
```

to make sure the Littleton submodule is pulled. If you don't, all Littleton
files will be missing when attempting to build with Visual Studio. Meson
thankfully corrects the mistake by pulling the submodules that have been added
as subprojects in meson.build.

If you already cloned the repo in the normal way, you can fix that by running

```console
$ git submodule update --init --recursive
```

### Linux

```console
$ meson build
$ ninja -C build
```

Run the game with

```console
$ build/ltt
```

### Windows

Visual Studio 2017 project files are in the repository. They make no further
assumptions over those in [Littleton](https://github.com/juliusikkala/Littleton/blob/master/README.md#windows).

## Controls

### Keyboard

W and S move the left paddle up and down, respectively. I and K move the right
paddle. Escape and Q exit the game.

### Controller

If only one controller is connected, it's left and right thumbsticks control the
paddles. If two are connected, their left thumbsticks control the paddles.
