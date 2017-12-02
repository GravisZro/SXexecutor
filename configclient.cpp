#include "configclient.h"

// PDTK
#include <object.h>
#include <cxxutils/hashing.h>

ConfigClient::ConfigClient(void) noexcept
{
  Object::connect(newMessage, this, &ConfigClient::receive);
}

void ConfigClient::receive(posix::fd_t socket, vfifo buffer, posix::fd_t fd) noexcept
{
  (void)socket;
  (void)fd;
  std::string str;
  if(!(buffer >> str).hadError() && str == "RPC")
  {
    buffer >> str;
    switch(hash(str))
    {
      case "configUpdated"_hash:
        Object::enqueue(configUpdated);
      break;
      case "unsetReturn"_hash:
      {
        struct { int errcode; } val;
        buffer >> val.errcode;
        if(!buffer.hadError())
          Object::enqueue(unsetReturn, val.errcode);
      }
      break;
      case "setReturn"_hash:
      {
        struct { int errcode; } val;
        buffer >> val.errcode;
        if(!buffer.hadError())
          Object::enqueue(setReturn, val.errcode);
      }
      break;
      case "getReturn"_hash:
      {
        struct { int errcode; std::string value; std::vector<std::string> children; } val;
        buffer >> val.errcode >> val.value >> val.children;
        if(!buffer.hadError())
          Object::enqueue(getReturn, val.errcode, val.value, val.children);
      }
      break;
    }
  }
}