/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include "galaxy_fds_client_exception.h"
#include "model/action.h"

using namespace std;

namespace galaxy {
namespace fds {

const string Action::GET_STORAGE_TOKEN_STR = "GetStorageToken";

const string Action::LIST_BUCKETS_STR = "ListBuckets";

const string Action::PUT_BUCKET_STR = "PutBucket";

const string Action::HEAD_BUCKET_STR = "HeadBucket";

const string Action::DELETE_BUCKET_STR = "DeleteBucket";

const string Action::LIST_OBJECTS_STR = "ListObjects";

const string Action::PUT_OBJECT_STR = "PutObject";

const string Action::POST_OBJECT_STR = "PostObject";

const string Action::HEAD_OBJECT_STR = "HeadObject";

const string Action::DELETE_OBJECT_STR = "DeleteObject";

const string Action::GET_OBJECT_STR = "GetObject";

const string Action::GET_BUCKET_ACL_STR = "GetBucketACL";

const string Action::PUT_BUCKET_ACL_STR = "PutBucketACL";

const string Action::DELETE_BUCKET_ACL_STR = "DeleteBucketACL";

const string Action::GET_OBJECT_ACL_STR = "GetObjectACL";

const string Action::PUT_OBJECT_ACL_STR = "PutObjectACL";

const string Action::DELETE_OBJECT_ACL_STR = "DeleteObjectACL";

const string Action::GET_BUCKET_QUOTA_STR = "GetBucketQuota";

const string Action::PUT_BUCKET_QUOTA_STR = "PutBucketQuota";

const string Action::RENAME_OBJECT_STR = "RenameObject";

const string Action::GET_METRICS_STR = "GetMetrics";

const string Action::PUT_CLIENT_METRICS_STR = "PutClientMetrics";

const string Action::GET_OBJECT_METADATA_STR = "GetObjectMetadata";

const string Action::INIT_MULTIPART_UPLOAD_STR = "InitMultiPartUpload";

const string Action::LIST_MULTIPART_UPLOADS_STR = "ListMultiPartUploads";

const string Action::COMPLETE_MULTIPART_UPLOAD_STR = "CompleteMultiPartUpload";

const string Action::ABORT_MULTIPART_UPLOAD_STR = "AbortMultiPartUpload";

const string Action::UPLOAD_PART_STR = "UploadPart";

const string Action::LIST_PARTS_STR = "ListParts";

const string Action::GET_BUCKET_USAGE_STR = "GetBucketUsage";

const string Action::GET_DEVELOPER_INFO_STR = "GetDeveloperInfo";

const string Action::PREFETCH_OBJECT_STR = "PrefetchObject";

const string Action::REFRESH_OBJECT_STR = "RefreshObject";

const string Action::PUT_DOMAIN_MAPPING_STR = "PutDomainMapping";

const string Action::LIST_DOMAIN_MAPPINGS_STR = "ListDomainMapping";

const string Action::DELETE_DOMAIN_MAPPING_STR = "DeleteDomainMapping";

const string Action::UNKNOWN_STR = "Unknown";


Action Action::valueOf(const string& text) {
  if (text == GET_STORAGE_TOKEN_STR) {
    return Action(GET_STORAGE_TOKEN);
  } else if (text == LIST_BUCKETS_STR) {
    return Action(LIST_BUCKETS);
  } else if (text == PUT_BUCKET_STR) {
    return Action(PUT_BUCKET);
  } else if (text == HEAD_BUCKET_STR) {
    return Action(HEAD_BUCKET);
  } else if (text == DELETE_BUCKET_STR) {
    return Action(DELETE_BUCKET);
  } else if (text == LIST_OBJECTS_STR) {
    return Action(LIST_OBJECTS);
  } else if (text == PUT_OBJECT_STR) {
    return Action(PUT_OBJECT);
  } else if (text == POST_OBJECT_STR) {
    return Action(POST_OBJECT);
  } else if (text == HEAD_OBJECT_STR ) {
    return Action(HEAD_OBJECT);
  } else if (text == DELETE_OBJECT_STR) {
    return Action(DELETE_OBJECT);
  } else if (text == GET_OBJECT_STR) {
    return Action(GET_OBJECT);
  } else if (text == GET_BUCKET_ACL_STR) {
    return Action(GET_BUCKET_ACL);
  } else if (text == DELETE_BUCKET_ACL_STR) {
    return Action(DELETE_BUCKET_ACL);
  } else if (text == GET_OBJECT_ACL_STR) {
    return Action(GET_OBJECT_ACL);
  } else if (text == DELETE_OBJECT_ACL_STR) {
    return Action(DELETE_OBJECT_ACL);
  } else if (text == GET_BUCKET_QUOTA_STR) {
    return Action(GET_BUCKET_QUOTA);
  } else if (text == PUT_BUCKET_QUOTA_STR) {
    return Action(PUT_BUCKET_QUOTA);
  } else if (text == RENAME_OBJECT_STR) {
    return Action(RENAME_OBJECT);
  } else if (text == GET_METRICS_STR) {
    return Action(GET_METRICS);
  } else if (text == PUT_CLIENT_METRICS_STR) {
    return Action(PUT_CLIENT_METRICS);
  } else if (text == GET_OBJECT_METADATA_STR) {
    return Action(GET_OBJECT_METADATA);
  } else if (text == INIT_MULTIPART_UPLOAD_STR) {
    return Action(INIT_MULTIPART_UPLOAD);
  } else if (text == LIST_MULTIPART_UPLOADS_STR) {
    return Action(LIST_MULTIPART_UPLOADS);
  } else if (text == COMPLETE_MULTIPART_UPLOAD_STR) {
    return Action(COMPLETE_MULTIPART_UPLOAD);
  } else if (text == ABORT_MULTIPART_UPLOAD_STR) {
    return Action(ABORT_MULTIPART_UPLOAD);
  } else if (text == UPLOAD_PART_STR) {
    return Action(UPLOAD_PART);
  } else if (text == LIST_PARTS_STR) {
    return Action(LIST_PARTS);
  } else if (text == GET_BUCKET_USAGE_STR) {
    return Action(GET_BUCKET_USAGE);
  } else if (text == GET_DEVELOPER_INFO_STR) {
    return Action(GET_DEVELOPER_INFO);
  } else if (text == PREFETCH_OBJECT_STR) {
    return Action(PREFETCH_OBJECT);
  } else if (text == REFRESH_OBJECT_STR) {
    return Action(REFRESH_OBJECT);
  } else if (text == PUT_DOMAIN_MAPPING_STR) {
    return Action(PUT_DOMAIN_MAPPING);
  } else if (text == LIST_DOMAIN_MAPPINGS_STR) {
    return Action(LIST_DOMAIN_MAPPINGS);
  } else if (text == DELETE_DOMAIN_MAPPING_STR) {
    return Action(DELETE_DOMAIN_MAPPING);
  } else if (text == UNKNOWN_STR) {
    return Action(UNKNOWN);
  } else {
    throw GalaxyFDSClientException("Unknown enum text");
  }
}

const string& Action::name() const {
  switch (_value) {
    case GET_STORAGE_TOKEN:
      return GET_STORAGE_TOKEN_STR;
    case LIST_BUCKETS:
      return LIST_BUCKETS_STR;
    case PUT_BUCKET:
      return PUT_BUCKET_STR;
    case HEAD_BUCKET:
      return HEAD_BUCKET_STR;
    case DELETE_BUCKET:
      return DELETE_BUCKET_STR;
    case LIST_OBJECTS:
      return LIST_OBJECTS_STR;
    case PUT_OBJECT:
      return PUT_OBJECT_STR;
    case POST_OBJECT:
      return POST_OBJECT_STR;
    case HEAD_OBJECT:
      return HEAD_OBJECT_STR;
    case DELETE_OBJECT:
      return DELETE_OBJECT_STR;
    case GET_OBJECT:
      return GET_OBJECT_STR;
    case GET_BUCKET_ACL:
      return GET_BUCKET_ACL_STR;
    case PUT_BUCKET_ACL:
      return PUT_BUCKET_ACL_STR;
    case DELETE_BUCKET_ACL:
      return DELETE_BUCKET_ACL_STR;
    case GET_OBJECT_ACL:
      return GET_OBJECT_ACL_STR;
    case PUT_OBJECT_ACL:
      return PUT_OBJECT_ACL_STR;
    case DELETE_OBJECT_ACL:
      return DELETE_OBJECT_ACL_STR;
    case GET_BUCKET_QUOTA:
      return GET_BUCKET_QUOTA_STR;
    case PUT_BUCKET_QUOTA:
      return PUT_BUCKET_QUOTA_STR;
    case RENAME_OBJECT:
      return RENAME_OBJECT_STR;
    case GET_METRICS:
      return GET_METRICS_STR;
    case PUT_CLIENT_METRICS:
      return PUT_CLIENT_METRICS_STR;
    case GET_OBJECT_METADATA:
      return GET_OBJECT_METADATA_STR;
    case INIT_MULTIPART_UPLOAD:
      return INIT_MULTIPART_UPLOAD_STR;
    case LIST_MULTIPART_UPLOADS:
      return LIST_MULTIPART_UPLOADS_STR;
    case COMPLETE_MULTIPART_UPLOAD:
      return COMPLETE_MULTIPART_UPLOAD_STR;
    case ABORT_MULTIPART_UPLOAD:
      return ABORT_MULTIPART_UPLOAD_STR;
    case UPLOAD_PART:
      return UPLOAD_PART_STR;
    case LIST_PARTS:
      return LIST_PARTS_STR;
    case GET_BUCKET_USAGE:
      return GET_BUCKET_USAGE_STR;
    case GET_DEVELOPER_INFO:
      return GET_DEVELOPER_INFO_STR;
    case PREFETCH_OBJECT:
      return PREFETCH_OBJECT_STR;
    case REFRESH_OBJECT:
      return REFRESH_OBJECT_STR;
    case PUT_DOMAIN_MAPPING:
      return PUT_DOMAIN_MAPPING_STR;
    case LIST_DOMAIN_MAPPINGS:
      return LIST_DOMAIN_MAPPINGS_STR;
    case DELETE_DOMAIN_MAPPING:
      return DELETE_DOMAIN_MAPPING_STR;
    case UNKNOWN:
      return UNKNOWN_STR;
    default:
      throw GalaxyFDSClientException("Unrecognized enum value");
  }
}

} // namespace fds
} // namespace galaxy
