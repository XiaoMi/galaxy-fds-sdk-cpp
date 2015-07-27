/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include <iostream>
#include <memory>
#include <vector>

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

int copyStream(istream& is, ostream& os);

int main() {
  string accessKey = "{access_key}";
  string secretKey = "{secret_key}";
  string bucketName = "cpp-sdk-example";
  string objectName = "test.txt";

  FDSClientConfiguration config;
  config.enableHttps(false);
  GalaxyFDSClient fdsClient(accessKey, secretKey, config);

  // check the existence of the bucket
  if (!fdsClient.doesBucketExist(bucketName)) {
    // create bucket
    fdsClient.createBucket(bucketName);
  }

  // list all my buckets
  vector<shared_ptr<FDSBucket> > buckets = fdsClient.listBuckets();
  for (size_t i = 0; i < buckets.size(); i++) {
    cout << buckets[i]->name() << endl;
  }

  // check the existence of the object
  if (!fdsClient.doesObjectExist(bucketName, objectName)) {
    // create the object
    stringstream ss("This is a test object");
    fdsClient.putObject(bucketName, objectName, ss);
  }

  // get the object
  shared_ptr<FDSObject> pObject = fdsClient.getObject(bucketName, objectName);
  istream& is = pObject->objectContent();
  copyStream(is, cout);
  cout << endl;
  
  // delete the object
  fdsClient.deleteObject(bucketName, objectName);

  // delete the bucket
  fdsClient.deleteBucket(bucketName);

  return 0;
}

int copyStream(istream& is, ostream& os) {
  int bufferSize = 1024;
  char* buffer = new char[bufferSize];

  streamsize len = 0;
  is.read(buffer, bufferSize);
  streamsize n = is.gcount();
  while(n > 0) {
    len += n;
    os.write(buffer, n);
    if (is) {
      is.read(buffer, bufferSize);
      n = is.gcount();
    } else {
      n = 0;
    }
  }
  delete[] buffer;
  return len;
}
