/*  f64.cpp
 *  A 64-bit floating-point C++ wrapper for Berkeley SoftFloat.
 *  Author:  Michael Moller
 *  Date:    2016.
 *  Inspired by Nick Gammon's BigNumber bc wrapper
*/

#include <stdlib.h>
#include "Float64.h"

char f64::str_[STRLEN];
uint8_t f64::aft_point = STRLEN;

// constructor
f64::f64 ()
{
  init();
  num_ = i32_to_f64(0); // default to zero
} // end of constructor from NULL

// constructor
f64::f64 (float32_t f)
{
  init();
  num_ = f32_to_f64(f);
} // end of constructor from float

f64::f64 (float64_t f)
{
  init();
  num_ = f;
} // end of constructor from float

// constructor
// doubles are 32-bit floats on Arduino
f64::f64 (double f)
{
  float32_t *fp = (float32_t *)&f;
  init();
  if(sizeof(double)==4){
    num_ = f32_to_f64(*fp);
  }
  else{
    num_ = *(float64_t*)&f;
  }
} // end of constructor from double

f64::f64 (const int n)  // constructor from int
{
  init();
  num_ = i32_to_f64(n);
} // end of constructor from int

f64::f64 (const long n)  // constructor from int
{
  init();
  num_ = i32_to_f64(n);
} // end of constructor from int

/*
f64::f64 (const int32_t n)  // constructor from int
{
  init();
  num_ = i32_to_f64(n);
} // end of constructor from int
*/

f64::f64 (const uint32_t n)  // constructor from uint
{
  init();
  num_ = ui32_to_f64(n);
} // end of constructor from int

/*
f64::f64 (const int64_t n)  // constructor from int64_t
{
  init();
  num_ = i64_to_f64(n);
} // end of constructor from int

f64::f64 (const uint64_t n)  // constructor from uint64_t
{
  init();
  num_ = ui64_to_f64(n);
} // end of constructor from int
*/

f64::f64 (const uint32_t hi, const uint32_t lo)  // constructor from two uint32_t
{
  init();
  hi_bits(hi);
  lo_bits(lo);
} // end of constructor from int

// copy constructor
f64::f64 (const f64 & rhs)
{
  init();
  if (this != &rhs)
    num_ = rhs.num_;
}  // end of f64::f64
  
//operator=
f64 & f64::operator= (const f64 & rhs)
{
  // gracefully handle self-assignment (eg. a = a;)
  if (this == &rhs )
    return *this;
  
  num_ = rhs.num_;
  return *this;
} // end of f64::f64 & operator=

f64 & f64::operator= (const float64_t & rhs)
{
  num_ = rhs;
  return *this;
} // end of f64::f64 & operator=

/*
float64_t f64::s_f64(void)
{
	return num_;
}
*/

// destructor - free memory used, if any
f64::~f64 ()
{
	//free(str_);
} // end of destructor

// initialize package
void f64::init (void)
{
}


f64::operator int () const
{
  return f64_to_i32(num_, softfloat_round_max, 0);
} // end of f64::operator int

f64::operator long () const
{
  return f64_to_i32(num_, softfloat_round_max, 0);
} // end of f64::operator long

f64::operator double () const
{
  return *(double*)&num_;
}

f64::operator float64_t () const
{
  return num_;
} // end of f64::operator long

f64::operator float32_t () const
{
  return f64_to_f32(num_);
} // end of f64::operator long

// Allow Arduino's Serial.print() to print f64 objects!
size_t f64::printTo(Print& p) const
{
  char *buf = toString(aft_point);
  p.write(buf);
}

void f64::printDecs(uint8_t n)
{
	aft_point = n;
}

// add
f64 & f64::operator+= (const f64 & n)
{ 
  float64_t result = i32_to_f64(0);
  result = f64_add (num_, n.num_);
  num_ = result;
  return *this; 
} // end of f64::operator+= 

// subtract
f64 & f64::operator-= (const f64 & n)
{ 
  float64_t result = i32_to_f64(0);
  result = f64_sub (num_, n.num_);
  num_ = result;
  return *this; 
}  // end of f64::operator-=

// divide
f64 & f64::operator/= (const f64 & n)
{ 
  float64_t result = i32_to_f64(0);
  result = f64_div (num_, n.num_);
  num_ = result;
  return *this; 
} // end of f64::operator/= 

// multiply
f64 & f64::operator*= (const f64 & n)
{ 
  float64_t result = i32_to_f64(0);
  result = f64_mul (num_, n.num_);
  num_ = result;
  return *this; 
}  // end of f64::operator*=

// modulo
f64 & f64::operator%= (const f64 & n)
{ 
  float64_t result = i32_to_f64(0);
  result = f64_rem (num_, n.num_);
  num_ = result;
  return *this; 
}  // end of f64::operator%=


// ----------------------------- COMPARISONS ------------------------------

// compare less with another f64
bool f64::operator< (const f64 & rhs) const
{
  return f64_lt(num_, rhs.num_);
} // end of f64::operator<


// compare greater with another f64
bool f64::operator> (const f64 & rhs) const
{
  return !(f64_lt(num_, rhs.num_) || f64_eq(num_, rhs.num_));
} // end of f64::operator>

// compare less-or-equal with another f64
bool f64::operator<= (const f64 & rhs) const
{
  return f64_lt(num_, rhs.num_) || f64_eq(num_, rhs.num_);
} // end of f64::operator<=

// compare greater-or-equal with another f64
bool f64::operator>= (const f64 & rhs) const
{
  return !f64_lt(num_, rhs.num_);
} // end of f64::operator>=

// compare not equal with another f64
bool f64::operator!= (const f64 & rhs) const
{
  return !f64_eq(num_, rhs.num_);
} // end of f64::operator!=

// compare equal with another f64
bool f64::operator== (const f64 & rhs) const
{
  return f64_eq(num_, rhs.num_);
} // end of f64::operator==

// special comparisons
bool f64::isNegative () const
{
  return f64_lt(num_,i32_to_f64(0));
} // end of f64::isNegative

bool f64::isZero () const
{
  return f64_eq(num_,i32_to_f64(0));
} // end of f64::isZero

