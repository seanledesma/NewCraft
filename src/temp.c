        /*
        alright, I want to SLOWLY add chunks over several frames, so the user doesn't notice
        it as badly when they walk around / cross chunk boundaries.

        i'm thinking i'll use the test counter i already have to say, maybe every tenth frame,
        take sometime to gen the next chunk, and to go pretty far.
        */

        // slowly load the rest of the world around current chunk
        if(chunkcounter < hash_table->length) {
            if (test_counter % 20 == 0) {

                // if(renderY <= RENDER_DISTANCE_Y) {
                //     renderY++;
                // } else {
                //     renderY = 0;
                // }
                // if(renderZ <= RENDER_DISTANCE_Z) {
                //     renderZ++;
                // } else {
                //     renderZ = 0;
                // }

                ChunkMesh* temp_chunkmesh_pos = FetchChunkEntry(
                    (Vector3) { 
                        current_chunk->chunk->world_pos.x + (renderX * CHUNK_SIZE),
                        current_chunk->chunk->world_pos.y + (0),
                        current_chunk->chunk->world_pos.z + (0) },
                    hash_table);
                if (temp_chunkmesh_pos->new) {
                    chunkmeshes[chunkcounter] = temp_chunkmesh_pos;
                    UploadMesh(chunkmeshes[chunkcounter]->mesh, false);
                    chunkcounter++;
                } else {
                    //free(temp_chunkmesh_pos); DON'T DO THIS
                }

                ChunkMesh* temp_chunkmesh_neg = FetchChunkEntry(
                    (Vector3) { 
                        current_chunk->chunk->world_pos.x + (-renderX * CHUNK_SIZE),
                        current_chunk->chunk->world_pos.y + (0),
                        current_chunk->chunk->world_pos.z + (0) },
                    hash_table);
                if (temp_chunkmesh_neg->new) {
                    chunkmeshes[chunkcounter] = temp_chunkmesh_neg;
                    UploadMesh(chunkmeshes[chunkcounter]->mesh, false);
                    chunkcounter++;
                } else {
                    //free(temp_chunkmesh_neg);
                }


                if(renderX <= RENDER_DISTANCE_X) {
                    renderX++;
                } else {
                    renderX = 1;
                }
            }

            if (test_counter % 30 == 0) {
                ChunkMesh* temp_chunkmesh_pos = FetchChunkEntry(
                    (Vector3) { 
                        current_chunk->chunk->world_pos.x + (0),
                        current_chunk->chunk->world_pos.y + (renderY * CHUNK_SIZE),
                        current_chunk->chunk->world_pos.z + (0) },
                    hash_table);
                if (temp_chunkmesh_pos->new) {
                    chunkmeshes[chunkcounter] = temp_chunkmesh_pos;
                    UploadMesh(chunkmeshes[chunkcounter]->mesh, false);
                    chunkcounter++;
                }

                ChunkMesh* temp_chunkmesh_neg = FetchChunkEntry(
                    (Vector3) { 
                        current_chunk->chunk->world_pos.x + (0),
                        current_chunk->chunk->world_pos.y + (-renderY * CHUNK_SIZE),
                        current_chunk->chunk->world_pos.z + (0) },
                    hash_table);
                if (temp_chunkmesh_neg->new) {
                    chunkmeshes[chunkcounter] = temp_chunkmesh_neg;
                    UploadMesh(chunkmeshes[chunkcounter]->mesh, false);
                    chunkcounter++;
                }


                if(renderY <= RENDER_DISTANCE_Y) {
                    renderY++;
                } else {
                    renderY = 1;
                }
            }

            if (test_counter % 40 == 0) {
                ChunkMesh* temp_chunkmesh_pos = FetchChunkEntry(
                    (Vector3) { 
                        current_chunk->chunk->world_pos.x + (0),
                        current_chunk->chunk->world_pos.y + (0),
                        current_chunk->chunk->world_pos.z + (renderZ * CHUNK_SIZE) },
                    hash_table);
                if (temp_chunkmesh_pos->new) {
                    chunkmeshes[chunkcounter] = temp_chunkmesh_pos;
                    UploadMesh(chunkmeshes[chunkcounter]->mesh, false);
                    chunkcounter++;
                }

                ChunkMesh* temp_chunkmesh_neg = FetchChunkEntry(
                    (Vector3) { 
                        current_chunk->chunk->world_pos.x + (0),
                        current_chunk->chunk->world_pos.y + (0),
                        current_chunk->chunk->world_pos.z + (-renderZ * CHUNK_SIZE) },
                    hash_table);
                if (temp_chunkmesh_neg->new) {
                    chunkmeshes[chunkcounter] = temp_chunkmesh_neg;
                    UploadMesh(chunkmeshes[chunkcounter]->mesh, false);
                    chunkcounter++;
                }

                if(renderZ <= RENDER_DISTANCE_Z) {
                    renderZ++;
                } else {
                    renderZ = 1;
                }
            }


            // diagonal now
            if (test_counter % 50 == 0) {
                
            }
        }