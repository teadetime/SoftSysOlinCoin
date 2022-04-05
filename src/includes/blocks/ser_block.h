#pragma once
/*
returns a pointer to end of memory of a serialized blockheader buffer(dest) used
for hashing, storage, or transmission

dest: buffer to write serialization into, must be sizeof(Block_header)
block_header: block header ot serialize
*/
unsigned char *ser_blockheader(unsigned char *dest, BlockHeader *block_header);

/*
returns a pointer to start of memory containing serialization of a block header 
allocates new memory, freeing is responsibility of caller

block_header: block_header to be serialized
*/
unsigned char *ser_blockheader_alloc(BlockHeader *block_header);

/*
Returns pointer to end of dest buffer that contains the serialization of a block

dest: buffer to write the block serialization into, must be of size_block(block)
block: block to be serialized
*/
unsigned char *ser_block(unsigned char *dest, Block *block);

/*
Serializes a block and returns a pointer to the beginning of the memory where 
that lives, it is of length size_block(block) long

block: block to serialize
*/
unsigned char *ser_block_alloc(Block *block);

int deser_blockheader(BlockHeader *dest_header, unsigned char *blockheader_data);

int deser_block(Block *dest_block, unsigned char *block_data);