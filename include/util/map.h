#ifndef HASHMAP_H
#define HASHMAP_H

#ifdef __cplusplus
extern "C" {
#endif

struct node;

struct hashmap {
	struct node** __data;
	size_t        __size;
};

typedef struct hashmap Map;

// new_map creates a new Map*. The resulting map should be closed when
// finished with.
Map* new_map();

// close_map closes a Map* and frees up all the memory it used.
void close_map(Map*);

// put puts the void pointer arguments at a spesific key in the Map.
void put(Map* m, char* key, void* val);

// get will get the void pointer stored at a spesific key in the Map.
void* get(Map* m, char* key);

// delete and item stored at a spesific key.
void delete(Map * m, char* key);

// resize_map will change the internal size of the map. Changing the size of a
// map could result in increased hash collisions.
void resize_map(Map** m, size_t size);

#ifdef __cplusplus
}
#endif

#endif
