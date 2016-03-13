#include <stdlib.h>
#include <string.h>
#include "Float64.h"

char      f64::str_[F64_STRLEN];
int8_t   f64::aft_point = 10;
int8_t   f64::obase = 10; //belongs to class
static int8_t ibase = 10; //belongs to ops

/* more f64 operations */

// square root
f64 f64::sqrt (void) const
{
  return f64_sqrt (num_);
} // end of f64::sqrt

f64 f64::fabs (void) const
{
  f64 result = *this;
  if( f64_lt(num_,i32_to_f64(0)) )result = f64_mul(num_,i32_to_f64(-1));
  return result;
}

f64 f64::floor(void) const
{
  return f64_roundToInt( num_, softfloat_round_min, 0 );
}

f64 f64::intval(void) const
{
  return f64_roundToInt( num_, softfloat_round_minMag, 0 );
}


void f64::setDecs(int8_t n)
{
  aft_point = n;
}

void f64::setBase(int8_t n)
{
  obase = ibase = n;
}

/*
int16_t f64::epart(void) const
{
  return epart(NULL);
}

int16_t f64::epart(float64_t *sig) const
{
  return f64_epart(num_, sig,obase);
}
*/

/* return base_x exponent and the significand */
static int16_t f64_epart(float64_t z, float64_t *sig, int8_t base)
{
  int16_t e = expF64UI(z.v);
  int16_t i=0;
  int16_t m=0;

  //const float64_t one = i32_to_f64(1);
  const float64_t bn = i32_to_f64(base);
  const float64_t big = i64_to_f64(powbase(16,base));
  //big = f64_mul(big,i64_to_f64(100000000)); //bigger
  if(e==0)return 0;
  if(e==0x7ff)return 0; //z==inf?

  //limit range
  if(e<1023){
    while((e=expF64UI(z.v))<1023){ // multiply number bigger 
      if(softfloat_exceptionFlags>1)return 0;
      z = f64_mul(z,big);
      m++;
    }
  }
  else{
    while(!f64_lt(z,big)){
      if(softfloat_exceptionFlags>1)return 0;
      z = f64_div(z,big);
      m--;
    }
  }

  //(0<z<big)
  while((e=expF64UI(z.v))>=1023){
    if(softfloat_exceptionFlags>1)return 0;
    z = f64_div(z,bn);
    i++;
  }

  if(sig){
    *sig = f64_mul(z,bn);
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
 
static int intToStr(i64 x, char str[], int8_t d, int8_t base)
{
    int8_t ch, i = 0;
    while (x)
    {
        ch = (x%base) + '0';
        if(ch>'9')ch+='A'-':';
        str[i++] = ch;
        x = x/base;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
 
    reverse(str, i);
    str[i] = '\0';
    return i;
}

// return base^n
i64 powbase(int n, int8_t base)
{
  int i;
  i64 pn=1;
  for(i=0;i<n;i++)pn*=base;
  return pn;
}

uint32_t f64::bits(int n) const
{
  return *(((uint32_t *)&num_)+n);
}

uint64_t f64::bits(void) const
{
  return *(int64_t *)&num_;
}

/*
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
*/

static float64_t bintof64(uint64_t u)
{
  float64_t f;
  f.v = u;
  return f;
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
  float64_t small;
  int64_t pn, ipart, ip0;
  int i = 0;
  int16_t e,ep=0;

  //if(f64_lt(v,i32_to_f64(0))){
  if(signF64UI(num_.v)){
    v=f64_mul(v,i32_to_f64(-1));
    str_[i++] = '-';
  }
  if(isNaN()){
    strcpy(str_,"NaN");
    return str_;
  }
  if(isInf()){
    strcpy(str_+i,"Inf");
    return str_;
  }
  if(expF64UI(num_.v)==0x7ff){ // unspecified error
    strcpy(str_,"Err ");
    intToStr(softfloat_exceptionFlags,str_+4,0,10);
    softfloat_exceptionFlags=0;
    return str_;
  }
  e=f64_epart(v,&sig,obase);

  if(e>expMax || e<-expMax){
    v = sig;
    afterpoint = f64scale;
    ep=e;
  }


  small=f64_div( i32_to_f64(obase/2),i64_to_f64(powbase(afterpoint+1,obase)) );    
  v = f64_add(v, small); /* force rounding upward */
  /* (make .99999999999999 = 1.0) */

  // Extract integer part
  ipart = f64_to_i64(v, softfloat_round_minMag, 0);
 
  // Extract floating part
  fpart = f64_sub(v, i64_to_f64(ipart));

  if(ep && ipart>9){ /* fix rounding errors */
    ipart /= 10;
    ep++;
  }
 
  // convert integer part to string
  i += intToStr(ipart, str_+i, 1,obase);
 
  // check for display option after point
  if (afterpoint != 0){
    str_[i++] = '.';  // add dot

    if(i + afterpoint >= F64_STRLEN-1)afterpoint=F64_STRLEN-i;
 
    // Get the value of fraction part upto given no.
    // of points after dot. The third parameter is needed
    // to handle cases like 233.007
    pn = powbase(afterpoint,obase);
    fpart = f64_mul( fpart, i64_to_f64(pn) );
 
    i += intToStr(f64_to_i64(fpart, softfloat_round_min, 0), str_ + i, afterpoint,obase);
  }

  if(ep){
    str_[i++]='e';
    if(ep<0){
      str_[i++]='-';
      ep=-ep;
    }
    intToStr(ep, str_ + i, 0,obase);
  }
  
  return str_;
}

// Allow Arduino's Serial.print() to print f64 objects!
size_t f64::printTo(Print& p) const
{
  char *buf = toString(aft_point);
  p.write(buf);
}

f64 f64::setNaN(void)
{
  num_.v = defaultNaNF64UI;
  return *this;
}

bool f64::isNaN(void) const
{
  //return f64_isSignalingNaN(num_);
  //return softfloat_isSigNaNF64UI(num_.v);
  //return isNaNF64UI(num_.v);
  return fracF64UI(num_.v) && (expF64UI(num_.v)==0x7ff);
}

bool f64::isInf(void) const
{
  return !fracF64UI(num_.v) && (expF64UI(num_.v)==0x7ff);
}

bool f64::isNum(void) const
{
  return expF64UI(num_.v)!=0x7ff;
}

//#define ten bintof64(0x4024000000000004) //10
//#define ten i32_to_f64(10)
//#define large bintof64(0x4341c37937e08000) //1e0^16

/* minimal strtod - no validation checking */
f64 strtof64(const char *nptr, char **endptr)
{
  f64 result;
  float64_t d = {0};
  int16_t e=0;
  int8_t i=0,npos=0;
  i64 nlog=10;
  char c=nptr[0];
  int8_t k;
  bool stop = false;
  bool neg = false;
  int8_t nexp = 0;
  bool sexp = false;
  float64_t large;

  for(i=0;c&&!stop;i++){
    c=nptr[i];
    k=c<'a'?c-'0':c-'a'+10;
    switch(c){
    case 'a' ... 'f': //accept hex as well
    case '0' ... '9':
      if(nexp){ /* exponent */
	e *= ibase;
	e += k;
	nexp++;
      }
      else if(npos>=0){ /* before the point */
	d = f64_mul(d,i32_to_f64(ibase));
	d = f64_add(d,i32_to_f64(k));
	npos++;
      }
      else{ /* after the point */
	d = f64_add(d, f64_div(i32_to_f64(k),i64_to_f64(nlog)) );
	npos--;
	nlog*=ibase;
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
    case 'E':
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

  large=i64_to_f64(powbase(15,ibase)); //can't go to 16 in hex mode
  large=f64_mul(large,i32_to_f64(ibase)); //large=ibase^16
  if(sexp){ // multiply exponent in
    while(e>=16){ d=f64_div(d,large); e-=16;}
    while(e--) d=f64_div(d,i32_to_f64(ibase));
  }
  else{
    while(e>=16){ d=f64_mul(d,large);e-=16;}
    while(e--) d=f64_mul(d,i32_to_f64(ibase));
  }

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


