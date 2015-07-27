#ifndef GALAXY_FDS_SIGNER_SIGNER_H_
#define GALAXY_FDS_SIGNER_SIGNER_H_

// Copyright (c) 2014, Xiaomi.com. All rights reserved.
// Author:  Wu Zesheng <wuzesheng@xiaomi.com>
// Created: 2014-06-04

#include <list>
#include <map>
#include <string>

namespace galaxy {
namespace fds {

typedef std::map<std::string, std::list<std::string> > LinkedListMultimap;

enum SignAlgorithm {
  kHmacSha1,
  kHmacSha256,
  kHmacSha512
};

class SubResources;

class Signer {
public:

  /**
   * Sign the specified http request.
   *
   * @param http_method   The http request method({@link #HttpMethod})
   * @param uri           The uri string
   * @param http_headers  The http request headers
   * @param secret_key_id The user's secret access key
   * @param algorithm     The sign algorithm({@link #SignAlgorithm})
   * @return The signed result
   */
  static std::string Sign(const std::string& http_method,
      const std::string& uri, const LinkedListMultimap& http_headers,
      const std::string& secret_key_id, SignAlgorithm algorithm);

  /**
   * A handy version of sign(), generates base64 encoded sign result.
   */
  static std::string SignToBase64(const std::string& http_method,
      const std::string& uri, const LinkedListMultimap& http_headers,
      const std::string& secret_key_id, SignAlgorithm algorithm);
protected:

  static std::string ConstructStringToSign(const std::string& http_method,
      const std::string& uri, const LinkedListMultimap& http_headers);

  static std::string CanonicalizeXiaomiHeaders(
      const LinkedListMultimap& http_headers);

  static std::string CanonicalizeResource(const std::string& uri);

  static std::string CheckAndGet(const LinkedListMultimap& http_headers,
      const std::string& header);

  static LinkedListMultimap ParseUriParameters(const std::string& uri);

  static long GetExpires(const std::string& uri);

  static std::string GetUriPath(const std::string& uri);

private:

  static SubResources s_sub_resources;
};
} // fds
} // galaxy

#endif // GALAXY_FDS_SIGNER_SIGNER_H_
