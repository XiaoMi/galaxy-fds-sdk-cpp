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

  static const std::string DATE_FORMAT;
  
  std::vector<std::shared_ptr<FDSBucket> > listBuckets();

  void createBucket(const std::string& bucketName);

  void deleteBucket(const std::string& bucketName);

  bool doesBucketExist(const std::string& bucketName);

  std::shared_ptr<AccessControlList> getBucketAcl(const std::string& bucketName);

  void setBucketAcl(const std::string& bucketName, const AccessControlList&
      acl);

  std::shared_ptr<QuotaPolicy> getBucketQuota(const std::string& bucketName);

  void setBucketQuota(const std::string& bucketName, const QuotaPolicy&
      quotaPolicy);

  std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName);

  std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName,
      const std::string& prefix);

  std::shared_ptr<FDSObjectListing> listObjects(const std::string& bucketName,
      const std::string& prefix, const std::string& delimiter);

  std::shared_ptr<FDSObjectListing> listNextBatchOfObjects(
      const FDSObjectListing& previousObjectListing);

  std::shared_ptr<PutObjectResult> putObject(const std::string& bucketName,
      const std::string& objectName, std::istream& is);

  std::shared_ptr<PutObjectResult> putObject(const std::string& bucketName,
      const std::string& objectName, std::istream& is,
      const FDSObjectMetadata& metadata);

  std::shared_ptr<PutObjectResult> postObject(const std::string& bucketName,
      std::istream& is);

  std::shared_ptr<PutObjectResult> postObject(const std::string& bucketName,
      std::istream& is, const FDSObjectMetadata& metadata);

  std::shared_ptr<FDSObject> getObject(const std::string& bucketName,
      const std::string& objectName);

  std::shared_ptr<FDSObject> getObject(const std::string& bucketName,
      const std::string& objectName, long pos);

  std::shared_ptr<FDSObjectMetadata> getObjectMetadata(const std::string&
      bucketName, const std::string& objectName);

  std::shared_ptr<AccessControlList> getObjectAcl(const std::string& bucketName,
      const std::string& objectName);

  void setObjectAcl(const std::string& bucketName, const std::string&
      objectName,
      const AccessControlList& acl);

  bool doesObjectExist(const std::string& bucketName, const std::string&
      objectName);

  void deleteObject(const std::string& bucketName, const std::string&
      objectName);

  void restoreObject(const std::string& bucketName, const std::string&
     objectName);

  void renameObject(const std::string& bucketName, const std::string&
      srcObjectName, const std::string& dstObjectname);

  void prefetchObject(const std::string& bucketName, const std::string&
      objectName);

  void refreshObject(const std::string& bucketName, const std::string&
      objectName);

  void setPublic(const std::string& bucketName, const std::string& objectName);

  std::string generateDownloadObjectUri(const std::string& bucketName,
      const std::string& objectName);

  std::string generatePresignedUri(const std::string& bucketName,
      const std::string& objectName, time_t expiration,
      const std::string& httpMethod);

  std::string generatePresignedUri(const std::string& bucketName,
      const std::string& objectName, time_t expiration,
      const std::string& httpMethod, const std::string& contentType);

  std::string generatePresignedCdnUri(const std::string& bucketName,
      const std::string& objectName, time_t expiration,
      const std::string& httpMethod);

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
  Poco::Net::Context* _pContext; 

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
