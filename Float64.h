#ifndef _float64_H_
#define _float64_H_

#include <Arduino.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#include "include/softfloat.h"
#include "include/specialize.h"
#include "include/internals.h"
}
#endif

//typedef float64_t f64;
typedef float32_t f32;
typedef uint64_t ui64;
typedef int64_t i64;

#define F64_STRLEN 16

class f64 : public Printable
{
  // member variable (the number)
  float64_t        num_;
  static char      str_[];
  static int   aft_point;
  static int   obase;

  void init(void);

public:
  f64 ();  // default constructor
  //f64 (const float32_t f);   // constructor from float
  f64 (const float64_t f);  // constructor from float64_t
  //f64 (const double f);   // constructor from double
  f64 (const int n);  // constructor from int
  f64 (const long n);  // constructor from int
  //f64 (const int32_t n);  // constructor from int32_t
  f64 (const uint32_t n);  // constructor from uint32_t
  //f64 (const int64_t n);  // constructor from int64_t
  //f64 (const uint64_t n);  // constructor from uint64_t
  f64 (const uint32_t hi, const uint32_t lo);  // constructor from two uint32_t
  // copy constructor
  f64 (const f64 & rhs);

  uint32_t bits(int n) const;
  uint64_t bits() const;
  //void lo_bits(int32_t b);
  //void hi_bits(int32_t b);
 
  // destructor  
  ~f64 ();
  
  // for outputting purposes ...
  char * toString (void) const;  // returns default number of decimal places.
  char * toString (int) const;  // returns number as string.
  operator int () const;
  operator long () const;
  //operator double () const;
  //operator float32_t () const;
  operator float64_t () const;
  void setDecs(int n); /* set no of digits after the decimal point */
  void setBase(int n); /* arbitrary base selection */
  virtual size_t printTo(Print& p) const; // for Serial.print()

  // operators ... assignment
  f64 & operator= (const f64 & rhs);
  f64 & operator= (const float64_t & rhs);
  
  // operations on the number which change it (eg. a += 5; )
  f64 & operator+= (const f64 & n);
  f64 & operator-= (const f64 & n);
  f64 & operator/= (const f64 & n);
  f64 & operator*= (const f64 & n);
  f64 & operator%= (const f64 & n);  // modulo

  // operations on the number which do not change it (eg. a = b + 5; )
  f64 operator+ (const f64 & n) const { f64 temp = *this; temp += n; return temp; };
  //f64 operator+ (const double & n) const { f64 temp = *this; temp += f64(n); return temp; };
  f64 operator- (const f64 & n) const { f64 temp = *this; temp -= n; return temp; };
  //f64 operator- (const double & n) const { f64 temp = *this; temp -= f64(n); return temp; };
  f64 operator/ (const f64 & n) const { f64 temp = *this; temp /= n; return temp; };
  //f64 operator/ (const double & n) const { f64 temp = *this; temp /= f64(n); return temp; };
  f64 operator* (const f64 & n) const { f64 temp = *this; temp *= n; return temp; };
  //f64 operator* (const double & n) const { f64 temp = *this; temp *= f64(n); return temp; };
  f64 operator% (const f64 & n) const { f64 temp = *this; temp %= n; return temp; };
  //f64 operator% (const double & n) const { f64 temp = *this; temp %= f64(n); return temp; };
  
  // prefix operations
  f64 & operator++ () { *this += f64(1); return *this; }
  f64 & operator-- () { *this -= f64(1); return *this; }
  f64 & operator- () { *this *= f64(-1); return *this; }
  
  // postfix operations (cannot return by reference)
  // we make a temporary object, change our current object, return the temporary one
  // if we returned a reference it would cease to exist, so we have to return a copy
  f64 operator++ (int) { f64 temp = *this; *this += 1; return temp; }
  f64 operator-- (int) { f64 temp = *this; *this -= 1; return temp; }
  
  // comparisons
  bool operator<  (const f64 & rhs) const;
  bool operator<  (const int rhs) const { return *this < f64 (rhs); }
  //bool operator<  (const double rhs) const { return *this < f64 (rhs); }
  bool operator>  (const f64 & rhs) const;
  bool operator>  (const int rhs) const { return *this > f64 (rhs); }
  //bool operator>  (const double rhs) const { return *this > f64 (rhs); }
  bool operator<= (const f64 & rhs) const;
  bool operator<= (const int rhs) const { return *this <= f64 (rhs); }
  //bool operator<= (const double rhs) const { return *this <= f64 (rhs); }
  bool operator>= (const f64 & rhs) const;
  bool operator>= (const int rhs) const { return *this >= f64 (rhs); }
  //bool operator>= (const double rhs) const { return *this >= f64 (rhs); }
  bool operator!= (const f64 & rhs) const;
  bool operator!= (const int rhs) const { return *this != f64 (rhs); }
  //bool operator!= (const double rhs) const { return *this != f64 (rhs); }
  bool operator== (const f64 & rhs) const;
  bool operator== (const int rhs) const { return *this == f64 (rhs); }
  //bool operator== (const double rhs) const { return *this == f64 (rhs); }

  // quick sign test
  bool isNegative (void) const;
  // quick zero test
  //bool isZero (void) const;
  // integer part
  int32_t ipart (void) const; //only for quick 32-bit conversion
  //int16_t epart (void) const;
  //int16_t epart (float64_t *) const;
  //float64_t s_f64(void);
  
  // other mathematical operations
  f64 sqrt (void) const;
  f64 fabs (void) const;
  f64 floor (void) const;
  f64 intval (void) const;
  f64 setNaN(void);
  bool isNaN(void) const;
  bool isInf(void) const;
  bool isNum(void) const;
  //f64 pow (const f64 power) const;
  // divide number by divisor, give quotient and remainder
  //void divMod (const f64 divisor, f64 & quotient, f64 & remainder) const;
  // raise number by power, modulus modulus
  //f64 powMod (const f64 power, const f64 & modulus) const;
  
};  // end class declaration

i64 powbase(int n, int8_t base);
f64 strtof64(const char *nptr, char **endptr);
f64 atof64(const char *str);
static int16_t f64_epart(float64_t z, float64_t *sig, int8_t base);

#endif

