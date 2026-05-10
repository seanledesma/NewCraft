#include "include.h"

//initialize all table entry keys to improbable coordinate
//this makes it easy to tell if table entry is "open" or not later
HashTable* InitializeTable(uint32_t capacity) {
    HashTable* hash_table = (HashTable*)calloc(1, sizeof(HashTable));
    hash_table->capacity = capacity;
    hash_table->length = 1;
    //decided to do double pointer here instead of array of pointers with fixed size
    //benefit being I can calloc memory here (capacity) without needing to hardcode array size
    hash_table->entries = (TableEntry**)calloc(capacity, sizeof(TableEntry*));
    for(int i = 0; i < capacity; i++) {

        // new way of adding table entries, right way :)
        hash_table->entries[i] = (TableEntry*)calloc(1, sizeof(TableEntry));
        if(hash_table->entries[i] == NULL) {
            TraceLog(LOG_ERROR, "null pointer in Initialize Table for loop");
        }
        hash_table->entries[i]->empty = true;
        hash_table->entries[i]->key.x = 1.234f;
        hash_table->entries[i]->key.y = 1.234f;
        hash_table->entries[i]->key.z = 1.234f;
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
//multiply coords by large primes, bit shift coords, mod size
//taken from google, did not invent hashing algorithm (mine wasn't working)
int32_t Hash(int32_t x, int32_t y, int32_t z, int32_t size) {
    if(x < 0) x = x*(-1);
    if(y<0) y = y*(-1);
    if(z<0) z = z*(-1);
    if(x == 0 && y == 0 && z == 0) {
        TraceLog(LOG_WARNING, "Hash division by zero");
        x++;
        y++;
        z++;
    }
    int32_t value = ((x * 19) ^ (y * 59) ^ (z * 97)) % size; 
    //TraceLog(LOG_WARNING, "Hash algo returned: %d", value);
    return value;
}

//create / add chunk to table
ChunkMesh* CreateChunkEntry(Vector3 pos, HashTable* hash_table) {
    //hash_table->length += 1;
    TableEntry* table_entry = (TableEntry*)calloc(1, sizeof(TableEntry));
    table_entry->chunk_mesh = gen_chunk_mesh(pos);
    table_entry->key = pos;
    table_entry->empty = false;

    table_entry->value = Hash(pos.x, pos.y, pos.z, TABLE_CAPACITY/5);

    int32_t index = table_entry->value; 
    //TraceLog(LOG_WARNING, "current x key before: %.2f", hash_table->entries[index]->key.x);
    while(hash_table->entries[index]->key.x != 1.234f) index++;
    //first see if that spot is taken
    if(hash_table->entries[index]->key.x == 1.234f) {
        hash_table->entries[index] = table_entry;
        TraceLog(LOG_WARNING, "found spot at index: %d", index);
        table_entry->chunk_mesh->new = true;
        return hash_table->entries[index]->chunk_mesh;
    } else {
        TraceLog(LOG_ERROR, "not supposed to get here, check create chunk entry");
    }

    TraceLog(LOG_ERROR, "something went wrong in create chunk entry");
    return table_entry->chunk_mesh;
}

//destroy chunk from table
void RemoveChunkEntry() {

}

//fetch chunk entry from table
//if chunk entry doesn't exist, create chunk entry
//all requests to fetch or add go through here
ChunkMesh* FetchChunkEntry(Vector3 pos, HashTable* hash_table) {
    int32_t index = Hash(pos.x, pos.y, pos.z, TABLE_CAPACITY/5);

    while(true) {
            
        if(hash_table->entries[index]->key.x == pos.x &&
            hash_table->entries[index]->key.y == pos.y &&
            hash_table->entries[index]->key.z == pos.z) { 
            
            TraceLog(LOG_WARNING, "Found previously created chunk entry");
            hash_table->entries[index]->chunk_mesh->new = false;
            return hash_table->entries[index]->chunk_mesh;
        }
        
        if(hash_table->entries[index]->key.x == 1.234f) {
            return CreateChunkEntry(pos, hash_table);
        }
        
        if(index+1 >= TABLE_CAPACITY) {
            TraceLog(LOG_ERROR, "Hit table capacity in fetch");
        }
        index++;
        //TraceLog(LOG_WARNING, "index: %d", index);
    }

    TraceLog(LOG_ERROR, "Check hash.c fetch chunk entry function");
    return hash_table->entries[index]->chunk_mesh;
}

