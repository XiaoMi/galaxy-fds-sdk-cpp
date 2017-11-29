/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include "galaxy_fds_client_exception.h"
#include "model/fds_object_metadata.h"

namespace galaxy {
namespace fds {

void FDSObjectMetadata::checkMetadata(const std::string& key) {
  std::string lowerCaseKey = Utils::toLowerCase(key);
  int len = Constants::USER_DEFINED_METADATA_PREFIX.size();
  if (lowerCaseKey.substr(0, len) == Constants::USER_DEFINED_METADATA_PREFIX) {
    return;
  }

  std::vector<std::string>::iterator iter;
  iter = find(_lowerCasedPreDefinedMetadata.begin(), _lowerCasedPreDefinedMetadata.end(), lowerCaseKey);
  if (iter != _lowerCasedPreDefinedMetadata.end()) {
    return;
  }

  throw GalaxyFDSClientException("Invalid metadata: " + key);
}

} // namesapce fds
} // namespace galaxy
