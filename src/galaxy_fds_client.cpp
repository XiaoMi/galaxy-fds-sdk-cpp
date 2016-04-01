/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include <Poco/Net/HTTPSessionFactory.h>
#include <Poco/Net/HTTPSessionInstantiator.h>
#include <Poco/Net/HTTPSSessionInstantiator.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/Context.h>
#include <Poco/JSON/Parser.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormatter.h>

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
#include "signer/signer.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::JSON;
using Poco::Dynamic::Var;
using Poco::JSON::Array;

namespace galaxy {
namespace fds {

const string GalaxyFDSClient::DATE_FORMAT = "%w, %d %b %Y %H:%M:%S %Z";

GalaxyFDSClient::GalaxyFDSClient(const string& accessKey, const string&
    secretKey, const FDSClientConfiguration& config) {
  _accessKey = accessKey;
  _secretKey = secretKey;
  _pConfig = shared_ptr<FDSClientConfiguration>(new FDSClientConfiguration(config));
  _pEmptyMetadata = shared_ptr<FDSObjectMetadata>(new FDSObjectMetadata());

  _pContext = new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE);
  _pSessionFacotry = shared_ptr<HTTPSessionFactory>(new HTTPSessionFactory());
  _pSessionFacotry->registerProtocol("https", new HTTPSSessionInstantiator(
     _pContext));
  _pSessionFacotry->registerProtocol("http", new HTTPSessionInstantiator());
}

vector<shared_ptr<FDSBucket> > GalaxyFDSClient::listBuckets() {
  string uri = formatUri(_pConfig->getBaseUri(), "", _emptySubResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry
      ->createClientSession(pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_GET, "", _emptyStream,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  ostream& os = pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "List buckets failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }

  vector<shared_ptr<FDSBucket> > res;
  Parser parser;
  parser.parse(rs);
  Var result = parser.result();
  Object::Ptr pObject = result.extract<Object::Ptr>();
  Object::Ptr pOwnerObject = pObject->getObject("owner");
  stringstream ss;
  pOwnerObject->stringify(ss);
  shared_ptr<Owner> pOwner = Owner::deserialize(ss);
  Array::Ptr pBucketsArray = pObject->getArray("buckets");
  for (size_t i = 0; i < pBucketsArray->size(); i++) {
    string bucketName = pBucketsArray->getObject(i)->getValue<string>("name");
    shared_ptr<FDSBucket> pBucket(new FDSBucket(bucketName));
    pBucket->setOwner(*pOwner);
    res.push_back(pBucket);
  }

  return res;
}

void GalaxyFDSClient::createBucket(const string& bucketName) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName, _emptySubResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_PUT, "", _emptyStream,
    *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Create bucket failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }
}

void GalaxyFDSClient::deleteBucket(const string& bucketName) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName, _emptySubResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_DELETE, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_DELETE, "", _emptyStream,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Delete bucket failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }
}

bool GalaxyFDSClient::doesBucketExist(const string& bucketName) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName, _emptySubResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_HEAD, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_HEAD, "", _emptyStream,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() == HTTPResponse::HTTP_OK) {
    return true;
  } else if (response.getStatus() == HTTPResponse::HTTP_NOT_FOUND) {
    return false;
  } else {
    stringstream msg;
    msg << "Check bucket existence failed, status=" << response.getStatus()
      << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }
}

shared_ptr<AccessControlList> GalaxyFDSClient::getBucketAcl(const string&
    bucketName) {
  vector<string> subResources;
  subResources.push_back(SubResource::ACL);
  string uri = formatUri(_pConfig->getBaseUri(), bucketName, subResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_GET, "", _emptyStream,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Get bucket acl failed, status=" << response.getStatus()
      << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }

  return AccessControlList::deserialize(rs);
}

void GalaxyFDSClient::setBucketAcl(const string& bucketName,
    const AccessControlList& acl) {
  vector<string> subResources;
  subResources.push_back(SubResource::ACL);
  string uri = formatUri(_pConfig->getBaseUri(), bucketName, subResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  stringstream ss(AccessControlList::serialize(acl));
  prepareRequestHeaders(uri, HTTPRequest::HTTP_PUT, "application/json", ss,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  ostream& os = pSession->sendRequest(request);
  StreamCopier::copyStream(ss, os);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Set bucket acl failed, status=" << response.getStatus()
      << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }
}

shared_ptr<QuotaPolicy> GalaxyFDSClient::getBucketQuota(const string&
    bucketName) {
  vector<string> subResources;
  subResources.push_back(SubResource::QUOTA);
  string uri = formatUri(_pConfig->getBaseUri(), bucketName, subResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_GET, "", _emptyStream,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Get bucket quota failed, status=" << response.getStatus()
      << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }

  return QuotaPolicy::deserialize(rs);
}

void GalaxyFDSClient::setBucketQuota(const string& bucketName,
    const QuotaPolicy& quotaPolicy) {
  vector<string> subResources;
  subResources.push_back(SubResource::QUOTA);
  string uri = formatUri(_pConfig->getBaseUri(), bucketName, subResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  stringstream ss(QuotaPolicy::serialize(quotaPolicy));
  prepareRequestHeaders(uri, HTTPRequest::HTTP_PUT, "application/json", ss,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  ostream& os = pSession->sendRequest(request);
  StreamCopier::copyStream(ss, os);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Set bucket quota failed, status=" << response.getStatus()
      << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }

}

shared_ptr<FDSObjectListing> GalaxyFDSClient::listObjects(const string&
    bucketName) {
  return listObjects(bucketName, "", "/");
}

shared_ptr<FDSObjectListing> GalaxyFDSClient::listObjects(const string&
    bucketName, const string& prefix) {
  return listObjects(bucketName, prefix, "/");
}

shared_ptr<FDSObjectListing> GalaxyFDSClient::listObjects(const string&
    bucketName, const string& prefix, const string& delimiter) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName, _emptySubResources);
  uri += "?prefix=" + prefix + "&delimiter=" + delimiter;
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry
      ->createClientSession(pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_GET, "", _emptyStream,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  ostream& os = pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "List objects failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }

  return FDSObjectListing::deserialize(rs);
}

shared_ptr<FDSObjectListing> GalaxyFDSClient::listNextBatchOfObjects(const
    FDSObjectListing& previousObjectListing) {
  string bucketName = previousObjectListing.bucketName();
  string prefix = previousObjectListing.prefix();
  string delimiter = previousObjectListing.delimiter();
  string marker = previousObjectListing.nextMarker();
  int maxKeys = previousObjectListing.maxKeys();

  string uri = formatUri(_pConfig->getBaseUri(), bucketName, _emptySubResources);
  stringstream ss;
  ss << uri << "?prefix=" << prefix << "&delimiter=" << delimiter;
  ss << "&marker=" << marker << "&maxKeys=" << maxKeys;
  uri = ss.str();
  cout << "uri: " << uri << endl;
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry
      ->createClientSession(pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_GET, "", _emptyStream,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  ostream& os = pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "List objects failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }

  return FDSObjectListing::deserialize(rs);
}

shared_ptr<PutObjectResult> GalaxyFDSClient::putObject(const string& bucketName,
    const string& objectName, istream& is) {
  return putObject(bucketName, objectName, is, FDSObjectMetadata());
}

shared_ptr<PutObjectResult> GalaxyFDSClient::putObject(const string& bucketName,
    const string& objectName, istream& is, const FDSObjectMetadata&
    metadata) {
  PutObjectResult res;

  string mediaType = "application/octet-stream";
  map<string, string>::const_iterator iter = metadata.metadata().find(
      Constants::CONTENT_TYPE);
  if (iter != metadata.metadata().end()) {
    mediaType = iter->second;
  }

  string uri = formatUri(_pConfig->getUploadBaseUri(), bucketName + "/" +
      objectName, _emptySubResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_PUT, mediaType, is, metadata,
      request);
  HTTPResponse response;

  ostream& os = pSession->sendRequest(request);
  StreamCopier::copyStream(is, os);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Put object failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }

  return PutObjectResult::deserialize(rs);
}

shared_ptr<PutObjectResult> GalaxyFDSClient::postObject(const string& bucketName,
    istream& is) {
  return postObject(bucketName, is, FDSObjectMetadata());
}

shared_ptr<PutObjectResult> GalaxyFDSClient::postObject(const string& bucketName,
    istream& is, const FDSObjectMetadata&  metadata) {
  PutObjectResult res;

  string mediaType = "application/octet-stream";
  map<string, string>::const_iterator iter = metadata.metadata().find(
      Constants::CONTENT_TYPE);
  if (iter != metadata.metadata().end()) {
    mediaType = iter->second;
  }

  string uri = formatUri(_pConfig->getUploadBaseUri(), bucketName + "/",
      _emptySubResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_POST, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_POST, mediaType, is, metadata,
      request);
  HTTPResponse response;

  ostream& os = pSession->sendRequest(request);
  StreamCopier::copyStream(is, os);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Post object failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }

  return PutObjectResult::deserialize(rs);
}

shared_ptr<FDSObject> GalaxyFDSClient::getObject(const string& bucketName,
    const string& objectName) {
  return getObject(bucketName, objectName, 0);
}

shared_ptr<FDSObject> GalaxyFDSClient::getObject(const string& bucketName,
    const string& objectName, long pos) {
  string uri = formatUri(_pConfig->getDownloadBaseUri(), bucketName + "/" +
      objectName, vector<string>());
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry
      ->createClientSession(pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_GET, "",  _emptyStream,
      FDSObjectMetadata(), request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Get object failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }

  shared_ptr<FDSObjectSummary> pObjectSummary(new FDSObjectSummary());
  pObjectSummary->setBucketName(bucketName);
  pObjectSummary->setObjectName(objectName);
  pObjectSummary->setSize(response.getContentLength());
  shared_ptr<FDSObject> pObject(new FDSObject());
  pObject->setObjectSummary(pObjectSummary);
  pObject->setObjectMetadata(this->parseMetadata(response));
  pObject->setSession(pSession);
  pObject->setObjectContent(rs);

  return pObject;
}

shared_ptr<FDSObjectMetadata> GalaxyFDSClient::getObjectMetadata(const string&
    bucketName, const string& objectName) {
  vector<string> subResources;
  subResources.push_back(SubResource::METADATA);
  string uri = formatUri(_pConfig->getBaseUri(), bucketName + "/" + objectName,
      subResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_GET, "", _emptyStream,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Get object metadata failed, status=" << response.getStatus()
      << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }

  return parseMetadata(response);
}

shared_ptr<AccessControlList> GalaxyFDSClient::getObjectAcl(const string&
    bucketName, const string& objectName) {
  vector<string> subResources;
  subResources.push_back(SubResource::ACL);
  string uri = formatUri(_pConfig->getBaseUri(), bucketName + "/" + objectName,
      subResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_GET, "", _emptyStream,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Get object acl failed, status=" << response.getStatus()
      << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }

  return AccessControlList::deserialize(rs);
}

void GalaxyFDSClient::setObjectAcl(const string& bucketName, const string&
    objectName, const AccessControlList& acl) {
  vector<string> subResources;
  subResources.push_back(SubResource::ACL);
  string uri = formatUri(_pConfig->getBaseUri(), bucketName + "/" + objectName,
      subResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  stringstream ss(AccessControlList::serialize(acl));
  prepareRequestHeaders(uri, HTTPRequest::HTTP_PUT, "application/json", ss,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  ostream& os = pSession->sendRequest(request);
  StreamCopier::copyStream(ss, os);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Set Object acl failed, status=" << response.getStatus()
      << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }
}

bool GalaxyFDSClient::doesObjectExist(const string& bucketName, const string&
    objectName) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName + "/" + objectName,
      _emptySubResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_HEAD, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_HEAD, "", _emptyStream,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() == HTTPResponse::HTTP_OK) {
    return true;
  } else if (response.getStatus() == HTTPResponse::HTTP_NOT_FOUND) {
    return false;
  } else {
    stringstream msg;
    msg << "Check object existence failed, status=" << response.getStatus()
      << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }
}

void GalaxyFDSClient::deleteObject(const string& bucketName, const string&
    objectName) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName + "/" + objectName,
      _emptySubResources);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_DELETE, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_DELETE, "", _emptyStream,
      *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Delete object failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }
}


void GalaxyFDSClient::restoreObject(const std::string& bucketName, const std::string&
    objectName) {
  vector<string> subResource;
  subResource.push_back("restore");
  string uri = formatUri(_pConfig->getBaseUri(), bucketName + "/" + objectName,
                         subResource);
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
          pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_PUT, "", _emptyStream,
                        *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Restore object failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }
}

void GalaxyFDSClient::renameObject(const string& bucketName, const string&
    srcObjectName, const string& dstObjectName) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName + "/" + srcObjectName,
      _emptySubResources);
  uri += "?renameTo=" + dstObjectName;
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_PUT, "", _emptyStream,
    *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Rename object failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }
}

void GalaxyFDSClient::prefetchObject(const string& bucketName, const string&
    objectName) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName + "/" + objectName,
      _emptySubResources);
  uri += "?prefetch";
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_PUT, "", _emptyStream,
    *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Prefetch object failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }
}

void GalaxyFDSClient::refreshObject(const string& bucketName, const string&
    objectName) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName + "/" + objectName,
      _emptySubResources);
  uri += "?refresh";
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
        pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_PUT, "", _emptyStream,
    *_pEmptyMetadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Prefetch object failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(msg.str());
  }
}

void GalaxyFDSClient::setPublic(const string& bucketName, const string&
    objectName) {
  AccessControlList acl;
  acl.addGrant(Grant(UserGroups(UserGroups::ALL_USERS).name(), Permission::READ,
        GrantType::GROUP));
  setObjectAcl(bucketName, objectName, acl);
}

string GalaxyFDSClient::generateDownloadObjectUri(const string&
    bucketName, const string& objectName) {
  return formatUri(_pConfig->getDownloadBaseUri(), bucketName + "/" + objectName,
      _emptySubResources);
}

string GalaxyFDSClient::generatePresignedUri(const string& bucketName,
    const string& objectName, time_t expiration, const string& httpMethod) {
  string contentType;
  return generatePresignedUri(_pConfig->getBaseUri(), bucketName, objectName, expiration, httpMethod, contentType);
}

string GalaxyFDSClient::generatePresignedCdnUri(const string& bucketName,
    const string& objectName, time_t expiration, const string& httpMethod) {
  return generatePresignedUri(_pConfig->getCdnBaseUri(), bucketName, objectName,
      expiration, httpMethod, string());
}

string GalaxyFDSClient::generatePresignedUri(const string& bucketName,
    const string& objectName, time_t expiration, const string& httpMethod, const std::string& contentType) {
  return generatePresignedUri(_pConfig->getBaseUri(), bucketName, objectName, expiration, httpMethod, contentType);
}

string GalaxyFDSClient::generatePresignedUri(const string& baseUri, const
    string& bucketName, const string& objectName, time_t expiration, const
    string& httpMethod, const string& contentType) {
  stringstream ss;
  ss << baseUri;
  ss << "/" << bucketName << "/" << objectName;
  ss << "?GalaxyAccessKeyId=" << _accessKey;
  ss << "&Expires=" << expiration * 1000;

  string uri = ss.str();
  string relativeUri = uri.substr(uri.find_first_of('/', 
      uri.find_first_of(':') + 3));
  LinkedListMultimap headers = LinkedListMultimap();
  if (!contentType.empty()) {
    list<string> l;
    l.push_front(contentType);
    headers[Constants::CONTENT_TYPE] = l;
  }
  string signature = Signer::SignToBase64(httpMethod, relativeUri,
      headers, _secretKey, kHmacSha1);
  ss << "&Signature=" << signature;
  return ss.str();
}

string GalaxyFDSClient::formatUri(const string& baseUri, const string&
    resource, const vector<string>& subResources) {
  string res = baseUri + "/" + resource;

  string tmp;
  for (int i = 0; i < subResources.size(); i++) {
    if (!tmp.empty()) {
      tmp += "&" + subResources[i];
    } else {
      tmp += subResources[i];
    }
  }

  if(!tmp.empty()) {
    res += "?" + tmp;
  }

  return res;
}

shared_ptr<FDSObjectMetadata> GalaxyFDSClient::parseMetadata(const HTTPResponse&
    response) {
  shared_ptr<FDSObjectMetadata> res(new FDSObjectMetadata());
  for (HTTPResponse::ConstIterator iter = response.begin()
      ; iter != response.end(); ++iter) {
    try {
      res->add(iter->first, iter->second);
    } catch(exception e) {
      // Ignored
    }
  }
  return res;
}

void GalaxyFDSClient::prepareRequestHeaders(const string& uri,
    const string& httpMethod, const string& mediaType, istream& is,
    const FDSObjectMetadata& metadata, HTTPRequest& request) {
  shared_ptr<LinkedListMultimap> pHeaders(new LinkedListMultimap());

  // metadata
  for (map<string, string>::const_iterator iter = metadata.metadata().begin();
      iter != metadata.metadata().end(); ++iter) {
    (*pHeaders)[iter->first].push_back(iter->second);
  }

  // date
  string date = DateTimeFormatter::format(DateTime(),
      GalaxyFDSClient::DATE_FORMAT);
  (*pHeaders)[Constants::DATE].push_back(date);

  // content type
  if (!mediaType.empty()) {
    (*pHeaders)[Constants::CONTENT_TYPE].push_back(mediaType);
  }

  // content length
  if (!is.seekg(0, is.end).fail()) {
    request.setContentLength(is.tellg());
    is.seekg(0, is.beg);
  } else {
    request.setChunkedTransferEncoding(true);
  }

  // authentication information
  string relativeUri = uri.substr(uri.find_first_of('/', 
      uri.find_first_of(':') + 3));
  string signature = Signer::SignToBase64(httpMethod, relativeUri, *pHeaders,
      _secretKey, kHmacSha1);
  string authStr = "Galaxy-V2 " + _accessKey + ":" + signature;
  (*pHeaders)[Constants::AUTHORIZATION].push_back(authStr);

  for (LinkedListMultimap::iterator iter = pHeaders->begin();
      iter != pHeaders->end(); ++iter) {
    for (list<string>::iterator iterList = iter->second.begin();
        iterList != iter->second.end(); ++iterList) {
      request.add(iter->first, *iterList);
    }
  }
}

} // namespace fds
} // namespace galaxy
