# Parser-FF-A03

# Dependencies
curl and libxml2 are required dependencies

To make the docker container in which the application is intended to run on, follow/run the install_docker_deps.sh file

Other build system dependencies can be added/reviewd in the install_build_deps.sh file

# Building the project
mkdir build

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=install . -Bbuild

or

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install . -Bbuild

make install -j`nproc` -Cbuild

# Building the docker file(s)

After following the instructions under `Building the project`, run

make bl-parsers-builder to make an ubuntu docker image or make bl-parsers-builder-minimal to try the test minimal image