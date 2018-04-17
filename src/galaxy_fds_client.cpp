/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include <sstream>

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
#include "model/fds_object_metadata_bean.h"
#include "model/put_object_result.h"
#include "model/quota_policy.h"
#include "model/sub_resource.h"
#include "model/delete_multi_objects_result.h"
#include "signer/signer.h"
#include "internal/utils.h"

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

  _pSessionFacotry = shared_ptr<HTTPSessionFactory>(new HTTPSessionFactory());
  _pSessionFacotry->registerProtocol("https", new HTTPSSessionInstantiator(
     new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE)));
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
  }

}


shared_ptr<FDSObjectListing> GalaxyFDSClient::listObjects(const string&
    bucketName) {
  return listObjects(bucketName, "", "/", false);
}

shared_ptr<FDSObjectListing> GalaxyFDSClient::listObjects(const string&
    bucketName, const string& prefix) {
  return listObjects(bucketName, prefix, "/", false);
}

shared_ptr<FDSObjectListing> GalaxyFDSClient::listObjects(const string&
    bucketName, const string& prefix, const string& delimiter) {
  return listObjects(bucketName, prefix, delimiter, false);
}

shared_ptr<FDSObjectListing> GalaxyFDSClient::listObjects(const string&
    bucketName, bool withMetaData) {
  return listObjects(bucketName, "", "/", withMetaData);
}

shared_ptr<FDSObjectListing> GalaxyFDSClient::listObjects(const string&
    bucketName, const string& prefix, bool withMetaData) {
  return listObjects(bucketName, prefix, "/", withMetaData);
}

shared_ptr<FDSObjectListing> GalaxyFDSClient::listObjects(const string&
    bucketName, const string& prefix, const string& delimiter, bool withMetaData) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName, _emptySubResources);
  uri += "?prefix=" + prefix + "&delimiter=" + delimiter;
  if (withMetaData) {
    uri += "&withMetaData";
  }
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
  }

  return FDSObjectListing::deserialize(rs);
}

shared_ptr<FDSObjectListing> GalaxyFDSClient::listNextBatchOfObjects(const
    FDSObjectListing& previousObjectListing) {
  return listNextBatchOfObjects(previousObjectListing, false);
}

shared_ptr<FDSObjectListing> GalaxyFDSClient::listNextBatchOfObjects(const
    FDSObjectListing& previousObjectListing, bool withMetaData) {
  string bucketName = previousObjectListing.bucketName();
  string prefix = previousObjectListing.prefix();
  string delimiter = previousObjectListing.delimiter();
  string marker = previousObjectListing.nextMarker();
  int maxKeys = previousObjectListing.maxKeys();

  string uri = formatUri(_pConfig->getBaseUri(), bucketName, _emptySubResources);
  stringstream ss;
  ss << uri << "?prefix=" << prefix << "&delimiter=" << delimiter;
  ss << "&marker=" << marker << "&maxKeys=" << maxKeys;
  if (withMetaData) {
    ss << "&withMetaData";
  }
  uri = ss.str();
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
  }

  return PutObjectResult::deserialize(rs);
}

shared_ptr<FDSObject> GalaxyFDSClient::getObject(const string& bucketName,
    const string& objectName) {
  return getObject(bucketName, objectName, 0);
}

shared_ptr<FDSObject> GalaxyFDSClient::getObject(const string& bucketName,
    const string& objectName, long pos) {
  return getObject(bucketName, objectName, pos, -1);
}

shared_ptr<FDSObject> GalaxyFDSClient::getObject(const string& bucketName,
    const string& objectName, long pos, long len) {
  string uri = formatUri(_pConfig->getDownloadBaseUri(), bucketName + "/" +
      objectName, vector<string>());
  URI pocoUri(uri);

  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry
      ->createClientSession(pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, uri, HTTPMessage::HTTP_1_1);
  FDSObjectMetadata metadata;
  if (pos >= 0 || len > 0) {
    std::ostringstream os;
    os << "bytes=" << pos << "-";
    if (len > 0) {
      os << (pos + len - 1); // inclusive
    }
    metadata.add(Constants::RANGE, os.str());
  }
  prepareRequestHeaders(uri, HTTPRequest::HTTP_GET, "",  _emptyStream,
      metadata, request);
  HTTPResponse response;

  pSession->sendRequest(request);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK &&
      response.getStatus() != HTTPResponse::HTTP_PARTIAL_CONTENT) {
    stringstream msg;
    msg << "Get object failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
  }
}

void GalaxyFDSClient::setObjectMetadata(const std::string& bucketName, const std::string&
        objectName, const MetadataBean& metadata) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName + "/" + objectName, _emptySubResources);
  uri += "?setMetaData";
  URI pocoUri(uri);
  shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(
          pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  stringstream ss(MetadataBean::serialize(metadata));
  prepareRequestHeaders(uri, HTTPRequest::HTTP_PUT, "application/json", ss,
        *_pEmptyMetadata, request);
  HTTPResponse response;

  ostream& os = pSession->sendRequest(request);
  StreamCopier::copyStream(ss, os);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Set object metadata failed, status=" << response.getStatus()
      << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
  }
}

void GalaxyFDSClient::deleteObject(const string& bucketName, const string&
    objectName) {
    deleteObject(bucketName, objectName, true);
  }

void GalaxyFDSClient::deleteObject(const string& bucketName, const string&
    objectName, bool enableTrash) {
  string uri = formatUri(_pConfig->getBaseUri(), bucketName + "/" + objectName,
      _emptySubResources);
  if (!enableTrash) {
    uri += "?enableTrash=false";
  }
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
  }
}

std::shared_ptr<FDSObjectsDeleting> GalaxyFDSClient::deleteObjects(const std::string& bucketName,
    const vector<std::string>& objectNameList) {
    return deleteObjects(bucketName, objectNameList, true);
  }

std::shared_ptr<FDSObjectsDeleting> GalaxyFDSClient::deleteObjects(const std::string& bucketName,
    const vector<std::string>& objectNameList, bool enableTrash) {
  string mediaType = "application/json";

  string uri = formatUri(_pConfig->getBaseUri(), bucketName, _emptySubResources);
  uri += "?deleteObjects=";
  if (!enableTrash) {
    uri += "&enableTrash=false";
  }
  URI pocoUri(uri);

  std::shared_ptr<HTTPClientSession> pSession(_pSessionFacotry->createClientSession(pocoUri));
  pSession->setHost(pocoUri.getHost());
  pSession->setPort(pocoUri.getPort());

  std::string json = serializeVectorString(objectNameList);
  std::istringstream is;
  is.str(json);

  HTTPRequest request(HTTPRequest::HTTP_PUT, uri, HTTPMessage::HTTP_1_1);
  prepareRequestHeaders(uri, HTTPRequest::HTTP_PUT, mediaType, is, *_pEmptyMetadata, request);

  HTTPResponse response;
  ostream& os = pSession->sendRequest(request);
  StreamCopier::copyStream(is, os);
  istream& rs = pSession->receiveResponse(response);

  if (response.getStatus() != HTTPResponse::HTTP_OK) {
    stringstream msg;
    msg << "Delete objects failed, status=" << response.getStatus() << ", reason=";
    StreamCopier::copyStream(rs, msg);
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
  }
  return FDSObjectsDeleting::deserialize(rs);
}

std::shared_ptr<FDSObjectsDeleting> GalaxyFDSClient::deleteObjects(const std::string& bucketName,
    const std::string& prefix) {
    return deleteObjects(bucketName, prefix, true);
  }

std::shared_ptr<FDSObjectsDeleting> GalaxyFDSClient::deleteObjects(const std::string& bucketName,
    const std::string& prefix, bool enableTrash) {
  if (bucketName.empty() || prefix.empty()) {
    string reason = string("invalid bucketName(") + bucketName + string(") or prefix(") + prefix + string(")");
    throw GalaxyFDSClientException(reason);
  }
  std::shared_ptr<FDSObjectListing> objects = listObjects(bucketName, prefix, std::string(""));
  // first, we should skip the files that already been marked deleted, but not physical deleted
  while (objects->objectSummaries().size() <= 0 && objects->commonPrefixes().size() <= 0 && objects->truncated()) {
      objects = listNextBatchOfObjects(*objects);
  }
  int batchDeleteSize = _pConfig->getMaxBatchDeleteSize();
  for (int iter_cnt = 0; !objects->objectSummaries().empty(); iter_cnt ++) {
    int cnt = objects->objectSummaries().size();
    if (cnt <= 0) {
      return std::make_shared<FDSObjectsDeleting>();
    }

    std::vector<std::string> objectNameList;
    for (const auto& objectsum : objects->objectSummaries()) {
      objectNameList.emplace_back(objectsum.objectName());
      cnt -= 1;
      if (objectNameList.size() >= batchDeleteSize || cnt <= 0) {
        try {
          std::shared_ptr<FDSObjectsDeleting> res = deleteObjects(bucketName, objectNameList, enableTrash);
          if (res->countFailedObjects() > 0) {
            for (const errorObject& errobject : res->getErrorList()) {
              //TODO: any error_code that represent deleting object succeed should add below
              if (errobject.errorCode != HTTPResponse::HTTP_NOT_FOUND ||
                  errobject.errorCode != HTTPResponse::HTTP_OK) {
                return res;
              }
            }
          }
        }
        catch(GalaxyFDSClientException& e) {
          throw e;
        }
        objectNameList.clear();
      }
    }

    objects = listNextBatchOfObjects(*objects);
  }
  return std::make_shared<FDSObjectsDeleting>();
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
    throw GalaxyFDSClientException(response.getStatus(), msg.str());
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
      res->add(Utils::toLowerCase(iter->first), iter->second);
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
