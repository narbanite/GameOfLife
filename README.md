# GameOfLife: Conway's Game of Life Implementation

This repository contains the source code for the GameOfLife project, an implementation of John Conway's classic Game of Life cellular automaton. This project was developed as part of a university course assignment to explore cellular automata and simulation programming.

## Overview

The Game of Life is a zero-player game where the evolution of a grid of cells is determined by a set of rules based on the state of neighboring cells. This project implements the simulation with a focus on efficient computation and user interaction, allowing users to observe the evolution of patterns over time.

## Features

- **Grid Simulation**: Initializes a grid of cells with configurable initial states (alive or dead).
- **Rule Implementation**: Applies Conway's Game of Life rules:
  - A live cell with fewer than 2 live neighbors dies (underpopulation).
  - A live cell with 2 or 3 live neighbors lives on.
  - A live cell with more than 3 live neighbors dies (overpopulation).
  - A dead cell with exactly 3 live neighbors becomes alive (reproduction).
- **Generation Updates**: Computes and displays subsequent generations of the grid.
- **User Interaction**: Allows users to set initial configurations and control the simulation (e.g., start, pause, or step through generations).

## Technologies Used
- **Programming Language**: C 

## Setup and Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/narbanite/GameOfLife.git
   cd GameOfLife
