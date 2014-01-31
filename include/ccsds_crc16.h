

#ifndef INCLUDED_GR_CRC16_H
#define INCLUDED_GR_CRC16_H

#include <ccsds_api.h>
#include <string>
#include <gr_types.h>

/*!
 * \brief update running CRC-16
 * \ingroup channelcoding
 *
 * Update a running CRC with the bytes buf[0..len-1] The CRC should be
 * initialized to all 1's, and the transmitted value is the 1's
 * complement of the final running CRC.  The resulting CRC should be
 * transmitted in big endian order.
 */
CCSDS_API unsigned int
ccsds_update_crc16(unsigned int crc, const unsigned char *buf, size_t len);

CCSDS_API unsigned int
ccsds_update_crc16(unsigned int crc, const std::string buf);

CCSDS_API unsigned int
ccsds_crc16(const unsigned char *buf, size_t len);

CCSDS_API unsigned int
ccsds_crc16(const std::string buf);

#endif /* INCLUDED_CRC16_H */

