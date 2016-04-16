//  Sandboxer, kernel module sandboxing stuff
//  Copyright (C) 2016  Vasiliy Alferov, Sayutin Dmitry
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.


//  Please, perform a splay tree test (could be found in tests module)
//  a couple of times after inserting ANY changes in this file.

#ifndef TEST_SPLAY

#include <linux/slab.h>

#include "sandboxer-core.h"
#include "sandboxer-mentor.h"

#else
// Than a splay tree test is performed
#include "test-splay.h"

#endif

struct splay_tree_node {
    struct mentor_stuff val;
    struct splay_tree_node *L, *R, *par;
};

static struct splay_tree_node *splay_tree_root;

static struct splay_tree_node *left_rotate(struct splay_tree_node *root) {
    struct splay_tree_node *rt = root->R;
    root->R = rt->L;
    if (rt->L)
        rt->L->par = root;
    rt->L = root;
    rt->par = root->par;
    root->par = rt;
    if (rt->par) {
        if (rt->par->L == root)
            rt->par->L = rt;
        else
            rt->par->R = rt;
    }
    return rt;
}

static struct splay_tree_node *right_rotate(struct splay_tree_node *root) {
    struct splay_tree_node *lt = root->L;
    root->L = lt->R;
    if (lt->R)
        lt->R->par = root;
    lt->R = root;
    lt->par = root->par;
    root->par = lt;
    if (lt->par) {
        if (lt->par->L == root)
            lt->par->L = lt;
        else
            lt->par->R = lt;
    }
    return lt;
}

static void rotate_edge(struct splay_tree_node *up, struct splay_tree_node *down) {
    if (up->L == down)
        right_rotate(up);
    else
        left_rotate(up);
}

static inline int which_child(struct splay_tree_node *up, struct splay_tree_node *down) {
    return up->L == down ? 0 : 1;
}

static struct splay_tree_node *splay(struct splay_tree_node *v) {
    while (v->par) {
        if (!(v->par->par)) {
            // zig
            rotate_edge(v->par, v);
        } else if (which_child(v->par, v) == which_child(v->par->par, v->par)) {
            // zig-zig
            if (v->par->L == v) {
                right_rotate(v->par->par);
                right_rotate(v->par);
            } else {
                left_rotate(v->par->par);
                left_rotate(v->par);
            }
        } else {
            // zig-zag
            rotate_edge(v->par, v);
            rotate_edge(v->par, v);
        }
    }
    return v;
}

static struct splay_tree_node *splay_tree_find(struct splay_tree_node *root, pid_t val, struct splay_tree_node *up) {
    if (!root) {
        if (up)
            splay_tree_root = splay(up);
        return NULL;
    }
    if (val == root->val.pid) {
        splay_tree_root = splay(root);
        return root;
    }
    else if (val < root->val.pid)
        return splay_tree_find(root->L, val, root);
    else
        return splay_tree_find(root->R, val, root);
}

static void splay_tree_add(struct splay_tree_node *v) {
    struct splay_tree_node *lt, *rt;

    if (!splay_tree_root) {
        splay_tree_root = v;
        return;
    }
    BUG_ON(splay_tree_find(splay_tree_root, v->val.pid, NULL) != NULL);
    if (splay_tree_root->val.pid < v->val.pid) {
        lt = splay_tree_root;
        rt = splay_tree_root->R;
        splay_tree_root->R = NULL;
    } else {
        rt = splay_tree_root;
        lt = splay_tree_root->L;
        splay_tree_root->L = NULL;
    }
    v->L = lt;
    v->R = rt;
    if (lt)
        lt->par = v;
    if (rt)
        rt->par = v;
    v->par = NULL;
    splay_tree_root = v;
}

static struct splay_tree_node *merge(struct splay_tree_node *u, struct splay_tree_node *v) {
    if (!u)
        return v;
    if (!v)
        return u;
    while (u->R)
        u = u->R;
    u = splay(u);
    u->R = v;
    v->par = u;
    return u;
}

static void splay_tree_remove(struct splay_tree_node *v) {
    BUG_ON(!v);
    splay_tree_root = splay(v);
    if (v->L)
        v->L->par = NULL;
    if (v->R)
        v->R->par = NULL;
    splay_tree_root = merge(v->L, v->R);
}

int init_mentor_stuff(void) {
    splay_tree_root = NULL;
    return 0;
}

void shutdown_mentor_stuff(void) {
    while (splay_tree_root) {
        free_mentor_stuff(splay_tree_root->val.pid);
    }
}

struct mentor_stuff *get_mentor_stuff(pid_t pid) {
    struct splay_tree_node *n;

    n = splay_tree_find(splay_tree_root, pid, NULL);
    if (n)
        return &(n->val);
    return NULL;
}

struct mentor_stuff *create_mentor_stuff(pid_t pid) {
    struct splay_tree_node *n;

    n = kmalloc(sizeof(struct splay_tree_node), GFP_KERNEL);
    n->val.pid = pid;
    splay_tree_add(n);
    return &(n->val);
}

void free_mentor_stuff(pid_t pid) {
    struct splay_tree_node *n;

    n = splay_tree_find(splay_tree_root, pid, NULL);
    BUG_ON(n == NULL);
    splay_tree_remove(n);
    kfree(n);
    n = NULL;
}

