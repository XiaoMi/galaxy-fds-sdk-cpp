#include <memory>

#include <Poco/StreamCopier.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormatter.h>

#include "gtest/gtest.h"

#include "fds_client_configuration.h"
#include "galaxy_fds_client.h"
#include "galaxy_fds_client_exception.h"
#include "model/access_control_list.h"
#include "model/fds_bucket.h"
#include "model/fds_object.h"
#include "model/fds_object_listing.h"
#include "model/put_object_result.h"
#include "model/quota_policy.h"
#include "model/sub_resource.h"

using namespace std;
using namespace galaxy::fds;
using namespace Poco;

class GalaxyFDSClientTest : public testing::Test {
protected:
  static void SetUpTestCase() {
    string accesskey = "{access_key}";
    string secretKey = "{secret_key}";
    FDSClientConfiguration config;
    config.enableHttps(false);
    config.enableCdnForDownload(false);
    _fdsClient = new GalaxyFDSClient(accesskey, secretKey, config);
  }

  static void TearDownTestCase() {
    delete _fdsClient;
  }

  static GalaxyFDS* _fdsClient;
};

GalaxyFDS* GalaxyFDSClientTest::_fdsClient = 0;

TEST_F(GalaxyFDSClientTest, GetObjectTest) {
  string bucketName = "get-object-test-bucket-name";
  string objectName = "get-object-test-object-name";

  _fdsClient->createBucket(bucketName);
  string expectContent = "This is a test object";
  stringstream ss(expectContent);
  _fdsClient->putObject(bucketName, objectName, ss);
  shared_ptr<FDSObject> pObject = _fdsClient->getObject(bucketName, objectName);
  stringstream res;
  StreamCopier::copyStream(pObject->objectContent(), res);
  EXPECT_EQ(expectContent, res.str());

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, PutObjectTest) {
  string bucketName = "put-object-test-bucket-name";
  string objectName = "put-object-test-object-name";

  _fdsClient->createBucket(bucketName);
  string expectContent = "This is a test object";
  stringstream ss(expectContent);
  _fdsClient->putObject(bucketName, objectName, ss);

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, PostObjectTest) {
  string bucketName = "post-object-test-bucket-name";
  string objectName = "post-object-test-object-name";

  _fdsClient->createBucket(bucketName);
  string expectContent = "This is a test object";
  stringstream ss(expectContent);
  shared_ptr<PutObjectResult> pResult = _fdsClient->postObject(bucketName, ss);

  _fdsClient->deleteObject(bucketName, pResult->objectName());
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, ListBucketsTest) {
  string bucketName = "list-buckets-test-bucket-name";

  _fdsClient->createBucket(bucketName);
  vector<shared_ptr<FDSBucket> > buckets = _fdsClient->listBuckets();
  bool flag = false;
  for (size_t i = 0; i < buckets.size(); i++) {
    if (buckets[i]->name() == bucketName) {
      flag = true;
      break;
    }
  }
  EXPECT_TRUE(true);

  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, CreateAndDeleteBucketTest) {
  string bucketName = "create-and-delete-bucket-test-bucket-name";

  _fdsClient->createBucket(bucketName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, DoesBucketExistTest) {
  string bucketName = "does-bucket-exist-test-bucket-name";

  bool exist = _fdsClient->doesBucketExist(bucketName);
  EXPECT_FALSE(exist);
  _fdsClient->createBucket(bucketName);
  exist = _fdsClient->doesBucketExist(bucketName);
  EXPECT_TRUE(exist);

  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, GetAndSetBucketAclTest) {
  string bucketName = "bucket-acl-test-bucket-name";

  _fdsClient->createBucket(bucketName);
  AccessControlList acl;
  acl.addGrant(Grant("user", Permission(Permission::READ),
        GrantType(GrantType::USER)));
  _fdsClient->setBucketAcl(bucketName, acl);
  shared_ptr<AccessControlList> pAcl = _fdsClient->getBucketAcl(bucketName);
  vector<Grant> grants = pAcl->getGrants();

  for (size_t i = 0; i < grants.size(); i++) {
    if (grants[i].granteeId() == "user") {
      EXPECT_TRUE(Permission::READ == grants[i].permission().value());
      EXPECT_TRUE(GrantType::USER == grants[i].grantType().value());
    }
  }

  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, GetAndSetBucketQuotaTest) {
  string bucketName = "bucket-quota-test-bucket-name";

  _fdsClient->createBucket(bucketName);
  QuotaPolicy quotaPolicy;
  quotaPolicy.addQuota(Quota(QuotaType::QPS, Action::GET_OBJECT, 100));
  _fdsClient->setBucketQuota(bucketName, quotaPolicy);

  shared_ptr<QuotaPolicy> pQuotaPolicy = _fdsClient->getBucketQuota(bucketName);
  const vector<Quota>& quotas = pQuotaPolicy->quotas();
  EXPECT_EQ(1, quotas.size());
  EXPECT_TRUE(QuotaType::QPS == quotas[0].type().value());
  EXPECT_TRUE(Action::GET_OBJECT == quotas[0].action().value());
  EXPECT_EQ(100, quotas[0].value());

  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, GetObjectMetadataTest) {
  string bucketName = "metadata-test-bucket-name";
  string objectName = "metadata-test-bucket-name";

  _fdsClient->createBucket(bucketName);
  stringstream ss("This is a test object");
  FDSObjectMetadata metadata;
  string key = Constants::USER_DEFINED_METADATA_PREFIX + "test";
  metadata.add(key, "test-value");
  _fdsClient->putObject(bucketName, objectName, ss, metadata);

  shared_ptr<FDSObjectMetadata> pMetadata =  _fdsClient->getObjectMetadata(
      bucketName, objectName);
  map<string, string>::const_iterator iter = pMetadata->metadata().find(key);
  EXPECT_EQ(iter->second, "test-value");

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, GetAndSetObjectAclTest) {
  string bucketName = "object-acl-test-bucket-name";
  string objectName = "object-acl-test-bucket-name";

  _fdsClient->createBucket(bucketName);
  stringstream ss("This is a test object");
  _fdsClient->putObject(bucketName, objectName, ss);
  AccessControlList acl;
  acl.addGrant(Grant("user", Permission(Permission::READ), GrantType::USER));
  _fdsClient->setObjectAcl(bucketName, objectName, acl);
  vector<Grant> grants = _fdsClient->getObjectAcl(bucketName, objectName)->getGrants();

  for (size_t i = 0; i < grants.size(); i++) {
    if (grants[i].granteeId() == "user") {
      EXPECT_TRUE(Permission::READ == grants[i].permission().value());
      EXPECT_TRUE(GrantType::USER == grants[i].grantType().value());
    }
  }

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, DoesObjectExistTest) {
  string bucketName = "does-object-exist-test-bucket-name";
  string objectName = "does-object-exist-test-bucket-name";

  _fdsClient->createBucket(bucketName);
  bool exist = _fdsClient->doesObjectExist(bucketName, objectName);
  EXPECT_FALSE(exist);
  stringstream ss("This is a test object");
  _fdsClient->putObject(bucketName, objectName, ss);
  exist = _fdsClient->doesObjectExist(bucketName, objectName);
  EXPECT_TRUE(exist);

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, RenameObjectTest) {
  string bucketName = "rename-object-test-bucket-name";
  string objectName = "rename-object-test-bucket-name";
  string dstObjectName = "rename-object-test-bucket-name-dst";

  _fdsClient->createBucket(bucketName);
  stringstream ss("This is a test object");
  _fdsClient->putObject(bucketName, objectName, ss);
  bool exist = _fdsClient->doesObjectExist(bucketName, dstObjectName);
  EXPECT_FALSE(exist);
  _fdsClient->renameObject(bucketName, objectName, dstObjectName);
  exist = _fdsClient->doesObjectExist(bucketName, dstObjectName);
  EXPECT_TRUE(exist);
  exist = _fdsClient->doesObjectExist(bucketName, objectName);
  EXPECT_FALSE(exist);

  _fdsClient->deleteObject(bucketName, dstObjectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, PrefetchObjectTest) {
  string bucketName = "prefetch-object-test-bucket-name";
  string objectName = "prefetch-object-test-bucket-name";

  _fdsClient->createBucket(bucketName);
  stringstream ss("This is a test object");
  _fdsClient->putObject(bucketName, objectName, ss);
  _fdsClient->setPublic(bucketName, objectName, true);
  _fdsClient->prefetchObject(bucketName, objectName);

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, RefreshObjectTest) {
  string bucketName = "refresh-object-test-bucket-name";
  string objectName = "refresh-object-test-bucket-name";

  _fdsClient->createBucket(bucketName);
  stringstream ss("This is a test object");
  _fdsClient->putObject(bucketName, objectName, ss);
  _fdsClient->setPublic(bucketName, objectName, true);
  _fdsClient->refreshObject(bucketName, objectName);

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, ListObjectsTest) {
  string bucketName = "list-objects-test-bucket-name";
  stringstream ss;

  _fdsClient->createBucket(bucketName);
  _fdsClient->putObject(bucketName, "foo/bar/test", ss);
  _fdsClient->putObject(bucketName, "foo", ss);
  _fdsClient->putObject(bucketName, "bar", ss);

  shared_ptr<FDSObjectListing> pObjectListing = _fdsClient->listObjects(
      bucketName, "", "");
  EXPECT_EQ(3, pObjectListing->objectSummaries().size());
  EXPECT_EQ(0, pObjectListing->commonPrefixes().size());

  pObjectListing = _fdsClient->listObjects(
      bucketName, "foo", "");
  EXPECT_EQ(2, pObjectListing->objectSummaries().size());
  EXPECT_EQ(0, pObjectListing->commonPrefixes().size());

  pObjectListing = _fdsClient->listObjects(
      bucketName, "foo/", "/");
  EXPECT_EQ(0, pObjectListing->objectSummaries().size());
  EXPECT_EQ(1, pObjectListing->commonPrefixes().size());

  _fdsClient->deleteObject(bucketName, "foo/bar/test");
  _fdsClient->deleteObject(bucketName, "foo");
  _fdsClient->deleteObject(bucketName, "bar");
  _fdsClient->deleteBucket(bucketName);
}
