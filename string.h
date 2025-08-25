//[c]string.h - the String and StringBuffer classes
//[c]
#include <string.h>

class String;
extern const char* emptyStr;
extern String theEmptyString;
//[c]
//[of]string:	Class String
//[c]The character string class
//[c]
class String
{
friend class SubString;

protected:
  const char* p;

  String(const char* s, int len);
  String(const char* s1, const char* s2);

public:
  String();
  String(const char* s);
  String(const String& s);
  String(const SubString& s);
  ~String();

  operator const char*() const    { return p; }

  void operator=(const String& s);
  void operator=(const char* s);
  void empty();

  // Accessors
  // ---------
  int len() const { return (int)strlen(p); }
  const char* chars() const { return p; }
  char operator[](unsigned i) const { return p[i]; }
  int hash() const;

  // Search
  // ------
  int firstOccurrence(char c) const;
  int lastOccurrence(char c) const;

  // Tests
  // -----
  bool isEmpty() const { return *p==0; }
  bool notEmpty() const { return *p!=0; }

  bool operator<(const String& s) const   { return strcmp(p, s.p) < 0; }
  bool operator>(const String& s) const   { return strcmp(p, s.p) > 0; }
  bool operator<=(const String& s) const  { return strcmp(p, s.p) <= 0; }
  bool operator>=(const String& s) const  { return strcmp(p, s.p) >= 0; }
  bool operator==(const String& s) const  { return strcmp(p, s.p) == 0; }
  bool operator!=(const String& s) const  { return strcmp(p, s.p) != 0; }

  bool operator<(const char* cs) const    { return strcmp(p,cs) < 0; }
  bool operator>(const char* cs) const    { return strcmp(p,cs) > 0; }
  bool operator<=(const char* cs) const   { return strcmp(p,cs) <= 0; }
  bool operator>=(const char* cs) const   { return strcmp(p,cs) >= 0; }
  bool operator==(const char* cs) const   { return strcmp(p,cs) == 0; }
  bool operator!=(const char* cs) const   { return strcmp(p,cs) != 0; }
  bool startWith(const char* cs) const;
  bool endWith(const char* cs) const;

  bool includes(const String& s) { return strstr(p, s.chars())!=NULL; }
  bool includes(const char* s)   { return strstr(p, s)!=NULL; }

  // Copying
  // -------
  String left(int len) const;
  String right(int len) const;
  String noLeft(int len) const;
  String upper() const;
  String lower() const;

  String operator+(const char* s) const;

  // Converting
  // ----------
  int toInteger() const;
  int hexaToInteger() const;
};
//[cf]
//[of]sub-string:	Class SubString
//[c]The sub-string class
//[c]
class SubString
{
friend class String;

private:
  const char* start;
  const char* end;

public:
  SubString(const char* s, const char* e) { start = s; end = e; }
  SubString(const char* s, int l)         { start = s; end = s+l; }
  SubString(const String& s, int l)       { start = s.chars(); end = start+l; }

  // Extracting
  // ----------
  void skip(int i) { start += i; }

  // Search
  // ------
  int firstOccurrence(char c) const;
  int lastOccurrence(char c) const;

  // Tests
  // -----
  bool isEmpty () const { return start==end; }
  bool notEmpty() const { return start!=end; }
  bool startWith(const String& s) const;
  bool endWith(const String& s) const;

  // Copying
  // -------
  String content() const;
  String left(int len) const;
  String right(int len) const;
  String noLeft(int len) const;
  String upper() const;
  String lower() const;
  String operator+(const String& s) const;

  int toInteger() const;
};
//[cf]
//[of]string-buffer:	Class StringBuffer
//[c]String buffer class
//[c]
class StringBuffer
{
private:
  char* p;
  int used;
  int size;

  void reserve(int len);

public:
  StringBuffer();
  StringBuffer(const String& s);
  ~StringBuffer();

  // Accessing
  // ---------
  int len() const { return used; }
  char* chars() const { return p; }
  char& operator[](unsigned i) { return p[i]; }
  char  operator[](unsigned i) const { return p[i]; }

  int firstOccurrence(const String& s);

  // Conversion
  // ----------
  String asString();
  SubString subString() const { return SubString(p, used); }

  // Adding-Removing
  // ---------------
  StringBuffer& operator<<(char c);
  StringBuffer& operator<<(const String& s);
  StringBuffer& operator<<(const StringBuffer& s);
  StringBuffer& operator<<(const char* s);
  StringBuffer& operator<<(int i);

  void remove(int start, int len);
};
//[cf]
