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

namespace tavl
{
    /**
     * @brief struct to express the relationship of two types
     * @tparam L left-hand-side of two types being compared
     * @tparam R right-hand-side of two types being compared
     * @note A static constexpr variable with type int should be defined. Less
     * than zero value means less-than relationship. Greater than zero value
     * means greater-than relationship, zero means equality. See test.cpp for
     * more information.
     * @note You should be sure that this struct is properly specialized for
     * every two key types passed as L and R.
     */
    template <typename L, typename R>
    struct compare;
    template <typename L, typename R>
    static constexpr int compare_v =
        compare<L, R>::value > 0 ? 1 : compare<L, R>::value < 0 ? -1 : 0;
    // for lazy evaluation
    template <typename T>
    struct identity
    {
        using type = T;
    };
    template <typename T1, typename T2>
    struct type_pair
    {
        using first_type  = T1;
        using second_type = T2;
    };
    // codes originally from
    // https://musteresel.github.io/posts/2018/03/c++-lazy-template-instantiation.html
    template <template <class...> class T, typename... Args>
    struct lazy_template
    {
        using type = T<Args...>;
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

    /**
     * @brief empty_node should be used when there is no node(empty tree) or no
     * children.
     */
    struct empty_node
    {
        using left                  = empty_node;
        using right                 = empty_node;
        static constexpr int height = -1;
        using key                   = void;
        using value                 = void;
    };
    template <typename T>
    static constexpr bool is_empty_node_v = std::is_same_v<T, empty_node>;
    template <typename V = Impl::not_found>
    struct value : public identity<V>
    {
    };
    /**
     * @brief node definition for AVL tree
     * @tparam L left child
     * @tparam R right child
     * @tparam H height of this tree
     * @tparam K key. Note that compare should be properly specialized for all
     * keys.
     * @tparam V value
     */
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

    // concepts for C++2a
#if __cplusplus > 201703L
    template <typename L, typename R>
    concept Comparable = requires
    {
        compare_v<L, R>;
    };
    template <typename T>
    concept Node = requires
    {
        typename T::left;
        typename T::right;
        T::height + 0;
        typename T::key;
        typename T::value;
    };
    template <typename T, typename K>
    concept ValidNodeAndKey = Node<T> &&
                              +(Comparable<typename T::key, typename T::key> ||
                                +is_empty_node_v<T>);
#define REQUIRES(e) requires e
#define COMPARABLE_CHECK(L, R) Comparable<L, R>
#define NODE_CHECK(T) Node<T>
#define VALID_NK(N, K) ValidNodeAndKey<N, K>
#else
#define REQUIRES(e)
#define COMPARABLE_CHECK(L, R)
#define NODE_CHECK(T)
#define VALID_NK(N, K)
#endif
    namespace impl
    {
        template <typename T, typename K, int = compare_v<K, typename T::key>>
        struct find_impl;
        template <typename T, typename K>
        struct find_nonmepty
        {
            using type = typename find_impl<T, K>::type;
        };
        template <typename K>
        struct find_nonmepty<empty_node, K>
        {
            using type = empty_node;
        };
        template <typename T, typename K>
        struct find_impl<T, K, 0>
        {
            using type = T;
        };
        template <typename T, typename K>
        struct find_impl<T, K, -1>
        {
            using type = typename find_nonmepty<typename T::left, K>::type;
        };
        template <typename T, typename K>
        struct find_impl<T, K, 1>
        {
            using type = typename find_nonmepty<typename T::right, K>::type;
        };
    } // namespace impl
    /**
     * @brief find an element whose key is K. Result is corresponding node
     * struct, empty_node otherwise.
     * @tparam T AVL tree
     * @tparam K key type
     */
    template <typename T, typename K>
    REQUIRES(VALID_NK(T, K))
    struct tavl_find
    {
        using type = typename impl::find_impl<T, K>::type;
    };
    template <typename K>
    REQUIRES(VALID_NK(T, K))
    struct tavl_find<empty_node, K>
    {
        using type = empty_node;
    };
    /**
     * @brief find an element whose key is K. Return the sub-tree with K as
     * its root, empty_node otherwise
     * @tparam T AVL tree
     * @tparam K key type
     */
    template <typename T, typename K>
    REQUIRES(VALID_NK(T, K))
    using tavl_find_t = typename tavl_find<T, K>::type;
    // check whether there is an element whose key is K in the given AVL
    // tree T
    template <typename T, typename K>
    REQUIRES(VALID_NK(T, K))
    inline constexpr bool tavl_contain_v =
        !std::is_same_v<tavl_find_t<T, K>, empty_node>;
    namespace impl
    {
        template <typename T>
        struct min_impl
        {
            using type = typename min_impl<typename T::left>::type;
        };
        template <typename R, int H, typename K, typename V>
        struct min_impl<tavl_node<empty_node, R, H, K, V>>
        {
            using type = tavl_node<empty_node, R, H, K, V>;
        };
        template <>
        struct min_impl<empty_node>
        {
            using type = empty_node;
        };
        template <typename T>
        struct max_impl
        {
            using type = typename max_impl<typename T::right>::type;
        };
        template <typename L, int H, typename K, typename V>
        struct max_impl<tavl_node<L, empty_node, H, K, V>>
        {
            using type = tavl_node<L, empty_node, H, K, V>;
        };
        template <>
        struct max_impl<empty_node>
        {
            using type = empty_node;
        };
    } // namespace impl
    /**
     * @brief get the minimal element in the given AVL tree, or empty_node
     * if there is no such element.
     * @tparam T AVL tree
     */
    template <typename T>
    REQUIRES(NODE_CHECK(T))
    using tavl_min = impl::min_impl<T>;
    /**
     * @brief get the minimal element in the given AVL tree, or empty_node
     * if there is no such element.
     * @tparam T AVL tree
     */
    template <typename T>
    REQUIRES(NODE_CHECK(T))
    using tavl_min_t = typename tavl_min<T>::type;
    /**
     * @brief get the maximal element in the given AVL tree, or empty_node
     * if there is no such element.
     * @tparam T AVL tree
     */
    template <typename T>
    REQUIRES(NODE_CHECK(T))
    using tavl_max = impl::max_impl<T>;
    /**
     * @brief get the maximal element in the given AVL tree, or empty_node
     * if there is no such element.
     * @tparam T AVL tree
     */
    template <typename T>
    REQUIRES(NODE_CHECK(T))
    using tavl_max_t = typename tavl_max<T>::type;
    namespace impl
    {
        template <typename TREE>
        using insert_reset_height =
            tavl_node<typename TREE::left,
                      typename TREE::right,
                      (TREE::left::height > TREE::right::height ?
                           TREE::left::height + 1 :
                           TREE::right::height + 1),
                      typename TREE::key,
                      typename TREE::value>;
        template <typename T,
                  typename K,
                  typename V,
                  int = compare_v<K, typename T::key>>
        struct insert_impl;
        template <typename T, typename K, typename V>
        struct insert_nonempty
        {
            using type =
                insert_reset_height<typename insert_impl<T, K, V>::type>;
        };
        template <typename K, typename V>
        struct insert_nonempty<empty_node, K, V>
        {
            using type = tavl_node<empty_node, empty_node, 0, K, V>;
        };
        template <typename T, typename K, typename V>
        struct insert_impl<T, K, V, 0>
        {
            // K is existed
            using type = Impl::invalid;
        };
        template <typename TREE, typename KL, typename VL>
        struct insert_recursive_left
        {
            static_assert(!std::is_same_v<TREE, empty_node>);
            using type = tavl_node<
                typename insert_nonempty<typename TREE::left, KL, VL>::type,
                typename TREE::right,
                TREE::height,
                typename TREE::key,
                typename TREE::value>;
        };
        template <typename TREE, typename KL, typename VL>
        struct insert_recursive_right
        {
            static_assert(!std::is_same_v<TREE, empty_node>);
            using type = tavl_node<
                typename TREE::left,
                typename insert_nonempty<typename TREE::right, KL, VL>::type,
                TREE::height,
                typename TREE::key,
                typename TREE::value>;
        };
        template <typename T,
                  typename K,
                  typename V,
                  int = insert_recursive_left<T, K, V>::type::left::height -
                        insert_recursive_left<T, K, V>::type::right::height>
        struct insert_left_impl
        {
            using type = typename insert_recursive_left<T, K, V>::type;
        };
        template <typename T,
                  typename K,
                  typename V,
                  bool double_rotate =
                      compare_v<K,
                                typename insert_recursive_left<T, K, V>::type::
                                    left::key> == -1>
        struct insert_left_rotate_or_double
        {
            using type = typename Impl::rotate_left<
                typename insert_recursive_left<T, K, V>::type>::type;
        };
        template <typename T, typename K, typename V>
        struct insert_left_rotate_or_double<T, K, V, false>
        {
            using type = typename Impl::double_left<
                typename insert_recursive_left<T, K, V>::type>::type;
        };
        template <typename T, typename K, typename V>
        struct insert_left_impl<T, K, V, 2>
        {
            using type = typename insert_left_rotate_or_double<T, K, V>::type;
        };
        template <typename T, typename K, typename V>
        struct insert_impl<T, K, V, -1>
        {
            using type = typename insert_left_impl<T, K, V>::type;
        };
        template <typename T,
                  typename K,
                  typename V,
                  int = insert_recursive_right<T, K, V>::type::left::height -
                        insert_recursive_right<T, K, V>::type::right::height>
        struct insert_right_impl
        {
            using type = typename insert_recursive_right<T, K, V>::type;
        };
        template <typename T,
                  typename K,
                  typename V,
                  bool double_rotate =
                      compare_v<K,
                                typename insert_recursive_right<T, K, V>::type::
                                    right::key> == 1>
        struct insert_right_rotate_or_double
        {
            using type = typename Impl::rotate_right<
                typename insert_recursive_right<T, K, V>::type>::type;
        };
        template <typename T, typename K, typename V>
        struct insert_right_rotate_or_double<T, K, V, false>
        {
            using type = typename Impl::double_right<
                typename insert_recursive_right<T, K, V>::type>::type;
        };
        template <typename T, typename K, typename V>
        struct insert_right_impl<T, K, V, -2>
        {
            using type = typename insert_right_rotate_or_double<T, K, V>::type;
        };
        template <typename T, typename K, typename V>
        struct insert_impl<T, K, V, 1>
        {
            using type = typename insert_right_impl<T, K, V>::type;
        };
    } // namespace impl
    /**
     * @brief insert an (K, V) element into given AVL tree T
     */
    template <typename T, typename K, typename V = std::true_type>
    REQUIRES(VALID_NK(T, K))
    struct tavl_insert
    {
    public:
        using type = impl::insert_reset_height<
            typename impl::insert_impl<T, K, V>::type>;
    };
    template <typename K, typename V>
    REQUIRES(VALID_NK(T, K))
    struct tavl_insert<empty_node, K, V>
    {
        using type = tavl_node<empty_node, empty_node, 0, K, V>;
    };
    /**
     * @brief insert an (K, V) element into given AVL tree T
     */
    template <typename T, typename K, typename V = std::true_type>
    REQUIRES(VALID_NK(T, K))
    using tavl_insert_t = typename tavl_insert<T, K, V>::type;
    namespace impl
    {
        template <typename TREE>
        struct remove_reset_height
        {
            using type = tavl_node<typename TREE::left,
                                   typename TREE::right,
                                   (TREE::left::height > TREE::right::height ?
                                        TREE::left::height + 1 :
                                        TREE::right::height + 1),
                                   typename TREE::key,
                                   typename TREE::value>;
        };
        template <>
        struct remove_reset_height<empty_node>
        {
            using type = empty_node;
        };
        template <typename TREE>
        using remove_reset_height_t = typename remove_reset_height<TREE>::type;
        template <typename T,
                  typename K,
                  int comp = compare_v<K, typename T::key>>
        struct remove_impl;
        template <typename T, typename K>
        struct remove_nonempty
        {
            using type =
                remove_reset_height_t<typename remove_impl<T, K>::type>;
        };
        template <typename K>
        struct remove_nonempty<empty_node, K>
        {
            using type = empty_node;
        };
        template <typename TREE, typename KEY>
        struct remove_recursion_left
        {
            using type = tavl_node<
                typename remove_nonempty<typename TREE::left, KEY>::type,
                typename TREE::right,
                TREE::height,
                typename TREE::key,
                typename TREE::value>;
        };
        template <typename TREE, typename KEY>
        struct remove_recursion_right
        {
            using type = tavl_node<
                typename TREE::left,
                typename remove_nonempty<typename TREE::right, KEY>::type,
                TREE::height,
                typename TREE::key,
                typename TREE::value>;
        };
        // do remove
        template <typename TREE, typename K>
        struct do_remove
        {
            using type = tavl_node<
                typename TREE::left,
                typename remove_recursion_right<
                    TREE,
                    typename tavl_min_t<typename TREE::right>::key>::type::
                    right,
                ((TREE::left::height >
                  remove_recursion_right<
                      TREE,
                      typename tavl_min_t<typename TREE::right>::key>::type::
                      right::height) ?

                     TREE::left::height + 1 :
                     remove_recursion_right<
                         TREE,
                         typename tavl_min_t<typename TREE::right>::key>::type::
                             right::height +
                         1),
                typename tavl_min_t<typename TREE::right>::key,
                typename tavl_min_t<typename TREE::right>::value>;
        };
        template <typename R, int H, typename K, typename V, typename KEY>
        struct do_remove<tavl_node<empty_node, R, H, K, V>, KEY>
        {
            using type = R;
        };
        template <typename L, int H, typename K, typename V, typename KEY>
        struct do_remove<tavl_node<L, empty_node, H, K, V>, KEY>
        {
            using type = L;
        };
        template <int H, typename K, typename V, typename KEY>
        struct do_remove<tavl_node<empty_node, empty_node, H, K, V>, KEY>
        {
            using type = empty_node;
        };
        template <typename T, typename K>
        struct remove_impl<T, K, 0>
        {
            using type = remove_reset_height_t<typename do_remove<T, K>::type>;
        };
        template <
            typename TREE,
            typename KEY,
            bool direct_recursion =
                remove_recursion_right<TREE, KEY>::type::left::height -
                    remove_recursion_right<TREE, KEY>::type::right::height !=
                2>
        struct remove_right
        {
            using type = typename remove_recursion_right<TREE, KEY>::type;
        };
        template <
            typename TREE,
            typename KEY,
            bool rotate =
                remove_recursion_right<TREE, KEY>::type::left::left::height >=
                remove_recursion_right<TREE, KEY>::type::left::right::height>
        struct remove_rotate_or_double_right
        {
            using type = typename Impl::rotate_left<
                typename remove_recursion_right<TREE, KEY>::type>::type;
        };
        template <typename TREE, typename KEY>
        struct remove_rotate_or_double_right<TREE, KEY, false>
        {
            using type = typename Impl::double_left<
                typename remove_recursion_right<TREE, KEY>::type>::type;
        };
        template <typename TREE, typename KEY>
        struct remove_right<TREE, KEY, false>
        {
            using type =
                typename remove_rotate_or_double_right<TREE, KEY>::type;
        };
        template <typename T, typename K>
        struct remove_impl<T, K, 1>
        {
            using type = typename remove_right<T, K>::type;
        };
        template <
            typename TREE,
            typename KEY,
            bool direct_recursion =
                remove_recursion_left<TREE, KEY>::type::left::height -
                    remove_recursion_left<TREE, KEY>::type::right::height !=
                -2>
        struct remove_left
        {
            using type = typename remove_recursion_left<TREE, KEY>::type;
        };
        template <
            typename TREE,
            typename KEY,
            bool rotate =
                remove_recursion_left<TREE, KEY>::type::right::right::height >=
                remove_recursion_left<TREE, KEY>::type::right::left::height>
        struct remove_rotate_or_double_left
        {
            using type = typename Impl::rotate_right<
                typename remove_recursion_left<TREE, KEY>::type>::type;
        };
        template <typename TREE, typename KEY>
        struct remove_rotate_or_double_left<TREE, KEY, false>
        {
            using type = typename Impl::double_right<
                typename remove_recursion_left<TREE, KEY>::type>::type;
        };
        template <typename TREE, typename KEY>
        struct remove_left<TREE, KEY, false>
        {
            using type = typename remove_rotate_or_double_left<TREE, KEY>::type;
        };
        template <typename T, typename K>
        struct remove_impl<T, K, -1>
        {
            using type = typename remove_left<T, K>::type;
        };
    } // namespace impl
    /**
     * @brief try to remopve the element whose key is K
     */
    template <typename T, typename K>
    REQUIRES(VALID_NK(T, K))
    struct tavl_remove
    {
    public:
        using type =
            impl::remove_reset_height_t<typename impl::remove_impl<T, K>::type>;
    };
    template <typename K>
    REQUIRES(VALID_NK(T, K))
    struct tavl_remove<empty_node, K>
    {
        using type = empty_node;
    };
    /**
     * @brief try to remopve the element whose key is K
     */
    template <typename T, typename K>
    REQUIRES(VALID_NK(T, K))
    using tavl_remove_t = typename tavl_remove<T, K>::type;
    /**
     * @brief default implementation of merging function for tavl_for_each
     */
    template <typename Left, typename Right, typename Current>
    struct tavl_for_each_default_merge
    {
        using type = std::void_t<Left, Right, Current>;
    };
    /**
     * @brief apply F<key, value> and M<left-tree-result, right-tree-result,
     * current-node-result> for each non-empty node
     * @tparam T AVL tree
     * @tparam F 'function' need to apply on the node, which should have two
     * parameters and a member type named type to 'store' the result. Note
     * that F should handle default value produced by empty_node properly.
     * @tparam M class used to merge results from left tree, right tree and
     * current result into a single result(a member type named type)
     * @tparam D default value for empty_node
     * @note Using default parameters will always produce void as result
     * unless some errors in F is made. So the result type of default
     * version is meaningless. Therefore you can use some undefined member
     * types to 'stop' for_each and show error messages.
     */
    template <typename T,
              template <typename K, typename V>
              typename F,
              template <typename L, typename R, typename C>
              typename M = tavl_for_each_default_merge,
              typename D = void>
    REQUIRES(NODE(T))
    struct tavl_for_each
    {
    private:
        using result = typename M<
            typename tavl_for_each<typename T::left, F, M, D>::type,
            typename tavl_for_each<typename T::right, F, M, D>::type,
            typename F<typename T::key, typename T::value>::type>::type;

    public:
        using type = result;
    };
    template <template <typename K, typename V> typename F,
              template <typename L, typename R, typename C>
              typename M,
              typename D>
    struct tavl_for_each<empty_node, F, M, D>
    {
        using type = D;
    };
    /**
     * @brief apply F<key, value> and M<left-tree-result, right-tree-result,
     * current-node-result> for each non-empty node
     * @tparam T AVL tree
     * @tparam F 'function' need to apply on the node, which should have two
     * parameters and a member type named type to 'store' the result. Note
     * that F should handle default value produced by empty_node properly.
     * @tparam M class used to merge results from left tree, right tree and
     * current result into a single result(a member type named type)
     * @tparam D default value for empty_node
     * @note Using default parameters will always produce void as result
     * unless some errors in F is made. So the result type of default
     * version is meaningless. Therefore you can use some undefined member
     * types to 'stop' for_each and show error messages.
     */
    template <typename T,
              template <typename K, typename V>
              typename F,
              template <typename L, typename R, typename C>
              typename M = tavl_for_each_default_merge,
              typename D = void>
    REQUIRES(NODE(T))
    using tavl_for_each_t = typename tavl_for_each<T, F, M, D>::type;
    /**
     * @brief default implementation of merging function for
     * tavl_for_each_middle_order
     */
    template <typename Before, typename Current>
    struct tavl_for_each_middle_order_default_merge
    {
        using type = std::void_t<Before, Current>;
    };
    /**
     * @brief key-value pair that can be used by
     * tavl_for_each_middle_order_tree_merger
     */
    template <typename K, typename V = std::true_type>
    using kv_pair = tavl_node<empty_node, empty_node, 0, K, V>;
    /**
     * @brief Another implementation of merging function for
     * tavl_for_each_middle_order used for creating a new Tree from the old
     * one
     * @note Result type of the 'function' used in
     * tavl_for_each_middle_order should have sub-type 'key' && 'value' ( or
     * using kv_pair<Key, Value> )
     */
    template <typename Previous, typename Current>
    struct tavl_for_each_middle_order_tree_merger
    {
        using type = tavl_insert_t<Previous,
                                   typename Current::key,
                                   typename Current::value>;
    };
    namespace impl
    {
        template <typename T,
                  template <typename K, typename V>
                  typename F,
                  template <typename B, typename C>
                  typename M,
                  typename L>
        struct for_each_mid_impl;
        template <typename T,
                  template <typename K, typename V>
                  typename F,
                  template <typename B, typename C>
                  typename M,
                  typename L>
        struct for_each_mid_recursive
        {
            using type = typename for_each_mid_impl<T, F, M, L>::type;
        };
        template <template <typename K, typename V> typename F,
                  template <typename B, typename C>
                  typename M,
                  typename L>
        struct for_each_mid_recursive<empty_node, F, M, L>
        {
            using type = L;
        };
        template <typename T,
                  template <typename K, typename V>
                  typename F,
                  template <typename B, typename C>
                  typename M,
                  typename L>
        struct for_each_mid_impl
        {
            using type = typename for_each_mid_recursive<
                typename T::right,
                F,
                M,
                typename M<
                    typename for_each_mid_recursive<typename T::left, F, M, L>::
                        type,
                    typename F<typename T::key,
                               typename T::value>::type>::type>::type;
        };
        template <template <typename K, typename V> typename F,
                  template <typename B, typename C>
                  typename M,
                  typename L>
        struct for_each_mid_impl<empty_node, F, M, L>
        {
            using type = L;
        };
    } // namespace impl
    /**
     * @brief apply F<key, value> and M<previous, current-node-result> for
     * each non-empty node in the middle order (that is ,left-tree ->
     * current-node -> right-tree)
     * @tparam T AVL tree
     * @tparam F 'function' need to apply on the node, which should have two
     * parameters and a member type named type to 'store' the result. Note
     * that F should handle default value produced by empty_node properly.
     * @tparam M class used to merge results from previous results (after
     * merging) and current result into a single result(a member type named
     * type)
     * @tparam Last initial value
     * @note Using default parameters will always produce void as result
     * unless some errors in F is made. So the result type of default
     * version is meaningless. Therefore you can use some undefined member
     * types to 'stop' for_each and show error messages.
     */
    template <typename T,
              template <typename K, typename V>
              typename F,
              template <typename B, typename C>
              typename M,
              typename Last>
    REQUIRES(NODE(T))
    struct tavl_for_each_middle_order
    {
    public:
        using type = typename impl::for_each_mid_impl<T, F, M, Last>::type;
    };
    /**
     * @brief apply F<key, value> and M<previous, current-node-result> for
     * each non-empty node in the middle order (that is ,left-tree ->
     * current-node -> right-tree)
     * @tparam T AVL tree
     * @tparam F 'function' need to apply on the node, which should have two
     * parameters and a member type named "type" to 'store' the result. Note
     * that F should handle default value produced by empty_node properly.
     * @tparam M class used to merge results from previous results (after
     * merging) and current result into a single result(a member type named
     * type)
     * @tparam Last initial value
     * @note Using default parameters will always produce void as result
     * unless some errors in F is made. So the result type of default
     * version is meaningless. Therefore you can use some undefined member
     * types to 'stop' for_each and show error messages.
     */
    template <typename T,
              template <typename K, typename V>
              typename F,
              template <typename B, typename C>
              typename M    = tavl_for_each_middle_order_default_merge,
              typename Init = void>
    REQUIRES(NODE(T))
    using tavl_for_each_middle_order_t =
        typename tavl_for_each_middle_order<T, F, M, Init>::type;
    /**
     * @brief create intersections between multiple trees
     * @tparam T1 the first tree to examine
     * @tparam Trees other trees
     * @note Trees can be empty
     */
    template <typename T1, typename... Trees>
    struct tavl_intersect
    {
    private:
        template <typename Key, typename Value>
        struct for_each_node
        {
            using type = std::conditional_t<(tavl_contain_v<Trees, Key> && ...),
                                            type_pair<Key, Value>,
                                            int>;
        };
        template <typename Previous, typename Current>
        struct merger
        {
            using type = tavl_insert_t<Previous,
                                       typename Current::first_type,
                                       typename Current::second_type>;
        };
        template <typename Previous>
        struct merger<Previous, int>
        {
            using type = Previous;
        };

    public:
        using type =
            tavl_for_each_middle_order_t<T1, for_each_node, merger, empty_node>;
    };
    template <typename... Trees>
    struct tavl_intersect<empty_node, Trees...>
    {
        using type = empty_node;
    };
    /**
     * @brief create intersections between multiple trees
     * @tparam T1 the first tree to examine
     * @tparam Trees other trees
     * @note Trees can be empty
     */
    template <typename Tree, typename... Trees>
    using tavl_intersect_t = typename tavl_intersect<Tree, Trees...>::type;
    namespace impl
    {
        template <typename Key, typename Value>
        struct union_for_each_item_in_Tree
        {
            using type = type_pair<Key, Value>;
        };
        template <typename TreeBefore, typename Pair>
        struct union_for_each_merger
        {
            using type = typename std::conditional_t<
                tavl_contain_v<TreeBefore, typename Pair::first_type>,
                identity<TreeBefore>,
                lazy_template<tavl_insert_t,
                              TreeBefore,
                              typename Pair::first_type,
                              typename Pair::second_type>>::type;
        };
    } // namespace impl
    /**
     * @brief computes the union of two sets
     * @tparam Tree the first tree to be computed
     * @tparam Tree2 the second tree to be computed
     * @tparam Others other trees to be computed (may be empty)
     * @note If the same key is shown in multiple trees, the former one will
     * be used regardless of the value of those elements. See
     * tavl_union_with_func is another version that supports user defined
     * actions for conflicts between different trees
     */
    template <typename Tree, typename Tree2, typename... Others>
    struct tavl_union
    {
    private:
        using union_result = tavl::tavl_for_each_middle_order_t<
            Tree2,
            impl::union_for_each_item_in_Tree,
            impl::union_for_each_merger,
            Tree>;

    public:
        using type = typename tavl_union<union_result, Others...>::type;
    };
    template <typename Tree, typename Tree2>
    struct tavl_union<Tree, Tree2>
    {
        using type = tavl::tavl_for_each_middle_order_t<
            Tree2,
            impl::union_for_each_item_in_Tree,
            impl::union_for_each_merger,
            Tree>;
    };
    /**
     * @brief computes the union of two sets
     * @tparam Tree the first tree to be computed
     * @tparam Tree2 the second tree to be computed
     * @tparam Others other trees to be computed (may be empty)
     * @note If the same key is shown in multiple trees, the former one will
     * be used regardless of the value of those elements. See
     * tavl_union_with_func is another version that supports user defined
     * actions for conflicts between different trees
     */
    template <typename Tree, typename... Trees>
    using tavl_union_t = typename tavl_union<Tree, Trees...>::type;
    /*
     * @brief A derived version of tavl_union_t for using as a merger in
     * tavl_for_each_middle_order(t)
     * @note see tavl::tavl_union for more information
     */
    template <typename T1, typename T2>
    using tavl_union_2 = tavl::tavl_union<T1, T2>;
    /*
     * @brief A derived version of tavl_union_t for using as a merger in
     * tavl_for_each(t)
     * @note see tavl::tavl_union for more information
     */
    template <typename T1, typename T2, typename T3>
    using tavl_union_3 = tavl::tavl_union<T1, T2, T3>;
    template <typename Lhs, typename Rhs>
    struct tavl_is_same : std::is_same<Lhs, Rhs>
    {
    };
    namespace impl
    {
        template <
            typename L,
            typename R,
            bool = tavl_is_same<typename L::key,
                                typename tavl_find_t<R, typename L::key>::key>::
                value&& tavl_is_same<
                    typename L::value,
                    typename tavl_find_t<R, typename L::key>::value>::value>
        struct comp_oneway;
        template <typename L, typename R>
        struct comp_oneway_nonempty
        {
            static constexpr bool value = comp_oneway<L, R>::value;
        };
        template <typename R>
        struct comp_oneway_nonempty<empty_node, R>
        {
            static constexpr bool value = true;
        };
        template <typename L, typename R, bool KVInR>
        struct comp_oneway
        {
            static constexpr bool value =
                comp_oneway_nonempty<typename L::left, R>::value &&
                comp_oneway_nonempty<typename L::right, R>::value;
        };
        template <typename L, typename R>
        struct comp_oneway<L, R, false>
        {
            static constexpr bool value = false;
        };

    } // namespace impl
    template <typename L1,
              typename R1,
              int H1,
              typename K1,
              typename V1,
              typename L2,
              typename R2,
              int H2,
              typename K2,
              typename V2>
    struct tavl_is_same<tavl_node<L1, R1, H1, K1, V1>,
                        tavl_node<L2, R2, H2, K2, V2>>
    {
    private:
        using lhs = tavl_node<L1, R1, H1, K1, V1>;
        using rhs = tavl_node<L2, R2, H2, K2, V2>;
        template <typename L, typename R>
        static std::bool_constant<(impl::comp_oneway<lhs, rhs>::value) &&
                                  (impl::comp_oneway<rhs, lhs>::value)>
            test(L*);
        template <typename L, typename R>
        static std::false_type test(...);

    public:
        static constexpr bool value =
            decltype(test<lhs, rhs>(static_cast<lhs*>(nullptr)))::value;
    };
    template <typename L, typename R>
    inline constexpr bool tavl_is_same_v = tavl_is_same<L, R>::value;
    // implementations for comparing between two TAVLs lexicographically
    namespace impl
    {
        template <template <typename, typename...> typename T, typename... Args>
        struct current_helper
        {
            using type = typename T<Args...>::current;
        };
        template <template <typename, typename...> typename T, typename... Args>
        struct next_helper
        {
            using type = typename T<Args...>::next;
        };
        template <template <typename, typename...> typename T, typename... Args>
        struct identity_helper
        {
            using type = T<Args...>;
        };
        struct end_of_tree_flag;
        template <typename T, typename... Ts>
        struct another_tavl_helper;
        template <typename T, typename... Ts>
        struct another_tavl_helper_cur;
        template <typename L, int H, typename K, typename V, typename... Ts>
        struct another_tavl_helper_cur<tavl_node<L, empty_node, H, K, V>, Ts...>
        {
            using current               = tavl_node<L, empty_node, H, K, V>;
            using next                  = another_tavl_helper<Ts...>;
            static constexpr bool value = false;
        };
        template <typename L,
                  typename R,
                  int H,
                  typename K,
                  typename V,
                  typename... Ts>
        struct another_tavl_helper_cur<tavl_node<L, R, H, K, V>, Ts...>
        {
            using current               = tavl_node<L, R, H, K, V>;
            using next                  = another_tavl_helper<R, Ts...>;
            static constexpr bool value = false;
        };
        template <>
        struct another_tavl_helper_cur<end_of_tree_flag>
        {
            using current               = end_of_tree_flag;
            using next                  = end_of_tree_flag;
            static constexpr bool value = true;
        };
        template <typename T, typename... Ts>
        struct another_tavl_helper
        {
            using current =
                typename std::conditional_t<is_empty_node_v<typename T::left>,
                                            identity<T>,
                                            current_helper<another_tavl_helper,
                                                           typename T::left,
                                                           T,
                                                           Ts...>>::type;
            using next = typename std::conditional_t<
                is_empty_node_v<typename T::left>,
                std::conditional_t<
                    is_empty_node_v<typename T::right>,
                    identity_helper<another_tavl_helper_cur, Ts...>,
                    identity_helper<another_tavl_helper,
                                    typename T::right,
                                    Ts...>>,
                next_helper<another_tavl_helper, typename T::left, T, Ts...>>::
                type;
            static constexpr bool value = false;
        };
        template <>
        struct another_tavl_helper<end_of_tree_flag>
        {
            using current               = end_of_tree_flag;
            using next                  = end_of_tree_flag;
            static constexpr bool value = true;
        };
        template <typename T1, typename T2>
        struct comp_kv_impl
            : std::conditional_t<
                  compare_v<typename T1::key, typename T2::key> == 0,
                  compare<typename T1::value, typename T2::value>,
                  compare<typename T1::key, typename T2::key>>
        {
        };
        template <typename T>
        struct comp_kv_impl<T, end_of_tree_flag>
            : std::integral_constant<int, 1>
        {
        };
        template <typename T>
        struct comp_kv_impl<empty_node, T> : std::integral_constant<int, -1>
        {
        };
        template <>
        struct comp_kv_impl<empty_node, end_of_tree_flag>
            : std::integral_constant<int, 0>
        {
        };
        template <typename T1, typename T2>
        inline constexpr int comp_kv = comp_kv_impl<T1, T2>::value;
        template <typename T1, typename T2>
        using comp_kv_t = comp_kv_impl<T1, T2>;
        struct next_wrapper
        {
            using next = another_tavl_helper<end_of_tree_flag>;
        };
        template <typename T, typename A>
        struct compare_impl
        {
            template <typename LEFT>
            struct compare_full
                : std::conditional_t<
                      compare_impl<LEFT, A>::value == 0,
                      std::conditional_t<
                          comp_kv<T,
                                  typename compare_impl<LEFT,
                                                        A>::next::current> == 0,
                          lazy_template<
                              compare_impl,
                              typename T::right,
                              typename compare_impl<LEFT, A>::next::next>,
                          lazy_template<
                              comp_kv_t,
                              T,
                              typename compare_impl<LEFT, A>::next::current>>,
                      lazy_template<compare_impl, typename T::left, A>>::type
            {
                using next = typename std::conditional_t<
                    compare_full::value == 0,
                    lazy_template<compare_impl,
                                  typename T::right,
                                  typename compare_impl<LEFT, A>::next::next>,
                    identity<next_wrapper>>::type::next;
            };
            static constexpr int value = std::conditional_t<
                is_empty_node_v<typename T::left>,
                std::conditional_t<
                    comp_kv<T, typename A::current> == 0,
                    std::conditional_t<is_empty_node_v<typename T::right>,
                                       std::integral_constant<int, 0>,
                                       lazy_template<compare_impl,
                                                     typename T::right,
                                                     typename A::next>>,
                    lazy_template<comp_kv_t, T, typename A::current>>,
                lazy_template<compare_full, typename T::left>>::type::value;
            using next = typename std::conditional_t<
                is_empty_node_v<typename T::left>,
                std::conditional_t<
                    comp_kv<T, typename A::current> == 0,
                    std::conditional_t<is_empty_node_v<typename T::right>,
                                       identity<A>,
                                       lazy_template<compare_impl,
                                                     typename T::right,
                                                     typename A::next>>,
                    identity<next_wrapper>>,
                lazy_template<compare_full, typename T::left>>::type::next;
        };
        template <typename T>
        struct compare_impl<T, another_tavl_helper<end_of_tree_flag>>
        {
            static constexpr int value = 1;
            using next                 = another_tavl_helper<end_of_tree_flag>;
        };
        template <typename T>
        struct compare_impl<T, another_tavl_helper_cur<end_of_tree_flag>>
        {
            static constexpr int value = 1;
            using next = another_tavl_helper_cur<end_of_tree_flag>;
        };
        template <typename A>
        struct compare_impl<empty_node, A>
        {
            static constexpr int value = -1;
            using next                 = A;
        };
        template <>
        struct compare_impl<empty_node, another_tavl_helper<end_of_tree_flag>>
        {
            static constexpr int value = 0;
            using next                 = another_tavl_helper<end_of_tree_flag>;
        };
        template <>
        struct compare_impl<empty_node,
                            another_tavl_helper_cur<end_of_tree_flag>>
        {
            static constexpr int value = 0;
            using next = another_tavl_helper_cur<end_of_tree_flag>;
        };
    } // namespace impl
    // @brief specialization for TAVLs
    template <typename L1,
              typename R1,
              int H1,
              typename K1,
              typename V1,
              typename L2,
              typename R2,
              int H2,
              typename K2,
              typename V2>
    struct compare<tavl_node<L1, R1, H1, K1, V1>, tavl_node<L2, R2, H2, K2, V2>>
    {
    private:
    public:
        static constexpr int value = impl::compare_impl<
            tavl_node<L1, R1, H1, K1, V1>,
            impl::another_tavl_helper<tavl_node<L2, R2, H2, K2, V2>,
                                      impl::end_of_tree_flag>>::value;
    };
    template <typename L1, typename R1, int H1, typename K1, typename V1>
    struct compare<tavl_node<L1, R1, H1, K1, V1>, empty_node>
    {
    private:
    public:
        static constexpr int value = 1;
    };
    template <typename L1, typename R1, int H1, typename K1, typename V1>
    struct compare<empty_node, tavl_node<L1, R1, H1, K1, V1>>
    {
    private:
    public:
        static constexpr int value = -1;
    };
    template <>
    struct compare<empty_node, empty_node>
    {
    private:
    public:
        static constexpr int value = 0;
    };
    namespace impl
    {
        template <typename T, typename K, typename V>
        struct tavl_update_nonempty;
        template <typename T,
                  typename K,
                  typename V,
                  int = (compare_v<typename T::key, K> > 0 ?
                             1 :
                             compare_v<typename T::key, K> < 0 ? -1 : 0)>
        struct tavl_update_impl;
        template <typename T, typename K, typename V>
        struct tavl_update_impl<T, K, V, 0>
        {
            using type = tavl_node<typename T::left,
                                   typename T::right,
                                   T::height,
                                   typename T::key,
                                   V>;
        };
        template <typename T, typename K, typename V>
        struct tavl_update_impl<T, K, V, 1>
        {
            using type = tavl_node<
                typename T::left,
                typename tavl_update_nonempty<typename T::right, K, V>::type,
                T::height,
                typename T::key,
                typename T::value>;
        };
        template <typename T, typename K, typename V>
        struct tavl_update_impl<T, K, V, -1>
        {
            using type = tavl_node<
                typename tavl_update_nonempty<typename T::left, K, V>::type,
                typename T::right,
                T::height,
                typename T::key,
                typename T::value>;
        };
        template <typename T, typename K, typename V>
        struct tavl_update_nonempty
        {
            using type = typename tavl_update_impl<T, K, V>::type;
        };
        template <typename K, typename V>
        struct tavl_update_nonempty<empty_node, K, V>
        {
            using type = empty_node;
        };
        template <typename T,
                  typename K,
                  typename V,
                  bool = tavl_contain_v<T, K>>
        struct tavl_update_exist
        {
            using type = typename tavl_update_nonempty<T, K, V>::type;
        };
        template <typename T, typename K, typename V>
        struct tavl_update_exist<T, K, V, false>
        {
            using type = tavl_insert_t<T, K, V>;
        };
    } // namespace impl
    /**
     * @brief set the value of K to V (even if K is not existed)
     * @tparam T AVL tree
     * @tparam K key to be updated
     * @tparam V value to be set to
     */
    template <typename T, typename K, typename V>
    struct tavl_update
    {
        using type = typename impl::tavl_update_exist<T, K, V>::type;
    };
    template <typename K, typename V>
    struct tavl_update<empty_node, K, V>
    {
        using type = kv_pair<K, V>;
    };
    /**
     * @brief set the value of K to V (even if K is not existed)
     * @tparam T AVL tree
     * @tparam K key to be updated
     * @tparam V value to be set to
     */
    template <typename T, typename K, typename V = std::true_type>
    using tavl_update_t = typename tavl_update<T, K, V>::type;
    /**
     * @brief computes the union of two sets(and perform user-defined
     * actions for conflicts)
     * @tparam Func when conflicting typename Func<value1, value2>::type is
     * used as the value of the result node. The two arguments of the Func
     * template are the values of two trees(nodes, actually)
     * @tparam Tree the first tree to be computed
     * @tparam Tree2 the second tree to be computed
     * @tparam Others other trees to be computed (may be empty)
     * @note If the same key is shown in multiple trees, Func<...>::type
     * will be used. See tavl_union is a simple version that always use the
     * first value as its result
     */
    template <template <typename, typename> typename Func,
              typename Tree,
              typename Tree2,
              typename... Others>
    struct tavl_union_with_func
    {
    private:
        template <typename Key, typename Value>
        struct for_each_item_in_Tree
        {
            using type = type_pair<Key, Value>;
        };
        template <typename TreeBefore, typename Pair>
        struct for_each_merger
        {
            template <typename Old>
            struct update_helper
            {
                using type = tavl_update_t<
                    Old,
                    typename Pair::first_type,
                    typename Func<
                        typename tavl_find_t<Old,
                                             typename Pair::first_type>::value,
                        typename Pair::second_type>::type>;
            };
            using type = typename std::conditional_t<
                tavl_contain_v<TreeBefore, typename Pair::first_type>,
                lazy_template<update_helper, TreeBefore>,
                lazy_template<tavl_insert,
                              TreeBefore,
                              typename Pair::first_type,
                              typename Pair::second_type>>::type::type;
        };
        using union_result =
            tavl::tavl_for_each_middle_order_t<Tree2,
                                               for_each_item_in_Tree,
                                               for_each_merger,
                                               Tree>;
        template <template <template <typename, typename> typename,
                            typename,
                            typename,
                            typename...>
                  typename T,
                  template <typename, typename>
                  typename TT,
                  typename... Args>
        struct lazy
        {
            using type = T<TT, Args...>;
        };

    public:
        using type = typename std::conditional_t<
            sizeof...(Others) != 0,
            lazy<tavl_union_with_func, Func, union_result, Others...>,
            identity<identity<union_result>>>::type::type;
    };
    /**
     * @brief computes the union of two sets(and perform user-defined
     * actions for conflicts)
     * @tparam Func when conflicting typename Func<value1, value2>::type is
     * used as the value of the result node. The two arguments of the Func
     * template are the values of two trees(nodes, actually)
     * @tparam Tree the first tree to be computed
     * @tparam Tree2 the second tree to be computed
     * @tparam Others other trees to be computed (may be empty)
     * @note If the same key is shown in multiple trees, Func<...>::type
     * will be used. See tavl_union is a simple version that always use the
     * first value as its result
     */
    template <template <typename, typename> typename Func,
              typename Tree,
              typename... Trees>
    using tavl_union_with_func_t =
        typename tavl_union_with_func<Func, Tree, Trees...>::type;
    /*
     * @brief A derived version of tavl_union_with_func_t for using as a
     * merger in tavl_for_each(t)
     * @note see tavl::tavl_union_with_func for more information
     */
    template <template <typename, typename> typename Func,
              typename T1,
              typename T2,
              typename T3>
    using _tavl_union_with_func_3 = tavl_union_with_func<Func, T1, T2, T3>;
    /**
     * @brief create intersections between multiple trees and the value of
     * result's node is the result of the given function
     * @tparam T1 the first tree to examine
     * @tparam Trees other trees
     * @note Trees can be empty
     */
    template <template <typename Key, typename... Values> typename Func,
              typename T1,
              typename... Trees>
    struct tavl_intersect_with_func
    {
    private:
        template <typename Key, typename Value>
        struct for_each_node
        {
            template <typename... T>
            static Func<Key, T...> get_value(T...);
            static auto            test()
            {
                return get_value(
                    (std::declval<typename tavl_find_t<Trees, Key>::value>(),
                     ...));
            }
            using type = std::conditional_t<
                (tavl_contain_v<Trees, Key> && ...),
                type_pair<Key,
                          typename Func<Key,
                                        typename tavl_find_t<T1, Key>::value,
                                        typename tavl_find_t<Trees, Key>::
                                            value...>::type>,
                int>;
        };
        template <typename Previous, typename Current>
        struct merger
        {
            using type = tavl_insert_t<Previous,
                                       typename Current::first_type,
                                       typename Current::second_type>;
        };
        template <typename Previous>
        struct merger<Previous, int>
        {
            using type = Previous;
        };

    public:
        using type =
            tavl_for_each_middle_order_t<T1, for_each_node, merger, empty_node>;
    };
    template <template <typename Key, typename... Values> typename Func,
              typename... Trees>
    struct tavl_intersect_with_func<Func, empty_node, Trees...>
    {
        using type = empty_node;
    };
    /**
     * @brief create intersections between multiple trees
     * @tparam T1 the first tree to examine
     * @tparam Trees other trees
     * @note Trees can be empty
     */
    template <template <typename Key, typename... Values> typename Func,
              typename Tree,
              typename... Trees>
    using tavl_intersect_with_func_t =
        typename tavl_intersect_with_func<Func, Tree, Trees...>::type;
    template <typename T1, typename T2>
    struct tavl_difference
    {
    private:
        template <typename Key, typename Value>
        struct for_each_in_t1
        {
            using type = std::conditional_t<!tavl_contain_v<T2, Key>,
                                            kv_pair<Key, Value>,
                                            empty_node>;
        };

    public:
        using type =
            tavl_for_each_t<T1, for_each_in_t1, tavl_union_3, empty_node>;
    };
    template <typename T1, typename T2>
    using tavl_difference_t = typename tavl_difference<T1, T2>::type;
    template <template <typename key, typename Val1, typename Val2>
              typename Func,
              typename T1,
              typename T2>
    struct tavl_difference_with_func
    {
    private:
        template <typename K, typename V>
        using call_func = kv_pair<
            K,
            typename Func<K, V, typename tavl_find_t<T2, K>::value>::type>;
        template <typename Key, typename Value>
        struct for_each_in_t1
        {
            using type = typename std::conditional_t<
                !tavl_contain_v<T2, Key>,
                lazy_template<call_func, Key, Value>,
                identity<empty_node>>::type;
        };

    public:
        using type =
            tavl_for_each_t<T1, for_each_in_t1, tavl_union_3, empty_node>;
    };
    template <typename T1, typename T2>
    using tavl_difference_with_func_t = typename tavl_difference<T1, T2>::type;
    /**
     * @brief check whether S1 is a subset of S2
     * @note value is meaningless here.
     */
    template <typename S1, typename S2>
    struct tavl_is_subset
    {
    private:
        template <typename Key, typename>
        struct check_exist
        {
            using type = std::bool_constant<tavl_contain_v<S2, Key>>;
        };
        template <typename L, typename R, typename C>
        struct merge
        {
            using type = std::bool_constant<L::value && R::value && C::value>;
        };

    public:
        static constexpr bool value =
            tavl_for_each_t<S1, check_exist, merge, std::true_type>::value;
    };
    template <typename S1, typename S2>
    inline constexpr bool tavl_is_subset_v = tavl_is_subset<S1, S2>::value;
    namespace Impl
    {
        template <typename T>
        struct rotate_left
        {
            static_assert(!std::is_same_v<T, empty_node>);
            using type =
                tavl_node<typename T::left::left,
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
                          typename T::left::value>;
        };
        template <typename T>
        struct rotate_right
        {
            static_assert(!std::is_same_v<T, empty_node>);
            using type =
                tavl_node<tavl_node<typename T::left,
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
                          typename T::right::value>;
        };
        template <typename T>
        struct double_left
        {
            static_assert(!std::is_same_v<T, empty_node>);
            using type = typename rotate_left<
                tavl_node<typename rotate_right<typename T::left>::type,
                          typename T::right,
                          T::height,
                          typename T::key,
                          typename T::value>>::type;
        };
        template <typename T>
        struct double_right
        {
            static_assert(!std::is_same_v<T, empty_node>);
            using type = typename rotate_right<
                tavl_node<typename T::left,
                          typename rotate_left<typename T::right>::type,
                          T::height,
                          typename T::key,
                          typename T::value>>::type;
        };
    } // namespace Impl
} // namespace tavl
#endif
