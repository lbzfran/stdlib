
# Intuition

After much thought and really trying to understand this project, I believe
I have an answer. I'm primarily using this not to create my own standard
library for C, but to create a standard for how I work with C.

Anything implemented here should be useable in other projects, and help me
set myself up faster either through me understanding how to implement
specific things, or having a point of reference for implementing it.

The subparts will be split up, sometimes more than it should be.

This split will mostly be for understanding-- so I can visually see how things
map to each other. For actual projects, they can be more isolated and portable.

current examples of mapping:
```
arena.c <- arena.h <- memory.h
vector.h <- math.h
utf.h <- string.h
```

# Notes

The rest of the markdown will be my notes on the different subprojects.

## Base

def.h

> This is one of the first header files I made a while back. It started out
> smaller than it is now, and I trimmed it down (from Mr. 4th Programming's own
> version) to only what made sense for me, that is only what I actually use in
> my projects. I dragged it around different projects and improved on it
> incrementally.

arena.h

> This is one of the longer projects I worked on, spanning about two weeks total.
> My intuition for this project was to think about memory differently.
> Its initial design used RAII, and a linear allocator that forced a fix
> size Arena on the user.

> Since then, there's been different versions of this arena that I've continuously
> iterated on (Credit to Handmade Hero and Ryan Fluery's article), and it is
> now more robust, and now allocates more space dynamically, and not
> requiring a fixed size component.

> The implementation also makes use of ZII, which is my first time applying it
> in my code. Essentially, constructing the arena is simply done by initializing
> it to Zero (hence ZII). The actual allocation (or pushing) is what allocates
> and adjusts the memory as required, and the default values are also set then
> (for example, with the minimum size requirement for an Arena).

memory.h

> This section abstracts the platform-dependent memory allocation necessary,
> effectively removing the need for malloc. I do leave malloc-compatible
> implementations throughout the codebase as I see fit for portability, but
> otherwise the memory management is done manually using this core.

random.h

> This implementation was pulled from Handmade Hero around the same time as
> the arena. I primarily used it for mock-up particle system with raylib, but
> it's been so useful on its own that I've dragged it around other projects too.

> It uses a pre-determined random table, and seeds passed iterate over this table.
> This allows for seemingly random but predictable outcomes, which is more than
> enough for my use cases. I am considering ways to repopulate the table, but
> it's good for now.

> I've actually overcome the random table, and now it only uses that random
> table in spirit, multiplying by a certain factor to "advance" the index
> much like how it was with the table.

string.h

> This implementation required a lot of iterating than I anticipated. I initially
> was very inspired to make it in the style similar to antirez/sds primarily
> because you could treat that string like cstrings. I learned that antirez's
> particular implementation essentially 'masked' the structure the string lives
> in, and I thought was super clever but required strings to be dynamically
> allocated at all times, which I could not compromise with at the end.

> I slowly converted it and beat the implementation to death until I landed
> very closely to Mr. 4th's iteration of the immutable, linear 'String8'.
> The only difference really is that I personalized some of the functions to
> be in a way that made more sense to me, which remains the most important
> part of this project. Overall, I'm happy I finally finished this part.

