/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_FDS_H_
#define GALAXY_FDS_H_

#include <string>
#include <memory>
#include <vector>

namespace galaxy {
namespace fds {

class AccessControlList;
class FDSBucket;
class FDSObject;
class FDSObjectListing;
class FDSObjectsDeleting;
class FDSObjectMetadata;
class PutObjectResult;
class QuotaPolicy;
class MetadataBean;

class GalaxyFDS {
public:
  virtual std::vector<std::shared_ptr<FDSBucket> > listBuckets() = 0;

  virtual void createBucket(const std::string& bucketName) = 0;

  virtual void deleteBucket(const std::string& bucketName) = 0;

  virtual bool doesBucketExist(const std::string& bucketName) = 0;

  virtual std::shared_ptr<AccessControlList> getBucketAcl(const std::string&
      bucketName) = 0;

  virtual void setBucketAcl(const std::string& bucketName,
      const AccessControlList& acl) = 0;

  virtual std::shared_ptr<QuotaPolicy> getBucketQuota(const std::string&
      bucketName) = 0;

  virtual void setBucketQuota(const std::string& bucketName,
      const QuotaPolicy& quotaPolicy) = 0;

  virtual std::shared_ptr<FDSObjectListing> listObjects(const std::string&
      bucketName) = 0;

  virtual std::shared_ptr<FDSObjectListing> listObjects(const std::string&
      bucketName, const std::string& prefix) = 0;

  virtual std::shared_ptr<FDSObjectListing> listObjects(const std::string&
      bucketName, const std::string& prefix, const std::string& delimiter) = 0;

  virtual std::shared_ptr<FDSObjectListing> listNextBatchOfObjects(
      const FDSObjectListing& previousObjectListing) = 0;

  virtual std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName,
      bool withMetaData) = 0;

  virtual std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName,
      const std::string& prefix, bool withMetaData) = 0;

  virtual std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName,
      const std::string& prefix, const std::string& delimiter, bool withMetaData) = 0;

  virtual std::shared_ptr<FDSObjectListing> listNextBatchOfObjects(
      const FDSObjectListing& previousObjectListing, bool withMetaData) = 0;

  virtual std::shared_ptr<PutObjectResult> putObject(const std::string&
      bucketName, const std::string& objectName, std::istream& is) = 0;

  virtual std::shared_ptr<PutObjectResult> putObject(const std::string&
      bucketName, const std::string& objectName, std::istream& is,
      const FDSObjectMetadata& metadata) = 0;

  virtual std::shared_ptr<PutObjectResult> postObject(const std::string&
      bucketName, std::istream& is) = 0;

  virtual std::shared_ptr<PutObjectResult> postObject(const std::string&
      bucketName, std::istream& is, const FDSObjectMetadata& metadata) = 0;

  virtual std::shared_ptr<FDSObject> getObject(const std::string& bucketName,
      const std::string& objectName) = 0;

  virtual std::shared_ptr<FDSObject> getObject(const std::string& bucketName,
      const std::string& objectName, long pos) = 0;

  virtual std::shared_ptr<FDSObject> getObject(const std::string& bucketName,
      const std::string& objectName, long pos, long len) = 0;

  virtual std::shared_ptr<FDSObjectMetadata> getObjectMetadata(
      const std::string& bucketName, const std::string& objectName) = 0;

  virtual std::shared_ptr<AccessControlList> getObjectAcl(const std::string&
      bucketName, const std::string& objectName) = 0;

  virtual void setObjectAcl(const std::string& bucketName, const std::string&
      objectName, const AccessControlList& acl) = 0;

  virtual bool doesObjectExist(const std::string& bucketName, const std::string& 
      objectName) = 0;

  virtual void deleteObject(const std::string& bucketName, const std::string&
      objectName) = 0;

  virtual std::shared_ptr<FDSObjectsDeleting> deleteObjects(const std::string& bucketName,
      const std::vector<std::string>& objectNameList) = 0;

  // return object that encounter error when deleting
  virtual std::shared_ptr<FDSObjectsDeleting> deleteObjects(const std::string& bucketName,
      const std::string& prefix) = 0;

  virtual void restoreObject(const std::string& bucketName, const std::string&
      objectName) = 0;

  virtual void renameObject(const std::string& bucketName, const std::string&
      srcObjectName, const std::string& dstObjectName) = 0;

  virtual void prefetchObject(const std::string& bucketName, const std::string&
      objectName) = 0;

  virtual void refreshObject(const std::string& bucketName, const std::string&
      objectName) = 0;

  virtual void setPublic(const std::string& bucketName, const std::string&
      objectName) = 0;

  virtual std::string generateDownloadObjectUri(const std::string& bucketName,
      const std::string& objectName) = 0;

  virtual std::string generatePresignedUri(const std::string& bucketName,
      const std::string& objectName, time_t expiration,
      const std::string& httpMethod) = 0;

  virtual std::string generatePresignedUri(const std::string& bucketName,
      const std::string& objectName, time_t expiration,
      const std::string& httpMethod, const std::string& contentType) = 0;

  virtual std::string generatePresignedCdnUri(const std::string& bucketName,
      const std::string& objectName, time_t expiration,
      const std::string& httpMethod) = 0;
  virtual void setObjectMetadata(const std::string& bucketName, const std::string&
      objectName, const MetadataBean& metadata) = 0;

}; // classs GalaxyFDS

} // namespace fds
} // namespace galaxy

#endif // GALAXY_FDS_H_
