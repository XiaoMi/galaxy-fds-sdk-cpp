/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_FDS_CLIENT_H_
#define GALAXY_FDS_CLIENT_H_

#include <sstream>

#include "galaxy_fds.h"

namespace Poco {
namespace Net {

class Context;
class HTTPSessionFactory;
class HTTPRequest;
class HTTPResponse;

}} // namespace Poco::Net

namespace galaxy {
namespace fds {

class FDSClientConfiguration;

class GalaxyFDSClient : public GalaxyFDS {
public:
  GalaxyFDSClient(const std::string& accessKey, const std::string& secretKey,
      const FDSClientConfiguration& config);

  ~GalaxyFDSClient() override = default;

  static const std::string DATE_FORMAT;

  std::vector<std::shared_ptr<FDSBucket> > listBuckets() override;

  void createBucket(const std::string& bucketName) override;

  void deleteBucket(const std::string& bucketName) override;

  bool doesBucketExist(const std::string& bucketName) override;

  std::shared_ptr<AccessControlList> getBucketAcl(const std::string& bucketName) override;

  void setBucketAcl(const std::string& bucketName, const AccessControlList&
      acl) override;

  std::shared_ptr<QuotaPolicy> getBucketQuota(const std::string& bucketName) override;

  void setBucketQuota(const std::string& bucketName, const QuotaPolicy&
      quotaPolicy) override;

  std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName) override;

  std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName,
      const std::string& prefix) override;

  std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName,
      const std::string& prefix, const std::string& delimiter) override;

  std::shared_ptr<FDSObjectListing> listNextBatchOfObjects(
      const FDSObjectListing& previousObjectListing) override;

  std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName, bool withMetaData) override;

  std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName,
      const std::string& prefix, bool withMetaData) override;

  std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName,
      const std::string& prefix, const std::string& delimiter, bool withMetaData) override;

  std::shared_ptr<FDSObjectListing> listNextBatchOfObjects(
      const FDSObjectListing& previousObjectListing, bool withMetaData) override;

  std::shared_ptr<PutObjectResult> putObject(const std::string& bucketName,
      const std::string& objectName, std::istream& is) override;

  std::shared_ptr<PutObjectResult> putObject(const std::string& bucketName,
      const std::string& objectName, std::istream& is,
      const FDSObjectMetadata& metadata) override;

  std::shared_ptr<PutObjectResult> postObject(const std::string& bucketName,
      std::istream& is) override;

  std::shared_ptr<PutObjectResult> postObject(const std::string& bucketName,
      std::istream& is, const FDSObjectMetadata& metadata) override;

  std::shared_ptr<FDSObject> getObject(const std::string& bucketName,
      const std::string& objectName) override;

  std::shared_ptr<FDSObject> getObject(const std::string& bucketName,
      const std::string& objectName, long pos) override;

  std::shared_ptr<FDSObject> getObject(const std::string& bucketName,
      const std::string& objectName, long pos, long len) override;

  std::shared_ptr<FDSObjectMetadata> getObjectMetadata(const std::string&
      bucketName, const std::string& objectName) override;

  std::shared_ptr<AccessControlList> getObjectAcl(const std::string& bucketName,
      const std::string& objectName) override;

  void setObjectAcl(const std::string& bucketName, const std::string&
      objectName,
      const AccessControlList& acl) override;

  bool doesObjectExist(const std::string& bucketName, const std::string&
      objectName) override;

  void deleteObject(const std::string& bucketName, const std::string&
      objectName) override;

  void deleteObject(const std::string& bucketName, const std::string&
      objectName, bool enableTrash) override;

  std::shared_ptr<FDSObjectsDeleting> deleteObjects(const std::string& bucketName,
      const std::vector<std::string>& objectNameList) override;

  std::shared_ptr<FDSObjectsDeleting> deleteObjects(const std::string& bucketName,
      const std::vector<std::string>& objectNameList, bool enableTrash) override;

  std::shared_ptr<FDSObjectsDeleting> deleteObjects(const std::string& bucketName,
      const std::string& prefix) override;

  std::shared_ptr<FDSObjectsDeleting> deleteObjects(const std::string& bucketName,
      const std::string& prefix, bool enableTrash) override;

  void restoreObject(const std::string& bucketName, const std::string&
     objectName) override;

  void renameObject(const std::string& bucketName, const std::string&
      srcObjectName, const std::string& dstObjectname) override;

  void prefetchObject(const std::string& bucketName, const std::string&
      objectName) override;

  void refreshObject(const std::string& bucketName, const std::string&
      objectName) override;

  void setPublic(const std::string& bucketName, const std::string& objectName) override;

  std::string generateDownloadObjectUri(const std::string& bucketName,
      const std::string& objectName) override;

  std::string generatePresignedUri(const std::string& bucketName,
      const std::string& objectName, time_t expiration,
      const std::string& httpMethod) override;

  std::string generatePresignedUri(const std::string& bucketName,
      const std::string& objectName, time_t expiration,
      const std::string& httpMethod, const std::string& contentType) override;

  std::string generatePresignedCdnUri(const std::string& bucketName,
      const std::string& objectName, time_t expiration,
      const std::string& httpMethod) override;

  void setObjectMetadata(const std::string& bucketName, const std::string&
        objectName, const MetadataBean& metadata) override;

private:
  std::string formatUri(const std::string& baseUri, const std::string&
      resource, const std::vector<std::string>& subResourceParams);

  std::shared_ptr<FDSObjectMetadata> parseMetadata(
      const Poco::Net::HTTPResponse& response);

  void prepareRequestHeaders(const std::string& uri,
      const std::string& httpMethod, const std::string& mediaType,
      std::istream& is, const FDSObjectMetadata& metadata,
      Poco::Net::HTTPRequest& request);

  std::string generatePresignedUri(const std::string& baseUri, const
      std::string& bucketName, const std::string& objectName, time_t expiration,
      const std::string& httpMethod, const std::string& contentType);

private:
  std::shared_ptr<Poco::Net::HTTPSessionFactory> _pSessionFacotry;

  std::shared_ptr<FDSClientConfiguration> _pConfig;
  std::string _accessKey;
  std::string _secretKey;
  std::stringstream _emptyStream;
  std::vector<std::string> _emptySubResources;
  std::shared_ptr<FDSObjectMetadata> _pEmptyMetadata;
}; // class GalaxyFDSClient

} // namespace fds
} // namespace galaxy

#endif // GALAXY_FDS_CLIENT_H_
