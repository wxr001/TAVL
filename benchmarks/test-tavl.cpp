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

namespace tavl
{
    template <typename T, T lhs, T rhs>
    struct compare<std::integral_constant<T, lhs>,
                   std::integral_constant<T, rhs>>
    {
        static constexpr int value = lhs - rhs;
    };
} // namespace tavl
template <int val>
using int_v = std::integral_constant<int, val>;
using namespace tavl;
template <typename T, int V, int count>
struct insert_n
{
    using type = tavl_insert_t<typename insert_n<T, V, count - 1>::type,
                               int_v<V + count>>;
};
template <typename T, int V>
struct insert_n<T, V, 0>
{
    using type = T;
};

int main()
{
    using search_first =
        tavl_insert_t<typename insert_n<empty_node, 0, 2000>::type,
                      int_v<2002>>;
    using search_last =
        typename insert_n<tavl_insert_t<empty_node, int_v<2002>>, 0, 2000>::
            type;
    using search_middle = typename insert_n<
        tavl_insert_t<typename insert_n<empty_node, 0, 1000>::type,
                      int_v<2002>>,
        1001,
        1000>::type;
    if (!std::is_same_v<tavl_find_t<search_first, int_v<2002>>::key,
                        int_v<2002>> &&
        !std::is_same_v<tavl_find_t<search_last, int_v<2002>>::key,
                        int_v<2002>> &&
        !std::is_same_v<tavl_find_t<search_middle, int_v<2002>>::key,
                        int_v<2002>>)
    {
        std::cout << 3 << std::endl;
    }
    return 0;
}
