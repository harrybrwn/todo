#include <stdlib.h>
#include <string.h>
#include "util/map.h"

// 'djb2' by Dan Bernstein
static hash_t hash(char* str) {
	hash_t hash = 5381;
	int    c;

	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}

static size_t _map_size = 32;

void set_map_size(size_t size) {
	_map_size = size;
}

Map* new_map() {
	Map* m = malloc(sizeof(Map));
	m->__size = _map_size;
	m->__data = malloc(sizeof(struct node*) * m->__size);

	for (int i = 0; i < m->__size; i++) {
		m->__data[i] = NULL;
	}
	return m;
}

static void delete_tree(struct node*);

void close_map(Map* m) {
	for (int i = 0; i < m->__size; i++) {
		delete_tree(m->__data[i]);
	}
	free(m->__data);
	free(m);
}

static struct node* new_node(char* key, void* data) {
	struct node* n = malloc(sizeof(struct node));
	n->key = key;
	n->value = data;
	n->_left = NULL;
	n->_right = NULL;
	return n;
}

static void insert_node(struct node* root, struct node* new) {
	if (new->_hash_val < root->_hash_val) {
		if (root->_left != NULL) {
			return insert_node(root->_left, new);
		} else {
			root->_left = new;
		}
	}
	if (new->_hash_val > root->_hash_val) {
		if (root->_right != NULL) {
			return insert_node(root->_right, new);
		} else {
			root->_right = new;
		}
	}
}

struct node* search(struct node* root, hash_t key_hash) {
	if (root->_hash_val == key_hash) {
		return root;
	}

	if (key_hash < root->_hash_val) {
		return search(root->_left, key_hash);
	} else if (key_hash > root->_hash_val) {
		return search(root->_right, key_hash);
	}
	return NULL;
}

static void delete_tree(struct node* leaf) {
	if (leaf != NULL) {
		delete_tree(leaf->_right);
		delete_tree(leaf->_left);

		free(leaf);
	}
}

static void delete_leaf(struct node** leaf, hash_t key_hash) {
	if ((*leaf) == NULL) {
		return;
	}
	if ((*leaf)->_hash_val == key_hash) {
		free(*leaf);
		(*leaf) = NULL;
		return;
	}

	if (key_hash < (*leaf)->_hash_val) {
		return delete_leaf(&(*leaf)->_left, key_hash);
	} else if (key_hash > (*leaf)->_hash_val) {
		return delete_leaf(&(*leaf)->_left, key_hash);
	}
}

static struct node* _new_node(char* key, void* val, hash_t key_hash) {
	struct node* n = new_node(key, val);
	n->_hash_val = key_hash;
	return n;
}

void put(Map* m, char* key, void* val) {
	hash_t key_hash = hash(key);
	int    index = key_hash % m->__size;

	if (m->__data[index] == NULL) {
		m->__data[index] = _new_node(key, val, key_hash);
	} else {
		insert_node(m->__data[index], _new_node(key, val, key_hash));
	}
}

void* get(Map* m, char* key) {
	hash_t k_hash = hash(key);
	int    index = k_hash % m->__size;

	struct node* root = m->__data[index];
	if (root == NULL) {
		return NULL;
	}

	if (k_hash != root->_hash_val) {
		return (search(root, k_hash))->value;
	}
	return root->value;
}

void delete(Map* m, char* key) {
	hash_t k_hash = hash(key);
	int    index = k_hash % m->__size;

	struct node* root = m->__data[index];
	if (root == NULL) {
		return;
	}

	if (k_hash != root->_hash_val) {
		delete_leaf(&root, k_hash);
	} else {
		free(m->__data[index]);
		m->__data[index] = NULL;
	}
}
