#pragma once
#include <memory>
#include <type_traits>
#include "null_param.h"
#include "traits.h"

namespace MetaNN
{
namespace NSMultiTypeDict
{
template<size_t N, template <typename...> class cont, typename... T>
struct Create_
{
	using type = Create_<N - 1, cont, NullParam(), T...>::type;
};

template<template <typename...> class cont, typename... T>
struct Create_<0, cont, T...>
{
	using type = cont<T...>;
};

//----------------------Type2ID----------------------------
template<typename TFindType, size_t N, typename TCurType, typename... Tags>
struct Tag2ID_
{
	constexpr static size_t value = Tag2ID_<TFindType, N + 1, Tags...>::value;
};

template<typename TFindType, size_t N, typename...Tags>
struct Tag2ID_<TFindType, N, TFindType, Tags...>
{
	constexpr static size_t value = N;
};

template<typename TFindTag, typename... Tags>
constexpr size_t Tag2ID = Tag2ID_<TFindTag, 0, Tags...>::value;

//----------------------------Pos2Type--------
template<size_t Pos, typename...TTypes>
struct Pos2Type_
{
	static_assert((Pos != 0), "Invalid position.");
};

template<size_t Pos, typename TCurType, typename... TTypes>
struct Pos2Type_
{
	using type = typename conditional_t<Pos == 0, Identity_<TCurType>, Pos2Type_<Pos - 1, TType...>>::type;
};

template<size_t Pos, typename... TTypes>
using Pos2Type = typename Pos2Type_<Pos, TTypes...>::type;

//-------------------------------NewTupleType---------------------
template <typename TVal, size_t N, size_t M, typename TProcessedTypes, typename... TRemainTypes>
struct NewTupleType_;

template<typename TVal, size_t N, size_t M, template<typename...> class Tcont,
	typename... TProcessedTypes, typename TCurType, typename... TRemainTypes>
struct NewTupleType_<TVal, N, M, Tcont<TProcessedTypes...>, TCurType, TRemainTypes...>
{
	using type = typename NewTupleType_<TVal, N, M + 1, Tcont<TProcessedTypes..., TCurType>, TRemainTypes...>::type;
};

template<typename TVal, size_t N, template<typename...> class Tcont,
	typename... TProcessedTypes, typename TCurType, typename... TRemainTypes>
struct NewTupleType_<TVal, N, N, Tcont<TProcessedTypes...>, TCurType, TRemainTypes...>
{
	using type = Tcont<TProcessedTypes..., TVal, TRemainTypes...>;
};

template<typename TVal, size_t N, typename Tcont, typename...TRemainTypes>
using NewTupleType = typename NewTupleType_<Tval, N, 0, Tcont, TRemainTypes...>::type;
}

template <typename... TParameters>
struct VarTypeDict
{
public:
	template <typename... TTypes>
	struct Values
	{
	public:
		Values() = default;
		Values(shared_ptr<void>(&& input)[sizeof...(TTypes)])
		{
			for (size_t i = 0; i < sizeof...(TTypes); i++)
			{
				m_tuple[i] = move(input[i]);
			}
		}
		template<typename TTag, typename TVal>
		auto Set(TVal&& val)&&
		{
			using namespace NSMultiTypeDict;
			constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;
			using rawVal = decay_t<TVal>;
			rawVal* tmp = new rawVal(forward<TVal>(val));
			m_tuple[TagPos] = shared_ptr<void>(tmp,
				[](void* ptr) {
					rawVal* nptr = static_cast<rawVal*>(ptr);
					delete nptr;
				});
			using new_type = NewTupleType<rawVal, TagPos, Value<>, TType...>;
			return new_type(move(m_tuple));
		}

		template<typename TTag>
		auto Get() const
		{
			using namespace NSMultiTypeDict;
			constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;
			using AimType = Pos2Type<TagPos, TTypes...>;
			void* tmp = m_tuple[TagPos].get();
			return static_cast<AimType*>(tmp);
		}
		template<typename TTag>
		using valueType = NSMultiTypeDict::Pos2Type<NSMultiTypeDict::Tag2ID<TTag, TParameters...>, TTypes...>;
	private:
		shared_ptr<void> m_tuple[sizeof...(TParameters)];
	};


public:
	static auto Create() {
		using namespace NSMultiTypeDict;
		using type = typename Create_<sizeof...(TParameters), Values>::type;
		return type{};
	}
};
};