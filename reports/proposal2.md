## Concurrent OlinCoin, a blockchain in C

### Reflection
This last project was a very successful learning experience for us, so much so that we have decided to extend this project! We accomplished most of our goals, felt we had a great teaming experience, and managed to practice good SWE practices with code review and testing. However, we have started to run into shortcomings in our task planning system, our testing system (minunit.h) and our lack of persistent storage. We are taking several steps to address these issue in our next project:
- We are switching to using [Github Projects](https://docs.github.com/en/issues/trying-out-the-new-projects-experience/about-projects) to track our tasks. This explicitly links our PRs with tasks, and makes tracking progress / closing tasks much easier. 
- We are testing out two new testing frameworks, [Check](https://libcheck.github.io/check/) and [Cmocka](https://cmocka.org/). We are looking for fixture and mock support for our more complicated features (concurrency and networking).
- We have made our plan for changing from the non-persistent storage via UTHash to persistent storage via LevelDB.

### Goals
This project sets out to create an implementation of a blockchain network from scratch in C. We will be modeling this network off of the original bitcoin network. This network will be used to keep track of transactions as this is the simplest blockchain use case for us to understand. 
Our previous project implemented a non-persistent single-process local blockchain.
It was made up of:
Wallet functions to create and track transactions
Miner functions to create new blocks
Node functions to validate both transactions and blocks
This project seeks to extend this basic functionality to a distributed decentralized network that supports persistence on individual nodes. This involves:
Refactoring required structures to be persistent using LevelDB or equivalent Key->Value mapping
Refactoring existing validation / update functions to handle potential disagreements that might arise from having a functioning network. Ex: dealing with blockchain branches.
Implementing multi-threaded base process to run the node, wallet and miner in parallel
Implementing multi-processing for miner to improve hashrate
Implementing TCP/IP networking to other nodes
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

                                                         
### Getting Started
We have investigated new testing frameworks and settled between Check and Cmocka. We have also tested LevelDB as a persistent Key->Value mapping that will replace our use of Uthash as a hashmap for global persistent data structures. We have architected how we plan to pass data in our multi-threaded application however, we have more uncertainty when it comes to networking. We have been investigating Beej’s Guide to Network Programming to understand how we might implement the networking for our project.

#### First Steps
We have already had an architecture meeting planning out most of the technical work of this project. Our next steps are captured in our [project management](https://github.com/users/teadetime/projects/1/views/1).
These tasks show the initial refactoring phase of the project. With this, we hope to have all our previous work finished, reviewed and merged in such a way that moving forward with the project is easy. We’ve also spent time planning out how we are going to implement persistence and concurrency, and will make the necessary tasks when our cleanup phase is done.
