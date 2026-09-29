# MINIX Grand Prix

<p align="center">
  <img src="project/docs/cover.jpg" width="700"/>
</p>

![Project Grade](https://img.shields.io/badge/Project_Grade-18%2F20-3a3a3a?style=for-the-badge&labelColor=111111)
![Course](https://img.shields.io/badge/Course-LCOM-3a3a3a?style=for-the-badge&labelColor=111111)
![Year](https://img.shields.io/badge/Year-2025%2F26-3a3a3a?style=for-the-badge&labelColor=111111)

## Project Description

MINIX Grand Prix is a top-down racing game for MINIX, built from scratch on top of our own device drivers:
- RTC - Lab 1;
- Timer - Lab 2;
- Keyboard - Lab 3;
- Mouse - Lab 4;
- Video Graphics - Lab 5.

Pick a vehicle (taxi, police car or ambulance) and a track (grass, arctic or lava), and race three laps against the clock. The game has a scrolling camera, a minimap, a speedometer, nitro and drifting, a pause menu, and a leaderboard that records each player's best times with the date read from the RTC.

<p align="center">
  <img src="project/docs/gameplay.gif" width="600" alt="MINIX Grand Prix gameplay"/>
</p>

This was a 4-person team project (myself (up202407610@edu.fe.up.pt), José Maio (up202404872@edu.fe.up.pt), Vasco Guimarães (up202403604@edu.fe.up.pt) and Victor Gomez (up202406138@edu.fe.up.pt)) for the Laboratório de Computadores (LCOM) course unit, FEUP, 2025/26.

The original delivered README is kept intact in [`Delivered_Readme.md`](./Delivered_Readme.md), and my individual lab assignments (lab1 to lab5) are in [`labs/`](./labs).

> This repository is a personal copy (with full commit history preserved) of the original group submission on FEUP's GitLab.

## My Contribution

- Scrolling track map and camera;
- Complete overhaul of the track, camera and collision system, including checkpoints in the collision maps;
- Car sprites: from 8 to 16 angles, and then 48 sprites for smooth rotation;
- Improved track 3 map and the controls screen shown at the start of a race.

## Environment

1. In the root of the repository, build the driver libraries:
```sh
./create_libs.sh
```

2. Build and run the game inside MINIX:
```sh
cd project
make
lcom_run proj
```
