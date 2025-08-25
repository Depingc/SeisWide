//[of]:Read-me-first
//[c]To browse this file, first re-open it in a new window using the 'Open in New Tab' 
//[c]feature from the 'Go' menu (CTRL-I), this step is to avoid having the tutorial.c
//[c]file in the first pane. Then select 'Browser - Four Panes' in the 'View' menu 
//[c](ALT-5) to browse this C++ file.
//[c]
//[c]Now you should have four panes in the window: three panes at the top for lists
//[c]and one pane to display source code and comments.
//[c]
//[c]Select classes in the first pane, then choose a protocol in the second one and finally
//[c]select a method in the third one.
//[c]
//[c]Enjoy.
//[c]
//[cf]
//[c]
//[of]:Includes
//[c]
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//[c]
#include "string.h"
//[cf]
//[of]:Constants and Macros
//[c]
#define MEMORY_BUFFER_DEF_SIZE 2048
#define STRING_BUFFER_DEF_SIZE 16384
//[c]
#define COUNT_TYPE int
#define HEADER     sizeof(COUNT_TYPE)
#define COUNT(x)   (*((COUNT_TYPE*)(x-HEADER)))
#define ALLOC(ptr, len) \
  ptr = new char[HEADER+len+1]+HEADER; \
  COUNT(ptr) = 1;
#define RELEASE(p)  if( p!=theNullCharPtr && --COUNT(p)==0 ) delete (void*)(p-HEADER);
//[cf]
//[of]:Globals
//[c]
const char* theNullCharPtr = "";
String theEmptyString;
//[cf]
//[c]
//[of]:Class String
//[l]:Declaration of String:string.h#string
//[c]
//[of]:Constructors - Destructor
//[of]:String()
//[c]Construct an empty string
//[c]
String::String()
{
  p = theNullCharPtr;
}
//[cf]
//[of]:String(str)
//[c]Construct a string from a raw buffer
//[c]
String::String(const char* s)
{
  int len = strlen(s);
  char* buf;
  ALLOC(buf, len)
  strcpy(buf, s);
  p = buf;
}
//[cf]
//[of]:String(str, len)
//[c]Construct a string from a raw buffer
//[c]
String::String(const char* s, int len)
{
  char* buf;
  ALLOC(buf, len)
  memcpy(buf, s, len);
  buf[len] = 0;
  p = buf;
}
//[cf]
//[of]:String(const String&)
//[c]Construct a string from another string
//[c]
String::String(const String& s)
{
  p = s.p;
  if( p!=theNullCharPtr ) COUNT(p)++;
}
//[cf]
//[of]:String(const SubString& s)
//[c]Construct a string from a substring
//[c]
String::String(const SubString& s)
{
  int len = s.end-s.start;
  char* buf;
  ALLOC(buf, len)
  memcpy(buf, s.start, len);
  buf[len] = 0;

  p = buf;
}
//[cf]
//[of]:String(const char* s1, const char* s2)
//[c]Construct a string with two raw buffers
//[c]
String::String(const char* s1, const char* s2)
{
  int l1 = strlen(s1);
  int l2 = strlen(s2);
  char* buf;

  ALLOC(buf, l1+l2)

  memcpy(buf,    s1, l1);
  memcpy(buf+l1, s2, l2);
  buf[l1+l2] = 0;

  p = buf;
}
//[cf]
//[c]
//[of]:~String()
//[c]Free the memory allocated by the string
//[c]
String::~String()
{
  RELEASE(p)
}
//[cf]
//[cf]
//[of]:Copying
//[of]:operator=(const String& s)
//[c]Assign a new string
//[c]
void String::operator=(const String& s)
{
  // already assign to this string
  if( p==s.p ) return;

  // release the old string
  RELEASE(p)

  p = s.p;
  if( p!=theNullCharPtr ) COUNT(p)++;
}
//[cf]
//[of]:operator=(const char* s)
//[c]Assign a new string
//[c]
void String::operator=(const char* s)
{
  // release the old string
  RELEASE(p)

  // copy the string in a new buffer
  int len = strlen(s);
  char* buf;
  ALLOC(buf, len);
  strcpy(buf, s);

  p = buf;
}
//[cf]
//[cf]
//[of]:Testing
//[of]:startWith(const char* s)
//[c]Test if the string starts with the argument
//[c]
bool String::startWith(const char* s) const
{
  return strncmp(p, s, strlen(s))==0;
}
//[cf]
//[of]:endWith(const char* s)
//[c]Tests if the string ends with the argument
//[c]
bool String::endWith(const char* s) const
{
  int l2 = strlen(s);
  int l1 = len();
  if( l2>l1 ) return false;
  return strcmp(p+l1-l2, s)==0;
}
//[cf]
//[cf]
//[of]:Searching
//[of]:firstOccurrence(char)
//[c]Returns the first occurrence of a char, -1 if not found
//[c]
int String::firstOccurrence(char c) const
{
  const char* q = p;
  char d = *q++;
  while( d )
  {
    if( d==c ) return q-p-1;
    d = *q++;
  }

  // not found
  return -1;
}
//[cf]
//[of]:lastOccurrence(char)
//[c]Returns the last occurence of a char, -1 if not found
//[c]
int String::lastOccurrence(char c) const
{
  const char* q = p+len()-1;
  while( q>=p )
  {
    if( *q==c ) return q-p;
    q--;
  }

  // not found
  return -1;
}
//[cf]
//[cf]
//[of]:Accessing
//[of]:empty()
//[c]Assign to the empty string
//[c]
void String::empty()
{
  if( isEmpty() ) return;

  RELEASE(p);

  p = theNullCharPtr;
}
//[cf]
//[c]
//[of]:hash()
//[c]Returns the hash code of the string
//[c]
int String::hash() const
{
  const char* q = p;
  int sum = 12345;

  while( *q ) sum = (sum<<1) ^ *q++;

  return sum;
}
//[cf]
//[of]:left(int l)
//[c]Return the <l> first char
//[c]
String String::left(int l) const
{
  return SubString(p, l);
}
//[cf]
//[of]:right(int l)
//[c]Return the <l> last characters
//[c]
//[c]	If the l is over the length of the string, all the string is returned.
//[c]
String String::right(int l) const
{
  int offset = len()-l;
  return p+offset;
}
//[cf]
//[of]:noLeft(int offset)
//[c]Return a copy of the string without the <offset> first characters
//[c]
String String::noLeft(int offset) const
{
  return p+offset;
}
//[cf]
//[of]:operator+(const char* s)
//[c]Concatenate two strings
//[c]
String String::operator+(const char* s) const
{
  return String(p, s);
}
//[cf]
//[cf]
//[of]:Converting
//[of]:upper()
//[c]Converts the string to an uppercase string
//[c]
String String::upper() const
{
  int   len = strlen(p);
  char* buf;

  ALLOC(buf, len)

  const char* src = p;
  char* dst = buf;
  char c;
  do { c=*src++; *dst++=toupper(c); } while( c );

  return RawString(buf);
}
//[cf]
//[of]:lower()
//[c]Converts the string to an uppercase string
//[c]
String String::lower() const
{
  int   len = strlen(p);
  char* buf;

  ALLOC(buf, len)

  const char* src = p;
  char* dst = buf;
  char c;
  do { c=*src++; *dst++=tolower(c); } while( c );

  return RawString(buf);
}
//[cf]
//[of]:toInteger()
//[c]Converts the string to an integer
//[c]
int String::toInteger() const
{
  return atoi(p);
}
//[cf]
//[of]:hexaToInteger()
//[c]Converts an hexadecimal string to an integer
//[c]
int String::hexaToInteger() const
{
  int value = 0;
  const char* q = p;
  char c = *q++;
  while( c )
  {
    value = value<<4;
    if( '0'<=c && c<'9'+1 )
      value += c-'0';
    else if( 'a'<=c && c<'f'+1 )
      value += c-'a'+10;
    else if( 'A'<=c && c<'F'+1 )
      value += c-'A'+10;
    else
      break;
    c = *q++;
  }

  return value;
}
//[cf]
//[cf]
//[cf]
//[of]:Class SubString
//[l]:Declaration of SubString:string.h#sub-string
//[c]
//[of]:Searching
//[of]:firstOccurrence(c)
//[c]Returns the first occurence of a char, -1 if not found
//[c]
int SubString::firstOccurrence(char c) const
{
  const char* q = start;
  while( q<end )
  {
    char d = *q++;
    if( d==c ) return q-start-1;
  }

  // not found
  return -1;
}
//[cf]
//[of]:lastOccurrence(c)
//[c]Returns the last occurence of a char, -1 if not found
//[c]
int SubString::lastOccurrence(char c) const
{
  const char* q = end-1;
  while( q>=start )
  {
    if( *q==c ) return q-start;
    q--;
  }

  // not found
  return -1;
}
//[cf]
//[cf]
//[of]:Testing
//[of]:startWith(const String&)
//[c]Tests if the string start with a substring
//[c]
bool SubString::startWith(const String& s) const
{
  int len = s.len();
  int used = end-start;
  if( len>used ) return false;
  return strncmp(start, s.chars(), len)==0;
}
//[cf]
//[of]:endWith(const String&)
//[c]Tests if the string end with a substring
//[c]
bool SubString::endWith(const String& s) const
{
  int len = s.len();
  int used = end-start;
  if( len>used ) return false;
  return strncmp(end-len, s.chars(), len)==0;
}
//[cf]
//[cf]
//[of]:Extracting
//[c]The following methods returns a copy of the selected
//[c]part of the sub-string.
//[c]
//[of]:content()
//[c]
String SubString::content() const
{
  return String(start, end);
}
//[cf]
//[of]:left(l)
//[c]
String SubString::left(int l) const
{
  return String(start, l);
}
//[cf]
//[of]:right(l)
//[c]
String SubString::right(int l) const
{
  return String(end-l, end);
}
//[cf]
//[of]:noLeft(l)
//[c]
String SubString::noLeft(int l) const
{
  return String(start+l, end);
}
//[cf]
//[c]
//[of]:upper()
//[c]Converts the string to an uppercase string
//[c]
String SubString::upper() const
{
  int   len = end-start;
  char* buf;

  ALLOC(buf, len)

  const char* src = start;
  char* dst = buf;
  buf[len] = 0;
  while(len-->0) { *dst++=toupper(*src++); }

  return RawString(buf);
}
//[cf]
//[of]:lower()
//[c]Converts the string to an uppercase string
//[c]
String SubString::lower() const
{
  int   len = end-start;
  char* buf;

  ALLOC(buf, len)

  const char* src = start;
  char* dst = buf;
  buf[len] = 0;
  while(len-->0) { *dst++=tolower(*src++); }

  return RawString(buf);
}
//[cf]
//[c]
//[of]:operator+(const String&)
//[c]Concatenates two strings
//[c]
String SubString::operator+(const String& s) const
{
  int l1 = end-start;
  int l2 = s.len();
  char* buf;

  ALLOC(buf, l1+l2)

  memcpy(buf, start, l1);
  memcpy(buf+l1, s.chars(), l2);
  buf[l1+l2] = 0;

  return RawString(buf);
}
//[cf]
//[cf]
//[of]:Converting
//[of]:toInteger()
//[c]Converts the substring to an integer
//[c]
int SubString::toInteger() const
{
  int sum = 0;
  const char* p = start;
  while( p<end && isdigit(*p) ) sum = sum*10 + ((*p++)-'0');
  return sum;
}
//[cf]
//[cf]
//[cf]
//[of]:Class StringBuffer
//[l]:Declaration of StringBuffer:string.h#string-buffer
//[c]
//[of]:Constructors - Destructor
//[of]:StringBuffer()
StringBuffer::StringBuffer()
{
  p = new char[STRING_BUFFER_DEF_SIZE];
  used = 0;
  size = STRING_BUFFER_DEF_SIZE;
}
//[cf]
//[of]:StringBuffer(const String& s)
StringBuffer::StringBuffer(const String& s)
{
  p = new char[STRING_BUFFER_DEF_SIZE];
  used = 0;
  size = STRING_BUFFER_DEF_SIZE;

  (*this)<<s;
}
//[cf]
//[of]:~StringBuffer()
StringBuffer::~StringBuffer()
{
  delete p;
}
//[cf]
//[cf]
//[of]:Converting
//[of]:asString()
//[c]Converts the buffer to a string
//[c]
String StringBuffer::asString()
{
  return subString();
}
//[cf]
//[cf]
//[of]:Accessing
//[of]:firstOccurrence(const String& s)
//[c]Returns the first occurence of a sub string
//[c]
int StringBuffer::firstOccurrence(const String& s)
{
  // mark the end to use the system function strstr
  p[used] = 0;

  // scan the buffer
  char* q = strstr(p, s.chars());

  // return -1 if not found
  if( q==NULL ) return -1;

  // return the index
  return q-p;
}
//[cf]
//[cf]
//[of]:Adding - Removing
//[of]:operator<<(char c)
//[c]Appends a single char
//[c]
StringBuffer& StringBuffer::operator<<(char c)
{
  reserve(1);
  p[used++] = c;
  return *this;
}
//[cf]
//[of]:operator<<(const String& s)
//[c]Appends a string
//[c]
StringBuffer& StringBuffer::operator<<(const String& s)
{
  int len = s.len();
  reserve(len);
  memcpy(p+used, s.chars(), len);
  used += len;
  return *this;
}
//[cf]
//[of]:operator<<(const StringBuffer& s)
//[c]Appends a string buffer
//[c]
StringBuffer& StringBuffer::operator<<(const StringBuffer& s)
{
  int len = s.len();
  reserve(len);
  memcpy(p+used, s.chars(), len);
  used += len;
  return *this;
}
//[cf]
//[of]:operator<<(const char* s)
//[c]Appends a string
//[c]
StringBuffer& StringBuffer::operator<<(const char* s)
{
  int len = strlen(s);
  reserve(len);
  memcpy(p+used, s, len);
  used += len;
  return *this;
}
//[cf]
//[of]:operator<<(int i)
//[c]Appends an integer
//[c]
StringBuffer& StringBuffer::operator<<(int i)
{
  char buf[12];
  sprintf(buf, "%d", i);

  int len = strlen(buf);
  reserve(len);
  memcpy(p+used, buf, len);
  used += len;
  return *this;
}
//[cf]
//[c]
//[of]:remove(start, len)
//[c]Removes a substring
//[c]
void StringBuffer::remove(int start, int len)
{
  // move the end to overlap the range to be removed
  memmove(p+start, p+start+len, used-(start+len));

  // update the used chars
  used -= len;
}
//[cf]
//[cf]
//[of]:Private
//[of]:reserve(int l)
//[c]Reserves a sufficient space
//[c]
void StringBuffer::reserve(int l)
{
  // we always must have one more char
  if( used+l+1 <= size ) return;

  // compute new size
  int newSize = (used+l+1) * 5 / 4;

  // allocate new buffer
  char* q = new char[newSize];

  // copy the old buffer into the new one
  memcpy(q, p, used);

  // free the old buffer
  delete p;

  // update object
  size = newSize;
  p = q;
}
//[cf]
//[cf]
//[cf]
//[of]:Class RawString
//[c]Allows the creation of a string without copying the content
//[c]
class RawString : public String
{
public:
  RawString(const char* s) { p = s; }
};
//[cf]
