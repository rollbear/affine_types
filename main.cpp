#include <iostream>
#include <affine_types.hpp>
#include <deque>
#include <cassert>

using int_d = affine::displacement<int, struct tag_d>;
using int_p = affine::position<int, struct tag_p, int_d>;

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

struct S {
  int value;
};
using Spp = affine::position<S*, struct tag_Spp>;

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

static_assert(is_detected<Addition, int_d>{});
static_assert(!is_detected<Addition, int_d, int>{});
static_assert(!is_detected<Addition, int, int_d>{});
static_assert(std::is_same<Addition<int_d>, int_d>{});


static_assert(is_detected<Subtraction, int_p>{});
static_assert(is_detected<Subtraction, int_p, int_d>{});
static_assert(!is_detected<Subtraction, int_d, int_p>{});
static_assert(std::is_same<Subtraction<int_p>, int_d>{});
static_assert(std::is_same<Subtraction<int_p, int_d>, int_p>{});

static_assert(is_detected<Subtraction, int_d>{});
static_assert(!is_detected<Subtraction, int_d, int>{});
static_assert(!is_detected<Subtraction, int, int_d>{});
static_assert(std::is_same<Subtraction<int_d>, int_d>{});


static_assert(!is_detected<Division, int_p>{});
static_assert(!is_detected<Division, int_p, int_d>{});
static_assert(!is_detected<Division, int_d, int_p>{});

static_assert(is_detected<Division, int_d>{});
static_assert(is_detected<Division, int_d, int>{});
static_assert(!is_detected<Division, int, int_d>{});
static_assert(std::is_same<Division<int_d>, int>{});
static_assert(std::is_same<Division<int_d, int>, int_d>{});


static_assert(!is_detected<Multiplication, int_p>{});
static_assert(!is_detected<Multiplication, int_p, int_d>{});
static_assert(!is_detected<Multiplication, int_d, int_p>{});
static_assert(!is_detected<Multiplication, int_p, int>{});
static_assert(!is_detected<Multiplication, int, int_p>{});

static_assert(!is_detected<Multiplication, int_d>{});
static_assert(is_detected<Multiplication, int_d, int>{});
static_assert(is_detected<Multiplication, int, int_d>{});
static_assert(std::is_same<Multiplication<int_d, int>, int_d>{});
static_assert(std::is_same<Multiplication<int, int_d>, int_d>{});

static_assert(!is_detected<Dereference, int_p>{});
static_assert(is_detected<Dereference, Spp>{});
static_assert(std::is_same<S&, decltype(*std::declval<Spp>())>{});
static_assert(std::is_same<S&, decltype(*std::declval<const Spp>())>{});
static_assert(std::is_same<Spp::displacement, affine::displacement<ptrdiff_t, struct tag_Spp>>{});
static_assert(std::is_same<int, decltype(std::declval<Spp>()->value)>{});

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
