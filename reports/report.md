# OlinCoin - a C-based Bitcoin-like cryptocurrency

## Overview

This project sets out to create an implementation of a blockchain network from
scratch. We will be modeling this network off of the original bitcoin network.
This network will be used to keep track of transactions as this is the simplest
blockchain use case for us to understand. Concretely, we plan on:

- Implementing a wallet to create and track transactions
- Implementing a miner to create new blocks
- Implementing validation to confirm both transactions and blocks

The final deliverable will chain these functions together to demonstrate a
single cycle of the blockchain network locally.

## Learning Goals

### Nathan

- Learn how C code can be deployed on multiple architectures (locally)
- Learn disadvantages and advantages of writing lower level code that interacts
  with itself through a network
- Learn more about how blockchains and cryptocurrency work

### Eamon

- Learn how bitcoin works
- Learn about peer to peer networking
- Learn how to architect a relatively complicated piece of software
- Learning common design patterns in C

## Resources

We utilized several resources to aid in completing our project.

- [Mastering Bitcoin](https://www.oreilly.com/library/view/mastering-bitcoin/9781491902639/ch01.html) is a free, online book that covers the technical details of Bitcoin. We found it extremely useful for helping to architect our project.
- [MbedTLS](https://tls.mbed.org/) has been our library of choice for hashing and signing.
- [UTHash](https://troydhanson.github.io/uthash/) has been our library of choice for hashmaps, which also function as linked lists

## Outcome

### By the numbers:

- 4,400 lines of code
- 10 test files (tests for every critical component)
- Automated build w/CMake
- Documentation for each header file
- 20 merged peer-reviewed PRs

### By the features:

Our implementation contains 3 distinct modules:
- Global Data Structures
  - Data structures necessary for operating a network node (Blockchain, Mempool, UTXO Pool)
  - See [globals](https://github.com/olincollege/SoftSysOlinCoin/tree/main/src/core/globals) 
- Block Operations
  - Creating and mining new blocks
  - Verifying a block and all transactions in a block are valid
  - Updating node data structures given a new block
  - See [blocks](https://github.com/olincollege/SoftSysOlinCoin/tree/main/src/core/blocks)
- Transaction Operations
  - Data structures necessary for tracking user transactions and user credentials (Key Pool, Wallet Pool)
  - Creating new transactions and cryptographically signing transactions
  - Verifying incoming transactions
  - See [txs](https://github.com/olincollege/SoftSysOlinCoin/tree/main/src/core/txs)

## Design Decisions

Throughout our development we made many design decisions. However, the most important decision was how we chose to run our development process. Early on, we committed to writing code in such a way that we could write good, functional code that we both contribute to and understand. To this end, we took several steps:
- We started off by working together to produce a detailed initial roadmap. This mapped out individual tasks that we could tackle one by one, and allowed us to work in parallel for the rest of the project. This structure allowed us to build our file structure such that things like transactions, blocks, etc were cleanly separated such that we could work on them simultaneously.
- We committed to writing tests for every unit of testable code we wrote. This meant that we could rely on code written in previous tasks to work, making parallelized work more comfortable. Writing tests also gives us the opportunity to show how the code is supposed to be called, making the job of reviewing a little simpler.
- We committed to writing sufficient documentation for our implementations/functionality. We settled on the [Doxygen](https://www.doxygen.nl/manual/docblocks.html#cppblock) comment blocks for nice syntax highlighting and clarity.
- We committed to doing code reviews for each new feature we implemented. This meant each feature was implemented in a separate feature branch, then merged back into the main branch with a PR after review. This ensured all our code got two eyes on it, and also ensured that we both had a holistic understanding of the project.
- We also committed to using a CMake as a build system to ensure that we were building the same things and that errors can be reproducible. 

On the technical side, one decision we made was to use the design pattern where one function input argument served as a destination to write to. Initially, we toyed with allocating memory within functions and returning the pointers. However, we found that for many use cases like serialization and hashing it was easier to store data in a static, fixed size buffer. Further, we often had to store data in struct fields that already had memory allocated. In either case, we would have to return the allocated memory, copy the data to our destination, then free the returned memory. By switching to passing a destination, we avoid this pattern while retaining the ability to allocate new memory outside the function if the need arises.

```C
size_t ser_pub_key(
    unsigned char *dest, mbedtls_ecp_point *point, mbedtls_ecp_group *grp
) {
  size_t num_bytes;
  char buf[ERR_BUF];
  int err;

  // Serialize ECC key to dest
  err = mbedtls_ecp_point_write_binary(
    grp, point,
    MBEDTLS_ECP_PF_UNCOMPRESSED,
    &num_bytes, dest, PUB_KEY_SER_LEN
  );
  if (err != 0) {
    mbedtls_strerror(err, buf, ERR_BUF);
    printf("Serialize pub key error! %s\n", buf);
    exit(1);
  }

  return num_bytes;
}
```

## Reflection
### Overall Project Reflection
Currently,  our project does not demonstrate a single “cycle” of the blockchain. However, we have written all of the components of the cycle and have tested nearly all of them. We are proud of our software development process and feel that it helped us achieve our learning goals of developing production-ready code. We feel like we have a much deeper understanding of the basic bitcoin network compared to when we started and feel much better about using blockchains in the future.  We are both still excited about the project and are going to continue developing it into a multithreaded process with persistent storage or even networking.

### Individual Reflections

#### Nathan
While my learning goals were to learn how to deploy C code on multiple architectures, write lower level code in a networked way, and understand how blockchains work. While I didn’t actually meet my learning goals surrounding networking and deploying C code on multiple architectures I got to engage in a very professional code development process which was more valuable for me than learning about C code deployment.  

I did gain a huge amount of understanding about blockchains and the actual implementation/runtime mechanics of bitcoin and feel much more comfortable engaging with cryptocurrency. Our planning and architecture process that went into the beginning of this project was empowering, exciting, fun, and led to a very successful working pattern. Our peer review process ensured that I understood and saw 100% of the code and was given feedback on the things that I wrote. This project has been one of the most fun-to-work-on projects of my life and has left me feeling like I have learned a great deal! I am excited to continue finishing out this project.


#### Eamon

My goals were to learn about bitcoin, learn about peer-to-peer networking, to architect a complicated piece of software and to learn C design patterns.Of these, I think I achieved networking the least. In the process of reading about Bitcoin I did learn quite a bit about the theory behind peer-to-peer networking and how it works (consensus mechanisms, building peer-to-peer connections, etc). However, we never got to implementing networking, and as a result I don’t have much insight into the technical side of networking.

On the other hand, I think I definitely achieved my other learning goals. In particular, I’m proud of how much technical understanding we built around Bitcoin. Coming into this project I knew nearly nothing about the internals of Bitcoin, and now I feel like I could explain almost every component of the system (except networking details). I was also happy with the planning that went into this project. We specced out the work we had to do early on with the hope that earlier components could be designed for later components. And, we were successful in this regard - the data structures and helper functions we implemented at the start plugged in smoothly into the more complicated features we implemented near the end. Finally, I enjoyed learning a bit more about C design patterns. Specifically, working with mbed-tls was helpful. Having an established, well documented library to use as a reference was nice and gave us some clear patterns to steal.

## Links

[Todoist List](https://todoist.com/app/project/2285406394)

[Github Repo](https://github.com/olincollege/SoftSysOlinCoin)