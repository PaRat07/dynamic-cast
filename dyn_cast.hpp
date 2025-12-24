#include <typeinfo>
#include "cxxabi.h"

template<typename T>
const std::type_info& kTypeInfo = typeid(T);

namespace internal {
bool Equal(const std::type_info* lhs, const std::type_info* rhs) {
    return __builtin_strcmp(lhs->name(), rhs->name()) == 0;
}


const void* GetVtablePtr(const void* obj) {
    const void* res;
    __builtin_memcpy(&res, obj, sizeof(void*));
    return res;
}


const std::type_info* Typeid(const void* obj) {
    return *(reinterpret_cast<const std::type_info*const*>(GetVtablePtr(obj)) - 1);
}

const void* GetMostDerived(const void* ptr) {
    return static_cast<const char*>(ptr) + *(static_cast<const ptrdiff_t*>(GetVtablePtr(ptr)) - 2);
}

const void* DynamicUpcast(const void* obj, const std::type_info* typeinfo_from, const std::type_info* typeinfo_to, bool& ambigous_or_private) {
    if (Equal(typeinfo_from, typeinfo_to)) {
        return obj;
    } else if (Equal(Typeid(typeinfo_from), &kTypeInfo<abi::__si_class_type_info>)) {
        return DynamicUpcast(obj, static_cast<const abi::__si_class_type_info*>(typeinfo_from)->__base_type, typeinfo_to, ambigous_or_private);
    } else if (Equal(Typeid(typeinfo_from), &kTypeInfo<abi::__vmi_class_type_info>)) {
        const auto* ti_fr = static_cast<const abi::__vmi_class_type_info*>(typeinfo_from);
        const void* result = nullptr;
        for (int i = 0; i < ti_fr->__base_count; ++i) {
            const abi::__base_class_type_info* bcti = &ti_fr->__base_info[i];
            const void* base_obj;
            bool public_base = bcti->__offset_flags & abi::__base_class_type_info::__public_mask;
            if (bcti->__offset_flags & abi::__base_class_type_info::__virtual_mask) {
                base_obj = static_cast<const char*>(GetMostDerived(obj)) + *(static_cast<const ptrdiff_t*>(GetVtablePtr(obj)) + (bcti->__offset_flags >> 8));
            } else {
                base_obj = static_cast<const char*>(obj) + (bcti->__offset_flags >> 8);
            }
            const void* cast_candidate = DynamicUpcast(base_obj, bcti->__base_type, typeinfo_to, ambigous_or_private);
            if (ambigous_or_private) {
                return nullptr;
            }
            if (cast_candidate) {
                if (result && cast_candidate != result || !public_base) {
                    ambigous_or_private = true;
                    return nullptr;
                } else {
                    result = cast_candidate;
                }
            }
        }
        return result;
    } else {
        return nullptr;
    }

}

const void* DynamicCastImpl(const void* ptr, const std::type_info* typeinfo_to) {
    ptr = GetMostDerived(ptr);
    bool ambigous_or_private = false;
    return DynamicUpcast(ptr, Typeid(ptr), typeinfo_to, ambigous_or_private);
}

template<auto Val>
struct Cw {
    static constexpr auto value = Val;
};

template<typename T>
struct PtrTrats {
    static constexpr bool is_ptr = false;
    using deref_t = void;
    using cv = void;
};

template<typename T>
struct PtrTrats<T*> {
    static constexpr bool is_ptr = true;
    using deref_t = T;
    using cv = void;
};

template<typename T>
struct PtrTrats<T* const> {
    static constexpr bool is_ptr = true;
    using deref_t = T;
    using cv = const void;
};

template<typename T>
struct PtrTrats<T* volatile> {
    static constexpr bool is_ptr = true;
    using deref_t = T;
    using cv = volatile void;
};

template<typename T>
struct PtrTrats<T* const volatile> {
    static constexpr bool is_ptr = true;
    using deref_t = T;
    using cv = const volatile void;
};

template<typename T, typename U>
struct Same : Cw<false> {};

template<typename T>
struct Same<T, T> : Cw<true> {};
}

template<typename T, typename F>
T DynamicCast(F obj) {
    static_assert(internal::PtrTrats<T>::is_ptr);
    static_assert(internal::PtrTrats<F>::is_ptr);
    static_assert(internal::Same<typename internal::PtrTrats<T>::cv, typename internal::PtrTrats<T>::cv>::value);
    return reinterpret_cast<T>(const_cast<void*>(internal::DynamicCastImpl(obj, &kTypeInfo<typename internal::PtrTrats<T>::deref_t>)));
}
