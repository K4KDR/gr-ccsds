#ifndef INCLUDED_CCSDS_INTERLEAVING_AUX_H
#define INCLUDED_CCSDS_INTERLEAVING_AUX_H

inline const unsigned int ccsds_interl_indx(const unsigned int index, unsigned int interleaving_depth, const unsigned int num_elements) {

	// maximum index we may address
	const unsigned int max_index = num_elements-1;

	// interleave all indices but the last one (which will also be the last
	// one in the new order
	if(index == max_index) {
		// last index
		return index;
	} else {
		// not the last index => interleave
		return ( (index*interleaving_depth) % max_index );
	}
} 

#endif /* INCLUDED_CCSDS_INTERLEAVING_AUX_H */
