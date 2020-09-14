# Parser-FF-A03

# Dependencies
libxml2 is a required dependency

# Building the project
mkdir build

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=install . -Bbuild

or

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install . -Bbuild

make install -j`nproc` -Cbuild