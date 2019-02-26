# OculusOpenGL

Connects to a UDP server and receives images from it to render to the Oculus HMD. Uses GLFW, GLAD, GLM, Oculus SDK 1.31.0, and stb_image.h.

Can download a VS 2017 (x64) version (without setting it all up again) from here: https://drive.google.com/file/d/1wC--qggUHdbIfZG03MvDfD8KO5MqjXd6/view?usp=sharing 

## Dependency Trees

### Full Dependency Tree
![Full Dependency Tree](https://i.ibb.co/9nTrfLR/Full-Dependency-Tree.jpg)

### Core Dependency Tree (Doesn't include inbuilt C/C++ libraries)

![Core Dependency Tree](https://i.ibb.co/5sWDQpD/Core-Dependency-Tree.jpg)

### How the diagrams were made
The diagrams were made by installing `graphviz` on a Ubuntu terminal.

`graphviz` can be installed by typing on the terminal this command: `sudo apt-get install graphviz`

Once installed, the command to generate JPEG dependency diagrams is: `dot -Tjpg InputFileName -o OutputFileName`

The files that were used to generate the diagrams can be found below:

* **Full Dependency Tree:** https://pastebin.com/raw/CaqZMtcd

* **Core Dependency Tree:** https://pastebin.com/raw/ek4MSy1T
