#include <memory>

#include <Poco/Net/HTTPSessionFactory.h>
#include <Poco/Net/HTTPSessionInstantiator.h>
#include <Poco/Net/HTTPSSessionInstantiator.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
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
using namespace Poco::Net;
using namespace Poco::JSON;

class GalaxyFDSClientTest : public testing::Test {
protected:
  static void SetUpTestCase() {
    string accesskey = "{accessKey}";
    string secretKey = "{secretKey}";
    FDSClientConfiguration config;
    config.enableHttps(false);
    config.enableCdnForDownload(false);
    _fdsClient = new GalaxyFDSClient(accesskey, secretKey, config);
  }

  void SetUp() {
    vector<shared_ptr<FDSBucket> > bucketList = _fdsClient->listBuckets();
    for (auto b: bucketList) {
      string bucketName = b->name();
      if (0 == bucketName.compare(0, 8, "cpp-test")) {
        ClearBucket(bucketName);
        _fdsClient->deleteBucket(bucketName);
      }
    }
  }

  static void TearDownTestCase() {
    delete _fdsClient;
  }

  void ClearBucket(const string& bucketName);

  static GalaxyFDS* _fdsClient;
};

GalaxyFDS* GalaxyFDSClientTest::_fdsClient = 0;

TEST_F(GalaxyFDSClientTest, GetObjectTest) {
  string bucketName = "cpp-test-get-object-test-bucket-name";
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

TEST_F(GalaxyFDSClientTest, GetObjectByRangeTest) {
  string bucketName = "cpp-test-get-object-byrange-test-bucket-name";
  string objectName = "get-object-test-object-name";

  _fdsClient->createBucket(bucketName);
  string expectContent = "This is a test object";
  stringstream ss(expectContent);
  _fdsClient->putObject(bucketName, objectName, ss);
  long startPos = 0;
  long length = 5;
  shared_ptr<FDSObject> pObject = _fdsClient->getObject(bucketName, objectName, startPos, length);
  stringstream res;
  StreamCopier::copyStream(pObject->objectContent(), res);
  EXPECT_EQ(expectContent.substr(startPos, length), res.str());

  startPos = 5;
  pObject = _fdsClient->getObject(bucketName, objectName, startPos, length);
  stringstream res2;
  StreamCopier::copyStream(pObject->objectContent(), res2);
  EXPECT_EQ(expectContent.substr(startPos, length), res2.str());

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, PutObjectTest) {
  string bucketName = "cpp-test-put-object-test-bucket-name";
  string objectName = "put-object-test-object-name";

  _fdsClient->createBucket(bucketName);
  string expectContent = "This is a test object";
  stringstream ss(expectContent);
  _fdsClient->putObject(bucketName, objectName, ss);

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, PostObjectTest) {
  string bucketName = "cpp-test-post-object-test-bucket-name";
  string objectName = "post-object-test-object-name";

  _fdsClient->createBucket(bucketName);
  string expectContent = "This is a test object";
  stringstream ss(expectContent);
  shared_ptr<PutObjectResult> pResult = _fdsClient->postObject(bucketName, ss);

  _fdsClient->deleteObject(bucketName, pResult->objectName());
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, ListBucketsTest) {
  string bucketName = "cpp-test-list-buckets-test-bucket-name";

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
  string bucketName = "cpp-test-create-and-delete-bucket-test-bucket-name";

  _fdsClient->createBucket(bucketName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, DoesBucketExistTest) {
  string bucketName = "cpp-test-does-bucket-exist-test-bucket-name";

  bool exist = _fdsClient->doesBucketExist(bucketName);
  EXPECT_FALSE(exist);
  _fdsClient->createBucket(bucketName);
  exist = _fdsClient->doesBucketExist(bucketName);
  EXPECT_TRUE(exist);

  _fdsClient->deleteBucket(bucketName);
}

void GalaxyFDSClientTest::ClearBucket(const string& bucketName) {
  shared_ptr<FDSObjectListing> pObjectListing = _fdsClient->listObjects(
          bucketName, "", "");
  while (true) {
    vector<FDSObjectSummary> s = pObjectListing->objectSummaries();
    for (FDSObjectSummary i: s) {
      string objectName = i.objectName();
      _fdsClient->deleteObject(bucketName, objectName);
    }

    if (!pObjectListing->truncated()) {
      break;
    }

    pObjectListing = _fdsClient->listNextBatchOfObjects(*pObjectListing);
  }
}

TEST_F(GalaxyFDSClientTest, GetAndSetBucketAclTest) {
  string bucketName = "cpp-test-bucket-acl-test-bucket-name";

  _fdsClient->createBucket(bucketName);
  AccessControlList acl;
  acl.addGrant(Grant("user_read", Permission(Permission::READ),
        GrantType(GrantType::USER)));
  acl.addGrant(Grant("user_write_with_sso", Permission(Permission::SSO_WRITE),
        GrantType(GrantType::USER)));
  _fdsClient->setBucketAcl(bucketName, acl);
  shared_ptr<AccessControlList> pAcl = _fdsClient->getBucketAcl(bucketName);
  vector<Grant> grants = pAcl->getGrants();

  for (size_t i = 0; i < grants.size(); i++) {
    const std::string& id = grants[i].granteeId();
    Permission permission = grants[i].permission().value();
    GrantType grantType = grants[i].grantType().value();
    if (id == "user") {
      EXPECT_EQ(Permission::READ_STR, permission.name());
      EXPECT_EQ(GrantType::USER_STR, grantType.name());
    } else if (id == "user_write_with_sso") {
      EXPECT_EQ(Permission::SSO_WRITE_STR, permission.name());
      EXPECT_EQ(GrantType::USER_STR, grantType.name());
    }
  }

  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, GetAndSetBucketQuotaTest) {
  string bucketName = "cpp-test-bucket-quota-test-bucket-name";

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
  string bucketName = "cpp-test-metadata-test-bucket-name";
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
  string bucketName = "cpp-test-object-acl-test-bucket-name";
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
  string bucketName = "cpp-test-does-object-exist-test-bucket-name";
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
  string bucketName = "cpp-test-rename-object-test-bucket-name";
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
  string bucketName = "cpp-test-prefetch-object-test-bucket-name";
  string objectName = "prefetch-object-test-bucket-name";

  if (!_fdsClient->doesBucketExist(bucketName))
    _fdsClient->createBucket(bucketName);
  stringstream ss("This is a test object");
  _fdsClient->putObject(bucketName, objectName, ss);
  _fdsClient->setPublic(bucketName, objectName);
  _fdsClient->prefetchObject(bucketName, objectName);

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, RefreshObjectTest) {
  string bucketName = "cpp-test-refresh-object-test-bucket-name";
  string objectName = "refresh-object-test-bucket-name";

  if (!_fdsClient->doesBucketExist(bucketName)) {
    _fdsClient->createBucket(bucketName);
  }
  stringstream ss("This is a test object");
  _fdsClient->putObject(bucketName, objectName, ss);
  _fdsClient->setPublic(bucketName, objectName);
  _fdsClient->refreshObject(bucketName, objectName);

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}

TEST_F(GalaxyFDSClientTest, ListObjectsTest) {
  string bucketName = "cpp-test-list-objects-test-bucket-name";
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

TEST_F(GalaxyFDSClientTest, RestoreObjectsTest) {
  string bucketName = "cpp-test-restore-objects-test-bucket-name";
  string objectName = "bar";
  string objectContent = "blablab";
  stringstream ss(objectContent);

  // put object
  _fdsClient->createBucket(bucketName);
  _fdsClient->putObject(bucketName, objectName, ss);

  // delete object
  _fdsClient->deleteObject(bucketName, objectName);
  ASSERT_THROW(_fdsClient->getObject(bucketName, objectName), GalaxyFDSClientException);

  // restore object
  _fdsClient->restoreObject(bucketName, objectName);

  // check object
  shared_ptr<FDSObject> object = _fdsClient->getObject(bucketName, objectName);
  stringstream res;
  StreamCopier::copyStream(object->objectContent(), res);
  ASSERT_EQ(objectContent, res.str());
}

string lowercase(const string& s) {
  string r;
  for (auto c: s) {
    r.push_back(c <= 'z' && c >= 'a' ? c : c + 'a' - 'A');
  }

  return r;
}

TEST_F(GalaxyFDSClientTest, PutWithPresignedURL) {
  const string bucketName = "cpp-test-put-with-presigned-url";
  const string objectName = "object-name";
  const string contentType = "text/blah";
  const string objectContent = "This is a test object";
  stringstream ss(objectContent);

  time_t t = time(NULL);
  // generate uri
  string uri = _fdsClient->generatePresignedUri(bucketName, objectName,
                            t * 1000 + 60000, "PUT", contentType);

  URI pocoUri(uri);
  bool r = pocoUri.isRelative();
  std::shared_ptr<Poco::Net::HTTPSessionFactory> _pSessionFactory(new HTTPSessionFactory());
  Poco::Net::Context*  _pContext = new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE);
  _pSessionFactory->registerProtocol("https", new HTTPSSessionInstantiator(
          _pContext));
  _pSessionFactory->registerProtocol("http", new HTTPSessionInstantiator());
  shared_ptr<HTTPClientSession> pSession(_pSessionFactory->createClientSession(
          pocoUri));

  _fdsClient->createBucket(bucketName);
  // put object
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  request.setContentType(contentType);
  request.setContentLength(objectContent.size());
  HTTPResponse response;
  ostream& os = pSession->sendRequest(request);
  StreamCopier::copyStream(ss, os);
  istream& rs = pSession->receiveResponse(response);

  ASSERT_EQ(200, response.getStatus());

  // check object
  shared_ptr<FDSObject> object = _fdsClient->getObject(bucketName, objectName);
  stringstream res;
  StreamCopier::copyStream(object->objectContent(), res);
  ASSERT_EQ(objectContent, res.str());
  map<string, string> m = object->objectMetadata().metadata();
  string contentTypeGot;
  for (auto i: m) {
    string k = i.first;
    string v = i.second;
    if (lowercase(k) == lowercase(Constants::CONTENT_TYPE)) {
      contentTypeGot = v;
      break;
    }
  }

  ASSERT_EQ(contentType, contentTypeGot);

  _fdsClient->deleteObject(bucketName, objectName);
  _fdsClient->deleteBucket(bucketName);
}
