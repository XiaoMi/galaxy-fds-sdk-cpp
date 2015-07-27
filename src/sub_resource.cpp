/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include "model/sub_resource.h"

using namespace std;

namespace galaxy {
namespace fds {

const string SubResource::ACL = "acl";
const string SubResource::QUOTA = "quota";
const string SubResource::UPLOADS = "uploads";
const string SubResource::PART_NUMBER = "partNumber";
const string SubResource::UPLOAD_ID = "uploadId";
const string SubResource::STORAGE_ACCESS_TOKEN = "storageAccessToken";
const string SubResource::METADATA = "metadata";

vector<string> SubResource::SUB_RESOURCES = SubResource::init();

vector<string> SubResource::init() {
  std::vector<std::string> coll;
  coll.push_back(ACL);
  coll.push_back(QUOTA);
  coll.push_back(UPLOADS);
  coll.push_back(PART_NUMBER);
  coll.push_back(UPLOAD_ID);
  coll.push_back(STORAGE_ACCESS_TOKEN);
  coll.push_back(METADATA);
  return coll;
}

} // namespace fds
} // namespace galaxy
