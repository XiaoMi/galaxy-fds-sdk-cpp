#include <string>
#include <algorithm>

#include <Poco/StreamCopier.h>

#include "gtest/gtest.h"

#include "constants.h"
#include "galaxy_fds_client_exception.h"
#include "model/fds_bucket.h"
#include "model/quota_policy.h"
#include "model/fds_object_summary.h"
#include "model/fds_object_metadata.h"
#include "model/fds_object_listing.h"
#include "model/put_object_result.h"
#include "model/sub_resource.h"
#include "model/access_control_list.h"

using namespace std;
using namespace galaxy::fds;
using namespace Poco;

TEST(FDSBucketTest, Positive) {
  Owner expectOwner;
  string expectId = "id";
  string expectDisplayName = "displayName";
  expectOwner.setId(expectId);
  expectOwner.setDisplayName(expectDisplayName);

  string expectName = "name";
  time_t expectCreationDate;
  time(&expectCreationDate);

  FDSBucket bucket(expectName);
  bucket.setCreationDate(expectCreationDate);
  bucket.setOwner(expectOwner);

  EXPECT_EQ(expectId, bucket.owner().id());
  EXPECT_EQ(expectDisplayName, bucket.owner().displayName());
  EXPECT_EQ(expectName, bucket.name());
  EXPECT_EQ(expectCreationDate, bucket.creationDate());
}

TEST(QuotaPolicyTest, Positive) {
  QuotaType expectQuotaType = QuotaType::QPS;
  Action expectAction = Action::GET_OBJECT;
  long expectValue = 10;
  Quota expectQuota(expectQuotaType, expectAction, expectValue);

  QuotaPolicy expectQuotaPolicy;
  expectQuotaPolicy.addQuota(expectQuota);
  
  EXPECT_EQ(1, expectQuotaPolicy.quotas().size());
  Quota actualQuota = expectQuotaPolicy.quotas()[0];
  EXPECT_EQ(expectQuotaType.value(), actualQuota.type().value());
  EXPECT_EQ(expectAction.value(), actualQuota.action().value());
  EXPECT_EQ(expectValue, actualQuota.value());

  // serialization
  expectQuotaPolicy.addQuota(Quota(QuotaType::THROUGHPUT, Action::PUT_OBJECT, 100));
  EXPECT_EQ(2, expectQuotaPolicy.quotas().size());
  string expectQuotaPolicyStr = QuotaPolicy::serialize(expectQuotaPolicy);
  stringstream ss(expectQuotaPolicyStr);
  QuotaPolicy actualQuotaPolicy = *QuotaPolicy::deserialize(ss);
  string actualQuotaPolicyStr = QuotaPolicy::serialize(actualQuotaPolicy);
  EXPECT_EQ(expectQuotaPolicyStr, actualQuotaPolicyStr);
}

TEST(FDSObjectSummaryTest, Positive) {
  Owner expectOwner;
  string expectId = "id";
  string expectDisplayName = "displayName";
  expectOwner.setId(expectId);
  expectOwner.setDisplayName(expectDisplayName);

  string expectBucketName = "name";
  string expectObjectName = "object-name";
  long expectSize = 10;

  FDSObjectSummary objectSummary;
  objectSummary.setOwner(expectOwner);
  objectSummary.setBucketName(expectBucketName);
  objectSummary.setObjectName(expectObjectName);
  objectSummary.setSize(expectSize);

  EXPECT_EQ(expectId, objectSummary.owner().id());
  EXPECT_EQ(expectDisplayName, objectSummary.owner().displayName());
  EXPECT_EQ(expectBucketName, objectSummary.bucketName());
  EXPECT_EQ(expectObjectName, objectSummary.objectName());
  EXPECT_EQ(expectSize, objectSummary.size());
}

TEST(FDSObjectMetadataTest, Positive) {
  FDSObjectMetadata metadata;
  metadata.add(Constants::USER_DEFINED_METADATA_PREFIX
      + "test", "value");
  metadata.add(Constants::CONTENT_LENGTH, "10");

  EXPECT_EQ(2, metadata.metadata().size());
}

TEST(FDSObjectMetadataTest, Negative) {
  FDSObjectMetadata metadata;
  ASSERT_THROW(metadata.add("wrong_key", "value"),
      GalaxyFDSClientException);
  EXPECT_EQ(0, metadata.metadata().size());
}

TEST(FDSObjectListingTest, Positive) {
  string expectBucketName = "bucket-name";
  string expectPrefix = "prefix";
  string expectDelimiter = "delimiter";
  string expectMarker = "marker";
  string expectNextMarker = "next-marker";
  int expectMaxKeys = 10;
  bool expectTruncated = false;
  vector<FDSObjectSummary> expectObjectSummaries;
  expectObjectSummaries.push_back(FDSObjectSummary());
  string expectCommonPrefix = "common-prefix";
  vector<string> expectCommonPrefixes;
  expectCommonPrefixes.push_back(expectCommonPrefix);

  FDSObjectListing expectObjectListing;
  expectObjectListing.setBucketName(expectBucketName);
  expectObjectListing.setPrefix(expectPrefix);
  expectObjectListing.setDelimiter(expectDelimiter);
  expectObjectListing.setMarker(expectMarker);
  expectObjectListing.setNextMarker(expectNextMarker);
  expectObjectListing.setMaxKeys(expectMaxKeys);
  expectObjectListing.setTruncated(expectTruncated);
  expectObjectListing.setObjectSummaries(expectObjectSummaries);
  expectObjectListing.setCommonPrefixes(expectCommonPrefixes);

  EXPECT_EQ(expectBucketName, expectObjectListing.bucketName());
  EXPECT_EQ(expectPrefix, expectObjectListing.prefix());
  EXPECT_EQ(expectDelimiter, expectObjectListing.delimiter());
  EXPECT_EQ(expectMarker, expectObjectListing.marker());
  EXPECT_EQ(expectNextMarker, expectObjectListing.nextMarker());
  EXPECT_EQ(expectMaxKeys, expectObjectListing.maxKeys());
  EXPECT_EQ(expectTruncated, expectObjectListing.truncated());
  EXPECT_EQ(1, expectObjectListing.objectSummaries().size());
  EXPECT_EQ(1, expectObjectListing.commonPrefixes().size());
  EXPECT_EQ(expectCommonPrefix, expectObjectListing.commonPrefixes()[0]);
}

TEST(PutObjectResultTest, Positive) {
  string expectBucketName = "bucket-name";
  string expectObjectname = "object-name";
  string expectAccessKeyId = "access-id";
  string expectSignature = "signature";
  long expectExpires = 100;

  PutObjectResult expectPutObjectResult;
  expectPutObjectResult.setBucketName(expectBucketName);
  expectPutObjectResult.setObjectName(expectObjectname);
  expectPutObjectResult.setAccessKeyId(expectAccessKeyId);
  expectPutObjectResult.setSignature(expectSignature);
  expectPutObjectResult.setExpires(expectExpires);

  // verify accessors
  EXPECT_EQ(expectBucketName, expectPutObjectResult.bucketName());
  EXPECT_EQ(expectObjectname, expectPutObjectResult.objectName());
  EXPECT_EQ(expectAccessKeyId, expectPutObjectResult.accessKeyId());
  EXPECT_EQ(expectSignature, expectPutObjectResult.signature());
  EXPECT_EQ(expectExpires, expectPutObjectResult.expires());

  // verify serialization
  string json = PutObjectResult::serialize(expectPutObjectResult);
  stringstream ss(json);
  PutObjectResult actualPutObjectResult = *PutObjectResult::deserialize(ss);
  EXPECT_EQ(expectPutObjectResult.bucketName(), actualPutObjectResult.bucketName());
  EXPECT_EQ(expectPutObjectResult.objectName(), actualPutObjectResult.objectName());
  EXPECT_EQ(expectPutObjectResult.accessKeyId(), actualPutObjectResult.accessKeyId());
  EXPECT_EQ(expectPutObjectResult.signature(), actualPutObjectResult.signature());
  EXPECT_EQ(expectPutObjectResult.expires(), actualPutObjectResult.expires());
}

TEST(SubResourceTest, Positive) {
  vector<string>& subResources = SubResource::SUB_RESOURCES;
  EXPECT_EQ(7, subResources.size());

  vector<string>::iterator iter;
  iter = find(subResources.begin(), subResources.end(), SubResource::ACL);
  EXPECT_NE(iter, subResources.end());
  iter = find(subResources.begin(), subResources.end(), SubResource::QUOTA);
  EXPECT_NE(iter, subResources.end());
  iter = find(subResources.begin(), subResources.end(), SubResource::UPLOADS);
  EXPECT_NE(iter, subResources.end());
  iter = find(subResources.begin(), subResources.end(), SubResource::PART_NUMBER);
  EXPECT_NE(iter, subResources.end());
  iter = find(subResources.begin(), subResources.end(), SubResource::UPLOAD_ID);
  EXPECT_NE(iter, subResources.end());
  iter = find(subResources.begin(), subResources.end(), SubResource::STORAGE_ACCESS_TOKEN);
  EXPECT_NE(iter, subResources.end());
  iter = find(subResources.begin(), subResources.end(), SubResource::METADATA);
  EXPECT_NE(iter, subResources.end());
}

TEST(AccessControlListTest, Positive) {
  AccessControlList expectAcl;
  expectAcl.addGrant(Grant("user1", Permission::READ, GrantType::USER));
  expectAcl.addGrant(Grant("user1", Permission::WRITE, GrantType::USER));
  expectAcl.addGrant(Grant("user2", Permission::WRITE, GrantType::USER));
  expectAcl.addGrant(Grant("user3", Permission::READ_OBJECTS, GrantType::USER));
  expectAcl.addGrant(Grant("admin", Permission::FULL_CONTROL, GrantType::USER));
  EXPECT_EQ(5, expectAcl.getGrants().size());

  string expectAclStr = AccessControlList::serialize(expectAcl);
  stringstream ss(expectAclStr);
  AccessControlList actualAcl = *AccessControlList::deserialize(ss);
  string actualAclStr = AccessControlList::serialize(actualAcl);
  EXPECT_EQ(expectAclStr, actualAclStr);
}
