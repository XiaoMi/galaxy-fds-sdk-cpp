/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_FDS_OBJECT_METADATA_H_
#define GALAXY_FDS_OBJECT_METADATA_H_

#include <algorithm>
#include <map>
#include <vector>
#include <string>

#include "constants.h"

namespace galaxy {
namespace fds {

class FDSObjectMetadata {
public:
  FDSObjectMetadata() {
    _preDefinedMetadata.push_back(Constants::CACHE_CONTROL);
    _preDefinedMetadata.push_back(Constants::CONTENT_ENCODING);
    _preDefinedMetadata.push_back(Constants::CONTENT_LENGTH);
    _preDefinedMetadata.push_back(Constants::CONTENT_MD5);
    _preDefinedMetadata.push_back(Constants::CONTENT_TYPE);
    _preDefinedMetadata.push_back(Constants::LAST_MODIFIED);
    _preDefinedMetadata.push_back(Constants::RANGE);
  }

  void add(const std::string& key, const std::string& value);

  const std::map<std::string, std::string>& metadata() const;

private:
  void checkMetadata(const std::string& key);

private:
  std::map<std::string, std::string> _metadata;
  std::vector<std::string> _preDefinedMetadata;
}; // class FDSObjectMetadata

inline void FDSObjectMetadata::add(const std::string& key,
    const std::string& value) {
   checkMetadata(key);
   _metadata.insert(std::pair<std::string, std::string>(key, value));
}

inline const std::map<std::string, std::string>& FDSObjectMetadata::metadata()
  const {
   return _metadata;
}

} // namesapce fds
} // namespace galaxy

#endif // GALAXY_FDS_OBJECT_METADATA_H_
