# Rocketman Adventures
<img align="left" src="https://github.com/jakub-swiniarski/rocketman-adventures/assets/77209709/37e16421-faf1-4ed8-bd9f-b11413bae6a4" width="25%" height="25%">
Rocketman Adventures is a simple video game built using C and Raylib. <br>
The only goal of the game is to jump as high as possible. <br>
The game is inspired by Team Fortress 2. <br>

---

Rocketman adventures is guaranteed to work on GNU/Linux. <br>
This project is just for fun, so there are no big updates planned. <br>

---

<img src="https://github.com/jakub-swiniarski/rocketman-adventures/assets/77209709/3f56512e-3dc0-44a0-9ad9-c7fa0d7ab261">
<img src="https://github.com/jakub-swiniarski/rocketman-adventures/assets/77209709/a46876ce-20cc-4b01-9c5f-ae316cc0be3f">
<img src="https://github.com/jakub-swiniarski/rocketman-adventures/assets/77209709/800aafdf-5149-403b-a4c4-f5a3a1d7f053">

## How to install?
### Prerequisites:
- gcc,
- raylib,
- git,
- make.

### Compile & install
```shell
git clone https://github.com/jakub-swiniarski/rocketman-adventures.git
cd rocketman-adventures
sudo make install
```

### How to run?
Use this command:
```shell
rocketman
```

## How to uninstall?
Go inside the cloned repository and run: <br/>
```shell
sudo make clean uninstall
```

## Default Controls
A - move left <br/>
D - move right <br/>
Space - jump <br/>
Left mouse button or R - shoot <br/>
Q - use the pickup <br/>
F1 - mute the sound <br/>
F2 - decrease the volume <br/>
F3 - increase the volume <br/>
Escape - exit the game

## Configuration
Edit the config file (src/config.h) to change the settings. Remember to recompile the game for the changes to take effect.

## Credits
SFX and background images by [Boper](https://github.com/boprr)
