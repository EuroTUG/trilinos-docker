# Docker Container for Trilinos Tutorial at EuroTUG

This repository contains material for demonstrating, learning, and testing the use of the [Trilinos](https://trilinos.github.io).

> The _Trilinos Project_ is a community of developers, users and user-developers focused on collaborative creation of algorithms and enabling technologies within an object-oriented software framework for the solution of large-scale, complex multi-physics engineering and scientific problems on new and emerging high-performance computing (HPC) architectures.
>
> ---
>
> See [https://trilinos.github.io](https://trilinos.github.io).

In order to make the material

+ easy to use,
+ accessible, and
+ portable,

the repository provides **scripts to build a [Docker](https://www.docker.com) container with all required software**. Within the Docker container, it is easy to compile, run, and modify the **exercises and tutorials for Trilinos**.

**Note:** *Of course, the required software can also be installed directly, such that the examples can be compiled and run without the Docker container.*

## Installing Docker

[Docker](https://www.docker.com) can be easily installed on Linux, Windows, and MacOS and facilitates the delivery software in packages called _containers_, which are isolated and bundle their own software, libraries, and configuration files.

Please first install Docker on your computer following the instructions on the [official website](https://www.docker.com/get-started).

---

**All following steps require that the software Docker is running on your computer. Moreover, if Docker complains about permissions, you should execute the scripts with `sudo`**.

---

### Building the Docker container

> It is assumed that you have cloned this repository to your machine.

The Docker container can be built by executing the script

```shell
./build-container.sh
```

in the main directory of the repository. Running this script will set up a docker image with the flag `trilinos_demo` as described above. Since all the above mentioned software will be installed, **this step will take some time (> 1 hour)**.

In order to **test if the Trilinos installation has been successful**, [run the docker container](#running-the-docker-container) (see below) and run the Trilinos tests using

```shell
cd /opt/trilinos/build
ctest -j <numProc>
```

where `<numProc>` specifies the number of threads to be used for testing in parallel (e.g. 1, 2, ...).

If all tests (approx. 350 tests) are successful, the Docker container is ready to be used.

### Running and using the Docker container

In order to run the Docker container, just execute the script

```shell
./run-container.sh
```

from the main directory of the repository. The script will **run the container** and **mount the current directory** (should be the main directory of the repository) as the local directory `/opt/trilinos_demo` within the container.

A pre-installed Trilinos is located at `/opt/trilinos/` with its subdirectories `build/`, `install/`, and `source/`.

For the hands-on exercises, navigate to `/opt/trilinos_demo/exercises/` and follow the instructions in the respective `README.md` files.

To stop the containter, type `exit` in the container's command line.

### Deleting the Docker container

In case you later want to remove the Docker image and the respective mounted volume (from running the container), just execute the script
```shell
./delete-container.sh
```

in the main directory of the repository.

## Additional references

+ **EuroTUG**

  + [Website](https://eurotug.github.io)
  + [Email](mailto:eurotug@tudelft.nl)

+ **Trilinos**

  + [Website](https://trilinos.github.io/index.html)
  + [GitHub repository](https://github.com/trilinos/Trilinos)
  + [Documentation](https://trilinos.github.io/documentation.html)
  + [Getting started](https://trilinos.github.io/getting_started.html)
  + [Hands-on tutorials](https://github.com/trilinos/Trilinos_tutorial/wiki/TrilinosHandsOnTutorial)

## Troubleshooting

+ When using a linux based system like Ubuntu, the Docker images and container might be stored in the root directory. If the root directory storage capacity is not sufficient please see [this post](https://www.guguweb.com/2019/02/07/how-to-move-docker-data-directory-to-another-location-on-ubuntu/) for changing the directory to another location.
