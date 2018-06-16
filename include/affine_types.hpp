#ifndef AFFINE_TYPES_HPP
#define AFFINE_TYPES_HPP

#include <utility>

#define AFFINE_TYPE_THRICE(...)   \
  noexcept(noexcept(__VA_ARGS__)) \
  -> decltype(__VA_ARGS__)        \
  {                               \
    return __VA_ARGS__;           \
  }

namespace affine
{
template <typename T>
class value
{
public:
  template <typename ... TT, std::enable_if_t<std::is_constructible_v<T, TT...>>* = nullptr>
  constexpr explicit value(TT&& ... t) noexcept(std::is_nothrow_constructible_v<T,TT...>): value_{std::forward<TT>(t)...}{}

  constexpr T& get() & noexcept { return value_;}
  constexpr T&& get() && noexcept { return std::move(value_);}
  constexpr const T& get() const & noexcept { return value_;}
  constexpr const T&& get() const && noexcept { return std::move(value_);}
private:
  T value_;
};

template <typename T>
constexpr auto value_of(T&& t) noexcept -> decltype(std::forward<T>(t).get())
{
  return std::forward<T>(t).get();
}

template <typename T>
constexpr
auto operator==(value<T> lh, value<T> rh)
AFFINE_TYPE_THRICE(value_of(lh) == value_of(rh))

template <typename T>
constexpr
auto operator!=(value<T> lh, value<T> rh)
AFFINE_TYPE_THRICE(value_of(lh != value_of(rh)))

template <typename T, typename Tag>
class displacement : public value<T>
{
public:
  using value<T>::value;

  template <typename V>
  std::enable_if_t<std::is_scalar_v<V>, displacement&>
  operator/=(V v) noexcept(noexcept(std::declval<T&>()/= v)) { value_of(*this)/= v; return *this;}
  template <typename V>
  std::enable_if_t<std::is_scalar_v<V>, displacement&>
  operator*=(V v) noexcept(noexcept(std::declval<T&>()*= v)) { value_of(*this) *= v; return *this;}

  template <typename TT = T, typename R = std::enable_if_t<std::is_scalar_v<TT>, decltype(std::declval<TT>()/std::declval<TT>())>>
  constexpr R operator/(displacement d) const noexcept(noexcept(std::declval<T>()/std::declval<T>())){ return value_of(*this) / value_of(d);}

  constexpr displacement& operator+=(displacement d) noexcept(noexcept(std::declval<T&>()+= std::declval<T>())){ value_of(*this) += value_of(d); return *this; }
  constexpr displacement& operator-=(displacement d) noexcept(noexcept(std::declval<T&>()-= std::declval<T>())){ value_of(*this) -= value_of(d); return *this; }

  template <typename TT = T, typename = decltype(++std::declval<TT&>())>
  constexpr displacement& operator++() noexcept(noexcept(++std::declval<TT&>())){ value_of(*this)++; return *this;}
  template <typename TT = T, typename = decltype(++std::declval<TT&>())>
  constexpr displacement operator++(int) noexcept(noexcept(++std::declval<TT&>()) && std::is_nothrow_copy_constructible_v<T>){ auto v = *this; ++*this; return v;}
  template <typename TT = T, typename = decltype(--std::declval<TT&>())>
  constexpr displacement& operator--() noexcept(noexcept(--std::declval<TT&>())){ value_of(*this)--; return *this;}
  template <typename TT = T, typename = decltype(--std::declval<TT&>())>
  constexpr displacement operator--(int) noexcept(noexcept(--std::declval<TT&>()) && std::is_nothrow_copy_constructible_v<T>){ auto v = *this; --*this; return v;}

};

template <typename T, typename Tag>
constexpr
auto operator+(displacement<T, Tag> lh, displacement<T, Tag> rh)
AFFINE_TYPE_THRICE( displacement<T, Tag>{ value_of(lh) + value_of(rh)} )

template <typename T, typename Tag>
constexpr
auto operator-(displacement<T, Tag> lh, displacement<T, Tag> rh)
AFFINE_TYPE_THRICE( displacement<T, Tag>{ value_of(lh) - value_of(rh)} )

template <typename T, typename Tag, typename V>
constexpr
auto operator*(displacement<T, Tag> lh, V v)
AFFINE_TYPE_THRICE(displacement<T, Tag>(value_of(lh) * v))

template <typename T, typename Tag, typename V>
constexpr
auto operator*(V v, displacement<T, Tag> lh)
AFFINE_TYPE_THRICE(displacement<T, Tag>(value_of(lh) * v))

template <typename T, typename Tag, typename V>
constexpr
auto operator/(displacement<T, Tag> lh, V v)
AFFINE_TYPE_THRICE(displacement<T, Tag>(value_of(lh) / v))

template <typename T, typename Tag, typename D = displacement<decltype(std::declval<T>()-std::declval<T>()), Tag>>
class position : public value<T>
{
public:
  using displacement = D;

  using value<T>::value;

  constexpr position& operator+=(displacement d) noexcept(noexcept(std::declval<T&>() += value_of(d))){ value_of(*this) += value_of(d); return *this;}
  constexpr position& operator-=(displacement d) noexcept(noexcept(std::declval<T&>() -= value_of(d))){ value_of(*this) -= value_of(d); return *this;}

  template <typename TT = displacement, typename = decltype(std::declval<position&>() += ++std::declval<TT>())>
  constexpr position& operator++() noexcept(noexcept(std::declval<TT&>() += ++std::declval<displacement>())){ return *this += ++displacement(); }
  template <typename TT = position, typename = decltype(++std::declval<TT>())>
  constexpr position operator++(int) noexcept(noexcept(++std::declval<TT&>()) && std::is_nothrow_copy_constructible_v<T>){ auto v = *this; ++*this; return v;}
  template <typename TT = displacement, typename = decltype(std::declval<position&>() -= ++std::declval<TT>())>
  constexpr position& operator--() noexcept(noexcept(std::declval<TT&>() -= ++std::declval<displacement>())){ return *this -= ++displacement(); }
  template <typename TT = position, typename = decltype(--std::declval<TT&>())>
  constexpr position operator--(int) noexcept(noexcept(--std::declval<TT&>()) && std::is_nothrow_copy_constructible_v<T>){ auto v = *this; --*this; return v;}

  template <typename P = T>
  decltype(*(P())) operator*() const noexcept(noexcept(*std::declval<T&>())){ return *value_of(*this); }
  template <typename P = T>
  decltype(&*P()) operator->() const noexcept { return value_of(*this);}
};

template <typename T, typename Tag, typename D>
constexpr
auto operator-(position<T, Tag, D> lh, position <T, Tag, D> rh)
AFFINE_TYPE_THRICE(D{ value_of(lh) - value_of(rh)} )

template <typename T, typename Tag, typename D>
constexpr
auto operator-(position<T, Tag, D> lh, D rh)
AFFINE_TYPE_THRICE( position<T, Tag, D>{ value_of(lh) - value_of(rh) })

template <typename T, typename Tag, typename D>
constexpr
auto operator+(position<T, Tag, D> lh, D rh)
AFFINE_TYPE_THRICE( position<T, Tag, D>{ value_of(lh) + value_of(rh) })

template <typename T, typename Tag, typename D>
constexpr
auto operator+(D lh, position<T, Tag, D> rh)
AFFINE_TYPE_THRICE( position<T, Tag, D>{ value_of(lh) + value_of(rh) })

}

#endif //AFFINE_TYPES_HPP
