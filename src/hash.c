#include "include.h"





HashTable* InitializeTable(uint32_t capacity) {
    HashTable* hash_table = (HashTable*)calloc(1, sizeof(HashTable));
    hash_table->capacity = capacity;
    hash_table->length = 1;
    hash_table->entries = (TableEntry**)calloc(capacity, sizeof(TableEntry*));
    for(int i = 0; i < capacity; i++) {
        //TableEntry* table_entry = (TableEntry*)calloc(1, sizeof(TableEntry));
        //table_entry->key.x = sizeof(float);
        //hash_table->entries[i] = table_entry;
        //hash_table->entries[i].key.x = sizeof(float);

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
int32_t Hash(int32_t x, int32_t y, int32_t z, int32_t size) {
    if(x < 0) x = x*(-1);
    if(y<0) y = y*(-1);
    if(z<0) z = z*(-1);
    if(x == 0 && y == 0 && z == 0) {
        TraceLog(LOG_WARNING, "Hash division by zero");
    }
    int32_t value = ((x * 73856093) ^ (y * 19349663) ^ (z * 83492791)) % size;
    TraceLog(LOG_WARNING, "Hash algo returned: %d", value);
    return value;
    // int32_t value = (int32_t)floor(((x + (y * 31) + (z * 31 * 31)) + 1) % size); //simple hash algo
    // TraceLog(LOG_WARNING, "Hash algo returned: %d", value);
    // if(value < 0) value = value * -1;
    // return value;
}

//create / add chunk to table
ChunkMesh* CreateChunkEntry(Vector3 pos, HashTable* hash_table) {
    TraceLog(LOG_WARNING, "receiving x coord: %.2f", pos.x);
    //hash_table->length += 1;
    TableEntry* table_entry = (TableEntry*)calloc(1, sizeof(TableEntry));
    table_entry->chunk_mesh = gen_chunk_mesh(pos);
    table_entry->key = pos;
    table_entry->empty = false;

    TraceLog(LOG_WARNING, "hashing coords x:%.2f, y:%.2f, z:%.2f", pos.x, pos.y, pos.z);

    table_entry->value = Hash(pos.x, pos.y, pos.z, TABLE_CAPACITY/5);

    int32_t index = table_entry->value; 
    //TraceLog(LOG_WARNING, "current x key before: %.2f", hash_table->entries[index]->key.x);
    while(hash_table->entries[index]->key.x != 1.234f) index++;
    //first see if that spot is taken
    if(hash_table->entries[index]->key.x == 1.234f) {
        hash_table->entries[index] = table_entry;
        TraceLog(LOG_WARNING, "current x key: %.2f", hash_table->entries[index]->key.x);
        TraceLog(LOG_WARNING, "found spot at index: %d", index);
        TraceLog(LOG_WARNING, "making sure new index works: %.2f", hash_table->entries[index]->chunk_mesh->chunk->blocks[0]->pos.x);
        return hash_table->entries[index]->chunk_mesh;
    } else {
        TraceLog(LOG_ERROR, "not supposed to get here, check create chunk entry");
    }

    //if not empty, find empty spot
    //while(hash_table->entries[index]->key.x != 1.234f) index++;

    //hopefully, we found an empty spot














    
    //is index empty? if so, add entry
    //if (hash_table->entries[index]->empty == true) {
    // if(hash_table->entries[index] == NULL) {
    //     TraceLog(LOG_WARNING, "got here tho");
    //     //memcpy(&hash_table->entries[index], &table_entry, sizeof(TableEntry)); // why am i copying? just point to what i've got?
        
    //     hash_table->entries[index] = table_entry;
    //     TraceLog(LOG_WARNING, "index was empty in create chunk entry");
    //     table_entry->empty = false;
    //     return hash_table->entries[index]->chunk_mesh;
    //     //return table_entry->chunk_mesh;
    // } else if (hash_table->entries[index]->empty == false) {
    //     while(hash_table->entries[index]->empty == false) {
    //         // overflow protection
    //         if (hash_table->capacity >= index + 1) {
    //             index++;
    //         } else {
    //             TraceLog(LOG_ERROR, "Could not find empty spot in hash table, hash.c CreateChunkEntry");
    //         }
    //     }
    //     //memcpy(&hash_table->entries[index], table_entry, sizeof(TableEntry));
    //     hash_table->entries[index] = table_entry;
    //     table_entry->empty = false;
    //     return table_entry->chunk_mesh;
    // }




    // while(hash_table->entries[index].key.x != sizeof(float)) {
    //     index++;
    // }
    // if(hash_table->entries[index].key.x != sizeof(float)) {
    //     //hash_table->entries[index] = table_entry;
    //     memcpy(&hash_table->entries[index], &table_entry, sizeof(TableEntry)); //keep eye on this
    // }
    TraceLog(LOG_ERROR, "something went wrong in create chunk entry");
    return table_entry->chunk_mesh;
}

//destroy chunk from table
void RemoveChunkEntry() {

}

//fetch chunk from table
ChunkMesh* FetchChunkEntry(Vector3 pos, HashTable* hash_table) {
    TraceLog(LOG_WARNING, "hashing coords in fetch x:%.2f, y:%.2f, z:%.2f", pos.x, pos.y, pos.z);
    int32_t index = Hash(pos.x, pos.y, pos.z, TABLE_CAPACITY/5);

    while(true) {
            
        if(hash_table->entries[index]->key.x == pos.x &&
            hash_table->entries[index]->key.y == pos.y &&
            hash_table->entries[index]->key.z == pos.z) return hash_table->entries[index]->chunk_mesh;
        
        if(hash_table->entries[index]->key.x == 1.234f) {
            TraceLog(LOG_WARNING, "sending x coord: %.2f", pos.x);
            return CreateChunkEntry(pos, hash_table);
        }
        
        if(index+1 >= TABLE_CAPACITY) {
            TraceLog(LOG_ERROR, "Hit table capacity in fetch");
        }
        index++;
        //TraceLog(LOG_WARNING, "index: %d", index);
    }
    //assuming we found our guy
    // if(hash_table->entries[index]->key.x == pos.x &&
    //     hash_table->entries[index]->key.y == pos.y &&
    //     hash_table->entries[index]->key.z == pos.z) return hash_table->entries[index]->chunk_mesh;


    







    // if (hash_table->entries[index] == NULL) {
    //     TraceLog(LOG_WARNING, "fetch null chunk at x:%.2f, y:%.2f, z:%.2f", pos.x, pos.y, pos.z);
    //     ChunkMesh* chunk_mesh = CreateChunkEntry(pos, hash_table);
    //     return hash_table->entries[index]->chunk_mesh;
    // }

    // //is there already a chunk at index? if no, create one
    // if (hash_table->entries[index]->empty == true) {
    //     TraceLog(LOG_WARNING, "fetch empty chunk at x:%.2f, y:%.2f, z:%.2f", pos.x, pos.y, pos.z);
    //     ChunkMesh* chunk_mesh = CreateChunkEntry(pos, hash_table);
    //     // hash_table->entries[index].chunk_mesh = chunk_mesh;
    //     // hash_table->entries[index].empty = false;
    //     // hash_table->entries[index].key = pos;
    //     return hash_table->entries[index]->chunk_mesh;
    // // and if it's full, is it our chunk?
    // } else if (hash_table->entries[index]->empty != true) {
    //     while ((int32_t)floor(hash_table->entries[index]->key.x) != Xpos 
    //         && (int32_t)floor(hash_table->entries[index]->key.y) != Ypos
    //             && (int32_t)floor(hash_table->entries[index]->key.z) != Zpos) {
    //         index++;
    //     }
    //     TraceLog(LOG_WARNING, "made it here");
    //     return hash_table->entries[index]->chunk_mesh;
    // }

    TraceLog(LOG_ERROR, "Check hash.c fetch chunk entry function");
    return hash_table->entries[index]->chunk_mesh;
}

//
