# 1D ARAP Energy with Cauchy-Green Invariants

The full title of the project is *1D Strand Simulation with Cauchy-Green Invariants
based ARAP Energy*. It is too long, so I took the liberty to do some deletion and re-ordering of words. 

> I usually feel hungry when seeing a topic of the form `<sth> with <sth>` because many recipe titles are of that form as well! Take apple pie with vanilla ice cream as an example, which is one of my favorite desserts.

You can find my C++ programs, example settings, animation results and a project report in this repository.

## Build & Run
The only dependencies are OpenGL (including glm and glfw) and the Eigen library. Linux is strongly recommended for Windows users, for the simplicity of configuring enviroment. 

I wrote a simple Makefile that only works in Linux. So after cloning this repository, you can just go to the repo's directory and enter `make` in the command line.

After successfully creating the executable `main`, you can use the command `make run` to experience the model in a simple 2-node setting. Alternatively, you can explore the example settings in the `files` directory. They are in `.txt` files, which I will elaborate in detail later. You can pass the location of a setting file as argument to run. For example, use the command `./main files/4nodes.txt` if you are still in the repo's root directory.

## Setting Format

The format of setting files is outlined below. You can use this to understand the example settings and even customise your settings.

* Line 1: title (can be anything, will appear in window)
* Line 2: g
* Line 3: true or false (true for gravity, false for no gravity)
* Line 4: p n i
    - n = number of particles
    - i = which particle is static (-1 for none)
* Line 4 + j: x y z w (for the particle with index j)
* Line 4 + (n + 1): s m
    - m = number of springs
* Line 4 + (n + 1) + k: i j k l
    - i = first particle index
    - j = second particle index
    - k = constant
    - l = rest length

## Control
Use the following keys to control after launching the OpenGL window.
* SPACE key -> toggles animation
* ENTER key -> steps through animation
* G key -> toggles gravity
