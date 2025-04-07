#ifndef BASIC_CLASS_HPP
#define BASIC_CLASS_HPP

#include "Parser.hpp"
#include "Validator.hpp"
#include <string>
#include <map>
#include <stdlib.h>

class ABasicClass {
public:
  ABasicClass();
  virtual ~ABasicClass() {};

  virtual void                              set(const std::string key, const std::string value);
  virtual bool                              has(const std::string key) const;
  virtual bool                              isset(const std::string key) const;
  virtual std::string                       get(const std::string key) const;

  virtual void                              init() = 0;

protected:
  ABasicClass(const ABasicClass &src);
  ABasicClass &operator=(const ABasicClass &src);
  
  std::map<std::string, void (ABasicClass::*)(const std::string &)>   _setters;
  std::map<std::string, std::string(ABasicClass::*)() const >         _getters;
  
  virtual void                                setHeader(std::string& value);
  virtual void                                setPipe(std::string& value);
  virtual void                                setListen(std::string& value);
  virtual void                                setServerName(std::string& value);
  virtual void                                setRoot(std::string& value);
  virtual void                                setIndex(std::string& value);
  virtual void                                setErrorPage(std::string& value);
  virtual void                                setAllowedMethod(std::string& value);
  virtual void                                setAllowListing(std::string& value);
  virtual void                                setRedirect(std::string& value);
  virtual void                                setMethod(std::string& value);
  virtual void                                setPath(std::string& value);
  virtual void                                setQuery(std::string& value);
  virtual void                                setProtocol(std::string& value);
  virtual void                                setArgs(std::string& value);
  virtual void                                setSize(std::string& value);
  virtual void                                setRessource(std::string& value);

  virtual std::string                         getRessource() const;
  virtual std::string                         getMethod() const;
  virtual std::string                         getPath() const;
  virtual std::string                         getQuery() const;
  virtual std::string                         getProtocol() const;
  virtual std::string                         getArgs(const std::string& key) const;
  virtual std::string                         getReady() const;
  virtual std::string                         getError() const;
  virtual std::string                         getType() const;
  virtual std::string                         getChunk() const;
  virtual std::string                         getServerName() const;
  virtual std::string                         getListen() const;
  virtual std::string                         getIndex() const;
  virtual std::string                         getRoot() const;
  virtual std::string                         getAllowedMethod() const;
  virtual std::string                         getAllowListing() const;
  virtual std::string                         getRedirect() const;
  virtual std::string                         getErrorPage() const;
  virtual std::string                         getHeader() const;
  virtual std::string                         getStatus() const;
  virtual std::string                         getDone() const;

};

#endif // !ABasicClass_HPP
