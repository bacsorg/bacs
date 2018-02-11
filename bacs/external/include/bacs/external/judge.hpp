#pragma once

#include <bacs/external/external.pb.h>

#include <boost/noncopyable.hpp>

#include <memory>

namespace bacs {
namespace external {

class Judge : private boost::noncopyable {
 public:
  explicit Judge(const std::string &address);
  ~Judge();

 public:
  SubmitResult::Id Send(const Submit &submit);
  SubmitResult::IdList SendAll(const SubmitList &submits);

  SubmitResult::Id Rejudge(const Submit::Id &id);
  SubmitResult::IdList RejudgeAll(const Submit::IdList &ids);

  SubmitResult FetchResult(const SubmitResult::Id &id);
  SubmitResultList FetchResultAll(const SubmitResult::IdList &ids);

  SubmitResult FetchLatestResult(const Submit::Id &id);
  SubmitResultList FetchLatestResultAll(const Submit::IdList &ids);

  SubmitResult::MultipleId GetResultRevisions(const Submit::Id &id);
  SubmitResult::MultipleIdList GetResultRevisionsAll(const Submit::IdList &ids);

 private:
  std::string Call(const std::string &method, const std::string &message);

 private:
  class impl;
  std::unique_ptr<impl> pimpl;
};

}  // namespace external
}  // namespace bacs
