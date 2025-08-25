# ASCII Threads Game 🧵🕹️
A terminal-based ASCII game written in **C**, demonstrating **multithreading** and real-time gameplay.
The player moves around the map, collects coins, and avoids enemies — while game logic (**enemies**, **input**, and **rendering**) runs in **parallel threads**.

---

## ✨ Features
- Real-time player movement on a text map
- Parallel threads for input, enemy AI/timers, and rendering (smooth updates)
- Coin pickups
- Enemy collisions
- Scoring
- Simple, portable C code (POSIX + `pthreads`)

> Controls: **W/A/S/D** or arrow keys to move, **Q** to quit (check `main.c` if you want to change/confirm).

---

## 📦 Requirements
- **gcc** or **clang**
- POSIX environment (Linux/macOS, or Windows via **WSL**)
- **pthreads** (linked by the Makefile)

---

## 🛠️ Build & Run
- To compile use type in terminal 'make all'
- To run 'make run'
- To clean the folder 'make clean'

## NOTE
- Scale the terminal to full screen in order to see the full map. Having the terminal in default size cuts the map and can be confusing when the character is not seen.