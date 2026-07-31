Sunday, April 5th 2026
So we begin anew. This time around it will be completely me, no AI involved.
Here is a vision for the features I'd like

- Same chunking system as last time, it worked well. However, perhaps we figure out how to access neighbor chunks in a less complicated way.
- Proprietary Meshing, from the ground up. No borrowing raylib cubes, we draw all the faces from the start.
- Chunks are one mesh. Probably don't worry about greedy meshing.
- I do not want to do multithreading, sorry. If minecraft didn't need it, I don't need it. Instead I will load in chunks sequentially, spaced out in time.
- Only render what the player sees (get this implemented from the start)
- Multiplayer goes beyond the scope of this project, so I will not be thinking about it.
- No AI/LLM (Google Search results fair game)
- Would like this to be perfomant, >120 frames on macbook pro, no hitches when crossing into new chunks

Order of operations:
- get a face drawn
- get your own cube drawn
- basic movement
- draw a chunk
- make chunk one mesh
- implement chunking system
- draw multiple chunks
- space out chunk loading
- only show what player sees
- then implement player movement, physics, adding / removing blocks, etc.



July 30th, 2026
I'm thinking of how to properly handle the player moving through the world and loading a whole bunch of chunks.
I should only have so many chunks in memory, but does that mean just the mesh or should i be removing 
the chunk data as well?

Also, i'm making a lot of hash table calls and having to create new entries all the time if I am deleting them/
freeing up the entry spot every time the chunk is out of players "reach". 

I could store them to disk? I mean, that would be a good idea regardless. I'm not sure if I was planning 
on implementing a save feature, but might as well if I start saving chunks to disk. 

That would mean removing the chunks from the hash table though, which doesn't really solve our issue.

The problem I'm trying to solve is the hitching as the player moves around, something is wrong with my
code logic because I'm loading everything again instead of just the new chunks. I think I had the right idea, I should take a new array of coords, check them against the old array of coords, then reassign the chunks to their new
positions in the new coords array, add the chunks that are new, delete the old chunks that have fallen out of scope.

Can I just unload the mesh and not the chunks? Because deleting the whole table entry means all the 
things the player has made will be deleted.