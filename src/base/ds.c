
#include "ds.h"
#include "random.h"

// NOTE(liam): idk if this is a good idea
// static Arena dict_arena = {0};
// static RandomSeries dict_series = {0};

void ArrayU32Push(Arena *arena, ArrayUInt32 *a, uint32 x)
{
	if (a->size + 1 >= a->capacity)
	{
		uint32 newCap = Max(a->capacity * 2, 32);
		/*a->V = (uint32 *)PushCopy(arena, newCap, a->V);*/
        uint32 *newV = PushArray(arena, uint32, newCap);
        if (a->V)
        {
            for (uint32 i = 0; i < a->size; i++)
            {
                newV[i] = a->V[i];
            }
        }

        a->V = newV;
		a->capacity = newCap;
	}
	a->V[a->size++] = x;
}

#define DictRot_(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define DictMix_(a,b,c) \
{ \
  a -= c;  a ^= DictRot_(c, 4);  c += b; \
  b -= a;  b ^= DictRot_(a, 6);  a += c; \
  c -= b;  c ^= DictRot_(b, 8);  b += a; \
  a -= c;  a ^= DictRot_(c,16);  c += b; \
  b -= a;  b ^= DictRot_(a,19);  a += c; \
  c -= b;  c ^= DictRot_(b, 4);  b += a; \
}

#define DictFinal_(a,b,c) \
{ \
  c ^= b; c -= DictRot_(b,14); \
  a ^= c; a -= DictRot_(c,11); \
  b ^= a; b -= DictRot_(a,25); \
  c ^= b; c -= DictRot_(b,16); \
  a ^= c; a -= DictRot_(c,4);  \
  b ^= a; b -= DictRot_(a,14); \
  c ^= b; c -= DictRot_(b,24); \
}
//
// static uint32_t lookup3(const void* key, size_t length, uint32_t initval)
// {
//     uint32_t  a,b,c;
//     const uint8_t* k;
//     const uint32_t* data32Bit;
//
//     data32Bit = key;
//     a = b = c = 0xdeadbeef + (((uint32_t)length)<<2) + initval;
//
//     while (length > 12) {
//         a += *(data32Bit++);
//         b += *(data32Bit++);
//         c += *(data32Bit++);
//         DictMix_(a,b,c);
//         length -= 12;
//     }
//
//     k = (const uint8_t *)data32Bit;
//     switch (length) {
//         case 12: c += ((uint32_t)k[11])<<24;
//         case 11: c += ((uint32_t)k[10])<<16;
//         case 10: c += ((uint32_t)k[9])<<8;
//         case 9 : c += k[8];
//         case 8 : b += ((uint32_t)k[7])<<24;
//         case 7 : b += ((uint32_t)k[6])<<16;
//         case 6 : b += ((uint32_t)k[5])<<8;
//         case 5 : b += k[4];
//         case 4 : a += ((uint32_t)k[3])<<24;
//         case 3 : a += ((uint32_t)k[2])<<16;
//         case 2 : a += ((uint32_t)k[1])<<8;
//         case 1 : a += k[0];
//                  break;
//         case 0 : return c;
//     }
//     DictFinal_(a,b,c);
//     return c;
// }
//
// static uint32 DictHash(uint32 key, uint32 size)
// {
//     uint32 initval;
//     uint32 hashAddress;
//
//     initval = 12345;
//     // If hashtable is guaranteed to always have a size that is a power of 2,
//     // replace the line above with the following more effective line:
//     hashAddress = lookup3(&key, 1, initval);
//     uint32 res = (hashAddress & (size - 1)); // (hashAddress % size);
//     printf("HASH: in: %d, out: %d\n", key, res);
//     return res;
// }
//
// void DictSetSize(DictStruct* dict, memory_index size)
// {
//     dict->minCapacity = size;
// }
//
// void DictAdd(DictStruct* dict, uint32 key, uint32 value)
// {
//     if ((dict->size + 1) > dict->capacity)
//     {
//         if (!dict->minCapacity)
//         {
//             // NOTE(liam): default value
//             dict->minCapacity = 64;
//         }
//         // NOTE(liam): performing immediate rehashing.
//         // This is an O(n) operation.
//         // We're not anticipating resizing too often,
//         // so this tradeoff is okay on average.
//         // If anything, we can just adjust the minimum size
//         // as necessary for the workload.
//         memory_index newCap = Max(dict->capacity * 2, dict->minCapacity);
//
//         uint32 *newKeys = PushArray(&dict_arena, uint32, newCap);
//         DictLinkedList* newValues = PushArray(&dict_arena, struct DictLinkedList, newCap);
//
//         if (dict->capacity)
//         {
//             //TODO(liam): capacity was not 0 before;
// 			// properly rehash everything.
// 			memory_index keyPos = 0;
// 			while (keyPos < dict->size)
// 			{
// 				uint32 oldHash = keys[keyPos++];
// 				if (oldHash)
// 				{
// 					uint32 newHash = DictHash(sizePos, newCap);
// 					newValues[newHash] = dict->values[oldHash];
// 				}
// 			}
// 		}
// 	}
// }
//
//
// uint32 DictGet(DictStruct dict, uint32 key)
// {
//     uint32 hashed_key = *(dict.keys + key);
//
//     uint32 res;
//     if (!hashed_key)
//     {
//         printf("Missing hash key.\n");
//         res = 0;
//     }
//     else
//     {
//         res = *(dict.values + hashed_key);
//     }
//
//     return res;
// }
//
// uint32 DictPop(DictStruct* dict, uint32 key)
// {
//     uint32 hashed_key = *(dict->keys + key);
//
//     uint32 res;
//     if (!hashed_key)
//     {
//         res = 0;
//     }
//     else
//     {
//         // NOTE(liam): I implemented a way to downsize capacity, but
//         // honestly it shouldn't matter because the memory is already
//         // allocated, and will continue to be allocated still,
//         // so downsizing accomplishes nothing.
//         //
//         // It would be more efficient to free the arena instead and
//         // reallocate.
//         // Also, we can't guarantee the hashed keys will continue to
//         // exist using the naive ArenaCopy since it won't read past
//         // a certain point of the original capacity.
//         // Basically, ArenaCopy is good only when you're allocating
//         // bigger than your current capacity, or if you know where
//         // all the data is indexed in the struct/array.
//
//         res = *(dict->values + hashed_key);
//         dict->values[hashed_key] = 0;
//         dict->keys[key] = 0;
//         dict->size--;
//
//     }
//
//     return res;
// }
//
// void DictClear(DictStruct* dict)
// {
//     // NOTE(liam): effective reset. Does not change capacity.
//     dict->size = 0;
// }
//
// void DictFree(void)
// {
//     // NOTE(liam): this frees the entire arena,
//     // rendering all previous allocations unusable.
//     ArenaFree(&dict_arena);
// }
