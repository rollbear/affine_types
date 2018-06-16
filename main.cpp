#include <iostream>
#include <affine_types.hpp>
#include <deque>
#include <cassert>
#include <algorithm>


template <typename T>
class dq : std::deque<T>
{
  using impl = std::deque<T>;
public:
  using iterator = affine::position<typename impl::iterator, dq>;
  using difference_type = typename iterator::displacement;

  using std::deque<T>::deque;
  iterator begin() { return iterator{impl::begin()};}
  iterator end() { return iterator{impl::end()};}
  using impl::push_back;
  using impl::push_front;
  using impl::emplace_back;
  using impl::emplace_front;
  using impl::pop_back;
  using impl::pop_front;
  using impl::front;
  using impl::back;
};

template <size_t D, typename T>
class V
{
public:
  template <typename ... TT, std::enable_if_t<sizeof...(TT) == D>* = nullptr>
  constexpr V(TT&& ... t) noexcept((std::is_nothrow_constructible_v<T, TT> && ...)) : values{std::forward<TT>(t)...} {}
  constexpr bool operator==(const V& v) const noexcept(noexcept(std::declval<T>() == std::declval<T>())) {
    size_t i = 0;
    while (i != D && values[i] == v.values[i])
      ++i;
    return i == D;
  }
  constexpr bool operator!=(const V& v) const noexcept(noexcept(std::declval<T>() == std::declval<T>())){
    return !(*this == v);
  }
  constexpr V& operator+=(const V& v) noexcept(noexcept(std::declval<T&>() += std::declval<T>())){
    for (size_t i = 0; i != D; ++i)
    {
      values[i] += v.values[i];
    }
    return *this;
  }
  constexpr V& operator-=(const V& v) noexcept(noexcept(std::declval<T&>() -= std::declval<T>())){
    for (size_t i = 0; i != D; ++i)
    {
      values[i] -= v.values[i];
    }
    return *this;
  }
  template <typename S, std::enable_if_t<std::is_scalar_v<S>>* = nullptr>
  constexpr V& operator/=(S v) noexcept(noexcept(std::declval<T&>() /= v))
  {
    for (size_t i = 0; i != D; ++i)
    {
      values[i] /= v;
    }
    return *this;
  }
  template <typename S, std::enable_if_t<std::is_scalar_v<S>>* = nullptr>
  constexpr V& operator*=(S v) noexcept(noexcept(std::declval<T&>() /= v))
  {
    for (size_t i = 0; i != D; ++i)
    {
      values[i] *= v;
    }
    return *this;
  }
private:
  T values[D];
};

template <size_t D, typename T>
constexpr auto operator+(V<D,T> lh, V<D,T> rh)
AFFINE_TYPE_THRICE(V<D,T>(lh += rh))

template <size_t D, typename T>
constexpr auto operator-(V<D,T> lh, V<D,T> rh)
AFFINE_TYPE_THRICE(V<D,T>(lh -= rh))

template <size_t D, typename T, typename S>
constexpr auto operator/(V<D,T> lh, S rh)
AFFINE_TYPE_THRICE(V<D,T>(lh/= rh))

template <size_t D, typename T, typename S>
constexpr auto operator*(V<D,T> lh, S rh)
AFFINE_TYPE_THRICE(V<D,T>(lh*= rh))

template <size_t D, typename T, typename S>
constexpr auto operator*(S lh, V<D,T> rh)
AFFINE_TYPE_THRICE(V<D,T>(rh*= lh))

struct S {
  int value;
};

using int_d = affine::displacement<int, struct tag_d>;
using int_p = affine::position<int, struct tag_p, int_d>;
using Spp = affine::position<S*, struct tag_Spp>;
using V2i_p = affine::position<V<2,int>, struct tag_V2ip>;
using V2i_d = V2i_p::displacement;
static_assert(std::is_same_v<affine::displacement<V<2,int>, struct tag_V2ip>, V2i_d>);

template <typename A, typename B = A>
using Addition = decltype(std::declval<A>() + std::declval<B>());
template <typename A, typename B = A>
using Subtraction = decltype(std::declval<A>() - std::declval<B>());
template <typename A, typename B = A>
  using Division = decltype(std::declval<A>() / std::declval<B>());
template <typename A, typename B = A>
  using Multiplication = decltype(std::declval<A>() * std::declval<B>());

template <typename A>
using Dereference = decltype(*std::declval<A>());

template <typename A>
using PreIncrement = decltype(++std::declval<A&>());

template <typename A>
using PostIncrement = decltype(std::declval<A&>()++);

template <typename A>
using PreDecrement = decltype(--std::declval<A&>());

template <typename A>
using PostDecrement = decltype(std::declval<A&>()--);

template <template <typename ...> class, typename...>
  struct detected_helper : std::false_type {};

template <template <typename ...> class D, typename ... Ts>
  struct detected_helper<D, std::void_t<D<Ts...>>, Ts...> : std::true_type {};

template <template <typename ...> class D, typename ... Ts>
  using is_detected = typename detected_helper<D, void, Ts...>::type;


static_assert(!is_detected<Addition, int_p>{});
static_assert(is_detected<Addition, int_p, int_d>{});
static_assert(is_detected<Addition, int_d, int_p>{});
static_assert(!is_detected<Addition, int_p, int>{});
static_assert(std::is_same<Addition<int_p, int_d>, int_p>{});
static_assert(std::is_same<Addition<int_d, int_p>, int_p>{});

static_assert(!is_detected<Addition, V2i_p>{});
static_assert(is_detected<Addition, V2i_p, V2i_d>{});
static_assert(is_detected<Addition, V2i_d, V2i_p>{});
static_assert(!is_detected<Addition, V2i_p, V<2,int>>{});
static_assert(std::is_same<Addition<V2i_p, V2i_d>, V2i_p>{});
static_assert(std::is_same<Addition<V2i_d, V2i_p>, V2i_p>{});


static_assert(is_detected<Addition, int_d>{});
static_assert(!is_detected<Addition, int_d, int>{});
static_assert(!is_detected<Addition, int, int_d>{});
static_assert(std::is_same<Addition<int_d>, int_d>{});

static_assert(is_detected<Addition, V2i_d>{});
static_assert(!is_detected<Addition, V2i_d, V<2,int>>{});
static_assert(!is_detected<Addition, V<2,int>, V2i_d>{});
static_assert(std::is_same<Addition<V2i_d, V2i_d>, V2i_d>{});



static_assert(is_detected<Subtraction, int_p>{});
static_assert(is_detected<Subtraction, int_p, int_d>{});
static_assert(!is_detected<Subtraction, int_d, int_p>{});
static_assert(std::is_same<Subtraction<int_p>, int_d>{});
static_assert(std::is_same<Subtraction<int_p, int_d>, int_p>{});

static_assert(is_detected<Subtraction, V2i_p>{});
static_assert(is_detected<Subtraction, V2i_p, V2i_d>{});
static_assert(!is_detected<Subtraction, V2i_d, V2i_p>{});
static_assert(std::is_same<Subtraction<V2i_p>, V2i_d>{});
static_assert(std::is_same<Subtraction<V2i_p, V2i_d>, V2i_p>{});


static_assert(is_detected<Subtraction, int_d>{});
static_assert(!is_detected<Subtraction, int_d, int>{});
static_assert(!is_detected<Subtraction, int, int_d>{});
static_assert(std::is_same<Subtraction<int_d>, int_d>{});

static_assert(is_detected<Subtraction, V2i_d>{});
static_assert(!is_detected<Subtraction, V2i_d, V<2,int>>{});
static_assert(!is_detected<Subtraction, V<2,int>, V2i_d>{});
static_assert(std::is_same<Subtraction<V2i_d>, V2i_d>{});



static_assert(!is_detected<Division, int_p>{});
static_assert(!is_detected<Division, int_p, int_d>{});
static_assert(!is_detected<Division, int_d, int_p>{});

static_assert(!is_detected<Division, V2i_p>{});
static_assert(!is_detected<Division, V2i_p, V2i_d>{});
static_assert(!is_detected<Division, V2i_d, V2i_p>{});
static_assert(!is_detected<Division, V2i_p, V<2,int>>{});
static_assert(!is_detected<Division, V2i_p, int>{});


static_assert(is_detected<Division, int_d>{});
static_assert(is_detected<Division, int_d, int>{});
static_assert(!is_detected<Division, int, int_d>{});
static_assert(std::is_same<Division<int_d>, int>{});
static_assert(std::is_same<Division<int_d, int>, int_d>{});

static_assert(!is_detected<Division, V2i_d>{});
static_assert(!is_detected<Division, V2i_d, V<2,int>>{});
static_assert(is_detected<Division, V2i_d, int>{});
static_assert(std::is_same<Division<V2i_d, int>, V2i_d>{});



static_assert(!is_detected<Multiplication, int_p>{});
static_assert(!is_detected<Multiplication, int_p, int_d>{});
static_assert(!is_detected<Multiplication, int_d, int_p>{});
static_assert(!is_detected<Multiplication, int_p, int>{});
static_assert(!is_detected<Multiplication, int, int_p>{});

static_assert(!is_detected<Multiplication, V2i_p>{});
static_assert(!is_detected<Multiplication, V2i_p, V2i_d>{});
static_assert(!is_detected<Multiplication, V2i_d, V2i_p>{});
static_assert(!is_detected<Multiplication, V2i_p, V<2,int>>{});
static_assert(!is_detected<Multiplication, V<2,int>, V2i_p>{});
static_assert(!is_detected<Multiplication, V2i_p, int>{});
static_assert(!is_detected<Multiplication, int, V2i_p>{});


static_assert(!is_detected<Multiplication, int_d>{});
static_assert(is_detected<Multiplication, int_d, int>{});
static_assert(is_detected<Multiplication, int, int_d>{});
static_assert(std::is_same<Multiplication<int_d, int>, int_d>{});
static_assert(std::is_same<Multiplication<int, int_d>, int_d>{});

static_assert(!is_detected<Multiplication, V2i_d>{});
static_assert(!is_detected<Multiplication, V2i_d, V<2,int>>{});
static_assert(!is_detected<Multiplication, V<2,int>, V2i_d>{});
static_assert(is_detected<Multiplication, V2i_d, int>{});
static_assert(is_detected<Multiplication, int, V2i_d>{});
static_assert(std::is_same<Multiplication<V2i_d, int>, V2i_d>{});
static_assert(std::is_same<Multiplication<int, V2i_d>, V2i_d>{});

static_assert(!is_detected<Dereference, int_p>{});
static_assert(is_detected<Dereference, Spp>{});
static_assert(std::is_same<S&, decltype(*std::declval<Spp>())>{});
static_assert(std::is_same<S&, decltype(*std::declval<const Spp>())>{});
static_assert(std::is_same<Spp::displacement, affine::displacement<ptrdiff_t, struct tag_Spp>>{});
static_assert(std::is_same<int, decltype(std::declval<Spp>()->value)>{});

static_assert(is_detected<PreIncrement, int_p>{});
static_assert(is_detected<PostIncrement, int_p>{});
static_assert(is_detected<PreDecrement, int_p>{});
static_assert(is_detected<PostDecrement, int_p>{});
static_assert(std::is_same<PreIncrement<int_p>, int_p&>{});
static_assert(std::is_same<PostIncrement<int_p>, int_p>{});
static_assert(std::is_same<PreDecrement<int_p>, int_p&>{});
static_assert(std::is_same<PostDecrement<int_p>, int_p>{});

static_assert(is_detected<PreIncrement, int_d>{});
static_assert(is_detected<PostIncrement, int_d>{});
static_assert(is_detected<PreDecrement, int_d>{});
static_assert(is_detected<PostDecrement, int_d>{});
static_assert(std::is_same<PreIncrement<int_d>, int_d&>{});
static_assert(std::is_same<PostIncrement<int_d>, int_d>{});
static_assert(std::is_same<PreDecrement<int_d>, int_d&>{});
static_assert(std::is_same<PostDecrement<int_d>, int_d>{});

static_assert(!is_detected<PreIncrement, V2i_p>{});
static_assert(!is_detected<PostIncrement, V2i_p>{});
static_assert(!is_detected<PreDecrement, V2i_p>{});
static_assert(!is_detected<PostDecrement, V2i_p>{});

static_assert(!is_detected<PreIncrement, V2i_d>{});
static_assert(!is_detected<PostIncrement, V2i_d>{});
static_assert(!is_detected<PreDecrement, V2i_d>{});
static_assert(!is_detected<PostDecrement, V2i_d>{});

static_assert(std::is_nothrow_constructible<int_d, int>{});
static_assert(std::is_nothrow_copy_constructible<int_d>{});
static_assert(std::is_nothrow_move_constructible<int_d>{});
static_assert(std::is_nothrow_constructible<int_p, int>{});
static_assert(std::is_nothrow_copy_constructible<int_p>{});
static_assert(std::is_nothrow_move_constructible<int_p>{});
static_assert(int_d{5}-int_d{2} == int_d{3});
static_assert(int_d{5}+int_d{2} == int_d{7});
static_assert(int_p{7}-int_p{2} == int_d{5});
static_assert(int_p{7}-int_d{2} == int_p{5});
static_assert(int_p{7}+int_d{2} == int_p{9});
static_assert(int_d{3}*2 == int_d{6});
static_assert(2*int_d{3} == int_d{6});
static_assert(int_d{6}/2 == int_d{3});
static_assert(int_d{6}/int_d{3} == 2);
static_assert(noexcept(int_d{5}+int_d{2}));
static_assert(noexcept(int_d{5}+=int_d{2}));
static_assert(noexcept(int_d{5}-int_d{2}));
static_assert(noexcept(int_d{5}-=int_d{2}));
static_assert(noexcept(++int_d{5}));
static_assert(noexcept(--int_d{5}));
static_assert(noexcept(int_d{5}--));
static_assert(noexcept(int_d{5}--));
static_assert(noexcept(int_d{5}/int_d{2}));
static_assert(noexcept(int_d{5}/2));
static_assert(noexcept(int_d{5}/=2));
static_assert(noexcept(int_d{5}*2));
static_assert(noexcept(2*int_d{5}));
static_assert(noexcept(int_d{5}*= 2));
static_assert(noexcept(int_p{2}+int_d{2}));
static_assert(noexcept(int_d{2}+int_p{2}));
static_assert(noexcept(int_p{5}-int_d{2}));
static_assert(noexcept(++int_p{5}));
static_assert(noexcept(--int_p{5}));
static_assert(noexcept(int_p{5}++));
static_assert(noexcept(int_p{5}--));
static_assert(V2i_d{15,9}/3 == V2i_d{5,3});
static_assert(V2i_d{3,5}*2 == V2i_d{6,10});
static_assert(2*V2i_d{3,5} == V2i_d{6,10});

template <typename T>
struct is_affine_position : std::false_type {};

template <typename T, typename Tag, typename D>
struct is_affine_position<affine::position<T, Tag, D>> : std::true_type {};

template <typename T>
struct is_affine_displacement : std::false_type {};

template <typename T, typename Tag>
struct is_affine_displacement<affine::displacement<T, Tag>> : std::true_type {};

int main()
{
  dq<int> q{3,4,5};
  auto b = q.begin();
  auto i = b;
  static_assert(is_affine_position<decltype(b)>{});
  assert(*b == 3);
  i++;
  assert(*i == 4);
  auto d = i - b;
  static_assert(is_affine_displacement<decltype(d)>{});
  assert(d == dq<int>::difference_type {1});
  *i = 1;
  assert(*++b == 1);
  std::puts("Cool!");
}
