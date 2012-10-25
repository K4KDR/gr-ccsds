#include <ccsds_api.h>
#include "fec.h"

class CCSDS_API ccsds_conv_encode
{
 private:
  uint32_t d_shift_register;
  uint32_t d_mask;
  uint32_t d_seed;
  uint32_t d_shift_register_length;	// less than 32
  int d_rate;
  int *d_polys;

  static uint32_t
  popCount(uint32_t x)
  {
    uint32_t r = x - ((x >> 1) & 033333333333)
                   - ((x >> 2) & 011111111111);
    return ((r + (r >> 3)) & 030707070707) % 63;
  }

 public:

  ccsds_conv_encode(uint32_t mask, uint32_t seed, uint32_t reg_len, int rate, int *polys)
    : d_shift_register(seed),
      d_mask(mask),
      d_seed(seed),
      d_shift_register_length(reg_len),
	  d_rate(rate),
	  d_polys(polys)
  {
    if (reg_len > 31)
      throw std::invalid_argument("reg_len must be <= 31");
  }

  // long int
  void 
  encode (
	unsigned char *syms, 
	int data, 
	int len) 
  {
	// sr = SYNC_WORD >> 25;
	// long int syms;
	for(int i = len - 1; i >= 0; i--){
		d_shift_register = (d_shift_register << 1) | ((data >> i) & 1);    
		for(int j = 0; j < d_rate; j++)
			syms[d_rate*i+j] = parityb(d_shift_register & d_polys[j]);
			// syms = (syms << 1) | (parityb(d_shift_register & d_polys[j]) & 1);
	}
	// return syms;
  }

  long int
  // void 
  encode (
	// unsigned char *syms, 
	int data, 
	int len) 
  {
	// sr = SYNC_WORD >> 25;
	long int syms;
	for(int i = len - 1; i >= 0; i--){
		d_shift_register = (d_shift_register << 1) | ((data >> i) & 1);    
		for(int j = 0; j < d_rate; j++)
			// syms[d_rate*i+j] = parityb(d_shift_register & d_polys[j]);
			syms = (syms << 1) | (parityb(d_shift_register & d_polys[j]) & 1);
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
