# Docker Container for Trilinos Tutorial at EuroTUG

## Installation

1. Clone this repository
1. Run `./build-container.sh` from the command line

## Using the container

To start-up the container, run `run-container.sh` from the command line.

A pre-installed Trilinos is located at `/opt/trilinos/` with its subdirectories `build/`, `install/`, and `source/`.

For the hands-on exercises, navigate to `/opt/trilinos_demo/exercises/` and follow the instructions in the respective `README.md` files.

To stop the containter, type `exit` in the container's command line.

## Clean-up

To remove the container permanently, run `delete-container.sh` from the command line.
