## OlinCoin: A C-based bitcoin recreation from scratch
## Goals
This project created implementation of a blockchain network from scratch in C. This network is modeled off of the original bitcoin network. This network will be used to keep track of transactions as this is the simplest blockchain use case for us to understand. 
We are extending our project 1 base to a distributed decentralized network that supports persistence on individual nodes. This involves:
- Refactoring required structures to be persistent using LevelDB or equivalent Key->Value mapping
- Refactoring existing validation / update functions to handle potential disagreements that might arise from having a functioning network. Ex: dealing with blockchain branches.
- Implementing multi-threaded base process to run the node, wallet and miner in parallel
- Implementing socket networking to propagate blocks and transactions
We were confident we could complete persistence, and were relatively certain we could also implement multi-threading. Implementing networking was a stretch goal.

## Learning Goals
**Nathan**
- I have not touched multithreading or multiprocessing in any real way. I want to understand and implement both in an effective manner in this project to be confident knowing when to use either in the future!
- Networking gives this system power, and I have never dealt with TCP/IP communication and want to add this to my skill set
- Make something that showcases the p2p nature of this project such that there is a demo that is seriously impressive.
- Test building on Raspberry PI/generate executables and get at least one other person to run a node at Olin

**Eamon**

-  As in the previous project, I would like to learn about and practice architecting large projects. For this continuation, one thing I’ve been looking at is thinking about how we separate concerns in our code such that networking and concurrency are easy to implement.
-  I’d like to learn more about writing concurrency in C. I’ve done concurrency in other languages before, and am curious to see how we might implement it in a lower level language.
- I’d also like to learn more about using a database in C and what a usable interface might look like. Some interesting problems I want to look at are figuring out how to make our operations atomic, and figuring out how to use a database across threads.
-  If possible, I would also love to learn about networking in C.

## Resources
We used a couple different libraries:

- [mbed-tls](https://tls.mbed.org/)
- [LevelDB](https://github.com/google/leveldb)
- [CMocka](https://cmocka.org/)

We also used several different resources for learning about the Bitcoin network and networking:
- [Mastering Bitcoin](https://github.com/bitcoinbook/bitcoinbook)
- [Beej’s Guide to Networking](https://beej.us/guide/bgnet/)

## What we accomplished

We accomplished many of our goals for this project, from implementing persistence, figuring out a concurrency architecture and even completing a simple networking scheme.

### Refactored serialization
In order to implement persistence, it was necessary to be able to serialize all objects that we wanted to write to disk. While some serialization functions existed, they weren’t uniform and didn’t cover all of our objects. So, to start this half of the project off, we wrote additional serialization for objects missing them (key pairs, wallet entries and UTXOs) while refactoring old serialization functions into a consistent interface (blocks, transactions, inputs and outputs).

Below is an example of our UTXO serialization functions. The provided interface (size, serialization with or without allocation, deserialization with or without allocation) is consistent for all objects.
```C
/******************************************************************************
 * UTXOs
 ******************************************************************************/

#define UTXO_SER_LEN \
  ( \
    sizeof(((UTXO*)0)->amt) + \
    sizeof(((UTXO*)0)->public_key_hash) \
  )

/**
 * @brief Serialize a UTXO
 *
 * @param dest Buffer of length UTXO_SER_LEN to write to
 * @param utxo UTXO to serialize
 * @return Number of bytes written if successful, -1 otherwise
 */
ssize_t ser_utxo(unsigned char *dest, UTXO *utxo);

/**
 * @brief Allocates memory and serializes a UTXO
 *
 * @param written Stores number of bytes written if successful, -1 otherwise
 * @param utxo UTXO to serialize
 * @return Serialized UTXO of length UTXO_SER_LEN if successful, NULL
 *   otherwise
 */
unsigned char *ser_utxo_alloc(ssize_t *written, UTXO *utxo);

/**
 * @brief Deserialize a UTXO
 *
 * @param dest UTXO to write to
 * @param src Buffer of length size_ser_utxo() to read from
 * @return Number of bytes read if successful, -1 otherwise
 */
ssize_t deser_utxo(UTXO *dest, unsigned char *src);

/**
 * @brief Allocate memory and deserialize a UTXO
 *
 * @param read Stores number of bytes read if successful, -1 otherwise
 * @param src Buffer of length UTXO_SER_LEN to read from
 * @return Deserialized UTXO if successful, NULL otherwise
 */
UTXO *deser_utxo_alloc(ssize_t* read, unsigned char *src);
```

### Persistence with Level DB
Persistence, the ability to save the blockchain and start it up again, was a necessary step for us to move towards a more fleshed out blockchain network. Further, persistent storage follows the path of Bitcoin Core where structures that are large and unchanging (the blockchain, the utxo pool, etc.) are stored on-disk.
Our implementation used [LevelDB](https://github.com/google/leveldb), the same key-value store that Bitcoin Core uses. This library is a C++ library which has C bindings. While we were able to use it successfully, the library was not well documented… especially in C. 
At a high level we switched from using uthash, an in-memory key-value store, to LevevlDB for the blockchain, UTXO pool, wallet pool and key pool. Much of the change involved simple drop-in replacements for our existing data structure interfaces, but some key factors forced large refactors. This includes the serialization changes (outlined above) since objects needed to be serialized before being saved to disk, and testing changes (outlined below) since carry over between tests functions or even test runs was causing problems.

### Better Unit Testing
We started our project using minunit, but found it to be lacking some features we wanted. In particular, we found ourselves writing many fixture-like functions for setting up test cases, but without the proper support fixtures normally received. This meant things like teardown were inconsistent and error prone, making tests harder to maintain. This was exacerbated with the switch to persistent storage – suddenly, failing to tear down objects could cause strange and inconsistent behavior between test runs. Additionally, in order to support networking down the line, we wanted the ability to set up mocks. Together, this pushed us to make the switch from minunit to CMocka, which is a pure C testing framework that supports both mocking and fixtures.
```C
const struct CMUnitTest tests[] = {
  cmocka_unit_test_prestate_setup_teardown(
    test_utxo_pool_init,
    fixture_setup_utxo_pool,
    fixture_teardown_utxo_pool,
    NULL
  ),
  cmocka_unit_test_prestate_setup_teardown(
    test_utxo_pool_add,
    fixture_setup_compose,
    fixture_teardown_compose,
    &composition
  ),
  cmocka_unit_test_prestate_setup_teardown(
    test_utxo_pool_find,
    fixture_setup_compose,
    fixture_teardown_compose,
    &composition
  ),
  cmocka_unit_test_prestate_setup_teardown(
    test_utxo_pool_remove,
    fixture_setup_compose,
    fixture_teardown_compose,
    &composition
  )
};
```

In the end, most of our tests have been converted to CMocka and use proper setup / teardown functions to build their test environments. This has allowed us to ensure we always use a test database and always clear the test database between tests. In addition, we added a simple CI pipeline with Github Actions that ensures all our tests build and pass before merging PRs.

### Multithreaded Operation
Our OlinCoin node needs to be able to do many things at once. In its final state we need to handle network requests while running an interactive shell, mining blocks, and verifying new blocks and transactions. This involves a large number of blocking operations: verification waits on objects being created or propagated, the shell waits on user input and networking waits on incoming data. We could’ve handled these blocking calls with a less optimal polling solution. However, we decided to implement a more complicated multithreaded architecture which allows us to run all node functions concurrently, cutting out time wasted waiting on operations or polling for responses.

This architecture was achieved with a number of mutexes and inter-thread queues. See the design decision section for a more indepth look of what this entailed. 

Below are the threads we used:
```C
/* Create independent threads each of which will execute function */
node_block_ret = pthread_create(&node_block, NULL, node_block_thread, (void*) globals);
node_tx_ret = pthread_create(&node_tx, NULL, node_tx_thread, (void*) globals);
shell_ret = pthread_create(&shell, NULL, shell_thread, (void*) globals);
miner_ret = pthread_create(&miner, NULL, miner_thread, (void*) globals);
server_ret = pthread_create(&server, NULL, server_thread, (void*) globals);
client_ret = pthread_create(&client, NULL, client_thread, (void*) globals);
```

* Shell (Responsible for running interactive shell and making new transactions)
* Miner (Responsible for mining blocks)
* Node Block (Responsible for validating and processing blocks)
* Node Transaction (Responsible for validating and processing transactions)
* Server (Responsible for listening for new connections and pushing updates)
* Client (Responsible for opening new connections and receiving updates)

### Networking Architecture
When implementing networking, we aimed for the most simple and most crucial part of a decentralized blockchain network – the propagation of blocks and transactions. To this end, we added two new threads: a server thread for handling incoming connections and pushing out updates, and a client thread for making outgoing connections and receiving updates.

Our server thread always listens for incoming connections. When a new connection is made, a new process is made to handle the connection, and a new message queue is created to communicate with the new process. Whenever one of our node threads handles and processes a new object, that object is then pushed out to all open server message queues. The server processes then receive the object and push it out over their connection, propagating the object across the network.

Our client thread opens new connections on startup, with each connection being handled by an individual process. Each client process shares a single message queue, which is consumed by the main process client thread. Client processes wait on incoming messages from their connections. When new data is received, it is pushed to the shared message queue. The data is then read and deserialized by the main process client thread, which adds it to the appropriate inter-thread queue for processing.

Between these two functions, we are able to successfully create new blocks and transactions locally, then see them propagate across a network of multiple connected nodes.

## Design Decision: Implementing Multithreading

### Thread Safe Databases
Figuring out how to properly multithread our application required us to resolve two problems. The first piece of complexity we handle is our persistent storage being shared across threads. While individual LevelDB databases are thread safe, we rely on our different databases remaining in sync with each other. For example, our UTXO database must always be updated in parallel with the blockchain. As a result, we must ensure that when updating one database, none of the related databases are read from or written to. As this isn’t handled by LevelDB’s thread safety, we had to implement our own solution.

The model we use is simple – each database has a single mutex associated with it, shared across all threads. When a thread starts an operation that hits the databases (reading or writing), it is responsible for acquiring a lock for all the databases that need to be modified For example, an operation writing to the blockchain must lock both the blockchain and the utxo pool. This ensures that no other thread can read or write to any of the databases being touched by the current operation until it finishes, avoiding race conditions arising from desynchronized databases.

This system, however, introduces the potential for deadlocks. If one thread locks the blockchain and waits on the utxo pool while another locks the utxo pool and waits on the blockchain we’re stuck. To avoid this problem, we impose an arbitrary order in which required databases should be locked, shared across all threads. As locking a database requires locking all previous databases in the ordering, two threads waiting on locks the other has already acquired is impossible – in that case, the locks would have to be acquired in two different orders! For example, assume we have two threads that want to use the blockchain and the utxo pool, and the blockchain comes before the utxo pool in our ordering. In order for our deadlock scenario to occur, one thread must be holding the blockchain lock while the other holds the utxo pool lock. However, under our system this is impossible as acquiring the utxo pool lock requires having already acquired the blockchain lock.

### Inter-Thread Communication
The second challenge we faced was in communicating between threads. The miner, shell and client threads produce blocks and transactions, which then need to be communicated to the node block and node transaction threads for handling. Additionally, we need the ability to keep track of multiple blocks and transactions that need to be validated. There is no guarantee that a new block is immediately dealt with, so the node threads must be able to handle a full sequence of incoming objects. To address these hurdles, we implemented a thread-safe queue. The queues include a mutex to prevent two threads from modifying it at the same time, and includes a semaphore which allows threads to wait on new items. When adding an item, we acquire the lock, add the item, then signal the semaphore and release the lock. When popping an item, we wait on the semaphore, then acquire the lock, pop the item and finally release the lock. By waiting on the semaphore and then locking we ensure that we do not reach a deadlock state where an empty queue is locked by a thread waiting on a new item.

In practice, we have two queues: one for blocks and one for transactions. The miner adds to the block queue, the shell adds to the transaction queue, and the client thread adds to both. On the other end, the node block thread consumes the block queue and the node transaction thread consumes the transaction queue. This design means each thread only has to deal with an extremely simple interface. Threads creating objects simply add the object to the queue then forget about it, without worrying about what will handle it. And, threads consuming objects simply pop objects off the queue then process them, without regard to how it was produced. This structure matches the setup of our network well – objects received over the network are simply objects created locally on another node. So, it makes sense that local objects and network objects are added to the same queue, and it makes sense that our threads consuming objects don’t care where the object came from.

## Reflection
Overall, we’re happy with the progress we made on this project. We reached all of our initial goals and even broke into our stretch goal of networking – not only did we succeed in having a functional concurrent application that mines, provides a wallet interface, and maintains our blockchain, but we also implemented a network which can propagate new blocks and transactions across itself.

__Eamon__

My learning goals for this project were to continue practicing architecting more complicated software, working on concurrency in C, working with databases in C and working with networking in C. I feel confident in saying that I met all of my learning goals and more. For this segment of the project, I think we achieved everything we set out to do and had a great teaming and learning experience in the process. I feel like I really challenged myself when it came to dealing with planning out our various new features, and also learned a ton about the various solutions we had to implement (storage / concurrency / networking). As a whole, I really enjoyed this project and am glad to have gone through it.

__Nathan__

This has been one of my favorite projects that I’ve ever done. My learning goals were to run this on a Raspberry Pi, learn how to implement multi-threading and multi-processed applications, implement TCP networking, and make a cool demo. I wrote the first pass of our multi-threading and multi-processed code. This, in addition to architecting our solution with Eamon was a huge point of learning and accomplishment. A Raspberry Pi 3 was used to do our networking test and development. We simulated a network in which there was only one miner and watched the block propagate through the Pi to an additional computer. This was our moment of truth that we were working towards for the last 3 months! Overall, this was an awesome project! I’m sad we don’t have an additional 2-3 weeks to polish some of our things up and then think about implementing the API. This being said, implementing an API is likely the size of another project! This project has also made me less scared of multi-threading and processing and has made me a stronger software engineer. 

Github: [https://github.com/teadetime/SoftSysOlinCoin](https://github.com/teadetime/SoftSysOlinCoin)

Tasks: [https://github.com/users/teadetime/projects/1](https://github.com/users/teadetime/projects/1)

Docs: [https://teadetime.github.io/SoftSysOlinCoin/](https://teadetime.github.io/SoftSysOlinCoin/)
