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