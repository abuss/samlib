#pragma once

#include <cstddef>
#include <utility>

namespace samlib {

namespace ports_impl {

// Implementation based on: 
// https://codereview.stackexchange.com/questions/44546/very-basic-tuple-implementation/44832#44832

// helpers
template <typename T>
struct id { using type = T; };

template <typename T>
using type_of = typename T::type;

template <size_t... N>
struct sizes : id <sizes <N...> > { };

// choose N-th element in list <T...>
template <size_t N, typename... T>
struct Choose;

template <size_t N, typename H, typename... T>
struct Choose <N, H, T...> : Choose <N-1, T...> { };

template <typename H, typename... T>
struct Choose <0, H, T...> : id <H> { };

template <size_t N, typename... T>
using choose = type_of <Choose <N, T...> >;

// given L>=0, generate sequence <0, ..., L-1>
template <size_t L, size_t I = 0, typename S = sizes <> >
struct Range;

template <size_t L, size_t I, size_t... N>
struct Range <L, I, sizes <N...> > : Range <L, I+1, sizes <N..., I> > { };

template <size_t L, size_t... N>
struct Range <L, L, sizes <N...> > : sizes <N...> { };

template <size_t L>
using range = type_of <Range <L> >;

// single tuple element
template <size_t N, typename T>
class TupleElem
{
    T elem;
public:
    TupleElem() = default;

    TupleElem(T&& e) : elem{e} { }

	T&       get()       { return elem; }
	const T& get() const { return elem; }

    template<typename Val>
    bool send_to(const Val& val)
    {
        elem->send(val);
        return true;
    }
};

// tuple implementation
template <typename N, typename... T>
class TupleImpl;

template <size_t... N, typename... T>
class TupleImpl <sizes <N...>, T...> : TupleElem <N, T>...
{
	template <size_t M> using pick = choose <M, T...>;
	template <size_t M> using elem = TupleElem <M, pick <M> >;

public:
    TupleImpl() = default;

    TupleImpl(T&&... args) : TupleElem <N, T>(std::forward<T>(args))... { }

	template <size_t M>
	pick <M>& get() { return elem <M>::get(); }

	template <size_t M>
	const pick <M>& get() const { return elem <M>::get(); }

	template <size_t M, typename Val>
	bool send_to(const Val& val) { return elem <M>::send_to(val); }
};

} // namespace ports_impl


template <typename... T>
struct ports 
  : ports_impl::TupleImpl <ports_impl::range <sizeof...(T)>, T...>
{
    ports() = default;

    ports(T&&... args) 
      : ports_impl::TupleImpl <ports_impl::range <sizeof...(T)>, T...>(std::forward<T>(args)...)
    { }

    static constexpr std::size_t size() { return sizeof...(T); }
};

} // namespace samlib
