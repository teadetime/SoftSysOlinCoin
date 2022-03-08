# OlinCoin

## Overview

This project sets out to create an implementation of a blockchain network from
scratch. We will be modeling this network off of the original bitcoin network.
This network will be used to keep track of transactions as this is the simplest
blockchain use case for us to understand. Concretely, we plan on:

- Implementing a wallet to create and track transactions.
- Implementing a miner to create new blocks
- Implementing validation to confirm both transactions and blocks

The final deliverable will chain these functions together to demonstrate a
single cycle of the blockchain network locally.

## Learning Goals

### Nathan

- Learn how C code can be deployed on multiple architectures (locally)
- Learn disadvantages and advantages of writing lower level code that interacts
  with itself through a network
- Learn more about how blockchains work and cryptocurrency

### Eamon

- Learn how bitcoin works
- Learn about peer to peer networking
- Learn how to architect a relatively complicated piece of software
- Learning common design patterns in C

## Resources

We found enough resources to start our project.

- Mastering Bitcoin is a free, online book that covers the technical details of
  Bitcoin. We found it extremely useful for helping to architect our project.
- MbedTLS has been our library of choice for hashing and signing.
- UTHash has been our library of choice for hashmaps

## Tasks

### Nathan

- Implement mining
  - Gather transactions function
    - Collect transactions from mempool
    - Build coinbase transaction
      - Calc coinbase reward
      - Calc transaction fees
  - Build block function
  - Validate block function
  - Hash block function
  - Test
- Handle new block (?)
  - (Not assigned yet)

### Eamon

- Implement wallet
  - Build structure to store transactions
  - Add public key to Input struct
  - Build transaction function
    - Build inputs
    - Build outputs
      - Generate ECDSA keypairs
    - Sign transaction
  - Test
- Handle new transaction (?)
  - (Not assigned yet)

## Links

[Todoist List](https://todoist.com/app/project/2285406394)
[Github Repo](https://github.com/olincollege/SoftSysOlinCoin)
