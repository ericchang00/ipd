#pragma once

#include "symbol.h"

#include <memory>
#include <stdexcept>
#include <string>

namespace islpp {

struct binding_not_found : std::runtime_error
{
    binding_not_found(const Symbol& sym)
            : runtime_error("Not found: " + sym.name()) { }
};

template<typename V>
class env_ptr
{
public:
    // The empty environment
    env_ptr();

    // Creates a new environment with an additional binding.
    env_ptr extend(const Symbol&, const V&) const;

    // Creates a new environment with an additional binding.
    env_ptr extend(const std::string&, const V&) const;

    // Updates an existing binding; throws `binding_not_found` otherwise.
    void update(const Symbol&, const V&);

    // Looks up the value of a binding or throws `binding_not_found`.
    const V& lookup(const Symbol&) const;

private:
    struct node;
    using node_ptr = std::shared_ptr<node>;

    env_ptr(const Symbol&, const V&, const node_ptr&);

    node_ptr head_;
};

template<typename V>
struct env_ptr<V>::node
{
    node(const Symbol& k, const V& v, const node_ptr& n)
            : key(k), value(v), next(n) { }

    Symbol   key;
    V        value;
    node_ptr next;
};

template<typename V>
env_ptr<V>::env_ptr()
        : head_{nullptr} { }

template<typename V>
env_ptr<V>::env_ptr(const Symbol& key, const V& value, const node_ptr& next)
        : head_{std::make_shared<node>(key, value, next)} { }

template<typename V>
const V& env_ptr<V>::lookup(const Symbol& key) const
{
    for (node* curr = head_.get(); curr != nullptr; curr = curr->next.get()) {
        if (curr->key == key) return curr->value;
    }

    throw binding_not_found{key};
}

template<typename V>
env_ptr<V> env_ptr<V>::extend(const Symbol& key, const V& value) const
{
    return env_ptr<V>{key, value, head_};
}

template<typename V>
env_ptr<V> env_ptr<V>::extend(const std::string& key, const V& value) const {
    return extend(intern(key), value);
}

template<typename V>
void env_ptr<V>::update(const Symbol& key, const V& value)
{
    for (node* curr = head_.get(); curr != nullptr; curr = curr->next.get()) {
        if (curr->key == key) {
            curr->value = value;
            return;
        }
    }

    throw binding_not_found{key};
}

}
