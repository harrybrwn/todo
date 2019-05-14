#ifndef HASHMAP_H
#define HASHMAP_H

struct node;

typedef struct {
	struct node** __data;
	size_t        __size;
} Map;

Map* new_map();
void close_map(Map*);
void put(Map* m, char* key, void* val);
void* get(Map* m, char* key);
void delete(Map * m, char* key);
void resize_map(Map** m, size_t size);
void print_map(Map*);

#endif
