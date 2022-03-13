# SoftSysOlinCoin

# What is this?
This repo is the source code for a project for the class [Software Systems](https://softsys.olin.edu/) @[Olin College](https://olin.edu).

This project seeks to recreate a blockchain to track transactions in the form of the original Bitcoin Network in C!

Take a look at ```/planning``` if you would like to see some of our design process

# Download instructions
Clone the repo in whatever fashion you would like! Or take a look @ our releases page to grab the binaries to run our an OlinCoin node

# Building from source
- Ensure build dependencies are met: gcc, cmake, make etc, (if on ubuntu ```sudo apt install build-essential```)
- Ensure other dependencies are met, uthash, on ubunutu```sudo apt install uthash-dev```
- After cloning the project locally navigate into the build directory ie.```cd ~/Downloads/SoftSysOlinCoin/build```
- To build the software(run from build directory)
    ```console
    cmake --build .. 
    ```
- to clean build files but not external libraries ([mbedtls](https://github.com/ARMmbed/mbedtls)) (run from build directory)
  ```console 
  cmake --build .. --target really-clean 
  ```
- to clean all build files (run from build directory)
  ```console 
  cmake --build .. --target clean 
  ```
 
- Note that a couple commands can change what is built:
    - BUILD_TESTS indicates if tests are built. To build with tests run:
      ```console
      cmake -DBUILD_TESTS=ON --build .. 
      ```
    - BUILD_RUNTIME indicates if runtime code is built. To build runtime run:
      ```console
      cmake -DBUILD_RUNTIME=ON --build .. 
      ```
