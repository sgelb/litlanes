## litlanes

Demonstration of various algorithms for procedural generation of spatial
infinite landscapes. Focuses on showing the basic differences of Perlin
Noise/fBm, Ridged-Multifractal Noise, Billow Noise and Random Noise and how
factors like frequency or lacunarity influence the outcome.

![Screenshot](/screenshot.png)

### Dependencies

- C++11-compatible compiler
- Cmake
- OpenGL 3.3-compatible graphics card
- Runs on Linux. Should run on Windows, but its untested

### Build and run instructions

Build with `cd build && cmake .. && make`

Run with `cd bin && ./litlanes`

### Todo

- [ ] resizable window
- [ ] better handling of algorithm options
- [ ] show wireframe in solid color
- [ ] remove code repetition in buffer setup
- [ ] add option for number and size of rendered tiles
- [ ] implement level of detail using Quadtrees (see Ulrich paper)
- [ ] basic textures/color models
- [ ] basic implementation of rivers
- [ ] skybox / fog
- [ ] basic animation of sea

