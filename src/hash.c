#include "include.h"





HashTable* InitializeTable(uint32_t capacity) {
    HashTable* hash_table = (HashTable*)calloc(1, sizeof(HashTable));
    hash_table->entries = (TableEntry*)calloc(capacity, sizeof(TableEntry));
    for(int i = 0; i < capacity; i++) {
        //TableEntry* table_entry = (TableEntry*)calloc(1, sizeof(TableEntry));
        //table_entry->key.x = sizeof(float);
        //hash_table->entries[i] = table_entry;
        hash_table->entries[i].key.x = sizeof(float);
    }
    return hash_table;
}

void DestroyTable(HashTable* hash_table) {
    // for(int i = 0; i < hash_table->capacity; i++) {
    //     free(hash_table->entries[i]);
    // }
    free(hash_table->entries);
    free(hash_table); //is this enough?
}

//hashing function
uint32_t Hash(int x, int y, int z, int size) {
    return (x + (y * 31) + (z * 31 * 31)) % size; //simple hash algo
    //return 1;
}

//create / add chunk to table
ChunkMesh* CreateChunkEntry(Vector3 pos, HashTable* hash_table) {
    TableEntry* table_entry = (TableEntry*)calloc(1, sizeof(TableEntry));
    table_entry->chunk_mesh = gen_chunk_mesh(pos);
    table_entry->key = pos;
    table_entry->value = Hash(pos.x, pos.y, pos.z, hash_table->capacity);

    //then add to table
    uint32_t index = table_entry->value; 
    while(hash_table->entries[index].key.x != sizeof(float)) {
        index++;
    }
    if(hash_table->entries[index].key.x != sizeof(float)) {
        //hash_table->entries[index] = table_entry;
        memcpy(&hash_table->entries[index], &table_entry, sizeof(TableEntry)); //keep eye on this
    }
    return table_entry->chunk_mesh;
}

//destroy chunk from table
void RemoveChunkEntry() {

}

//fetch chunk from table
Chunk* FetchChunkEntry(Vector3 pos, HashTable* hash_table) {
    uint32_t tempVal = Hash(pos.x, pos.y, pos.z, hash_table->capacity);

    while(hash_table->entries[tempVal].key.x != pos.x 
            && hash_table->entries[tempVal].key.y != pos.y
                && hash_table->entries[tempVal].key.z != pos.z) {
        //watch for overflow
        if((tempVal + 1) <= hash_table->capacity){
            tempVal++;
        } else {
            TraceLog(LOG_ERROR, "Failed to find chunk entry");
        }
        
    }
    if(hash_table->entries[tempVal].key.x == pos.x 
            && hash_table->entries[tempVal].key.y == pos.y
                && hash_table->entries[tempVal].key.z == pos.z) {

        return hash_table->entries[tempVal].chunk_mesh;
    }
    TraceLog(LOG_ERROR, "Check hash fetch chunk entry function");
    return hash_table->entries[tempVal].chunk_mesh;
}

//
