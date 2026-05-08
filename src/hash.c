#include "include.h"





HashTable* InitializeTable(uint32_t capacity) {
    HashTable* hash_table = (HashTable*)calloc(1, sizeof(HashTable));
    hash_table->capacity = capacity;
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
int32_t Hash(int32_t x, int32_t y, int32_t z, int32_t size) {
    if(x < 0) x = x*(-1);
    if(y<0) y = y*(-1);
    if(z<0) z = z*(-1);
    if(x == 0 && y == 0 && z == 0) {
        TraceLog(LOG_WARNING, "Hash division by zero");
    }

    return ((x + (y * 31) + (z * 31 * 31)) + 1) % size; //simple hash algo
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
ChunkMesh* FetchChunkEntry(Vector3 pos, HashTable* hash_table) {
    int32_t Xpos = (int32_t)floor(pos.x);
    int32_t Ypos = (int32_t)floor(pos.y);
    int32_t Zpos = (int32_t)floor(pos.z);
    int32_t size = (int32_t)floor(hash_table->capacity);
    int32_t tempVal = Hash(Xpos, Ypos, Zpos, size);

    while((int32_t)floor(hash_table->entries[tempVal].key.x) != Xpos 
            && (int32_t)floor(hash_table->entries[tempVal].key.y) != Ypos
                && (int32_t)floor(hash_table->entries[tempVal].key.z) != Zpos) {

        // if chunk doesn't exist..
        if (hash_table->entries[tempVal].key.x == sizeof(float)) {
            ChunkMesh* chunk_mesh = CreateChunkEntry(pos, hash_table);
            return chunk_mesh;
        }

        //watch for overflow
        if((tempVal + 1) < hash_table->capacity){
            tempVal++;
        } else {
            TraceLog(LOG_ERROR, "Failed to find chunk entry");
        }
        
    }
    if((int32_t)floor(hash_table->entries[tempVal].key.x) == Xpos 
            && (int32_t)floor(hash_table->entries[tempVal].key.y) == Ypos
                && (int32_t)floor(hash_table->entries[tempVal].key.z) == Zpos) {

        return hash_table->entries[tempVal].chunk_mesh;
    } 
    TraceLog(LOG_ERROR, "Check hash.c fetch chunk entry function");
    return hash_table->entries[tempVal].chunk_mesh;
}

//
