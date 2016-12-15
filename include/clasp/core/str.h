/*
    File: str.h
*/

/*
Copyright (c) 2014, Christian E. Schafmeister
 
CLASP is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.
 
See directory 'clasp/licenses' for full details.
 
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/* -^- */
#ifndef _core_str_H
#define _core_str_H

#include <clasp/core/foundation.h>
#include <clasp/core/object.h>
#include <clasp/core/character.fwd.h>
#include <clasp/core/lispString.h>

namespace cl {
  extern core::Symbol_sp& _sym_simple_vector;
};

namespace core {

FORWARD(Str);
class Str_O : public String_O {
  LISP_CLASS(core, ClPkg, Str_O, "base-string",String_O);
 protected:
  typedef gctools::gcstring str_type;
  str_type _Contents;
 public:
  typedef str_type::iterator iterator;
  typedef str_type::const_iterator const_iterator;
 public:
  static Str_sp create(const boost::format &fmt);
  static Str_sp create(const string &nm);
  static Str_sp create(const char *nm);
  static Str_sp create(size_t numChars);
  static Str_sp create(const char *nm, int numChars);
  static Str_sp create(claspChar initial_element, int dimension);
  static Str_sp create(Str_sp orig);
public:
  static Bignum stringToBignum(const char *str);
public:
  virtual bool adjustableArrayP() const { return false; }
public:
  //! dimension() ignores the fill pointer
  virtual gc::Fixnum dimension() const { return this->_Contents.size(); };
  //! size is subclassed by StrWithFillPtr_O and uses the fill-pointer
CL_LISPIFY_NAME("core:size");
CL_DEFMETHOD   virtual gc::Fixnum size() const { return this->_Contents.size(); };
  virtual void swapElements(uint i1, uint i2) {
    char t = this->_Contents[i2];
    this->_Contents[i2] = this->_Contents[i1];
    this->_Contents[i1] = t;
  }
  virtual T_sp aset_unsafe(cl_index j, T_sp val);
  virtual T_sp aref_unsafe(cl_index index) const { return clasp_make_character(this->_Contents[index]); };
  virtual T_sp elementType() const;
  /*! Return the value at the indices */
  virtual T_sp aref(VaList_sp indices) const;
  /*! Return the value at the indices */
  virtual T_sp setf_aref(List_sp indices_val);

  virtual void __write__(T_sp strm) const;

  virtual T_sp elt(cl_index index) const;
  virtual T_sp setf_elt(cl_index index, T_sp value);

  virtual T_sp svref(cl_index index) const { TYPE_ERROR(this->asSmartPtr(), cl::_sym_simple_vector); };
  virtual T_sp setf_svref(cl_index index, T_sp value) { TYPE_ERROR(this->asSmartPtr(), cl::_sym_simple_vector); };

  virtual T_sp subseq(cl_index start, T_sp end) const;
  virtual T_sp setf_subseq(cl_index start, T_sp end, T_sp new_subseq);

  virtual void fillArrayWithElt(T_sp element, Fixnum_sp start, T_sp end);

  virtual void *addressOfBuffer() const;
  virtual size_t elementSizeInBytes() const { return sizeof(claspChar); };

  virtual void fillInitialContents(T_sp initialContents);

  virtual void sxhash_(HashGenerator &hg) const;

  inline char &operator[](cl_index i) { return this->_Contents[i]; };
  inline const char &operator[](cl_index i) const { return this->_Contents[i]; };




  const char *c_str() const { return this->_Contents.c_str(); };
  string __str__() { return this->_Contents.asStdString(); };
  virtual string get() const { return this->_Contents.asStdString(); };
  virtual void set(const string &v) {
    str_type temp(v);
    this->_Contents.swap(temp);
  };
  virtual void setFromChars(const char *v) {
    str_type temp(v);
    this->_Contents.swap(temp);
  };
  void setFromSize(size_t num) {
    str_type temp(num);
    this->_Contents.swap(temp);
  };
  void setFromChars(const char *v, int num) {
    str_type temp(v, num);
    this->_Contents.swap(temp);
  };

  gctools::gcstring &contents() { return this->_Contents; };
  string __repr__() const;
  uint countOccurances(const string &chars);
  List_sp splitAtWhiteSpace();
  List_sp split(const string &splitChars);
//  Fixnum_sp asInt() const;
  Rational_sp parseInteger();
  DoubleFloat_sp asReal() const;
  Symbol_sp asSymbol() const;
  Symbol_sp asKeywordSymbol() const;
  string left(gc::Fixnum num) const;
  string right(gc::Fixnum num) const;
  string substr(gc::Fixnum start, gc::Fixnum num) const;

  Str_O &operator+=(const string &s) {
    this->_Contents += s;
    return *this;
  }

  Str_sp operator+(const string &s) {
    string result(this->_Contents.data(), this->_Contents.size());
    result += s;
    return Str_O::create(result);
  }

  iterator begin() { return this->_Contents.data(); }
  iterator end() { return this->_Contents.data() + this->size(); };
  const_iterator begin() const { return this->_Contents.data(); }
  const_iterator end() const { return this->_Contents.data() + this->size(); };

  claspChar schar(gc::Fixnum index) const;
  claspChar scharSet(gc::Fixnum index, claspChar c);

  /*! Return the index of where substring is found 
	  or nil
	*/
  T_sp find(const string &substring, gc::Fixnum start);

public:
  cl_index length() const { return this->size(); };
  
#ifndef USE_TEMPLATE_STRING_MATCHER
  virtual T_sp string_EQ_(Str_sp string2, int start1, int end1, int start2, int end2) const;
  virtual T_sp string_NE_(Str_sp string2, int start1, int end1, int start2, int end2) const;
  virtual T_sp string_LT_(Str_sp string2, int start1, int end1, int start2, int end2) const;
  virtual T_sp string_GT_(Str_sp string2, int start1, int end1, int start2, int end2) const;
  virtual T_sp string_LE_(Str_sp string2, int start1, int end1, int start2, int end2) const;
  virtual T_sp string_GE_(Str_sp string2, int start1, int end1, int start2, int end2) const;

  virtual T_sp string_equal(Str_sp string2, int start1, int end1, int start2, int end2) const;
  virtual T_sp string_not_equal(Str_sp string2, int start1, int end1, int start2, int end2) const;
  virtual T_sp string_lessp(Str_sp string2, int start1, int end1, int start2, int end2) const;
  virtual T_sp string_greaterp(Str_sp string2, int start1, int end1, int start2, int end2) const;
  virtual T_sp string_not_greaterp(Str_sp string2, int start1, int end1, int start2, int end2) const;
  virtual T_sp string_not_lessp(Str_sp string2, int start1, int end1, int start2, int end2) const;
#endif
  
public:
  explicit Str_O() : Base(){};
  virtual ~Str_O(){};
};
};
template <>
struct gctools::GCInfo<core::Str_O> {
  static bool constexpr NeedsInitialization = false;
  static bool constexpr NeedsFinalization = false;
  static GCInfo_policy constexpr Policy = normal;
};


namespace core {
T_sp core__base_string_concatenate(T_sp vargs);

 #if 0
 // What the hell am I doing here - comment out and see what breaks
inline T_sp base_string_concatenate(LCC_ARGS_ELLIPSIS) {
  VaList_S lcc_arglist_s;
  va_start(lcc_arglist_s._Args, LCC_VA_START_ARG);
  LCC_SPILL_REGISTER_ARGUMENTS_TO_VA_LIST(lcc_arglist_s);
  VaList_sp valist_sp(&lcc_arglist_s);
  return core__base_string_concatenate(valist_sp);
  T_sp replace_array(T_sp other) {
    *this = *gc::As<Str_sp>(other);
    return this->asSmartPtr();
  }
};
#endif


 
inline claspChar clasp_char(Str_sp s, Fixnum pos) { return s->schar(pos); };
};

#endif
