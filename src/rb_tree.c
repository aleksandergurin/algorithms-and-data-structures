/* (C) Aleksander Guring */

#include <stdlib.h>
#include <assert.h>

#include "rb_tree.h"

static void left_rotate(rb_tree *, rb_node *);
static void right_rotate(rb_tree *, rb_node *);
static void delete_node(rb_tree *tree, rb_node *z);
static void destroy(rb_node *node, void (*dest)(const void *));
static void for_each(const rb_node *, void (*func)(const void *));


static rb_node *
min(rb_node *node) {
    rb_node *prev = node;

    while (node != NULL) {
        prev = node;
        node = node->left;
    }

    return prev;
}

#define color_of(node)  ((node == NULL) ? rb_black : (node->color))
#define parent_of(node) ((node == NULL) ? NULL : (node->parent))
#define right_of(node)  ((node == NULL) ? NULL : (node->right))
#define left_of(node)   ((node == NULL) ? NULL : (node->left))

static inline void
set_parent_of(rb_node *node, rb_node *parent) {
    if (node != NULL)
        node->parent = parent;
}

static inline void
set_color(rb_node *node, enum rb_color color) {
    if (node != NULL)
        node->color = color;
}

static void
transplant(rb_tree *tree, rb_node *u, rb_node *v) {
    if (u->parent == NULL) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }

    if (v != NULL)
        v->parent = u->parent;
}

rb_tree *
rb_tree_create(int (*cmp)(void const *, void const *), void (*dest)(const void *)) {
    // comparison function shouldn't be NULL
    if (cmp == NULL)
        return NULL;

    rb_tree *tree = malloc(sizeof(rb_tree));

    // couldn't allocate memory
    if (tree == NULL)
        return NULL;

    tree->cmp = cmp;
    tree->dest = dest;
    tree->root = NULL;
    tree->size = 0;

    return tree;
}


void
rb_tree_destroy(rb_tree *tree) {
    if (tree->dest != NULL) {
        if (tree->root != NULL) {
            destroy(tree->root, tree->dest);
        }
    }

    free(tree);
}

static void
destroy(rb_node *node, void (*dest)(const void *)) {
    if (node->left != NULL)
        destroy(node->left, dest);

    if (node->right != NULL)
        destroy(node->right, dest);

    dest(node);
}

rb_node *
rb_tree_find(rb_tree *tree, const void *key) {
    if (key == NULL)
        return NULL;

    rb_node *node;
    int (*compar)(const void *, const void *);
    int cmp_res;

    node = tree->root;
    compar = tree->cmp;

    while (node != NULL) {
        cmp_res = compar(key, node->key);

        if (cmp_res < 0) {
            node = node->left;
        } else if (cmp_res > 0) {
            node = node->right;
        } else {
            return node;
        }
    }

    return NULL;
}

rb_node *
rb_tree_insert(rb_tree *tree, void *key) {
    // TODO: consider check whether key is NULL

    rb_node *y;
    rb_node *x;
    rb_node *new_node;
    int (*compare)(const void *, const void *);

    compare = tree->cmp;

    y = NULL;
    x = tree->root;

    while (x != NULL) {
        y = x;
        if (compare(key, x->key) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    new_node = malloc(sizeof(struct _rb_node));
    if (new_node == NULL) {
        // Couldn't allocate memory
        return NULL;
    }

    new_node->key = key;
    new_node->parent = y;
    if (y == NULL) {
        tree->root = new_node;
    } else if (compare(new_node->key, y->key) < 0) {
        y->left = new_node;
    } else {
        y->right = new_node;
    }
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->color = rb_red;

    //<editor-fold desc='Rebalance tree after insertion'>
    rb_node *z = new_node;
    rb_node *y1;

    while (z != NULL && z != tree->root && color_of(parent_of(z)) == rb_red) {
        if (parent_of(z) == left_of(parent_of(parent_of(z)))) {
            y1 = right_of(parent_of(parent_of(z)));
            if (color_of(y1) == rb_red) {
                set_color(parent_of(z), rb_black);
                set_color(y1, rb_black);
                set_color(parent_of(parent_of(z)), rb_red);
                z = parent_of(parent_of(z));
            } else {
                if (z == right_of(parent_of(z))) {
                    z = parent_of(z);
                    left_rotate(tree, z);
                }
                set_color(parent_of(z), rb_black);
                set_color(parent_of(parent_of(z)), rb_red);
                right_rotate(tree, parent_of(parent_of(z)));
            }
        } else {
            y1 = left_of(parent_of(parent_of(z)));
            if (color_of(y1) == rb_red) {
                set_color(parent_of(z), rb_black);
                set_color(y1, rb_black);
                set_color(parent_of(parent_of(z)), rb_red);
                z = parent_of(parent_of(z));
            } else {
                if (z == left_of(parent_of(z))) {
                    z = parent_of(z);
                    right_rotate(tree, z);
                }
                set_color(parent_of(z), rb_black);
                set_color(parent_of(parent_of(z)), rb_red);
                left_rotate(tree, parent_of(parent_of(z)));
            }
        }
    }

    set_color(tree->root, rb_black);
    //</editor-fold>

    tree->size += 1;
    return new_node;
}


void
rb_tree_delete(rb_tree *tree, const void *key) {
    rb_node *node;

    node = rb_tree_find(tree, key);

    if (node != NULL) {
        delete_node(tree, node);

        tree->size -= 1;
    }
}

static void
delete_node(rb_tree *tree, rb_node *z) {
    rb_node *x;
    rb_node *y;
    enum rb_color original_color;

    y = z;
    original_color = y->color;
    if (z->left == NULL) {
        x = z->right;
        transplant(tree, z, x);
    } else if (z->right == NULL) {
        x = z->left;
        transplant(tree, z, x);
    } else {
        y = min(right_of(z));

        assert(y != NULL);  // y shouldn't  be NULL

        original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            set_parent_of(x, y);
        } else {
            transplant(tree, y, x);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(tree, z, y);
        y->left = z->left;
        set_parent_of(left_of(y), y);
        y->color = z->color;
    }

    //<editor-fold desc='Rebalance tree after deletion'>
    if (original_color == rb_black) {
        rb_node *w;

        while (x != NULL && x != tree->root && color_of(x) == rb_black) {
            if (x == left_of(parent_of(x))) {
                w = right_of(parent_of(x));
                if (color_of(w) == rb_red) {
                    set_color(w, rb_black);
                    set_color(parent_of(x), rb_red);
                    left_rotate(tree, parent_of(x));
                    w = right_of(parent_of(x));
                }
                if (color_of(left_of(w)) == rb_black && color_of(right_of(w)) == rb_black) {
                    set_color(w, rb_red);
                    x = parent_of(x);
                } else {
                    if (color_of(right_of(w)) == rb_black) {
                        set_color(left_of(w), rb_black);
                        set_color(w, rb_red);
                        right_rotate(tree, w);
                        w = right_of(parent_of(x));
                    }
                    set_color(w, color_of(parent_of(x)));
                    set_color(parent_of(x), rb_black);
                    set_color(right_of(w), rb_black);
                    left_rotate(tree, parent_of(x));
                    x = tree->root;
                }
            } else {
                w = left_of(parent_of(x));
                if (color_of(w) == rb_red) {
                    set_color(w, rb_black);
                    set_color(parent_of(x), rb_red);
                    right_rotate(tree, parent_of(x));
                    w = left_of(parent_of(x));
                }
                if (color_of(right_of(w)) == rb_black && color_of(left_of(w)) == rb_black) {
                    set_color(w, rb_red);
                    x = parent_of(x);
                } else {
                    if (color_of(left_of(w)) == rb_black) {
                        set_color(right_of(w), rb_black);
                        set_color(w, rb_red);
                        left_rotate(tree, w);
                        w = left_of(parent_of(x));
                    }
                    set_color(w, color_of(parent_of(x)));
                    set_color(parent_of(x), rb_black);
                    set_color(left_of(w), rb_black);
                    right_rotate(tree, parent_of(x));
                    x = tree->root;
                }
            }
        }
        set_color(x, rb_black);
    }
    //</editor-fold>
}

void
rb_tree_for_each(const rb_tree *tree, void (*func)(const void *)) {
    if (tree->root != NULL)
        for_each(tree->root, func);
}

static void
for_each(const rb_node *node, void (*func)(const void *)) {
    if (node->left != NULL)
        for_each(node->left, func);

    func(node->key);

    if (node->right != NULL)
        for_each(node->right, func);
}

static void
left_rotate(rb_tree *tree, rb_node *x) {
    assert(x != NULL);

    rb_node *y = x->right;
    x->right = y->left;

    if (y->left != NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        tree->root = y;
    } else if (x->parent->left == x) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

static void
right_rotate(rb_tree *tree, rb_node *y) {
    assert(y != NULL);

    rb_node *x = y->left;
    y->left = x->right;

    if (x->right != NULL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NULL) {
        tree->root = x;
    } else if (y->parent->left == y) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

