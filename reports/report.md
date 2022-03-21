# OlinCoin - a C-based Bitcoin-like cryptocurrency

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

We utilized several resources to aid in completing our project.

- [Mastering Bitcoin](https://www.oreilly.com/library/view/mastering-bitcoin/9781491902639/ch01.html) is a free, online book that covers the technical details of
  Bitcoin. We found it extremely useful for helping to architect our project.
- [MbedTLS](https://tls.mbed.org/) has been our library of choice for hashing and signinmbedtlsg.
- [UTHash](https://troydhanson.github.io/uthash/) has been our library of choice for hashmaps, which also function as linked lists

## Outcome

**By the numbers:**

- 4,400 lines of code
- 10 test files (tests for each function)
- Automated build w/CMake
- Documentation for each header file 
- 20 merged PRs that were peer-reviewed

**By Demo**
Our blochain can be created, and added to with all the verification steps that the bitcoin network does:
Note that this seemingly simple demo, utilizes all of the code we've written.

```C
init_blockchain()
print out blockchain
create_txs() 
validate_txs()
create_block()
validate_block()
Print out blockchain
```

Generates output:

SOMEOUTPUT



## Design Decisions
Throughout our development there were many decisions that we had to make.
Here is a summary of some notable decisions:

- Error Handling
  - We chose to 

- Non-Persistent Storage
  - Our project does not write to the disk at all....

- Transaction and Block Memory Allocation/Deallocation
  - Our model has txs and blocks that live in one place

## Implementation
We think of 4 main "pieces" needed for crypto currency:

- The Blockchain datactructure
  - Containing datastructures (Transactions, Mempool, UTXOs, Blocks, BlockHeaders)

- Mining capability
  - Add new blocks containing transaction to the blockchain
  - Verify a block and all transactions in a block are valid

- Wallet capability
  - The wallet creates transactions and allowes users to control "coins" on the blockchain
  - Stores credentials to interact with the blockchain

- Networking to share all of this information (we have not yet implemented this)

The implementation can be thought of in two large pieces:
- Creation of the data structures
- Manipulating these data structures

HOW TO TIES OUR DIFFERENT FIELS TO THIS?


## Reflection
Our project ended up



## Ongoing Tasks

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
