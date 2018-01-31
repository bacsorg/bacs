#include <bunsan/curl/options/option_set.hpp>

#include <boost/assert.hpp>

#include <set>

namespace bunsan {
namespace curl {
namespace options {

namespace detail {
option_base::~option_base() {}
}  // namespace detail

option_set::option_set(const option_set &set) {
  for (std::size_t i = 0; i < m_options.size(); ++i)
    if (set.m_options[i]) m_options[i] = set.m_options[i]->clone();
}

option_set &option_set::operator=(const option_set &set) {
  option_set(set).swap(*this);
  return *this;
}

void option_set::swap(option_set &set) noexcept {
  using std::swap;

  swap(m_options, set.m_options);
}

namespace {
CURLoption curl_opt_id(CURLoption id) {
  constexpr CURLoption type = static_cast<CURLoption>(10000);
  while (id >= type) id = static_cast<CURLoption>(id - type);
  BOOST_ASSERT(0 <= id);
  BOOST_ASSERT(id < CURLOPT_LASTENTRY);
  return id;
}
}  // namespace

void option_set::store_and_setopt(option_ptr opt, CURL *const curl) {
  BOOST_ASSERT(curl);
  BOOST_ASSERT(opt);
  const auto range = opt->ids();
  BOOST_ASSERT(!range.empty());
  shared_option_ptr option(opt.release());
  for (const CURLoption id : range) unsetopt(curl, id);
  for (const CURLoption id : range) m_options[curl_opt_id(id)] = option;
  option->setopt(curl);
}

void option_set::setopt(CURL *const curl) const {
  std::set<shared_option_ptr> set;
  for (const shared_option_ptr &opt : m_options) {
    if (opt && set.find(opt) == set.end()) {
      opt->setopt(curl);
      set.insert(opt);
    }
  }
}

void option_set::clear() {
  for (shared_option_ptr &opt : m_options) opt.reset();
}

void option_set::unsetopt(CURL *const curl, const CURLoption id) {
  const CURLoption id_ = curl_opt_id(id);
  const shared_option_ptr opt = m_options[id_];
  if (opt) {
    for (const CURLoption &other_id : opt->ids()) {
      const CURLoption other_id_ = curl_opt_id(other_id);
      BOOST_ASSERT(m_options[other_id_]);
      BOOST_ASSERT(m_options[other_id_] == opt);
      m_options[other_id_].reset();
    }
    opt->unsetopt(curl);
  }
  BOOST_ASSERT(!m_options[id_]);
}

}  // namespace options
}  // namespace curl
}  // namespace bunsan
