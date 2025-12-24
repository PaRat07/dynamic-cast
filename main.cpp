#include "dyn_cast.hpp"

struct T {
    int x;
    virtual char f() {
        return 'T';
    }
};
struct T1 {
    int x;
    virtual char f() {
        return 'T';
    }
};

struct S : virtual T, virtual T1 {
    int x;
    char f() override {
        return 'S';
    }
};
struct A : virtual T, virtual T1 {
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
    assert(DynamicCast<T1*>(ar) == DynamicCast<T1*>(sr));
    assert(DynamicCast<T*>(ar) == DynamicCast<T*>(sr));
    assert(DynamicCast<A*>(sr));
    assert(DynamicCast<V*>(tp)->f() == 'V');
    assert(DynamicCast<S*>(ar)->f() == 'V');

}