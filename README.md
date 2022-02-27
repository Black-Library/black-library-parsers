# black-library-parser
Black Library parser library.

## Dependencies
curl, libxml2, python3, pip, and selenium are required dependencies

To make the docker container in which the application is intended to be run on, follow/run the install_docker_deps.sh file

Other build system dependencies can be added/reviewed in the install_build_deps.sh file

## Building the project
```mkdir build```

and

```cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=install . -Bbuild```

or

```cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install . -Bbuild```

then

```make install -j`nproc` -Cbuild```

## Building the docker file(s)

After following the instructions under `Building the project`, run

make -Cbuild bl-parsers-builder to make an ubuntu docker image or make -Cbuild bl-parsers-builder-minimal to try the test minimal image

## Syntax guidelines

We will (loosely) follow the google cpp style guide for this project

https://google.github.io/styleguide/cppguide.html
