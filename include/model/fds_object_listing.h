/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_FDS_OBJECT_LISTING_H_
#define GALAXY_FDS_OBJECT_LISTING_H_

#include <string>
#include <vector>

#include "model/fds_object_summary.h"

namespace galaxy {
namespace fds {

class FDSObjectListing {
public:
  const std::string& bucketName() const;

  void  setBucketName(const std::string& bucketName);

  const std::string& prefix() const;

  void setPrefix(const std::string& prefix);

  const std::string& delimiter() const;

  void setDelimiter(const std::string& delimiter);

  const std::string& marker() const;

  void setMarker(const std::string& marker);

  const std::string& nextMarker() const;

  void setNextMarker(const std::string& nextMarker);

  int maxKeys() const;

  void setMaxKeys(int maxKeys);

  bool truncated() const;

  void setTruncated(bool truncated);

  const std::vector<FDSObjectSummary>& objectSummaries() const;

  void setObjectSummaries(const std::vector<FDSObjectSummary>& objectSummaries);

  const std::vector<std::string>& commonPrefixes() const;

  void setCommonPrefixes(const std::vector<std::string>& commonPrefixes);

  static std::shared_ptr<FDSObjectListing> deserialize(std::istream& is);

  static std::string serialize(const FDSObjectListing& fdsObjectListing);

private:
  std::string _bucketName;
  std::string _prefix;
  std::string _delimiter;
  std::string _marker;
  std::string _nextMarker;
  int _maxKeys;
  bool _truncated;
  std::vector<FDSObjectSummary> _objectSummaries;
  std::vector<std::string> _commonPrefixes;
}; // class FDSObjectListing

inline const std::string& FDSObjectListing::bucketName() const {
  return _bucketName;
}

inline void FDSObjectListing::setBucketName(const std::string& bucketName) {
  _bucketName = bucketName;
}

inline const std::string& FDSObjectListing::prefix() const {
  return _prefix; 
}

inline void FDSObjectListing::setPrefix(const std::string& prefix) {
  _prefix = prefix;
}

inline const std::string& FDSObjectListing::delimiter() const {
  return _delimiter;
}

inline void FDSObjectListing::setDelimiter(const std::string& delimiter) {
  _delimiter = delimiter;
}

inline const std::string& FDSObjectListing::marker() const {
  return _marker;
}

inline void FDSObjectListing::setMarker(const std::string& marker) {
  _marker = marker;
}

inline const std::string& FDSObjectListing::nextMarker() const {
  return _nextMarker;
}

inline void FDSObjectListing::setNextMarker(const std::string& nextMarker) {
  _nextMarker = nextMarker;
}

inline int FDSObjectListing::maxKeys() const {
  return _maxKeys;
}

inline void FDSObjectListing::setMaxKeys(int maxKeys) {
  _maxKeys = maxKeys;
}

inline bool FDSObjectListing::truncated() const {
  return _truncated;
}

inline void FDSObjectListing::setTruncated(bool truncated) {
  _truncated = truncated;
}

inline const std::vector<FDSObjectSummary>& FDSObjectListing::objectSummaries()
  const {
   return _objectSummaries;
}

inline void FDSObjectListing::setObjectSummaries(
    const std::vector<FDSObjectSummary>& objectSummaries) {
  _objectSummaries = objectSummaries;
}

inline const std::vector<std::string>& FDSObjectListing::commonPrefixes()
  const {
   return _commonPrefixes;
}

inline void FDSObjectListing::setCommonPrefixes(const std::vector<std::string>&
    commonPrefixes) {
  _commonPrefixes = commonPrefixes;
}

} // namespace fds
} // namespace galaxy

#endif // GALAXY_FDS_OBJECT_LISTING_H_
