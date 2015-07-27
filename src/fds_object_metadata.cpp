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
  int len = Constants::USER_DEFINED_METADATA_PREFIX.size();
  if (key.substr(0, len) == Constants::USER_DEFINED_METADATA_PREFIX) {
    return;
  }

  std::vector<std::string>::iterator iter;
  iter = find(_preDefinedMetadata.begin(), _preDefinedMetadata.end(), key);
  if (iter != _preDefinedMetadata.end()) {
    return;
  }

  throw GalaxyFDSClientException("Invalid metadata: " + key);
}

} // namesapce fds
} // namespace galaxy
