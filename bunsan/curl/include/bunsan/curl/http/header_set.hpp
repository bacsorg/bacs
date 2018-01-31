#pragma once

#include <bunsan/curl/http/header.hpp>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index_container.hpp>

#include <iterator>

namespace bunsan {
namespace curl {
namespace http {

class header_set {
 private:
  using container_type = boost::multi_index_container<
      header, boost::multi_index::indexed_by<boost::multi_index::hashed_unique<
                  boost::multi_index::tag<struct tag_hashed>,
                  boost::multi_index::const_mem_fun<header, const std::string &,
                                                    &header::name> > > >;
  using index_type = container_type::index<tag_hashed>::type;

 public:
  using const_iterator = index_type::const_iterator;

  class plain_const_iterator;
  using plain_const_range = boost::iterator_range<plain_const_iterator>;

 public:
  void merge_insert(const header &h);

  void erase(const std::string &name);

  const_iterator find(const std::string &name) const;

  const_iterator begin() const { return index().begin(); }
  const_iterator end() const { return index().end(); }
  std::size_t size() const { return index().size(); }
  bool empty() const { return index().empty(); }

  plain_const_range plain_headers() const;

  bool operator==(const header_set &h) const {
    return m_headers == h.m_headers;
  }

 private:
  const index_type &index() const { return m_headers.get<tag_hashed>(); }
  index_type &index() { return m_headers.get<tag_hashed>(); }

 private:
  container_type m_headers;
};

class header_set::plain_const_iterator
    : public boost::iterator_facade<
          plain_const_iterator,
          std::iterator_traits<header::plain_const_iterator>::value_type,
          boost::forward_traversal_tag,
          std::iterator_traits<header::plain_const_iterator>::reference> {
 public:
  explicit plain_const_iterator(const const_iterator &iterator1_begin,
                                const const_iterator &iterator1_end)
      : m_iterator1_begin(iterator1_begin), m_iterator1_end(iterator1_end) {
    propagate();
    normalize();
  }

 private:
  friend class boost::iterator_core_access;

  bool is_end() const { return m_iterator1_begin == m_iterator1_end; }

  bool is_inner_end() const { return m_iterator2_begin == m_iterator2_end; }

  /// Extracts current m_iterator1_begin into m_iterator2_*
  void propagate() {
    if (!is_end()) {
      const auto range2 = m_iterator1_begin->plain_headers();
      m_iterator2_begin = range2.begin();
      m_iterator2_end = range2.end();
    }
  }

  /*!
   * Keeps invariant: iterator always in one of two states:
   *  - is_end()
   *  - !is_end() && !is_innter_end()
   */
  void normalize() {
    // skips all empty subranges
    while (!is_end() && is_inner_end()) {
      ++m_iterator1_begin;
      propagate();
    }
  }

  void increment() {
    if (!is_end()) {
      // since it is not end iterator this is always safe
      ++m_iterator2_begin;
      normalize();
    }
  }

  bool equal(const plain_const_iterator &other) const {
    if (is_end() && other.is_end()) return true;
    if (is_end() && !other.is_end()) return false;
    if (!is_end() && other.is_end()) return false;
    return m_iterator1_begin == other.m_iterator1_begin &&
           m_iterator1_end == other.m_iterator1_end &&
           m_iterator2_begin == other.m_iterator2_begin &&
           m_iterator2_end == other.m_iterator2_end;
  }

  std::iterator_traits<header::plain_const_iterator>::reference dereference()
      const {
    return *m_iterator2_begin;
  }

 private:
  const_iterator m_iterator1_begin, m_iterator1_end;
  header::plain_const_iterator m_iterator2_begin, m_iterator2_end;
};

std::ostream &operator<<(std::ostream &out, const header_set &h);

}  // namespace http
}  // namespace curl
}  // namespace bunsan
