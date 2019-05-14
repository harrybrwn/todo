#include <stdlib.h>
#include <string.h>
#include "util/map.h"

#include <stdio.h>

typedef unsigned long hash_t;

typedef struct node {
	char* key;
	void* value;

	struct node* _right, * _left;
	hash_t       _hash_val;
} __tree_leaf;

// 'djb2' by Dan Bernstein
hash_t hash(char* str) {
	hash_t hash = 5381;
	int    c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;

	return hash;
}

Map* new_map() {
	Map* m = malloc(sizeof(Map));
	m->__size = 32;
	m->__data = malloc(sizeof(struct node*) * m->__size);

	m->length = 0;
	m->keys = malloc(sizeof(char*));

	for (int i = 0; i < m->__size; i++) {
		m->__data[i] = NULL;
	}
	return m;
}

static void delete_tree(struct node*);

#define BUG(msg) printf("%s:%d - %s: \"%s\"\n", __FILE__, __LINE__, __FUNCTION__, msg)

void close_map(Map* m) {
	for (int i = 0; i < m->__size; i++) {
		delete_tree(m->__data[i]);
	}
	free(m->__data);
	for (int i = 0; i < m->length; i++) {
		free(m->keys[i]);
	}
	free(m->keys);
	free(m);
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

static struct node* _new_node(char* key, void* val, hash_t key_hash) {
	struct node* n = malloc(sizeof(struct node));
	n->key = key;
	n->value = val;
	n->_left = NULL;
	n->_right = NULL;
	n->_hash_val = key_hash;
	return n;
}

static void add_node(Map* m, struct node* node, int index) {
	struct node* old_node = m->__data[index];

	if (old_node == NULL) { // || old_node->_hash_val == node->_hash_val) {
		m->__data[index] = node;
	} else if (old_node->_hash_val == node->_hash_val) {
		free(m->__data[index]);
		m->__data[index] = node;
	} else {
		insert_node(old_node, node);
	}
}

static void add_key(Map* m, char* key) {
	for (int i = 0; i < m->length; i++) {
		if (strcmp(m->keys[i], key) == 0) {
			return;
		}
	}

	m->keys = realloc(m->keys, (m->length + 1) * sizeof(char*));
	m->keys[m->length] = malloc(strlen(key) + 1);
	// m->keys[m->length++] = key;
	strcpy(m->keys[m->length++], key);
}

void put(Map* m, char* key, void* val) {
	hash_t key_hash = hash(key);
	int    index = key_hash % m->__size;

	// m->keys = realloc(m->keys, (m->length + 1) * sizeof(char*));
	// m->keys[m->length++] = key;
	add_key(m, key);
	add_node(m, _new_node(key, val, key_hash), index);
}

void* get(Map* m, char* key) {
	hash_t k_hash = hash(key);
	int    index = k_hash % m->__size;

	struct node* root = m->__data[index];
	if (root == NULL) {
		return NULL;
	}
	if (k_hash != root->_hash_val) {
		struct node* n = search(root, k_hash);
		if (n == NULL) {
			return NULL;
		}
		return n->value;
	}
	return root->value;
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

static void copy_nodes(Map* m, struct node* n) {
	if (n->_left != NULL) {
		copy_nodes(m, n->_left);
	}
	if (n->_right != NULL) {
		copy_nodes(m, n->_right);
	}

	int index = n->_hash_val % m->__size;
	add_node(m, _new_node(n->key, n->value, n->_hash_val), index);
}

void resize_map(Map** old_m, size_t size) {
	Map* new_m = malloc(sizeof(Map));
	new_m->__size = size;
	new_m->__data = malloc(sizeof(struct node*) * new_m->__size);

	for (int i = 0; i < new_m->__size; i++) {
		new_m->__data[i] = NULL;
	}

	for (int i = 0; i < (*old_m)->__size; i++) {
		if ((*old_m)->__data[i] != NULL) {
			copy_nodes(new_m, (*old_m)->__data[i]);
		}
	}
	close_map(*old_m);
	(*old_m) = new_m;
}
