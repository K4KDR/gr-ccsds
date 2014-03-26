#ifndef INCLUDED_CCSDS_MPSK_CONST_H
#define INCLUDED_CCSDS_MPSK_CONST_H

#include <complex>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

namespace gr {
  namespace ccsds {

    /*! \brief Converts binary number into corresponding gray coded number.
     *
     *  \param binary Binary coded number to transform into a gray coded number.
     *  \return Gray coded number
     *
     *  Code from  http://en.wikipedia.org/wiki/Grey_code#Converting_to_and_from_Gray_code
     */
    inline unsigned int togray(const unsigned int binary) {
    	return (binary >> 1) ^ binary;
    }
    
    /*! \brief Converts gray coded number into corresponding binary number.
     *
     *  \param num Gray coded number to transform into a binary number.
     *  \return Binary coded number
     *
     *  Code from  http://en.wikipedia.org/wiki/Grey_code#Converting_to_and_from_Gray_code
     */
    inline unsigned int tobinary(unsigned int num) {
        unsigned int numBits = 8 * sizeof(num);
        unsigned int shift;
        for (shift = 1; shift < numBits; shift = 2 * shift)
        {
            num = num ^ (num >> shift);
        }
        return num;
    }
    
    /*! \brief Create general M-PSK constellation.
     *
     *  \param M Modulation order.
     *  \return Pointer to array of \c M constellation elements,
     *	allocated with new. This array must be deallocated with delete[] after
     *	usage.
     *
     *  Gray coded M-PSK constellation starting with zero symbol on the real axis
     *  and rotating counterclockwise afterwards.
     */
    inline gr_complex* make_psk_const(unsigned int M) {
    	// create phasor for rotation between the symbols
    	const gr_complex diff_phasor = gr_complex(cos(2*M_PI/M),sin(2*M_PI/M));
    
    	// initial phasor is on the real axis
    	gr_complex phasor = gr_complex(1.0f,0.0f);
    	// make sure we only have abs(.)=1 phasors
    	phasor = phasor / std::abs(phasor);
    
    	// allocate memory for the constellation points
    	gr_complex *map = new gr_complex[M];
    
    	// go through all constellation points
    	for(unsigned int i=0;i<M;i++) {
    		unsigned int i_gray = togray(i);		
    
    		// check if the next access is valid (not true for e.g M=3)
    		if(i_gray >= M) {
    			fprintf(stderr,"ERROR: modulation order M=%d not supported. M needs to be a power of two.\n",M);
    			exit(EXIT_FAILURE);
    			return NULL;
    		}		
    		// store constellation point
    		map[i_gray] = phasor;
    		// rotate to next point
    		phasor *= diff_phasor;
    	}
    
    	return map;
    }
    
    
    /*! \brief Create QPSK constellation according to ECSS-E-ST-50-05C.
     *
     *  \return Pointer to array of 4 constellation elements,
     *	allocated with new. This array must be deallocated with delete[] after
     *	usage.
     *
     *  In contrast to the general M-PSK constellation above, ECSS QPSK mapping is
     *  in reverse direction and rotated by 45 degree, so this constellation is
     *  created manually.
     *
     *  According to ECSS-E-ST-50-05C Rev.2 from 4th October 2011, Figure 6-3
     */
    inline gr_complex* make_qpsk_const(void) {
    	// allocate meory
    	gr_complex *map = new gr_complex[4];
    
    	// scale factor to normalize constellation
    	const float scale = 1.0/sqrt(2.0);
    
    	map[0] = gr_complex( 1.0f, 1.0f) * scale;
    	map[1] = gr_complex( 1.0f,-1.0f) * scale;
    	map[2] = gr_complex(-1.0f, 1.0f) * scale;
    	map[3] = gr_complex(-1.0f,-1.0f) * scale;
    	
    	return map;
    }
    
    
    /*! \brief Convenience wrapper to generate a ECSS compatible M-PSK constellation.
     *
     *  \param M Modulation order.
     *  \return Pointer to array of \c M constellation elements,
     *	allocated with new. This array must be deallocated with delete[] after
     *	usage.
     *  \sa make_psk_cons
     *  \sa make_qpsk_const
     *
     *  Create a M-PSK constellation. For M=4 i.e. QPSK \ref make_qpsk_const,
     *  otherwise \ref make_psk_const is called
     */
    inline gr_complex* make_mpsk_const(unsigned int M) {
    	//printf("\n++ Creating constelation for %u-PSK++\n",M);
    	if(M == 4) {
    		return make_qpsk_const();
    	} else {
    		return make_psk_const(M);
    	}
    }
    
    
    /*! \brief Create general M-PSK mapping.
     *
     *  \param M Modulation order.
     *  \return Pointer to array of \c M constellation bits, allocated with new.
     *	This array must be deallocated with delete[] after usage.
     *
     *  M-PSK constellation mapping with binary coded index and gray coded output.
     */
    inline unsigned char* make_psk_map(unsigned int M) {
    	// allocate memory for the constellation mapping
    	unsigned char *map = new unsigned char[M];
    
    	// go through all constellation points
    	for(unsigned int i=0;i<M;i++) {
    		unsigned int i_gray = togray(i);		
    
    		// check if the next access is valid (not true for e.g M=3)
    		if(i_gray >= M) {
    			fprintf(stderr,"ERROR: modulation order M=%d not supported. M needs to be a power of two.\n",M);
    			exit(EXIT_FAILURE);
    			return NULL;
    		}		
    		// store constellation mapping
    		map[i] = (unsigned char)i_gray;
    	}
    
    	return map;
    }
    
    
    /*! \brief Create QPSK constellation mapping according to ECSS-E-ST-50-05C.
     *
     *  \return Pointer to array of 4 elements, mapping binary indices to gray coded
     *	bits. This array is allocated with new and must be deallocated with
     *	delete[] after usage.
     *
     *  In contrast to the general M-PSK mapping above, ECSS QPSK mapping is in
     *  reverse direction and rotated by 45 degree, so this constellation is created
     *  manually.
     *
     *  According to ECSS-E-ST-50-05C Rev.2 from 4th October 2011, Figure 6-3
     */
    inline unsigned char* make_qpsk_map(void) {
    	// allocate meory
    	unsigned char *map = new unsigned char[4];
    
    	map[0] = 0;
    	map[1] = 2;
    	map[2] = 3;
    	map[3] = 1;
    	
    	return map;
    }
    
    
    /*! \brief Convenience wrapper to generate a ECSS compatible M-PSK constellation
     *	mapping.
     *
     *  \param M Modulation order.
     *  \return Pointer to array of \c M elements, mapping binary constellation
     *	indices to gray coded bits. This array is allocated with new and must be
     *	deallocated with delete[] after usage.
     *
     *  \sa make_psk_cons
     *  \sa make_qpsk_const
     *
     *  Create a M-PSK constellation mapping. For M=4 i.e. QPSK
     *  \ref make_qpsk_map, otherwise \ref make_psk_map is called
     */
    inline unsigned char* make_mpsk_map(unsigned int M) {
    	if(M == 4) {
    		return make_qpsk_map();
    	} else {
    		return make_psk_map(M);
    	}
    }
    
    
    /*! \brief Create general reverse M-PSK mapping.
     *
     *  \param M Modulation order.
     *  \return Pointer to array of \c M constellation bits, allocated with new.
     *	This array must be deallocated with delete[] after usage.
     *
     *  Reverse M-PSK constellation mapping with gray coded index and binary coded
     *  output.
     */
    inline unsigned char* make_psk_rev_map(unsigned int M) {
    	// allocate memory for the constellation mapping
    	unsigned char *map = new unsigned char[M];
    
    	// go through all constellation points
    	for(unsigned int i=0;i<M;i++) {
    		unsigned int i_bin = tobinary(i);		
    
    		// check if the next access is valid (not true for e.g M=3)
    		if(i_bin >= M) {
    			fprintf(stderr,"ERROR: modulation order M=%d not supported. M needs to be a power of two.\n",M);
    			exit(EXIT_FAILURE);
    			return NULL;
    		}		
    		// store constellation mapping
    		map[i] = (unsigned char)i_bin;
    	}
    
    	return map;
    }
    
    
    /*! \brief Create reverse QPSK constellation mapping according to ECSS-E-ST-50-05C.
     *
     *  \return Pointer to array of 4 elements, mapping gray coded indices to binary
     *	coded bits. This array is allocated with new and must be deallocated
     *	with delete[] after usage.
     *
     *  In contrast to the general reverse M-PSK mapping above, ECSS QPSK mapping is
     *  in reverse direction and rotated by 45 degree, so this mapping is created
     *  manually.
     *
     *  According to ECSS-E-ST-50-05C Rev.2 from 4th October 2011, Figure 6-3
     */
    inline unsigned char* make_qpsk_rev_map(void) {
    	// allocate meory
    	unsigned char *map = new unsigned char[4];
    
    	map[0] = 0;
    	map[1] = 3;
    	map[2] = 1;
    	map[3] = 2;
    	
    	return map;
    }
    
    
    /*! \brief Convenience wrapper to generate a ECSS compatible reverse M-PSK
     *	constellation mapping.
     *
     *  \param M Modulation order.
     *  \return Pointer to array of \c M elements, mapping cray coded bits to binary
     *	coded constellation indices. This array is allocated with new and must
     *	be deallocated with delete[] after usage.
     *
     *  \sa make_psk_cons
     *  \sa make_qpsk_const
     *
     *  Create a reverse M-PSK constellation mapping. For M=4 i.e. QPSK
     *  \ref make_qpsk_rev_map, otherwise \ref make_psk_rev_map is called
     */
    inline unsigned char* make_mpsk_rev_map(unsigned int M) {
    	if(M == 4) {
    		return make_qpsk_rev_map();
    	} else {
    		return make_psk_rev_map(M);
    	}
    }

  } //namespace ccsds
} // namepsace gr

#endif /* INCLUDED_CCSDS_MPSK_CONST_H */
