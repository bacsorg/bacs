#pragma once

#include <boost/serialization/split_free.hpp>

#include <chrono>

namespace boost {
namespace serialization {

template <typename Archive, typename Rep, typename Period>
inline void serialize(Archive &ar,
                      typename std::chrono::duration<Rep, Period> &dur,
                      const unsigned int file_version) {
  split_free(ar, dur, file_version);
}

template <typename Archive, typename Rep, typename Period>
void save(Archive &ar, const typename std::chrono::duration<Rep, Period> &dur,
          const unsigned int /*version*/) {
  Rep rp = dur.count();
  ar &rp;
}

template <typename Archive, typename Rep, typename Period>
void load(Archive &ar, typename std::chrono::duration<Rep, Period> &dur,
          const unsigned int /*version*/) {
  Rep rp;
  ar &rp;
  dur = typename std::chrono::duration<Rep, Period>(rp);
}

}  // namespace serialization
}  // namespace boost
