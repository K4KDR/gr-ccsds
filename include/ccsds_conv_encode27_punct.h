#ifndef CCSDS_CONV_ENCODE27_PUNCT_H
#define CCSDS_CONV_ENCODE27_PUNCT_H

#include <boost/shared_ptr.hpp>
#include <stdio.h>

namespace ccsds_conv_encode27_punct {


	/*! \brief Enum of supported puncturing schemes */
	enum punct_t {NONE=0, ECSS_23, ECSS_34, ECSS_56, ECSS_78};

	/*!
	 *  \brief Helper function to determine the length after which the puncturing
	 *	pattern repeats.
	 */
	inline unsigned int get_pattern_len(const punct_t punct) {
		switch(punct) {
										   // + = keep, - = drop, | = repeat
			case NONE:	return  1; // + |
			case ECSS_23:	return  4; // + + - + |
			case ECSS_34:	return  3; // + + - |
			case ECSS_56:	return 10; // + + - + + - - + + - |
			case ECSS_78:	return 14; // + + - + - + - + + - - + + - |
		}

		fprintf(stderr,"ERROR CONV ENCODE AUX: unknown puncturing type.\n");
		return 0;
	}

	/*!
	 *  \brief Helper function to fill the puncturing pattern item. Memory for the
	 *	number of elements returned by \c ccsds_conv_encode27_aux_get_punct_len
	 *	for the same puncturing type must be allocated for \c pattern.
	 */
	inline boost::shared_ptr<bool[]> get_pattern(const punct_t punct) {
		// allocate memory
		boost::shared_ptr<bool[]> pattern( new bool[get_pattern_len(punct)] );

		// fill memory
		switch(punct) {
							  // + = keep, - = drop, | = repeat
			case NONE: 	pattern[0]=true;  // + |
					break;
			case ECSS_23:	pattern[0]=true;  // + + - + |
					pattern[1]=true;
					pattern[2]=false;
					pattern[3]=true;
					break;
			case ECSS_34:	pattern[0]=true;  // + + - |
					pattern[1]=true;
					pattern[2]=false;
					break;
			case ECSS_56:	pattern[0]=true;  // + + - + + - - + + - |
					pattern[1]=true;
					pattern[2]=false;
					pattern[3]=true;
					pattern[4]=true;
					pattern[5]=false;
					pattern[6]=false;
					pattern[7]=true;
					pattern[8]=true;
					pattern[9]=false;
					break;
			case ECSS_78:	pattern[0]=true;  // + + - + - + - + + - - + + - |
					pattern[1]=true;
					pattern[2]=false;
					pattern[3]=true;
					pattern[4]=false;
					pattern[5]=true;
					pattern[6]=false;
					pattern[7]=true;
					pattern[8]=true;
					pattern[9]=false;
					pattern[10]=false;
					pattern[11]=true;
					pattern[12]=true;
					pattern[13]=false;
					break;
			default:	fprintf(stderr,"ERROR CONV ENCODE AUX: unknown puncturing type.\n");
		}

		return pattern;
	}

}

#endif /* CCSDS_CONV_ENCODE27_PUNCT_H */
