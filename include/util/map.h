#ifndef MAP_H
#define MAP_H

typedef unsigned long hash_t;

typedef struct node {
	char* key;
	void* value;

	struct node* _right, * _left;
	hash_t       _hash_val;
} __tree_leaf;

typedef struct {
	struct node** __data;
	size_t        __size;
} Map;

void set_map_size(size_t size);
Map* new_map();
void close_map(Map* map);
void put(Map* m, char* key, void* val);
void* get(Map* m, char* key);
void delete(Map * m, char* key);

#endif
