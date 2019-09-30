// TAVL -*- C++ -*-

// BSD 3-Clause License

// Copyright (c) 2018-2019, wxr001
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.

// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "../TAVL.hpp"
#include <iostream>
template <typename T>
struct identity
{
	using type = T;
};
template <typename... elem>
struct type_list;
template <typename T, int idx = 0>
struct type_list_helper;
template <typename elem, typename... tails, int idx>
struct type_list_helper<type_list<elem, tails...>, idx> : identity<elem>
{
    static constexpr int  index = idx;
    static constexpr auto size  = 1 + sizeof...(tails);
    using tail                  = type_list<tails...>;
    template <typename T>
    using push_back = type_list<elem, tails..., T>;
    template <typename T>
    using push_front = type_list<T, elem, tails...>;
    template <typename T, int n>
    struct push_back_n_helper
    {
        using type = typename type_list_helper<
            typename push_back_n_helper<T, n - 1>::type>::template push_back<T>;
    };
    template <typename T>
    struct push_back_n_helper<T, 0> : identity<type_list<elem, tails...>>
    {
    };
    template <typename T, int n>
    using push_back_n = typename push_back_n_helper<T, n>::type;
    template <typename T>
    struct push_back_m_helper : identity<push_back<T>>
    {
    };
    template <typename... elems>
    struct push_back_m_helper<type_list<elems...>>
        : identity<type_list<elem, tails..., elems...>>
    {
    };
    template <typename T>
    using push_back_m = typename push_back_m_helper<T>::type;

    template <typename T>
    static constexpr int
        find = (std::is_same_v<elem, T> ?
                    index :
                    type_list_helper<tail, index + 1>::template find<T>);
    template <int n>
    struct lazy_get_false
        : identity<typename type_list_helper<tail, index + 1>::template get<n>>
    {
    };
    template <int n>
    using get = typename std::
        conditional_t<index == n, identity<elem>, lazy_get_false<n>>::type;
};
template <int idx>
struct type_list_helper<type_list<>, idx>
{
    static constexpr int  index = -1;
    static constexpr auto size  = 0;
    using tail                  = type_list<>;
    template <typename T>
    using push_back = type_list<T>;
    template <typename T>
    using push_front = type_list<T>;
    template <typename T, int n>
    struct push_back_n_helper
    {
        using type = typename type_list_helper<
            typename push_back_n_helper<T, n - 1>::type>::template push_back<T>;
    };
    template <typename T>
    struct push_back_n_helper<T, 0> : identity<type_list<>>
    {
    };
    template <typename T, int n>
    using push_back_n = typename push_back_n_helper<T, n>::type;
    template <typename T>
    struct push_back_m_helper : identity<push_back<T>>
    {
    };
    template <typename... elems>
    struct push_back_m_helper<type_list<elems...>>
        : identity<type_list<elems...>>
    {
    };
    template <typename T>
    using push_back_m = typename push_back_m_helper<T>::type;

    template <template <typename> typename Apply>
    struct for_each_helper
    {
        using type = type_list<>;
    };

    template <typename T>
    static constexpr int find = -1;
};
template <typename T>
inline constexpr auto type_list_size = type_list_helper<T>::size;
template <typename T>
using type_list_top = typename type_list_helper<T>::type;
template <typename T>
using type_list_tail = typename type_list_helper<T>::tail;
template <typename T, typename val>
using type_list_push_back =
    typename type_list_helper<T>::template push_back<val>;
template <typename T, typename val>
using type_list_push_front =
    typename type_list_helper<T>::template push_front<val>;
template <typename T, typename val, int n>
using type_list_push_back_n =
    typename type_list_helper<T>::template push_back_n<val, n>;
template <typename T, typename val>
using type_list_push_back_m =
    typename type_list_helper<T>::template push_back_m<
        val>; //! merge val into T if val is type_list, push_back otherwise
template <typename T, template <typename> typename Apply>
struct type_list_for_each_helper;
template <typename elem, template <typename> typename Apply, typename... tails>
struct type_list_for_each_helper<type_list<elem, tails...>, Apply>
{
    using type = type_list_push_front<
        typename type_list_for_each_helper<type_list<tails...>, Apply>::type,
        typename Apply<elem>::type>;
};
template <template <typename> typename Apply>
struct type_list_for_each_helper<type_list<>, Apply>
{
    using type = type_list<>;
};
template <typename T, template <typename> typename Apply>
using type_list_for_each = typename type_list_for_each_helper<T, Apply>::type;

template <typename T, typename val, int idx = 0>
struct type_list_find_helper
{
    static constexpr int value =
        (std::is_same_v<type_list_top<T>, val> ?
             idx :
             type_list_find_helper<type_list_tail<T>, val, idx + 1>::value);
};
template <typename val, int idx>
struct type_list_find_helper<type_list<>, val, idx>
{
    static constexpr int value = -1; // not fount
};
template <typename T, typename val>
inline constexpr int type_list_find = type_list_helper<T>::template find<val>;

int main()
{
	using search_first = type_list_push_front<type_list_push_back_n<type_list<>, int, 2000>, float>;
	using search_last = type_list_push_back<type_list_push_back_n<type_list<>, int, 2000>, float>;
	using search_middle = type_list_push_back_n<type_list_push_back<type_list_push_back_n<type_list<>, int, 1000>, float>, int, 1000>;
    int result_1 = type_list_find<search_first, float> + type_list_find<search_middle, float> + type_list_find<search_last, float>;
    std::cout<<result_1<<std::endl;
    return 0;
}
