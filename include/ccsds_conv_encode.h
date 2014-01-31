#include <stdio.h>
#include <ccsds_api.h>
#include "fec.h"
// #define PY_ARRAY_UNIQUE_SYMBOL aaa

/*! \brief Helper class for convolutional encoding
 *
 *  \ingroup channelcoding
 */
class CCSDS_API ccsds_conv_encode
{
 private:
  int d_shift_register;
  int d_seed;
  int d_shift_register_length;	// less than 32
  int d_rate;
  int d_polys[];

  static uint32_t
  popCount(uint32_t x)
  {
    uint32_t r = x - ((x >> 1) & 033333333333)
                   - ((x >> 2) & 011111111111);
    return ((r + (r >> 3)) & 030707070707) % 63;
  }

 public:

  ccsds_conv_encode(int seed, int reg_len) //, int *polys, int rate)
    : d_shift_register(seed),
      d_seed(seed),
      d_shift_register_length(reg_len)
	  // d_rate(rate),
	  // d_polys(polys)
  {
    if (reg_len > 31)
      throw std::invalid_argument("reg_len must be <= 31");
	d_rate = 2;
	// d_polys[0] = 79;
	// d_polys[1] = 109;
  }

  // long int
  void 
  encode (
	unsigned char *syms, 
	int data, 
	int len,
	int POLYA,
	int POLYB
	) 
  {
	// sr = SYNC_WORD >> 25;
	d_polys[0] = POLYA;
	d_polys[1] = POLYB;
	// long int syms;
	for(int i = len - 1; i >= 0; i--){
		d_shift_register = (d_shift_register << 1) | ((data >> i) & 1);    
		for(int j = 0; j < d_rate; j++)
			syms[d_rate*i+j] = parityb(d_shift_register & d_polys[j]);
			// syms = (syms << 1) | (parityb(d_shift_register & d_polys[j]) & 1);
	}
	// return syms;
  }

  long long int
  // void 
  encode (
	// unsigned char *syms, 
	int data, 
	int len,
	int POLYA,
	int POLYB
	) 
  {
	// sr = SYNC_WORD >> 25;
	d_polys[0] = POLYA & 127;
	d_polys[1] = POLYB & 127;
	long long int syms;
	// printf("POLYA: %#x \nPOLYB: %#x\n",d_polys[0],d_polys[1]);
	for(int i = len - 1; i >= 0; i--){
		d_shift_register = (d_shift_register << 1) | ((data >> i) & 1);    
		// printf("shift_register: %#x \n",d_shift_register);
		syms = (syms << 1) | (parityb(d_shift_register & d_polys[0]) & 1);
		syms = (syms << 1) | !(parityb(d_shift_register & d_polys[1]) & 1);
	}
	return syms;
  }

  // work in progress
  // std::string
  // encode (
	// const std::string &data,
	// int len,
	// ) 
  // {
	// sr = SYNC_WORD >> 25;
	// std::string syms;
	// len ||= data.length();
	// for(int i = len - 1; i >= 0; i--){
		// d_shift_register = (d_shift_register << 1) | ((data >> i) & 1);    
		// for(int j = 0; j < d_rate; j++)
			// syms[d_rate*i+j] = parityb(d_shift_register & d_polys[j]);
			// syms = (syms << 1) | (parityb(d_shift_register & d_polys[j]) & 1);
	// }
	// return syms;
  // }
};
