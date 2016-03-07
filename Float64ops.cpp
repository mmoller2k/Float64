#include <stdlib.h>
#include "Float64.h"
#include "include/internals.h"

  char      f64::str_[F64_STRLEN];
  uint8_t   f64::aft_point = 10;

/* more f64 operations */

// square root
f64 f64::sqrt () const
{
  f64 result = 0.0;
  result = f64_sqrt (num_);
  return result;
} // end of f64::sqrt

f64 f64::fabs () const
{
  f64 result = *this;
  if( f64_lt(num_,i32_to_f64(0)) )result = f64_mul(num_,i32_to_f64(-1));
  return result;
}

f64 f64::floor() const
{
  f64 result;
  result = f64_roundToInt( num_, softfloat_round_min, 0 );
  return result;
}

int32_t f64::ipart() const
{
  return f64_to_i32(num_,softfloat_round_minMag, 0);
}

int16_t f64::epart(void) const
{
  return epart(NULL);
}

int16_t f64::epart(float64_t *sig) const
{
  return f64_epart(num_, sig);
}

/* return base10 exponent and the significand */
static int16_t f64_epart(float64_t z, float64_t *sig)
{
  int16_t e = expF64UI(z.v);
  int16_t i=0;
  int16_t m=0;

  const float64_t one = i32_to_f64(1);
  const float64_t ten = i32_to_f64(10);
  const float64_t big = i64_to_f64(10000000000000000);
  //big = f64_mul(big,i64_to_f64(100000000)); //bigger
  if(!e)return 0;

  //limit range
  if(e<1023){
    while((e=expF64UI(z.v))<1023){ // multiply number bigger 
      z = f64_mul(z,big);
      m++;
    }
  }
  else{
    while(!f64_lt(z,big)){
      z = f64_div(z,big);
      m--;
    }
  }

  //(0<z<big)
  while((e=expF64UI(z.v))>=1023){
    z = f64_div(z,ten);
    i++;
  }

  if(sig){
    *sig = f64_mul(z,ten);
  }  
  return i-(m*16)-1;
}

// reverses a string 'str' of length 'len'
static void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
 
static int intToStr(i64 x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
 
    reverse(str, i);
    str[i] = '\0';
    return i;
}

// return 10^n
i64 pow10(int n)
{
  int i;
  i64 pn=1;
  for(i=0;i<n;i++)pn*=10;
  return pn;
}

int64_t f64::get_bits(void) const
{
  int64_t *result = (int64_t *)&num_;
  return *result;
}

void f64::lo_bits(int32_t b)
{
	uint32_t *i = (uint32_t *)&num_;
	i[0] = b;
}

void f64::hi_bits(int32_t b)
{
	uint32_t *i = (uint32_t *)&num_;
	i[1] = b;
}

#define expMax 12
#define f64scale 10

char * f64::toString(void) const
{
  return toString(f64scale);
}

char * f64::toString(int afterpoint) const
{
  float64_t fpart, sig, v = num_;
  int64_t pn, ipart;
  int i = 0;
  int16_t e,ep=0;

  if(f64_lt(v,i32_to_f64(0))){
    v=f64_mul(v,i32_to_f64(-1));
    str_[i++] = '-';
  }

  e=f64_epart(v,&sig);
  if(e>expMax || e<-expMax){
    v = sig;
    afterpoint = f64scale;
    ep=e;
  }
    
  // Extract integer part
  ipart = f64_to_i64(v, softfloat_round_minMag, 0);
 
  // Extract floating part
  fpart = f64_sub(v, i64_to_f64(ipart));

  //if(ep && i>9){ /* fix rounding errors */
  //  i /= 10;
  //  ep++;
  //}
 
  // convert integer part to string
  i += intToStr(ipart, &str_[i], 1);
 
  // check for display option after point
  if (afterpoint != 0){
    str_[i++] = '.';  // add dot

    if(i + afterpoint >= F64_STRLEN-1)afterpoint=F64_STRLEN-i;
 
    // Get the value of fraction part upto given no.
    // of points after dot. The third parameter is needed
    // to handle cases like 233.007
    pn = pow10(afterpoint);
    fpart = f64_mul( fpart, i64_to_f64(pn) );
 
    i += intToStr(f64_to_i64(fpart, softfloat_round_min, 0), str_ + i, afterpoint);
  }

  if(ep){
    str_[i++]='e';
    if(ep<0){
      str_[i++]='-';
      ep=-ep;
    }
    intToStr(ep, str_ + i, 0);
  }
  
  return str_;
}

// Allow Arduino's Serial.print() to print f64 objects!
size_t f64::printTo(Print& p) const
{
  char *buf = toString(aft_point);
  p.write(buf);
}

f64 f64::NaN(void)
{
  float64_t z = {defaultNaNF64UI};
  return z;
}

/* minimal strtod - no validation checking */
f64 strtof64(const char *nptr, char **endptr)
{
  f64 result;
  float64_t d = {0};
  int16_t k,e=0;
  int8_t i=0,npos=0;
  i64 nlog=10;
  char c=nptr[0];
  bool stop = false;
  bool neg = false;
  int8_t nexp = 0;
  bool sexp = false;
  const float64_t ten = i32_to_f64(10);
  const float64_t large = i64_to_f64(10000000000000000);
  const float64_t small = f64_div(i32_to_f64(1), i64_to_f64(100000000000000));

  for(i=0;c&&!stop;i++){
    c=nptr[i];
    switch(c){
    case '0' ... '9':
      if(nexp){ /* exponent */
	e *= 10;
	e += c-'0';
	nexp++;
      }
      else if(npos>=0){ /* before the point */
	d = f64_mul(d,ten);
	d = f64_add(d,i32_to_f64(c-'0'));
	npos++;
      }
      else{ /* after the point */
	d = f64_add(d, f64_div(i32_to_f64(c-'0'),i64_to_f64(nlog)) );
	npos--;
	nlog*=10;
      }
      break;
    case '.':
      if(npos>=0){
	npos=-1;
      }
      break;
    case '-':
      if(nexp)sexp=true;
      else neg=true;
      break;
    case 'e':
      nexp=1;
      break;
    case '_': /* placeholders that does nothing */
    case '+':
      continue;
    default:
      stop = true;
      break;
    }
  }

  d = f64_add(d, small); /* force rounding upward */
  /* make .99999999999999 = 1.0 */

  if(sexp) while(e>=16){ d=f64_div(d,large); e-=16;}
  else while(e>=16){ d=f64_mul(d,large);e-=16;}

  if(sexp) while(e--) d=f64_div(d,ten);
  else while(e--) d=f64_mul(d,ten);

  if(endptr)*endptr += i;
  if(!neg)result = d;
  else result = f64_mul(d,i32_to_f64(-1));

  return result;
}

f64 atof64(const char *str)
{
  float64_t result = strtof64(str, NULL);
  return result;
}

