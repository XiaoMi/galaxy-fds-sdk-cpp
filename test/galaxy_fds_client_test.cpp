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
#include "model/delete_multi_objects_result.h"
#include "fds_test_common.h"

using namespace std;
using namespace galaxy::fds;
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::JSON;

class GalaxyFDSClientTest : public testing::Test {
protected:
  static void SetUpTestCase() {
    FDSClientConfiguration config;
    config.enableHttps(false);
    config.enableCdnForDownload(false);
    _fdsClient = new GalaxyFDSClient(test::ACCESS_KEY, test::SECRET_KEY, config);
    GalaxyFDSClientTest::_bucketName = "cpp-test-bucket-name-"+to_string(rand()).substr(0,6);
  }

  void SetUp() {
    if (_fdsClient->doesBucketExist(_bucketName)){
      ClearBucket(_bucketName);
    }
    else{
      _fdsClient->createBucket(_bucketName);
    }

    /*
    vector<shared_ptr<FDSBucket> > bucketList = _fdsClient->listBuckets();

    for (auto b: bucketList) {
      string _bucketName = b->name();
      if (0 == _bucketName.compare(0, 8, "cpp-test")) {
        ClearBucket(_bucketName);
        _fdsClient->deleteBucket(_bucketName);
      }
    }
    */
  }

  void TearDown(){
    if (_fdsClient->doesBucketExist(_bucketName)){
      ClearBucket(_bucketName);
      _fdsClient->deleteBucket(_bucketName);
    }
  }

  static void TearDownTestCase() {
    delete _fdsClient;
  }

  void ClearBucket(const string& _bucketName);

  // consturct some objects with the same prefix, and upload these object to fds-server
  // return:
  //    -- objects: total objects that be constructed
  //    -- true upload object succeed, otherwise false
  bool uploadObjectsWithSamePrefix(const string &bucketname, const string &prefix/*in*/,
                                   vector<string> &objects/*output*/);

  // check whether the object is exist under the prefix
  // return:
  //    -- true: no object exist under prefix
  //    -- false: object exist under prefix
  bool isEmptyPrefix(const string &bucketname, const string &prefix);
  static GalaxyFDS* _fdsClient;
  static string _bucketName;
};

GalaxyFDS* GalaxyFDSClientTest::_fdsClient = 0;
string GalaxyFDSClientTest::_bucketName = "";

TEST_F(GalaxyFDSClientTest, GetObjectTest) {
  string objectName = "get-object-test-object-name";

  string expectContent = "This is a test object";
  stringstream ss(expectContent);
  _fdsClient->putObject(_bucketName, objectName, ss);
  shared_ptr<FDSObject> pObject = _fdsClient->getObject(_bucketName, objectName);
  stringstream res;
  StreamCopier::copyStream(pObject->objectContent(), res);
  EXPECT_EQ(expectContent, res.str());
}

TEST_F(GalaxyFDSClientTest, GetObjectByRangeTest) {
  string objectName = "get-object-test-object-name";

  string expectContent = "This is a test object";
  stringstream ss(expectContent);
  _fdsClient->putObject(_bucketName, objectName, ss);
  long startPos = 0;
  long length = 5;
  shared_ptr<FDSObject> pObject = _fdsClient->getObject(_bucketName, objectName, startPos, length);
  stringstream res;
  StreamCopier::copyStream(pObject->objectContent(), res);
  EXPECT_EQ(expectContent.substr(startPos, length), res.str());

  startPos = 5;
  pObject = _fdsClient->getObject(_bucketName, objectName, startPos, length);
  stringstream res2;
  StreamCopier::copyStream(pObject->objectContent(), res2);
  EXPECT_EQ(expectContent.substr(startPos, length), res2.str());
}

TEST_F(GalaxyFDSClientTest, PutObjectTest) {
  string objectName = "put-object-test-object-name";

  string expectContent = "This is a test object";
  stringstream ss(expectContent);
  _fdsClient->putObject(_bucketName, objectName, ss);

  _fdsClient->deleteObject(_bucketName, objectName);
}

TEST_F(GalaxyFDSClientTest, PostObjectTest) {
  string objectName = "post-object-test-object-name";

  string expectContent = "This is a test object";
  stringstream ss(expectContent);
  shared_ptr<PutObjectResult> pResult = _fdsClient->postObject(_bucketName, ss);

  _fdsClient->deleteObject(_bucketName, pResult->objectName());
  _fdsClient->deleteBucket(_bucketName);
}

TEST_F(GalaxyFDSClientTest, ListBucketsTest) {
  vector<shared_ptr<FDSBucket> > buckets = _fdsClient->listBuckets();
  bool flag = false;
  for (size_t i = 0; i < buckets.size(); i++) {
    if (buckets[i]->name() == _bucketName) {
      flag = true;
      break;
    }
  }
  EXPECT_TRUE(true);

  _fdsClient->deleteBucket(_bucketName);
}

TEST_F(GalaxyFDSClientTest, DoesBucketExistTest) {
  _fdsClient->deleteBucket(_bucketName);
  bool exist = _fdsClient->doesBucketExist(_bucketName);
  EXPECT_FALSE(exist);
  _fdsClient->createBucket(_bucketName);
  exist = _fdsClient->doesBucketExist(_bucketName);
  EXPECT_TRUE(exist);

  _fdsClient->deleteBucket(_bucketName);
}

void GalaxyFDSClientTest::ClearBucket(const string& _bucketName) {
  shared_ptr<FDSObjectListing> pObjectListing = _fdsClient->listObjects(
          _bucketName, "", "");
  while (true) {
    vector<FDSObjectSummary> s = pObjectListing->objectSummaries();
    for (FDSObjectSummary i: s) {
      string objectName = i.objectName();
      _fdsClient->deleteObject(_bucketName, objectName);
    }

    if (!pObjectListing->truncated()) {
      break;
    }

    pObjectListing = _fdsClient->listNextBatchOfObjects(*pObjectListing);
  }
}

TEST_F(GalaxyFDSClientTest, GetAndSetBucketAclTest) {
  AccessControlList acl;
  acl.addGrant(Grant("user_read", Permission(Permission::READ),
        GrantType(GrantType::USER)));
  acl.addGrant(Grant("user_write_with_sso", Permission(Permission::SSO_WRITE),
        GrantType(GrantType::USER)));
  _fdsClient->setBucketAcl(_bucketName, acl);
  shared_ptr<AccessControlList> pAcl = _fdsClient->getBucketAcl(_bucketName);
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
}

TEST_F(GalaxyFDSClientTest, GetAndSetBucketQuotaTest) {
  QuotaPolicy quotaPolicy;
  quotaPolicy.addQuota(Quota(QuotaType::QPS, Action::GET_OBJECT, 100));
  _fdsClient->setBucketQuota(_bucketName, quotaPolicy);

  shared_ptr<QuotaPolicy> pQuotaPolicy = _fdsClient->getBucketQuota(_bucketName);
  const vector<Quota>& quotas = pQuotaPolicy->quotas();
  EXPECT_EQ(1, quotas.size());
  EXPECT_TRUE(QuotaType::QPS == quotas[0].type().value());
  EXPECT_TRUE(Action::GET_OBJECT == quotas[0].action().value());
  EXPECT_EQ(100, quotas[0].value());
}

TEST_F(GalaxyFDSClientTest, GetObjectMetadataTest) {
  string objectName = "metadata-test-bucket-name";

  stringstream ss("This is a test object");
  FDSObjectMetadata metadata;
  string key = Constants::USER_DEFINED_METADATA_PREFIX + "test";
  metadata.add(key, "test-value");
  _fdsClient->putObject(_bucketName, objectName, ss, metadata);

  shared_ptr<FDSObjectMetadata> pMetadata =  _fdsClient->getObjectMetadata(
      _bucketName, objectName);
  map<string, string>::const_iterator iter = pMetadata->metadata().find(key);
  EXPECT_EQ(iter->second, "test-value");
}

TEST_F(GalaxyFDSClientTest, GetAndSetObjectAclTest) {
  string objectName = "object-acl-test-bucket-name";

  stringstream ss("This is a test object");
  _fdsClient->putObject(_bucketName, objectName, ss);
  AccessControlList acl;
  acl.addGrant(Grant("user", Permission(Permission::READ), GrantType::USER));
  _fdsClient->setObjectAcl(_bucketName, objectName, acl);
  vector<Grant> grants = _fdsClient->getObjectAcl(_bucketName, objectName)->getGrants();

  for (size_t i = 0; i < grants.size(); i++) {
    if (grants[i].granteeId() == "user") {
      EXPECT_TRUE(Permission::READ == grants[i].permission().value());
      EXPECT_TRUE(GrantType::USER == grants[i].grantType().value());
    }
  }
}

TEST_F(GalaxyFDSClientTest, DoesObjectExistTest) {
  string objectName = "does-object-exist-test-bucket-name";

  bool exist = _fdsClient->doesObjectExist(_bucketName, objectName);
  EXPECT_FALSE(exist);
  stringstream ss("This is a test object");
  _fdsClient->putObject(_bucketName, objectName, ss);
  exist = _fdsClient->doesObjectExist(_bucketName, objectName);
  EXPECT_TRUE(exist);
}

TEST_F(GalaxyFDSClientTest, RenameObjectTest) {
  string objectName = "rename-object-test-bucket-name";
  string dstObjectName = "rename-object-test-bucket-name-dst";

  stringstream ss("This is a test object");
  _fdsClient->putObject(_bucketName, objectName, ss);
  bool exist = _fdsClient->doesObjectExist(_bucketName, dstObjectName);
  EXPECT_FALSE(exist);
  _fdsClient->renameObject(_bucketName, objectName, dstObjectName);
  exist = _fdsClient->doesObjectExist(_bucketName, dstObjectName);
  EXPECT_TRUE(exist);
  exist = _fdsClient->doesObjectExist(_bucketName, objectName);
  EXPECT_FALSE(exist);
}

TEST_F(GalaxyFDSClientTest, PrefetchObjectTest) {
  string objectName = "prefetch-object-test-bucket-name";

  stringstream ss("This is a test object");
  _fdsClient->putObject(_bucketName, objectName, ss);
  _fdsClient->setPublic(_bucketName, objectName);
  _fdsClient->prefetchObject(_bucketName, objectName);
}

TEST_F(GalaxyFDSClientTest, RefreshObjectTest) {
  string objectName = "refresh-test-object-name";

  stringstream ss("This is a test object");
  _fdsClient->putObject(_bucketName, objectName, ss);
  _fdsClient->setPublic(_bucketName, objectName);
  _fdsClient->refreshObject(_bucketName, objectName);
}

TEST_F(GalaxyFDSClientTest, ListObjectsTest) {
  stringstream ss;

  _fdsClient->putObject(_bucketName, "foo/bar/test", ss);
  _fdsClient->putObject(_bucketName, "foo", ss);
  _fdsClient->putObject(_bucketName, "bar", ss);

  shared_ptr<FDSObjectListing> pObjectListing = _fdsClient->listObjects(
      _bucketName, "", "");
  EXPECT_EQ(3, pObjectListing->objectSummaries().size());
  EXPECT_EQ(0, pObjectListing->commonPrefixes().size());

  pObjectListing = _fdsClient->listObjects(
      _bucketName, "foo", "");
  EXPECT_EQ(2, pObjectListing->objectSummaries().size());
  EXPECT_EQ(0, pObjectListing->commonPrefixes().size());

  pObjectListing = _fdsClient->listObjects(
      _bucketName, "foo/", "/");
  EXPECT_EQ(0, pObjectListing->objectSummaries().size());
  EXPECT_EQ(1, pObjectListing->commonPrefixes().size());

  _fdsClient->deleteObject(_bucketName, "foo/bar/test");
  _fdsClient->deleteObject(_bucketName, "foo");
  _fdsClient->deleteObject(_bucketName, "bar");
}

TEST_F(GalaxyFDSClientTest, RestoreObjectsTest) {
  string objectName = "bar";
  string objectContent = "blablab";
  stringstream ss(objectContent);

  // put object
  _fdsClient->putObject(_bucketName, objectName, ss);

  // delete object
  _fdsClient->deleteObject(_bucketName, objectName);
  ASSERT_THROW(_fdsClient->getObject(_bucketName, objectName), GalaxyFDSClientException);

  // restore object
  _fdsClient->restoreObject(_bucketName, objectName);

  // check object
  shared_ptr<FDSObject> object = _fdsClient->getObject(_bucketName, objectName);
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
  const string objectName = "object-name";
  const string contentType = "text/blah";
  const string objectContent = "This is a test object";
  stringstream ss(objectContent);

  time_t t = time(NULL);
  // generate uri
  string uri = _fdsClient->generatePresignedUri(_bucketName, objectName,
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
  shared_ptr<FDSObject> object = _fdsClient->getObject(_bucketName, objectName);
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
}

bool GalaxyFDSClientTest::uploadObjectsWithSamePrefix(const string &bucketname, const string &prefix/*in*/,
                                                      vector<string> &objects/*output*/) {
  objects.clear();
  string content("This is for test");
  // control the depth of the objectname,
  // depth of "prefix/test1/test2/test3/object" is 3
  int max_depth = 3;
  int object_cnt = 3;
  string objectname("test_object");
  string objectname_prefix = prefix;
  for (int depth = 0; depth <= max_depth; depth ++) {
    if (depth != 0) {
      objectname_prefix += ("/test_" + to_string(depth));
    }
    for (int cnt = 0; cnt < object_cnt; cnt ++) {
      string object = objectname_prefix + "/" + objectname + "_" + to_string(cnt);
      objects.emplace_back(object);
      stringstream ss(content);
      try {
        _fdsClient->putObject(bucketname, object, ss);
      }
      catch(GalaxyFDSClientException& e) {
        cout << "putObject encounter exception: " << e.what()<<endl;
        return false;
      }
    }
  }
  return true;
}

bool GalaxyFDSClientTest::isEmptyPrefix(const string &bucketname, const string &prefix) {
  std::shared_ptr<FDSObjectListing> listresult = _fdsClient->listObjects(bucketname, prefix);
  if (listresult->objectSummaries().empty()) {
    return true;
  }
  else {
    return false;
  }
}

TEST_F(GalaxyFDSClientTest, deleteObjectsTest)
{
  string bucketname = "cpp-test-delete-objects-test-bucket-name";
  string prefix = "test-prefix";
  vector<string> objects;
  cout << "Testing deleteObjects(not assign prefix)" << std::endl;
  ASSERT_TRUE(uploadObjectsWithSamePrefix(bucketname, prefix, objects));
  cout << "upload objects succeed, sleep 10s" << endl;
  sleep(10);

  shared_ptr<FDSObjectsDeleting> res = _fdsClient->deleteObjects(bucketname, objects);
  ASSERT_TRUE(res->countFailedObjects() <= 0);
  ASSERT_TRUE(isEmptyPrefix(bucketname, prefix + "/"));

  cout << "sleep 10s to wait next test" << endl;
  sleep(10);

  objects.clear();
  cout << "Testing deleteObjects(assign prefix)" << std::endl;
  ASSERT_TRUE(uploadObjectsWithSamePrefix(bucketname, prefix, objects));
  cout << "upload objects succeed, sleep 10s" << endl;
  sleep(10);
  res = _fdsClient->deleteObjects(bucketname, prefix);
  ASSERT_TRUE(res->countFailedObjects() <= 0);
  ASSERT_TRUE(isEmptyPrefix(bucketname, prefix + "/"));
}
