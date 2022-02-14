# OlinCoin

## Overview
This project sets out to create an implementation of a blockchain network from scratch. We will be modeling this network off of the original bitcoin network. This network will be used to keep track of transactions as this is the simplest blockchain use case for us to understand. A simpler version of this project involves the creation of a local blockchain with the ability to add and validate new blocks. We are confident that we can achieve this. Our stretch goal is to borrow ~10 IOT devices and build our network out such that we can do transfers (aka add blocks) on the olin network and make it such that someone could join the network. This implementation would include more complicated tasks: establishing consensus among several nodes and achieving peer to peer communication.
## Learning Goals
### Nathan
- Learn how C code can be deployed on multiple architectures (locally) 
- Learn disadvantages and advantages of writing lower level code that interacts with itself through a network
- Learn more about how blockchains work and cryptocurrency
### Eamon
- Learn how bitcoin works
- Learn about peer to peer networking
- Learn how to architect a relatively complicated piece of software
- Potentially explore multithreading and multiprocessing in C
## Resources
We have found a fair amount of resources surrounding bitcoin, and are confident we have enough to start work on the local version. Mastering Bitcoin has in particular been a nice, open source resource for technical details. However, we haven’t found many resources relating to networking in C, and would appreciate pointers.

We were also thinking of running the network on embedded devices, and weren’t sure if we we’re going to face a significant challenge in using them. In particular:
- Do we have any budget to buy additional boards (RaspPis / Esps)?
- What do we need to watch out for compiling/writing c on our laptop vs RaspPi vs Esp8266?
## Initial Steps
We have already spent a good bit of time talking about blockchain but have many implementation details to sort out. Our initial steps are:
1. An architecture and planning meeting in which we draw out our entire system and the functionality it needs. This means we can understand what functions we will need, and what their inputs and outputs will be. Final goal: build Archtiecture reference/spec sheet
2. Find C implementations of data structures (ie. hashmaps, sets) and functions (ie hashing, networking, encryption).
3. Implement OlinCoin core data structures (Blocks, Blockchain, UTXOs)
