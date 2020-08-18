#ifndef INCLUDED_CCSDS_HEXSTRING_TO_BINARY_H
#define INCLUDED_CCSDS_HEXSTRING_TO_BINARY_H

#include <cstdio>
#include <vector>
#include <string>

namespace gr {
  namespace ccsds {


/*! \brief Converts hexadecimal value into decimal one.
 *
 *  \param hex Hexadecimal value to convert. Must be in range from '0'-'9',
 *	'A'-'F', or 'a'-'f'
 *  \return Decimal value in range 0-15
 */
inline unsigned char hex2bin(char hex) {
	switch(hex) {
		case '0': return  0;
		case '1': return  1;
		case '2': return  2;
		case '3': return  3;
		case '4': return  4;
		case '5': return  5;
		case '6': return  6;
		case '7': return  7;
		case '8': return  8;
		case '9': return  9;
		case 'A': return 10;
		case 'B': return 11;
		case 'C': return 12;
		case 'D': return 13;
		case 'E': return 14;
		case 'F': return 15;
		case 'a': return 10;
		case 'b': return 11;
		case 'c': return 12;
		case 'd': return 13;
		case 'e': return 14;
		case 'f': return 15;
		default:
			fprintf(stderr,"ERROR: '%c' not a valid hexadecimal value.\n", hex);
			return 0;
	}
}


/*! \brief Converts a hexadecimal string to an array of unsigned chars (bytes).
 *
 *  \param str String to convert. Must be of even length as two hexadecimal
 *	symbols describe one byte.
 *  \param bin Array of bytes where the binary representation should be stored.
 *	Memory for at least \c str.length()/2 elements must be aligned.
 */
inline void hexstring_to_binary(const std::string *str, unsigned char *bin) {

	// convert string into char array
	const char *const c_str = str->c_str();

	// ensure even input length
	if(str->length()%2 != 0) {
		fprintf(stderr,"ERROR: String '%s' is not of even length.\n",c_str);
		return;
	}

	// go through all output bytes
	for(unsigned int i=0;i<str->length()/2;i++) {
		//       first hex value shifted by 4   second hex value (unshifted)
		bin[i] =  static_cast<unsigned char>(hex2bin(c_str[2*i]) << 4)  |    hex2bin(c_str[2*i+1]);
	}

	return;
}

namespace byteutils {
	template <typename T> inline std::vector<T> unpack_bytes(const std::vector<T> &bytes, size_t num_bits_per_symbol, size_t num_symbols_total) {
		size_t num_bits_read = 0u;
		size_t num_bits_collected = 0u;
		size_t num_symbols_collected = 0u;
		std::vector<T> unpacked_bytes(num_symbols_total);
		T tmp = 0u;
		for (uint8_t byte : bytes) {
			num_bits_read = 0u;
			
			while(num_bits_read <= 7u) {
				// read bit
				const T bit = (byte>>(7-num_bits_read++)) & 0x01;
				tmp = static_cast<T>(tmp << 1) | bit;
				num_bits_collected++;
				
				if (num_bits_collected >= num_bits_per_symbol) {
					unpacked_bytes[num_symbols_collected++] = tmp;
					tmp = 0u;
					num_bits_collected = 0u;

					if (num_symbols_collected == num_symbols_total) {
						return unpacked_bytes;
					}
				}
			}
		}

		assert(false && "Logic error, should not reach this.");
		return {};
    }

	inline std::vector<uint8_t> decode_hex(const std::string &hex) {
		assert(hex.length() % 2u == 0u);
		
		const char *const c_str = hex.c_str();
		const size_t num_bytes = hex.length()/2u;
		std::vector<uint8_t> bytes(num_bytes);
		for (size_t i=0u; i<num_bytes; i++) {
			bytes[i] = static_cast<unsigned char>(hex2bin(c_str[2u*i]) << 4) | hex2bin(c_str[2u*i+1u]);
		}
		return bytes;
	}

	inline std::vector<uint8_t> decode_hex_to_bits(const std::string &hex, size_t num_bits_per_symbol, size_t num_symbols) {
		std::vector<uint8_t> bytes = decode_hex(hex);

		assert(bytes.size()*8 >= num_symbols*num_bits_per_symbol);
		return unpack_bytes<uint8_t>(bytes, num_bits_per_symbol, num_symbols);
	}
}
	

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_HEXSTRING_TO_BINARY_H */
