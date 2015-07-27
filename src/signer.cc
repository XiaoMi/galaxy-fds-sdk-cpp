// Copyright (c) 2014, Xiaomi.com. All rights reserved.
// Author:  Wu Zesheng <wuzesheng@xiaomi.com>
// Created: 2014-06-04

#include "signer/signer.h"

#include <openssl/hmac.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#include <set>
#include <sstream>

#include "signer/base64.h"

namespace galaxy {
namespace fds {

class SubResources {
public:

  SubResources() {
    m_resources.insert("acl");
    m_resources.insert("quota");
    m_resources.insert("uploads");
    m_resources.insert("partNumber");
    m_resources.insert("uploadId");
    m_resources.insert("storageAccessToken");
    m_resources.insert("metadata");
  }

  bool Find(const std::string& res) {
    return m_resources.find(res) != m_resources.end();
  }
private:

  std::set<std::string> m_resources;
};

SubResources Signer::s_sub_resources;

static const std::string CONTENT_MD5 = "Content-MD5";
static const std::string CONTENT_TYPE = "Content-Type";
static const std::string DATE = "Date";
static const std::string EXPIRES = "Expires";
static const std::string XIAOMI_HEADER_PREFIX = "x-xiaomi-";
static const std::string XIAOMI_DATE = XIAOMI_HEADER_PREFIX + "date";

const EVP_MD* ConvertSignAlgorithm(SignAlgorithm algorithm) {
  switch (algorithm) {
  case kHmacSha256:
    return EVP_sha256();
  case kHmacSha512:
    return EVP_sha512();
  case kHmacSha1:
  default:
    return EVP_sha1();
  }
}

std::string Signer::Sign(const std::string& http_method, const std::string& uri,
    const LinkedListMultimap& http_headers, const std::string& secret_key_id,
    SignAlgorithm algorithm) {
  std::string stringToSign = ConstructStringToSign(http_method,
      uri, http_headers);
  const EVP_MD* hmac_algo = ConvertSignAlgorithm(algorithm);

  unsigned int len;
  unsigned char* result = HMAC(hmac_algo, secret_key_id.c_str(),
      static_cast<int>(secret_key_id.size()),
      (const unsigned char* )(stringToSign.c_str()),
      stringToSign.size(), NULL, &len);
  return std::string((char* )(result), len);
}

std::string Signer::SignToBase64(const std::string& http_method,
    const std::string& uri, const LinkedListMultimap& http_headers,
    const std::string& secret_key_id, SignAlgorithm algorithm) {
  std::string signature = Sign(http_method, uri, http_headers,
      secret_key_id, algorithm);
  return Base64::Encode(signature.c_str(), signature.size());
}

std::string Signer::ConstructStringToSign(const std::string& http_method,
    const std::string& uri, const LinkedListMultimap& http_headers) {
  std::ostringstream sout;
  sout << http_method << "\n";
  sout << CheckAndGet(http_headers, CONTENT_MD5) << "\n";
  sout << CheckAndGet(http_headers, CONTENT_TYPE) << "\n";

  long expires = GetExpires(uri);
  if (expires > 0) {
    // For pre-signed uri
    sout << expires << "\n";
  } else {
    std::string xiaomi_date = CheckAndGet(http_headers, XIAOMI_DATE);
    std::string date;
    if (xiaomi_date.empty()) {
      date = CheckAndGet(http_headers, DATE);
    }
    sout << date << "\n";
  }

  sout << CanonicalizeXiaomiHeaders(http_headers);
  sout << CanonicalizeResource(uri);
  return sout.str();
}

std::string Signer::CanonicalizeXiaomiHeaders(
    const LinkedListMultimap& http_headers) {
  if (http_headers.empty()) {
    return std::string("");
  }

  // Sort the headers and merge values
  std::map<std::string, std::string> sorted_map;
  LinkedListMultimap::const_iterator iter = http_headers.begin();
  size_t prefix_len = XIAOMI_HEADER_PREFIX.size();
  while (iter != http_headers.end()) {
    if (strncmp(XIAOMI_HEADER_PREFIX.c_str(), iter->first.c_str(),
          prefix_len) != 0) {
      ++iter;
      continue;
    }

    std::list<std::string>::const_iterator listIter = iter->second.begin();
    bool first = true;
    while (listIter != iter->second.end()) {
      if (first) {
        sorted_map[iter->first] = *listIter;
        first = false;
      } else {
        sorted_map[iter->first] += "," + *listIter;
      }
      ++listIter;
    }
    ++iter;
  }

  // TODO(wuzesheng) Unfold multiple lines long header

  // Generate the canonicalized result
  std::ostringstream sout;
  std::map<std::string, std::string>::iterator it = sorted_map.begin();
  while (it != sorted_map.end()) {
    sout << it->first << ":" << it->second << "\n";
    ++it;
  }
  return sout.str();
}

std::string Signer::CanonicalizeResource(const std::string& uri) {
  std::ostringstream sout;
  sout << GetUriPath(uri);

  // Parse and sort subresources
  LinkedListMultimap params = ParseUriParameters(uri);
  std::map<std::string, std::string> sorted_params;
  LinkedListMultimap::iterator iter = params.begin();
  while (iter != params.end()) {
    if (s_sub_resources.Find(iter->first)) {
      sorted_params[iter->first] = iter->second.front();
    }
    ++iter;
  }

  // Generate the canonicalized result
  if (!sorted_params.empty()) {
    sout << "?";
    bool first = true;
    std::map<std::string, std::string>::iterator it =
      sorted_params.begin();
    while (it != sorted_params.end()) {
      if (first) {
        first = false;
        sout << it->first;
      } else {
        sout << "&" << it->first;
      }

      if (!it->second.empty()) {
        sout << "=" << it->second;
      }
      ++it;
    }
  }
  return sout.str();
}

std::string Signer::CheckAndGet(const LinkedListMultimap& http_headers,
    const std::string& header) {
  LinkedListMultimap::const_iterator iter = http_headers.find(header);
  if (iter != http_headers.end()) {
    return iter->second.front();
  }
  return std::string("");
}

LinkedListMultimap Signer::ParseUriParameters(const std::string& uri) {
  // Skip part before '?'
  size_t index = 0;
  while (index < uri.size() && uri[index++] != '?') {
    // Need do nothing
  }

  // Parse parameters
  LinkedListMultimap params;
  size_t start_pos = index;
  std::string key;
  std::string value;
  bool has_value = false;

  while (index < uri.size()) {
    if (uri[index] == '=') {
      has_value=true;
      key = uri.substr(start_pos, index - start_pos);
      start_pos = index + 1;
    } else if (uri[index] == '&') {

      if (has_value) {
        value = uri.substr(start_pos, index - start_pos);
        params[key].push_back(value);
      } else {
        key = uri.substr(start_pos, index - start_pos);
        params[key].push_back("");
      }
      start_pos = index + 1;
    }
    ++index;
  }

  // Process last parameter
  if (index > start_pos) {
    if (has_value) {
      value = uri.substr(start_pos, index - start_pos);
      params[key].push_back(value);
    } else {
      key = uri.substr(start_pos, index - start_pos);
      params[key].push_back("");
    }
  }
  return params;
}

long Signer::GetExpires(const std::string& uri) {
  LinkedListMultimap params = ParseUriParameters(uri);
  LinkedListMultimap::iterator iter = params.find(EXPIRES);
  if (iter != params.end()) {
    std::string expires = iter->second.front();
    return atol(expires.c_str());
  }
  return 0l;
}

std::string Signer::GetUriPath(const std::string& uri) {
  size_t pos = uri.find_first_of('?');
  if (pos == std::string::npos) {
    return uri;
  } else {
    return uri.substr(0, pos);
  }
}
} // fds
} // galaxy
