# Dynamic cast written in c++

My target was to practice my c++ rtti knowledge. \
This is implementation of dynamic_cast. For more experience I didnt use some language features:
- typeid(expr)
- dynamic_cast
- any libraries(except <typeinfo> and cxxabi.h for types of typeinfo(like abi::__si_class_type_info))\
Only knowledge my code gets from language about rtti is typeid(type) (wrapped in kTypeInfo\<T> for simpler control of use of forbidden features). \
This implementation is full of ub, but will likely work correctly.

