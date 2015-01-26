/* (C) Aleksander Guring */

#ifndef RB_TREE_H
#define RB_TREE_H

enum rb_color {
    rb_red,
    rb_black
};


typedef struct _rb_node {
    void *key;

    enum rb_color color;

    struct _rb_node *left;
    struct _rb_node *right;
    struct _rb_node *parent;
} rb_node;


typedef struct _rb_tree {
    int (*cmp) (const void *key1, const void *key2);
    void (*dest)(const void *key);
    struct _rb_node *root;
    size_t size;
} rb_tree;

// return NULL if error occurs
rb_node *rb_tree_insert(rb_tree *, void *key);

// return NULL if there is no such key in the tree
rb_node *rb_tree_find(rb_tree *, const void *key);

// todo: add return value
void rb_tree_delete(rb_tree *, const void *key);

// return NULL if error occurs
rb_tree *rb_tree_create(int (*cmp)(const void *, const void *), void (*dest)(const void *));

void rb_tree_destroy(rb_tree *);


/* some additional functions */

// todo: implement

//rb_node *rb_tree_min(const rb_tree *);  // return NULL if tree is empty
//rb_node *rb_tree_max(const rb_tree *);  // return NULL if tree is empty

void rb_tree_for_each(const rb_tree *, void (*func)(const void *));

#endif  /* RB_TREE_H */
