// Copyright (c) 2016 Martin Ridgers
// License: http://opensource.org/licenses/MIT

#pragma once

#include <core/array.h>

class editor_module;

//------------------------------------------------------------------------------
class binder
{
public:
    // key==0 + id==0xff is special:  it tells is_bound() that control codes and
    // extended keys not explicitly bound will be eaten (i.e. will not cancel
    // the binding group and will not be re-dispatched).
    static constexpr unsigned char id_catchall_only_printable = 0xff;

                        binder();
    int                 get_group(const char* name=nullptr);
    int                 create_group(const char* name);
    bool                bind(unsigned int group, const char* chord, editor_module& module, unsigned char id, bool has_params=false);
    int                 is_bound(unsigned int group, const char* seq, int len) const;

private:
    static const int    link_bits = 9;
    static const int    module_bits = 5;

    struct node
    {
        unsigned short  is_group    : 1;
        unsigned short  next        : link_bits;
        unsigned short  module      : module_bits;
        unsigned short  has_params  : 1;

        unsigned short  child       : link_bits;
        unsigned short  depth       : 4;
        unsigned short  bound       : 1;
        unsigned short              : 2;

        unsigned char   key;
        unsigned char   id;
    };

    struct group_node
    {
        unsigned short  is_group    : 1;
        unsigned short  next        : link_bits;
        unsigned short              : 6;
        unsigned short  hash[2];
    };

    typedef fixed_array<editor_module*, (1 << module_bits)> modules;

    friend class        bind_resolver;
    int                 insert_child(int parent, unsigned char key, bool has_params);
    int                 find_child(int parent, unsigned char key) const;
    int                 add_child(int parent, unsigned char key, bool has_params);
    int                 find_tail(int head);
    int                 append(int head, unsigned char key);
    const node&         get_node(unsigned int index) const;
    group_node*         get_group_node(unsigned int index);
    int                 alloc_nodes(unsigned int count=1);
    int                 add_module(editor_module& module);
    editor_module*      get_module(unsigned int index) const;
    modules             m_modules;
    node                m_nodes[1 << link_bits];
    unsigned int        m_next_node;
};
