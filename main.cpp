#include "dyn_cast.hpp"

#include <cassert>
#include <iostream>

namespace not_ai {
struct T {
  int x;
  virtual char f() { return 'T'; }
};
struct T1 {
  int x;
  virtual char f() { return 'T'; }
};

struct S : virtual T, virtual T1 {
  int x;
  char f() override { return 'S'; }
};
struct A : virtual T, virtual T1 {
  int x;
  char f() override { return 'A'; }
};
struct V : S, A {
  int x;
  char f() override { return 'V'; }
};

int main() {
  V *v = new V;
  A *ar = v;
  S *sr = v;
  T *tp = v;
  assert(internal::Typeid(ar) == &typeid(*ar));
  assert(internal::GetMostDerived(ar) == (void *)v);
  assert(DynamicCast<T *>(v));
  assert(DynamicCast<S *>(ar));
  assert(DynamicCast<T1 *>(ar) == DynamicCast<T1 *>(sr));
  assert(DynamicCast<T *>(ar) == DynamicCast<T *>(sr));
  assert(DynamicCast<A *>(sr));
  assert(DynamicCast<V *>(tp)->f() == 'V');
  assert(DynamicCast<S *>(ar)->f() == 'V');
  return 0;
}
} // namespace not_ai

struct A {
  virtual ~A() {}
};
struct B : A {};
struct C : A {};
struct D : B, C {};
struct V {
  virtual ~V() {}
};
struct X : virtual V {};
struct Y : virtual V {};
struct Z : X, Y {};

void TestSingleInheritance() {
  B b;
  A *a1 = &b;
  B *b1 = DynamicCast<B *>(a1);
  assert(b1 == &b);
  std::cout << "Single inheritance test passed.\n";
}

void TestMultipleInheritance() {
  D d;
  B *b_ptr = &d;
  C *c_ptr = DynamicCast<C *>(b_ptr);
  assert(c_ptr == static_cast<C *>(&d)); // cross cast
  std::cout << "Multiple inheritance cross-cast test passed.\n";
}

void TestVirtualInheritance() {
  Z z;
  V *v_ptr = DynamicCast<V *>(&z);
  assert(v_ptr == static_cast<V *>(&z));
  std::cout << "Virtual inheritance test passed.\n";
}

void TestNullForWrongCast() {
  A a;
  C *c_ptr = DynamicCast<C *>(&a);
  assert(c_ptr == nullptr);
  std::cout << "Wrong cast returns nullptr test passed.\n";
}

void TestAmbiguity() {
  D d;
  A *a_ptr = static_cast<B *>(&d); // multiple paths to A
  A *result = DynamicCast<A *>(a_ptr);
  // Ambiguous, should return nullptr if your impl handles ambiguity
  std::cout << "Ambiguity test: " << (result == nullptr ? "passed" : "failed")
            << "\n";
}

int main() {
  TestSingleInheritance();
  TestMultipleInheritance();
  TestVirtualInheritance();
  TestNullForWrongCast();
  TestAmbiguity();
  not_ai::main();

  std::cout << "All tests finished.\n";
}