#include "dyn_cast.hpp"

struct T {
    int x;
    virtual char f() {
        return 'T';
    }
};

struct S : virtual T {
    int x;
    char f() override {
        return 'S';
    }
};
struct A : virtual T {
    int x;
    char f() override {
        return 'A';
    }
};
struct V : S, A {
    int x;
    char f() override {
        return 'V';
    }
};

#include <cassert>
int main() {
    V* v = new V;
    A* ar = v;
    S* sr = v;
    T* tp = v;
    assert(internal::Typeid(ar) == &typeid(*ar));
    assert(internal::GetMostDerived(ar) == (void*)v);
    assert(DynamicCast<T*>(v));
    assert(DynamicCast<S*>(ar));
    assert(DynamicCast<A*>(sr));
    assert(DynamicCast<V*>(tp)->f() == 'V');
    assert(DynamicCast<S*>(ar)->f() == 'V');

}