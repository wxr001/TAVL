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

#ifndef TAVL_TAVL
#define TAVL_TAVL

#include <type_traits>

namespace Compiler
{
    template <typename L, typename R>
    struct compare;
    template <typename L, typename R>
    static constexpr int compare_v = compare<L, R>::value;
    template <typename T>
    struct identity
    {
        using type = T;
    };
    namespace Impl
    {
        template <typename T>
        struct rotate_left;
        template <typename T>
        struct rotate_right;
        template <typename T>
        struct double_left;
        template <typename T>
        struct double_right;
        struct not_found;
        struct invalid;
    } // namespace Impl

    struct empty_node
    {
        using left                  = empty_node;
        using right                 = empty_node;
        static constexpr int height = -1;
        using key                   = void;
        using value                 = void;
    };
    template <typename V = Impl::not_found>
    struct value : public identity<V>
    {
    };
    template <typename L,
              typename R,
              int H,
              typename K,
              typename V = std::true_type>
    struct tavl_node
    {
        using left                  = L;
        using right                 = R;
        static constexpr int height = H;
        using key                   = K;
        using value                 = V;
    };
    template <typename T, typename K>
    struct tavl_find
    {
    private:
        static constexpr int comp_result = compare_v<K, typename T::key>;
        template <typename Key, typename Tree>
        struct lazy_less
            : public identity<
                  typename tavl_find<typename Tree::left, Key>::type>
        {
        };
        template <typename Key, typename Tree>
        struct lazy_more
            : public identity<
                  typename tavl_find<typename Tree::right, Key>::type>
        {
        };

    public:
        using type = typename std::conditional_t<
            comp_result == 0,
            identity<value<typename T::value>>,
            std::conditional_t<(comp_result > 0),
                               lazy_more<K, T>,
                               lazy_less<K, T>>>::type;
    };
    template <typename K>
    struct tavl_find<empty_node, K>
    {
        using type = value<>;
    };
    template <typename T, typename K>
    using tavl_find_t = typename tavl_find<T, K>::type;
    template <typename T, typename K>
    static constexpr bool tavl_contain_v =
        !std::is_same_v<tavl_find_t<T, K>, value<>>;
    template <typename T>
    struct tavl_min
    {
    private:
        template <typename TREE>
        struct lazy
            : public identity<typename tavl_min<typename TREE::left>::type>
        {
        };

    public:
        using type = typename std::conditional_t<
            std::is_same_v<typename T::left, empty_node>,
            identity<T>,
            lazy<T>>::type;
    };
    template <>
    struct tavl_min<empty_node>
    {
        using type = empty_node;
    };
    template <typename T>
    using tavl_min_t = typename tavl_min<T>::type;
    template <typename T>
    struct tavl_max
    {
    private:
        template <typename TREE>
        struct lazy
            : public identity<typename tavl_max<typename TREE::right>::type>
        {
        };

    public:
        using type =
            typename std::conditional_t<std::is_same_v<T::right, empty_node>,
                                        identity<T>,
                                        lazy<T>>::type;
    };
    template <>
    struct tavl_max<empty_node>
    {
        using type = empty_node;
    };
    template <typename T>
    using tavl_max_t = typename tavl_max<T>::type;
    template <typename T, typename K, typename V = std::true_type>
    struct tavl_insert
    {
    private:
        template <typename TREE, typename KL, typename VL>
        struct recursive_left
            : public identity<tavl_node<
                  typename tavl_insert<typename TREE::left, KL, VL>::type,
                  typename TREE::right,
                  TREE::height,
                  typename TREE::key,
                  typename TREE::value>>
        {
        };
        template <typename TREE, typename KL, typename VL>
        struct recursive_right
            : public identity<tavl_node<
                  typename TREE::left,
                  typename tavl_insert<typename TREE::right, KL, VL>::type,
                  TREE::height,
                  typename TREE::key,
                  typename TREE::value>>
        {
        };
        template <typename TREE>
        static constexpr int diff_height =
            TREE::left::height - TREE::right::height;
        template <typename TREE, typename KEY, typename VALUE>
        struct lazy_insert_left
            : public identity<typename std::conditional_t<
                  diff_height<
                      typename recursive_left<TREE, KEY, VALUE>::type> == 2,
                  std::conditional_t<
                      (compare_v<KEY,
                                 typename recursive_left<TREE, KEY, VALUE>::
                                     type::left::key> < 0),
                      Impl::rotate_left<
                          typename recursive_left<TREE, KEY, VALUE>::type>,
                      Impl::double_left<
                          typename recursive_left<TREE, KEY, VALUE>::type>>,
                  recursive_left<TREE, KEY, VALUE>>::type>
        {
        };
        template <typename TREE, typename KEY, typename VALUE>
        struct lazy_insert_right
            : public identity<typename std::conditional_t<
                  diff_height<
                      typename recursive_right<TREE, KEY, VALUE>::type> == -2,
                  std::conditional_t<
                      (compare_v<KEY,
                                 typename recursive_right<TREE, KEY, VALUE>::
                                     type::right::key>> 0),
                      Impl::rotate_right<
                          typename recursive_right<TREE, KEY, VALUE>::type>,
                      Impl::double_right<
                          typename recursive_right<TREE, KEY, VALUE>::type>>,
                  recursive_right<TREE, KEY, VALUE>>::type>
        {
        };
        static constexpr int comp_result = compare_v<K, typename T::key>;
        template <typename TREE>
        struct reset_height
            : public identity<tavl_node<typename TREE::type::left,
                                        typename TREE::type::right,
                                        (TREE::type::left::height >
                                                 TREE::type::right::height ?
                                             TREE::type::left::height + 1 :
                                             TREE::type::right::height + 1),
                                        typename TREE::type::key,
                                        typename TREE::type::value>>
        {
        };

    public:
        using type = typename std::conditional_t<
            comp_result == 0,
            identity<Impl::invalid>,
            std::conditional_t<(comp_result > 0),
                               reset_height<lazy_insert_right<T, K, V>>,
                               reset_height<lazy_insert_left<T, K, V>>>>::type;
    };
    template <typename K, typename V>
    struct tavl_insert<empty_node, K, V>
    {
        using type = tavl_node<empty_node, empty_node, 0, K, V>;
    };
    template <typename T, typename K, typename V = std::true_type>
    using tavl_insert_t = typename tavl_insert<T, K, V>::type;
    template <typename T, typename K>
    struct tavl_remove
    {
    private:
        static constexpr int comp_result = compare_v<K, typename T::key>;
        template <typename TREE>
        static constexpr int diff_height =
            TREE::left::height - TREE::right::height;
        template <typename TREE, typename KEY>
        struct recursion_left
            : public identity<tavl_node<
                  typename tavl_remove<typename TREE::left, KEY>::type,
                  typename TREE::right,
                  TREE::height,
                  typename TREE::key,
                  typename TREE::value>>
        {
        };
        template <typename TREE, typename KEY>
        struct recursion_right
            : public identity<tavl_node<
                  typename TREE::left,
                  typename tavl_remove<typename TREE::right, KEY>::type,
                  TREE::height,
                  typename TREE::key,
                  typename TREE::value>>
        {
        };
        template <typename TREE, typename KEY>
        struct lazy_left
            : public identity<typename std::conditional_t<
                  diff_height<typename recursion_left<TREE, KEY>::type> == -2,
                  std::conditional_t<
                      recursion_left<TREE, KEY>::type::right::right::height >=
                          recursion_left<TREE, KEY>::type::right::left::height,
                      Impl::rotate_right<
                          typename recursion_left<TREE, KEY>::type>,
                      Impl::double_right<
                          typename recursion_left<TREE, KEY>::type>>,
                  recursion_left<TREE, KEY>>::type>
        {
        };
        template <typename TREE, typename KEY>
        struct lazy_right
            : public identity<typename std::conditional_t<
                  diff_height<typename recursion_right<TREE, KEY>::type> == 2,
                  std::conditional_t<
                      (recursion_right<TREE, KEY>::type::left::left::height >=
                       recursion_right<TREE, KEY>::type::left::right::height),
                      Impl::rotate_left<
                          typename recursion_right<TREE, KEY>::type>,
                      Impl::double_left<
                          typename recursion_right<TREE, KEY>::type>>,
                  recursion_right<TREE, KEY>>::type>
        {
        };
        template <typename TREE, typename KEY>
        struct lazy_current
            : public identity<typename std::conditional_t<
                  std::is_same_v<typename TREE::left, empty_node> ||
                      std::is_same_v<typename TREE::right, empty_node>,
                  std::conditional_t<
                      std::is_same_v<typename TREE::left, empty_node>,
                      identity<typename TREE::right>,
                      identity<typename TREE::left>>,
                  identity<tavl_node<
                      typename TREE::left,
                      typename recursion_right<
                          TREE,
                          typename tavl_min_t<typename TREE::right>::key>::
                          type::right,
                      ((TREE::left::height >
                        recursion_right<
                            TREE,
                            typename tavl_min_t<typename TREE::right>::key>::
                            type::right::height) ?

                           TREE::left::height + 1 :
                           recursion_right<
                               TREE,
                               typename tavl_min_t<typename TREE::right>::key>::
                                   type::right::height +
                               1),
                      typename tavl_min_t<typename TREE::right>::key,
                      typename tavl_min_t<typename TREE::right>::value>>>::type>
        {
        };
        template <typename TREE>
        struct reset_height
            : public identity<std::conditional_t<
                  std::is_same_v<typename TREE::type, empty_node>,
                  empty_node,
                  tavl_node<typename TREE::type::left,
                            typename TREE::type::right,
                            (TREE::type::left::height >
                                     TREE::type::right::height ?
                                 TREE::type::left::height + 1 :
                                 TREE::type::right::height + 1),
                            typename TREE::type::key,
                            typename TREE::type::value>>>
        {
        };

    public:
        using type = typename std::conditional_t<
            (comp_result > 0),
            reset_height<lazy_right<T, K>>,
            std::conditional_t<(comp_result < 0),
                               reset_height<lazy_left<T, K>>,
                               reset_height<lazy_current<T, K>>>>::type;
    };
    template <typename K>
    struct tavl_remove<empty_node, K>
    {
        using type = empty_node;
    };
    template <typename T, typename K>
    using tavl_remove_t = typename tavl_remove<T, K>::type;
    namespace Impl
    {
        template <typename T>
        struct rotate_left
            : public identity<tavl_node<
                  typename T::left::left,
                  tavl_node<typename T::left::right,
                            typename T::right,
                            (T::left::right::height > T::right::height ?
                                 T::left::right::height + 1 :
                                 T::right::height + 1),
                            typename T::key,
                            typename T::value>,
                  (T::left::left::height >
                           (T::left::right::height > T::right::height ?
                                T::left::right::height + 1 :
                                T::right::height + 1) ?
                       T::left::left::height + 1 :
                       (T::left::right::height > T::right::height ?
                            T::left::right::height + 1 :
                            T::right::height + 1) +
                           1),
                  typename T::left::key,
                  typename T::left::value>>
        {
        };
        template <typename T>
        struct rotate_right
            : public identity<tavl_node<
                  tavl_node<typename T::left,
                            typename T::right::left,
                            (T::left::height > T::right::left::height ?
                                 T::left::height + 1 :
                                 T::right::left::height + 1),
                            typename T::key,
                            typename T::value>,
                  typename T::right::right,
                  (T::right::right::height >
                           (T::left::height > T::right::left::height ?
                                T::left::height + 1 :
                                T::right::left::height + 1) ?
                       T::right::right::height + 1 :
                       (T::left::height > T::right::left::height ?
                            T::left::height + 1 :
                            T::right::left::height + 1) +
                           1),
                  typename T::right::key,
                  typename T::right::value>>
        {
        };
        template <typename T>
        struct double_left
            : public identity<typename rotate_left<
                  tavl_node<typename rotate_right<typename T::left>::type,
                            typename T::right,
                            T::height,
                            typename T::key,
                            typename T::value>>::type>
        {
        };
        template <typename T>
        struct double_right
            : public identity<typename rotate_right<
                  tavl_node<typename T::left,
                            typename rotate_left<typename T::right>::type,
                            T::height,
                            typename T::key,
                            typename T::value>>::type>
        {
        };
    } // namespace Impl
#ifdef IN_HEADER_DEBUG
    // for tests only
    template <typename T, T lhs, T rhs>
    struct compare<std::integral_constant<T, lhs>,
                   std::integral_constant<T, rhs>>
    {
        static constexpr int value = lhs - rhs;
    };
    template <int val>
    using int_v = std::integral_constant<int, val>;
    namespace InHeaderDebug
    {
        //  tree          height
        //    5             2
        //  2   8           1
        // 1 3 7 9          0
        // key: node index
        // value: node index
        using test_avl_template = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_find_input    = test_avl_template;
        using test_contain_input = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<0>>,
                      tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<0>>,
                      1,
                      int_v<2>,
                      int_v<1>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<0>>,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<0>>,
                      1,
                      int_v<8>,
                      int_v<1>>,
            2,
            int_v<5>,
            int_v<2>>;
        using test_insert_result_no_new = test_avl_template;
        using test_insert_leftmost      = tavl_node<
            tavl_node<empty_node,
                      tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_insert_rightmost = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      empty_node,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_insert_normal_1 = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      empty_node,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_insert_normal_2 = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<empty_node,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_insert_result_new = tavl_node<
            tavl_node<
                tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                tavl_node<
                    empty_node,
                    tavl_node<empty_node, empty_node, 0, int_v<4>, int_v<4>>,
                    1,
                    int_v<3>,
                    int_v<3>>,
                2,
                int_v<2>,
                int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            3,
            int_v<5>,
            int_v<5>>;
        using test_insert_new                = test_avl_template;
        using test_insert_result_rotate_left = tavl_node<
            tavl_node<empty_node, empty_node, 0, int_v<2>, int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<4>, int_v<4>>,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<7>,
                      int_v<7>>,
            2,
            int_v<3>,
            int_v<3>>;
        using test_insert_rotate_left = tavl_node<
            tavl_node<empty_node, empty_node, 0, int_v<2>, int_v<2>>,
            tavl_node<empty_node,
                      tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      1,
                      int_v<4>,
                      int_v<4>>,
            2,
            int_v<3>,
            int_v<3>>;
        using test_insert_result_rotate_right =
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<2>, int_v<2>>,
                      tavl_node<empty_node, empty_node, 0, int_v<4>, int_v<4>>,
                      1,
                      int_v<3>,
                      int_v<3>>;
        using test_insert_rotate_right =
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      empty_node,
                      1,
                      int_v<4>,
                      int_v<4>>;
        using test_insert_result_double_left = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      empty_node,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<4>, int_v<4>>,
                      tavl_node<empty_node, empty_node, 0, int_v<8>, int_v<8>>,
                      1,
                      int_v<5>,
                      int_v<5>>,
            2,
            int_v<3>,
            int_v<3>>;
        using test_insert_double_left = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<empty_node, empty_node, 0, int_v<8>, int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_insert_result_double_right = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<2>, int_v<2>>,
                      tavl_node<empty_node, empty_node, 0, int_v<6>, int_v<6>>,
                      1,
                      int_v<5>,
                      int_v<5>>,
            tavl_node<empty_node,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<7>,
            int_v<7>>;
        using test_insert_double_right = tavl_node<
            tavl_node<empty_node, empty_node, 0, int_v<2>, int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_remove_result_rotate_left = tavl_node<
            tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      empty_node,
                      1,
                      int_v<5>,
                      int_v<5>>,
            2,
            int_v<2>,
            int_v<2>>;
        using test_remove_rotate_left = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<empty_node, empty_node, 0, int_v<8>, int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_remove_result_rotate_right = tavl_node<
            tavl_node<empty_node,
                      tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      1,
                      int_v<5>,
                      int_v<5>>,
            tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
            2,
            int_v<8>,
            int_v<8>>;
        using test_remove_rotate_right = tavl_node<
            tavl_node<empty_node, empty_node, 0, int_v<2>, int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_remove_result_double_left = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      empty_node,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<4>, int_v<4>>,
                      tavl_node<empty_node, empty_node, 0, int_v<8>, int_v<8>>,
                      1,
                      int_v<5>,
                      int_v<5>>,
            2,
            int_v<3>,
            int_v<3>>;
        using test_remove_double_left = tavl_node<
            tavl_node<
                tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                tavl_node<
                    empty_node,
                    tavl_node<empty_node, empty_node, 0, int_v<4>, int_v<4>>,
                    1,
                    int_v<3>,
                    int_v<3>>,
                2,
                int_v<2>,
                int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      empty_node,
                      1,
                      int_v<8>,
                      int_v<8>>,
            3,
            int_v<5>,
            int_v<5>>;
        using test_remove_result_double_right = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<2>, int_v<2>>,
                      tavl_node<empty_node, empty_node, 0, int_v<6>, int_v<6>>,
                      1,
                      int_v<5>,
                      int_v<5>>,
            tavl_node<empty_node,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<7>,
            int_v<7>>;
        using test_remove_double_right = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      empty_node,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<
                tavl_node<
                    tavl_node<empty_node, empty_node, 0, int_v<6>, int_v<6>>,
                    empty_node,
                    1,
                    int_v<7>,
                    int_v<7>>,
                tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                2,
                int_v<8>,
                int_v<8>>,
            3,
            int_v<5>,
            int_v<5>>;
        using test_remove_no_rotate_input = test_avl_template;
        using test_remove_result_leftmost = tavl_node<
            tavl_node<empty_node,
                      tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_remove_result_rightmost = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      empty_node,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_remove_result_normal_1 = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      empty_node,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_remove_result_normal_2 = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<empty_node,
                      tavl_node<empty_node, empty_node, 0, int_v<9>, int_v<9>>,
                      1,
                      int_v<8>,
                      int_v<8>>,
            2,
            int_v<5>,
            int_v<5>>;
        using test_remove_result_branch = tavl_node<
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<1>, int_v<1>>,
                      tavl_node<empty_node, empty_node, 0, int_v<3>, int_v<3>>,
                      1,
                      int_v<2>,
                      int_v<2>>,
            tavl_node<tavl_node<empty_node, empty_node, 0, int_v<7>, int_v<7>>,
                      empty_node,
                      1,
                      int_v<9>,
                      int_v<9>>,
            2,
            int_v<5>,
            int_v<5>>;
        inline namespace TestFind
        {
            static_assert(std::is_same_v<tavl_find_t<empty_node, int>, value<>>,
                          "tavl_find for empty tree");
            static_assert(std::is_same_v<tavl_find_t<test_find_input, int_v<5>>,
                                         value<int_v<5>>>,
                          "tavl_find for root");
            static_assert(std::is_same_v<tavl_find_t<test_find_input, int_v<1>>,
                                         value<int_v<1>>>,
                          "tavl_find for the leftmost node");
            static_assert(std::is_same_v<tavl_find_t<test_find_input, int_v<9>>,
                                         value<int_v<9>>>,
                          "tavl_find for the rightmost node");
            static_assert(std::is_same_v<tavl_find_t<test_find_input, int_v<2>>,
                                         value<int_v<2>>>,
                          "tavl_find for non-leaf nodes");
            static_assert(std::is_same_v<tavl_find_t<test_find_input, int_v<7>>,
                                         value<int_v<7>>>,
                          "tavl_find for normal leaves");
            static_assert(std::is_same_v<tavl_find_t<test_find_input, int_v<3>>,
                                         value<int_v<3>>>,
                          "tavl_find for normal leaves");
            static_assert(
                std::is_same_v<tavl_find_t<test_find_input, int_v<10>>,
                               value<>>,
                "tavl_find for not existing keys");
        } // namespace TestFind
        inline namespace TestContain
        {
            static_assert(!tavl_contain_v<empty_node, int>,
                          "tavl_contain for empty tree");
            static_assert(tavl_contain_v<test_contain_input, int_v<5>>,
                          "tavl_contain for root");
            static_assert(tavl_contain_v<test_contain_input, int_v<1>>,
                          "tavl_contain for the leftmost node");
            static_assert(tavl_contain_v<test_contain_input, int_v<9>>,
                          "tavl_contain for the rightmost node");
            static_assert(tavl_contain_v<test_contain_input, int_v<2>>,
                          "tavl_contain for non-leaf nodes");
            static_assert(tavl_contain_v<test_contain_input, int_v<7>>,
                          "tavl_contain for normal leaves");
            static_assert(tavl_contain_v<test_contain_input, int_v<3>>,
                          "tavl_contain for normal leaves");
            static_assert(!tavl_contain_v<test_contain_input, int_v<0>>,
                          "tavl_contain for not-existing keys");
        } // namespace TestContain
        inline namespace TestInsert
        {
            static_assert(
                std::is_same_v<tavl_insert_t<empty_node, int, int>,
                               tavl_node<empty_node, empty_node, 0, int, int>>,
                "tavl_insert for empty tree");
            static_assert(
                std::is_same_v<
                    tavl_insert_t<test_insert_leftmost, int_v<1>, int_v<1>>,
                    test_insert_result_no_new>,
                "tavl_insert for leftmost nodes");
            static_assert(
                std::is_same_v<
                    tavl_insert_t<test_insert_rightmost, int_v<9>, int_v<9>>,
                    test_insert_result_no_new>,
                "tavl_insert for rightmost nodes");
            static_assert(
                std::is_same_v<
                    tavl_insert_t<test_insert_normal_1, int_v<3>, int_v<3>>,
                    test_insert_result_no_new>,
                "tavl_insert for normal nodes");
            static_assert(
                std::is_same_v<
                    tavl_insert_t<test_insert_normal_2, int_v<7>, int_v<7>>,
                    test_insert_result_no_new>,
                "tavl_insert for normal nodes");
            static_assert(
                std::is_same_v<
                    tavl_insert_t<test_insert_new, int_v<4>, int_v<4>>,
                    test_insert_result_new>,
                "tavl_insert for new branches");
            static_assert(
                std::is_same_v<
                    tavl_insert_t<test_insert_rotate_left, int_v<9>, int_v<9>>,
                    test_insert_result_rotate_left>,
                "tavl_insert for left-rotate");
            static_assert(
                std::is_same_v<
                    tavl_insert_t<test_insert_rotate_right, int_v<2>, int_v<2>>,
                    test_insert_result_rotate_right>,
                "tavl_insert for right-rotate");
            static_assert(
                std::is_same_v<
                    tavl_insert_t<test_insert_double_left, int_v<4>, int_v<4>>,
                    test_insert_result_double_left>,
                "tavl_insert for left-right");
            static_assert(
                std::is_same_v<
                    tavl_insert_t<test_insert_double_right, int_v<6>, int_v<6>>,
                    test_insert_result_double_right>,
                "tavl_insert for right-left");
        } // namespace TestInsert
        inline namespace TestRemove
        {
            static_assert(
                std::is_same_v<tavl_remove_t<test_avl_template, int_v<10>>,
                               test_avl_template>,
                "tavl_remove for not-existing nodes");
            static_assert(
                std::is_same_v<
                    tavl_remove_t<test_remove_no_rotate_input, int_v<1>>,
                    test_remove_result_leftmost>,
                "tavl_remove for leftmost nodes");
            static_assert(
                std::is_same_v<
                    tavl_remove_t<test_remove_no_rotate_input, int_v<9>>,
                    test_remove_result_rightmost>,
                "tavl_remove for rightmost nodes");
            static_assert(
                std::is_same_v<
                    tavl_remove_t<test_remove_no_rotate_input, int_v<3>>,
                    test_remove_result_normal_1>,
                "tavl_remove for normal nodes");
            static_assert(
                std::is_same_v<
                    tavl_remove_t<test_remove_no_rotate_input, int_v<7>>,
                    test_remove_result_normal_2>,
                "tavl_remove for normal nodes");
            static_assert(
                std::is_same_v<
                    tavl_remove_t<test_remove_no_rotate_input, int_v<8>>,
                    test_remove_result_branch>,
                "tavl_remove for leftmost nodes");
            static_assert(
                std::is_same_v<tavl_remove_t<test_remove_rotate_left, int_v<8>>,
                               test_remove_result_rotate_left>,
                "tavl_remove for left-rotate");
            static_assert(std::is_same_v<
                              tavl_remove_t<test_remove_rotate_right, int_v<2>>,
                              test_remove_result_rotate_right>,
                          "tavl_remove for right-rotate");
            static_assert(
                std::is_same_v<tavl_remove_t<test_remove_double_left, int_v<7>>,
                               test_remove_result_double_left>,
                "tavl_remove for left-right");
            static_assert(std::is_same_v<
                              tavl_remove_t<test_remove_double_right, int_v<1>>,
                              test_remove_result_double_right>,
                          "tavl_remove for right-left");
        } // namespace TestRemove
    }     // namespace InHeaderDebug
#endif
} // namespace Compiler
#endif
