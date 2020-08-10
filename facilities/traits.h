#pragma once
#include <type_traits>
namespace MetaNN
{
template<typename T>
struct Identity_
{
	using type = T;
};

//---------------OrValue---------
template <bool cur, typename TNext>
constexpr static bool OrValue = true;

template <typename TNext>
constexpr static bool OrValue<false, TNext> = TNext::value;

//---------------AndValue------------
template<bool cur, typename TNext>
constexpr static bool AndValue = false;

template<typename TNext>
constexpr static bool AndValue<true, TNext> = TNext::value;

//-----------------ArraySize-----------
template<typename T>
struct ArraySize_{};

template<template<typename...> class Tcont, typename... T>
struct ArraySize_<Tcont<T...>>
{
	constexpr static size_t value = sizeof...(T);
};

template<typename T>
constexpr static size_t ArraySize = ArraySize_<T>::value;

//-----------------IsEmptyArray----------------------------
template<typename T>
struct IsEmptyArray_{};

template<template<typename...> class Tcont, typename T1, typename... T2>
struct IsEmptyArray_<Tcont<T1, T2...>>
{
	constexpr static bool value = false;
};

template<template<typename...> class Tcont>
struct IsEmptyArray_<Tcont<>>
{
	constexpr static bool value = true;
};

template<typename T>
constexpr static bool IsEmptyArray = IsEmptyArray_<T>::value;

template<typename T>
using RemConstRef = std::remove_cv_t<std::remove_reference_t<T>>;
}