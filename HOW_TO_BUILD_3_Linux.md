# Prerequisites for Ipponboard build
## Debian based
`sudo apt install g++ (the minimum gcc-version is gcc 11.1.0, because c++-17 is needed) cmake libboost-all-dev`

## Redhat based
`sudo yum install g++ cmake boost boost-devel`  
If you want to use an IDE: `sudo yum install qt-creator qt6-designer gdb`

# Ipponboard
## Checkout and setup sources
git clone https://gitlab.com/r_bernhard/Ipponboard.git

## build the application
execute `./_build/linux/build_cmake_qt4.cmd \<x32\|x64\>`
