/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_ACTION_H_
#define GALAXY_ACTION_H_

#include <string>

namespace galaxy {
namespace fds {

class Action {
public:
  Action() {}

  Action(int value) { _value = value; }

  static const int GET_STORAGE_TOKEN = 0;

  static const std::string GET_STORAGE_TOKEN_STR;

  static const int LIST_BUCKETS = 1;

  static const std::string LIST_BUCKETS_STR;

  static const int PUT_BUCKET = 2;

  static const std::string PUT_BUCKET_STR;

  static const int HEAD_BUCKET = 3;

  static const std::string HEAD_BUCKET_STR;

  static const int DELETE_BUCKET = 4;

  static const std::string DELETE_BUCKET_STR;

  static const int LIST_OBJECTS = 5;

  static const std::string LIST_OBJECTS_STR;

  static const int PUT_OBJECT = 6;

  static const std::string PUT_OBJECT_STR;

  static const int POST_OBJECT = 7;

  static const std::string POST_OBJECT_STR;

  static const int HEAD_OBJECT = 8;
  
  static const std::string HEAD_OBJECT_STR;

  static const int DELETE_OBJECT = 9;

  static const std::string DELETE_OBJECT_STR;

  static const int GET_OBJECT = 10;

  static const std::string GET_OBJECT_STR;

  static const int GET_BUCKET_ACL = 11;

  static const std::string GET_BUCKET_ACL_STR;

  static const int PUT_BUCKET_ACL = 12;

  static const std::string PUT_BUCKET_ACL_STR;

  static const int DELETE_BUCKET_ACL = 13;

  static const std::string DELETE_BUCKET_ACL_STR;
  
  static const int GET_OBJECT_ACL = 14;

  static const std::string GET_OBJECT_ACL_STR;

  static const int PUT_OBJECT_ACL = 15;

  static const std::string PUT_OBJECT_ACL_STR;

  static const int DELETE_OBJECT_ACL = 16;
  
  static const std::string DELETE_OBJECT_ACL_STR;

  static const int GET_BUCKET_QUOTA = 17;

  static const std::string GET_BUCKET_QUOTA_STR;

  static const int PUT_BUCKET_QUOTA = 18;

  static const std::string PUT_BUCKET_QUOTA_STR;

  static const int RENAME_OBJECT = 19;

  static const std::string RENAME_OBJECT_STR;

  static const int GET_METRICS = 20;

  static const std::string GET_METRICS_STR;

  static const int PUT_CLIENT_METRICS = 21;

  static const std::string PUT_CLIENT_METRICS_STR;

  static const int GET_OBJECT_METADATA = 22;

  static const std::string GET_OBJECT_METADATA_STR;

  static const int INIT_MULTIPART_UPLOAD = 23;

  static const std::string INIT_MULTIPART_UPLOAD_STR;

  static const int LIST_MULTIPART_UPLOADS = 24;

  static const std::string LIST_MULTIPART_UPLOADS_STR;

  static const int COMPLETE_MULTIPART_UPLOAD = 25;

  static const std::string COMPLETE_MULTIPART_UPLOAD_STR;

  static const int ABORT_MULTIPART_UPLOAD = 26;

  static const std::string ABORT_MULTIPART_UPLOAD_STR;

  static const int UPLOAD_PART = 27;

  static const std::string UPLOAD_PART_STR;

  static const int LIST_PARTS = 28;

  static const std::string LIST_PARTS_STR;

  static const int GET_BUCKET_USAGE = 29;

  static const std::string GET_BUCKET_USAGE_STR;

  static const int GET_DEVELOPER_INFO = 30;

  static const std::string GET_DEVELOPER_INFO_STR;

  static const int PREFETCH_OBJECT = 31;

  static const std::string PREFETCH_OBJECT_STR;

  static const int REFRESH_OBJECT = 32;

  static const std::string REFRESH_OBJECT_STR;

  static const int PUT_DOMAIN_MAPPING = 33;

  static const std::string PUT_DOMAIN_MAPPING_STR;

  static const int LIST_DOMAIN_MAPPINGS = 34;

  static const std::string LIST_DOMAIN_MAPPINGS_STR;

  static const int DELETE_DOMAIN_MAPPING = 35;

  static const std::string DELETE_DOMAIN_MAPPING_STR;

  static const int UNKNOWN = 36;

  static const std::string UNKNOWN_STR;

  static Action valueOf(const std::string& text);

  const std::string& name() const;

  int value() const { return _value; }

  void setValue(int value) { _value = value; }

private:
  int _value;
};

} // namespace fds
} // namespace galaxy

#endif // GALAXY_ACTION_H_
