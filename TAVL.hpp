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
    static constexpr int compare_v = compare<L, R>::value;
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
    /**
     * @brief find an element whose key is K. Result is corresponding node
     * struct, empty_node otherwise.
     * @tparam T AVL tree
     * @tparam K key type
     */
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
            identity<T>,
            typename std::conditional_t<(comp_result > 0),
                                        lazy_template<lazy_more, K, T>,
                                        lazy_template<lazy_less, K, T>>::type>::
            type;
    };
    template <typename K>
    struct tavl_find<empty_node, K>
    {
        using type = empty_node;
    };
    /**
     * @brief find an element whose key is K. Return the sub-tree with K as its
     * root, empty_node otherwise
     * @tparam T AVL tree
     * @tparam K key type
     */
    template <typename T, typename K>
    using tavl_find_t = typename tavl_find<T, K>::type;
    // check whether there is an element whose key is K in the given AVL tree T
    template <typename T, typename K>
    inline constexpr bool tavl_contain_v =
        !std::is_same_v<tavl_find_t<T, K>, empty_node>;
    /**
     * @brief get the minimal element in the given AVL tree, or empty_node if
     * there is no such element.
     * @tparam T AVL tree
     */
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
            lazy_template<identity, T>,
            lazy_template<lazy, T>>::type::type;
    };
    template <>
    struct tavl_min<empty_node>
    {
        using type = empty_node;
    };
    /**
     * @brief get the minimal element in the given AVL tree, or empty_node if
     * there is no such element.
     * @tparam T AVL tree
     */
    template <typename T>
    using tavl_min_t = typename tavl_min<T>::type;
    /**
     * @brief get the maximal element in the given AVL tree, or empty_node if
     * there is no such element.
     * @tparam T AVL tree
     */
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
        using type = typename std::conditional_t<
            std::is_same_v<typename T::right, empty_node>,
            lazy_template<identity, T>,
            lazy_template<lazy, T>>::type::type;
    };
    template <>
    struct tavl_max<empty_node>
    {
        using type = empty_node;
    };
    /**
     * @brief get the maximal element in the given AVL tree, or empty_node if
     * there is no such element.
     * @tparam T AVL tree
     */
    template <typename T>
    using tavl_max_t = typename tavl_max<T>::type;
    /**
     * @brief insert an (K, V) element into given AVL tree T
     */
    template <typename T, typename K, typename V = std::true_type>
    struct tavl_insert
    {
    private:
        template <typename TREE, typename KL, typename VL>
        struct recursive_left
        {
            static_assert(!std::is_same_v<TREE, empty_node>);
            using type = tavl_node<
                typename tavl_insert<typename TREE::left, KL, VL>::type,
                typename TREE::right,
                TREE::height,
                typename TREE::key,
                typename TREE::value>;
        };
        template <typename TREE, typename KL, typename VL>
        struct recursive_right
        {
            static_assert(!std::is_same_v<TREE, empty_node>);
            using type = tavl_node<
                typename TREE::left,
                typename tavl_insert<typename TREE::right, KL, VL>::type,
                TREE::height,
                typename TREE::key,
                typename TREE::value>;
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
                      lazy_template<
                          Impl::rotate_left,
                          typename recursive_left<TREE, KEY, VALUE>::type>,
                      lazy_template<
                          Impl::double_left,
                          typename recursive_left<TREE, KEY, VALUE>::type>>,
                  lazy_template<recursive_left, TREE, KEY, VALUE>>::type::type>
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
                      lazy_template<
                          Impl::rotate_right,
                          typename recursive_right<TREE, KEY, VALUE>::type>,
                      lazy_template<
                          Impl::double_right,
                          typename recursive_right<TREE, KEY, VALUE>::type>>,
                  lazy_template<recursive_right, TREE, KEY, VALUE>>::type::type>
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
            lazy_template<identity, Impl::invalid>,
            lazy_template<reset_height,
                          std::conditional_t<(comp_result > 0),
                                             lazy_insert_right<T, K, V>,
                                             lazy_insert_left<T, K, V>>>>::
            type::type;
    };
    template <typename K, typename V>
    struct tavl_insert<empty_node, K, V>
    {
        using type = tavl_node<empty_node, empty_node, 0, K, V>;
    };
    /**
     * @brief insert an (K, V) element into given AVL tree T
     */
    template <typename T, typename K, typename V = std::true_type>
    using tavl_insert_t = typename tavl_insert<T, K, V>::type;

    /**
     * @brief try to remopve the element whose key is K
     */
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
                      lazy_template<Impl::rotate_right,
                                    typename recursion_left<TREE, KEY>::type>,
                      lazy_template<Impl::double_right,
                                    typename recursion_left<TREE, KEY>::type>>,
                  lazy_template<recursion_left, TREE, KEY>>::type::type>
        {
        };
        template <typename TREE, typename KEY>
        struct lazy_right
            : public identity<typename std::conditional_t<
                  diff_height<typename recursion_right<TREE, KEY>::type> == 2,
                  std::conditional_t<
                      (recursion_right<TREE, KEY>::type::left::left::height >=
                       recursion_right<TREE, KEY>::type::left::right::height),
                      lazy_template<Impl::rotate_left,
                                    typename recursion_right<TREE, KEY>::type>,
                      lazy_template<Impl::double_left,
                                    typename recursion_right<TREE, KEY>::type>>,
                  lazy_template<recursion_right, TREE, KEY>>::type::type>
        {
        };
        template <typename TREE>
        using lazy_current_rrecursive_right =
            recursion_right<TREE,
                            typename tavl_min_t<typename TREE::right>::key>;
        template <typename TREE, typename KEY>
        struct lazy_current
            : public std::conditional_t<
                  std::is_same_v<typename TREE::left, empty_node> ||
                      std::is_same_v<typename TREE::right, empty_node>,
                  std::conditional_t<
                      std::is_same_v<typename TREE::left, empty_node>,
                      identity<typename TREE::right>,
                      identity<typename TREE::left>>,
                  identity<tavl_node<
                      typename TREE::left,
                      typename lazy_current_rrecursive_right<TREE>::type::right,
                      ((TREE::left::height > lazy_current_rrecursive_right<
                                                 TREE>::type::right::height) ?

                           TREE::left::height + 1 :
                           lazy_current_rrecursive_right<
                               TREE>::type::right::height +
                               1),
                      typename tavl_min_t<typename TREE::right>::key,
                      typename tavl_min_t<typename TREE::right>::value>>>
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
            lazy_template<reset_height, lazy_right<T, K>>,
            std::conditional_t<
                (comp_result < 0),
                lazy_template<reset_height, lazy_left<T, K>>,
                lazy_template<reset_height, lazy_current<T, K>>>>::type::type;
    };
    template <typename K>
    struct tavl_remove<empty_node, K>
    {
        using type = empty_node;
    };
    /**
     * @brief try to remopve the element whose key is K
     */
    template <typename T, typename K>
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
     * parameters and a member type named type to 'store' the result. Note that
     * F should handle default value produced by empty_node properly.
     * @tparam M class used to merge results from left tree, right tree and
     * current result into a single result(a member type named type)
     * @tparam D default value for empty_node
     * @note Using default parameters will always produce void as result unless
     * some errors in F is made. So the result type of default version is
     * meaningless. Therefore you can use some undefined member types to 'stop'
     * for_each and show error messages.
     */
    template <typename T,
              template <typename K, typename V>
              typename F,
              template <typename L, typename R, typename C> typename M =
                  tavl_for_each_default_merge,
              typename D = void>
    struct tavl_for_each
    {
    private:
        static constexpr bool is_left_empty = is_empty_node_v<typename T::left>;
        static constexpr bool is_right_empty =
            is_empty_node_v<typename T::right>;
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
     * parameters and a member type named type to 'store' the result. Note that
     * F should handle default value produced by empty_node properly.
     * @tparam M class used to merge results from left tree, right tree and
     * current result into a single result(a member type named type)
     * @tparam D default value for empty_node
     * @note Using default parameters will always produce void as result unless
     * some errors in F is made. So the result type of default version is
     * meaningless. Therefore you can use some undefined member types to 'stop'
     * for_each and show error messages.
     */
    template <typename T,
              template <typename K, typename V>
              typename F,
              template <typename L, typename R, typename C> typename M =
                  tavl_for_each_default_merge,
              typename D = void>
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
    template <typename K, typename V>
    using kv_pair = tavl_node<empty_node, empty_node, 0, K, V>;
    /**
     * @brief Another implementation of merging function for
     * tavl_for_each_middle_order used for creating a new Tree from the old one
     * @note Result type of the 'function' used in tavl_for_each_middle_order
     * should have sub-type 'key' && 'value' ( or using kv_pair<Key, Value> )
     */
    template <typename Previous, typename Current>
    struct tavl_for_each_middle_order_tree_merger
    {
        using type = tavl_insert_t<Previous,
                                   typename Current::key,
                                   typename Current::value>;
    };
    /**
     * @brief apply F<key, value> and M<previous, current-node-result> for each
     * non-empty node in the middle order (that is ,left-tree -> current-node ->
     * right-tree)
     * @tparam T AVL tree
     * @tparam F 'function' need to apply on the node, which should have two
     * parameters and a member type named type to 'store' the result. Note that
     * F should handle default value produced by empty_node properly.
     * @tparam M class used to merge results from previous results (after
     * merging) and current result into a single result(a member type named
     * type)
     * @tparam Last initial value
     * @note Using default parameters will always produce void as result unless
     * some errors in F is made. So the result type of default version is
     * meaningless. Therefore you can use some undefined member types to 'stop'
     * for_each and show error messages.
     */
    template <typename T,
              template <typename K, typename V>
              typename F,
              template <typename B, typename C>
              typename M,
              typename Last>
    struct tavl_for_each_middle_order
    {
    private:
        static constexpr bool is_left_empty = is_empty_node_v<typename T::left>;
        static constexpr bool is_right_empty =
            is_empty_node_v<typename T::right>;
        template <typename Tree, typename LastR = Last>
        using recursive = std::conditional_t<
            std::is_same_v<Tree, empty_node>,
            LastR,
            typename tavl_for_each_middle_order<Tree, F, M, LastR>::type>;
        using left_result = recursive<typename T::left>;
        using current_result =
            typename F<typename T::key, typename T::value>::type;
        using merge_left_current =
            typename M<left_result, current_result>::type;
        using right_result = recursive<typename T::right, merge_left_current>;

    public:
        using type = right_result;
    };
    template <template <typename K, typename V> typename F,
              template <typename B, typename C>
              typename M,
              typename Last>
    struct tavl_for_each_middle_order<empty_node, F, M, Last>
    {
        using type = Last;
    };
    /**
     * @brief apply F<key, value> and M<previous, current-node-result> for each
     * non-empty node in the middle order (that is ,left-tree -> current-node ->
     * right-tree)
     * @tparam T AVL tree
     * @tparam F 'function' need to apply on the node, which should have two
     * parameters and a member type named type to 'store' the result. Note that
     * F should handle default value produced by empty_node properly.
     * @tparam M class used to merge results from previous results (after
     * merging) and current result into a single result(a member type named
     * type)
     * @tparam Last initial value
     * @note Using default parameters will always produce void as result unless
     * some errors in F is made. So the result type of default version is
     * meaningless. Therefore you can use some undefined member types to 'stop'
     * for_each and show error messages.
     */
    template <typename T,
              template <typename K, typename V>
              typename F,
              template <typename B, typename C>
              typename M,
              typename Init>
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
    template <typename Tree, typename... Trees>
    using tavl_intersect_t = typename tavl_intersect<Tree, Trees...>::type;
    template <typename Tree, typename Tree2, typename... Others>
    struct tavl_union
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
            using type = tavl_insert_t<TreeBefore,
                                       typename Pair::first_type,
                                       typename Pair::second_type>;
        };
        using union_result =
            tavl::tavl_for_each_middle_order_t<Tree2,
                                               for_each_item_in_Tree,
                                               for_each_merger,
                                               Tree>;

    public:
        using type = typename std::conditional_t<
            sizeof...(Others) != 0,
            lazy_template<tavl_union, union_result, Others...>,
            identity<identity<union_result>>>::type::type;
    };
    template <typename Tree, typename... Trees>
    using tavl_union_t = typename tavl_union<Tree, Trees...>::type;
    /*
     * @brief A derived version of tavl_union_t for using as a merger in
     * tavl_for_each(t)
     * @note see tavl::tavl_union for more information
     *
     */
    template <typename T1, typename T2, typename T3>
    using tavl_union_3 = tavl::tavl_union_t<T1, T2, T3>;
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
            static_assert(!std::is_same_v<T, empty_node>);
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
            static_assert(!std::is_same_v<T, empty_node>);
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
            static_assert(!std::is_same_v<T, empty_node>);
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
            static_assert(!std::is_same_v<T, empty_node>);
        };
    } // namespace Impl
} // namespace tavl
#endif
