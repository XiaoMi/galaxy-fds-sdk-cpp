/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_FSD_CLIENT_EXCEPTION_H_
#define GALAXY_FSD_CLIENT_EXCEPTION_H_

#include <string>
#include <exception>

namespace galaxy {
namespace fds {

class GalaxyFDSClientException : public std::exception {
public:
  GalaxyFDSClientException(const std::string& message) {
    this->message = message;
  }

  virtual const char* what() const throw() {
    return message.c_str();
  }

  ~GalaxyFDSClientException() throw() {}

private:
  std::string message;
}; // class GalaxyFDSClientException

} // namespace fds
} // namespace galaxy

#endif // GALAXY_FSD_CLIENT_EXCEPTION_H_
