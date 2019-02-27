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

#include "TAVL.hpp"

namespace tavl
{
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
} // namespace tavl
int main()
{
    return 0;
}
