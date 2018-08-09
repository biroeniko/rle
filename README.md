# RLE
A c++ implementation of the RLE (Run-length encoding) algorithm.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

The basic requirements for building the executable are:

* CMake 2.8+
* a c++ compiler
* make

#### Installation on Ubuntu

```
sudo apt-get install build-essentials cmake
```

### Installing

This program have been tested on Ubuntu 16.04 but should work under any systems that fulfills the aforementioned requirements.

#### Installation on Ubuntu

If you succesfully cloned the source files and you are currently in the project directory, you can generate the Makefile using the following command:

```
mkdir build
cd build/
cmake ..
```
Now you can build the binary by running:

```
make
```

## Usage instructions
The program awaits two parameters: the path to the image to process and the action (preceded by -t), which can be: 

0. encode image
1. decode image
2. encode and decode image
3. encode and decode image with the PackBits algorithm
```
./build/rle images/test5.ppm -t 2
```
This line will encode the "test5.ppm" file and then decode it from the resulting rle file.

## Authors

* **Biró Enikő** - [BiroEniko](https://github.com/biroeniko)
