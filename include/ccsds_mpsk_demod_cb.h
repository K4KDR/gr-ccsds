#ifndef INCLUDED_CCSDS_MPSK_DEMOD_CB_H
#define INCLUDED_CCSDS_MPSK_DEMOD_CB_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <gr_complex.h>

class ccsds_mpsk_demod_cb;

typedef boost::shared_ptr<ccsds_mpsk_demod_cb> ccsds_mpsk_demod_cb_sptr;

/*! \brief Create an M-PSK demodulator and return it's shared pointer.
 *
 *  \param M Modulation order of the M-PSK signal.
 *  \return Shared pointer of the created demodulator.
 */
CCSDS_API ccsds_mpsk_demod_cb_sptr ccsds_make_mpsk_demod_cb (unsigned int M);


/*! \brief M-PSK demodulator. 
 *  \ingroup demodulation
 *  \sa ccsds_mpsk_demod_cb::detect_bpsk_symbol
 *  \sa ccsds_mpsk_demod_cb::detect_qpsk_symbol
 *  \sa ccsds_mpsk_demod_cb::detect_mpsk_symbol
 *
 *  M-PSK demodulation based on nearest neighbour principle.
 *
 *  For M=2 and M=4 detection is done by algorithmic decision borders.
 *
 *  For all other modulation orders a constellation is generated and for every
 *  every sample the symbol with the minimum distance to the constellation
 *  symbol is chosen from the precomputed constellation.
 */
class CCSDS_API ccsds_mpsk_demod_cb : public gr_sync_block
{
private:
	friend CCSDS_API ccsds_mpsk_demod_cb_sptr ccsds_make_mpsk_demod_cb (unsigned int M);

	/*! \brief Private constructor of the M-PSK demodulator
	 *
	 *  \param M Modulation order of the M-PSK signal.
	 *  \sa ccsds_mpsk_demod2_cb
	 *  \sa ccsds_mpsk_demod_cb::detect_bpsk_symbol
	 *  \sa ccsds_mpsk_demod_cb::detect_qpsk_symbol
	 *  \sa ccsds_mpsk_demod_cb::detect_mpsk_symbol
	 *
	 *  Demodulate M-PSK modulated symbols back to data bits. This
	 *  demodulator	has an optimized detector for BPSK and QPSK. For other
	 *  modulation orders it created a constellation and compares this
	 *  constellation to the received sample to get a nearest enighbour
	 *  decision.
	 */
	ccsds_mpsk_demod_cb (unsigned int M);

	/*! \brief Modulation order. */
	unsigned int d_M;

	/*! \brief Pointer to the precomputed constellation */
	gr_complex* d_constellation;

	/*! \brief Optimized BPSK detector
	 *
	 *  \param symbol Symbol to demodulate.
	 *  \return Detected data bits.
	 *
	 *  BPSK detector based on the decision border on the Quadrature
	 *  component axis.
	 *
	 *  \code return (real(symbol) > 0) ? 0 : 1; \endcode 
	 */
	char detect_bpsk_symbol(gr_complex symbol);

	/*! \brief Optimized QPSK detector
	 *
	 *  \param symbol Symbol to demodulate.
	 *  \return Detected data bits.
	 *
	 *  QPSK detector based on the two angle bisectors as decision borders.
	 *
	 *  \code
	 *	if(imag(symbol)>real(symbol)) {
	 *		if(real(symbol)>-imag(symbol)) {
	 *			return 1;
	 *		} else {
	 *			return 3;
	 *		}
	 *	} else {
	 *		if(real(symbol)>-imag(symbol)) {
	 *			return 0;
	 *		} else {
	 *			return 2;
	 *		}
	 *	}
	 *  \endcode 
	 */
	char detect_qpsk_symbol(gr_complex symbol);

	/*! \brief Detect general M-PSK modulated symbol.
	 *
	 *  \param symbol Symbol to demodulate.
	 *
	 *  The constructor ccsds_mpsk_demod_cb() generates a M-PSK
	 *  constellation. For each sample the symbol with the minimum distance
	 *  to the constellation symbol is chosen from the precomputed
	 *  constellation.
	 */
	char detect_mpsk_symbol(gr_complex symbol);

	/*! \brief Function pointer to the used detector function, based on the
	 *	modulation order.
	 *
	 *  \sa detect_bpsk_symbol
	 *  \sa detect_qpsk_symbol
	 *  \sa detect_mpsk_symbol
	 */
	char (ccsds_mpsk_demod_cb:: *detect_symbol)(gr_complex symbol);

public:

	/*! \brief Public deconstructor. */
	~ccsds_mpsk_demod_cb ();  // public destructor

	int work (  int                         noutput_items,
                gr_vector_const_void_star   &input_items,
                gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_MPSK_DEMOD_CB_H */
