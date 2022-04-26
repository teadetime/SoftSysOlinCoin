#### Goals
This project sets out to create an implementation of a blockchain network from scratch in C. We will be modeling this network off of the original bitcoin network. This network will be used to keep track of transactions as this is the simplest blockchain use case for us to understand. 
We are extending our project 1 base to a distributed decentralized network that supports persistence on individual nodes. 

This involves:

- Refactoring required structures to be persistent using LevelDB or equivalent Key->Value mapping
- Refactoring existing validation / update functions to handle potential disagreements that might arise from having a functioning network. Ex: dealing with blockchain branches.
- Implementing multi-threaded base process to run the node, wallet and miner in parallel
- Implementing multi-processing for miner to improve hashrate
- Implementing TCP/IP networking to other nodes

We are confident we can complete persistence, and are relatively certain we can also implement multi-threading and multi-processing. Implementing networking is a stretch goal, and may not be feasible within the project timeline.

### Learning Goals
**Nathan**
- I have not touched multithreading or multiprocessing in any real way. I want to understand and implement both in an effective manner in this project to be confident knowing when to use either in the future!
- Networking gives this system power, and I have never dealt with TCP/IP communication and want to add this to my skill set
- Make something that showcases the p2p nature of this project such that there is a demo that is seriously impressive.
- Test building on Raspberry PI/generate executables and get at least one other person to run a node at Olin

**Eamon**
- As in the previous project, I would like to learn about and practice architecting large projects. For this continuation, one thing I’ve been looking at is thinking about how we separate concerns in our code such that networking and concurrency are easy to implement.
- I’d like to learn more about writing concurrency in C. I’ve done concurrency in other languages before, and am curious to see how we might implement it in a lower level language.
- I’d also like to learn more about using a database in C and what a usable interface might look like. Some interesting problems I want to look at are figuring out how to make our operations atomic, and figuring out how to use a database across threads.
- If possible, I would also love to learn about networking in C.


#### Getting started
We have started to implement CMocka as our unit testing framework of choice, and we have also implemented  LevelDB as a persistent Key->Value mapping to replace our use of Uthash for global persistent data structures. We are looking at the little book of semaphores and Beej’s Guide to Network Programming for our next steps.
#### What we are working on right now
We have finished updating our codebase and implementing persistence, so our next steps are implementing concurrency and networking. We have an architecture sketched out for handling both, and are moving towards implementing it. 

Eamon is currently refactoring our unit tests to use CMocka instead of MinUnit to facilitate testing networking functions down the line.
- Requires writing proper fixtures for objects used in tests and databases used in tests, as well as moving old MinUnit asserts to CMocka asserts
- __Done when all tests are using fixtures and moved to CMocka__

Nathan is currently implementing our multi-threaded primary runtime.
  -  Requires implementing thread functions for a miner, a validation node and a wallet, as well a a thread-safe queue to pass messages between them
  - __Done when we can run our node with all functions__

Github: https://github.com/teadetime/SoftSysOlinCoin
Tasks: https://github.com/users/teadetime/projects/1 
