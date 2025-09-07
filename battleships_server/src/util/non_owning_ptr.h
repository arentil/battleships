#pragma once

namespace util
{
template<typename T>
using non_owning_ptr = std::enable_if_t<!std::is_pointer_v<T>, T*>;
}