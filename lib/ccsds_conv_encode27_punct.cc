#include <ccsds_conv_encode27_punct.h>
#include <stdio.h>


unsigned int ccsds_conv_encode27_punct::get_pattern_len(const punct_t punct) {
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

boost::shared_ptr<bool[]> ccsds_conv_encode27_punct::get_pattern(const punct_t punct) {
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
