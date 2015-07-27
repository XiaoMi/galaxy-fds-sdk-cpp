#ifndef GALAXY_FDS_SIGNER_BASE64_H_
#define GALAXY_FDS_SIGNER_BASE64_H_

// Copyright (c) 2014, Xiaomi.com. All rights reserved.
// Author:  Wu Zesheng <wuzesheng@xiaomi.com>
// Created: 2014-06-05

#include <string>

namespace galaxy {
namespace fds {

class Base64 {
public:

  static std::string Encode(const char* data, size_t length);

  static std::string Decode(const std::string& data);
};

} // fds
} // galaxy
#endif // GALAXY_FDS_SIGNER_BASE64_H_
