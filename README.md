# Rocketman Adventures
<img align="left" src="https://github.com/jakub-swiniarski/rocketman-adventures/assets/77209709/37e16421-faf1-4ed8-bd9f-b11413bae6a4" width="25%" height="25%">
Rocketman Adventures is a simple video game built using C and Raylib. <br>
The only goal of the game is to jump as high as possible. <br>
The game is inspired by Team Fortress 2. <br>

---

Rocketman adventures is guaranteed to work on GNU/Linux. <br>
This project is just for fun, so there are no big updates planned. <br>

---

![image](https://github.com/jakub-swiniarski/rocketman-adventures/assets/77209709/1e154f97-cbec-4072-9987-ea0415a8e891)
![image](https://github.com/jakub-swiniarski/rocketman-adventures/assets/77209709/a9ed9f4f-a149-415d-bde6-90b44fd1d5b5)
![image](https://github.com/jakub-swiniarski/rocketman-adventures/assets/77209709/f0596104-8796-4737-95c3-acda63d164f7)

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
SFX by [Boper](https://github.com/boprr)
