/* -*- c++ -*- */

#include <boost/test/unit_test.hpp>
#include <ccsds_asm_operator.h>
#include <stdio.h>


//////////////////////////////////////////////////////////
//  BPSK
//////////////////////////////////////////////////////////


BOOST_AUTO_TEST_CASE(qa_ccsds_asm_operator_check_bpsk){
	const unsigned char ASM[]   = {0x1A,0xCF,0xFC,0x1D};
	const unsigned int  ASM_LEN = 4;

	// BPSK, byte_offset=3 (011), bit_offset=0
	const unsigned char input[]  = {0,1,1,0,0,0,1,1,0,1,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,0,1,0,0};

	ccsds_asm_operator oper(ASM, ASM_LEN, 0, 1);

	// check for byte offset 0 (FALSE)
	BOOST_CHECK( oper.check_for_asm(&input[0], 0) == false );

	// check for byte offset 1 (FALSE)
	BOOST_CHECK( oper.check_for_asm(&input[1], 0) == false );

	// check for byte offset 2 (FALSE)
	BOOST_CHECK( oper.check_for_asm(&input[2], 0) == false );

	// check for byte offset 3 (TRUE)
	BOOST_CHECK( oper.check_for_asm(&input[3], 0) == true );
}

BOOST_AUTO_TEST_CASE(qa_ccsds_asm_operator_search_bpsk_right){
	const unsigned char ASM[]   = {0x1A,0xCF,0xFC,0x1D};
	const unsigned int  ASM_LEN = 4;

	// BPSK, byte_offset=3 (011), bit_offset=0
	const unsigned char input[]  = {0,1,1,0,0,0,1,1,0,1,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,0,1,0,0};

	ccsds_asm_operator oper(ASM, ASM_LEN, 0, 1);

	unsigned int byte_offset;
	unsigned int bit_offset;

	bool found = oper.search_asm(input, 37, &byte_offset, &bit_offset);

	// check for match
	BOOST_CHECK( found == true );

	// check for byte offset 1
	BOOST_CHECK( byte_offset == 3 );

	// check for bit offset 1
	BOOST_CHECK( bit_offset == 0 );

}


BOOST_AUTO_TEST_CASE(qa_ccsds_asm_operator_search_bpsk_false1){
	const unsigned char ASM[]   = {0x1A,0xCF,0xFC,0x1D};
	const unsigned int  ASM_LEN = 4;

	// BPSK, byte_offset=3 (011), bit_offset=0,  one bit error
	const unsigned char input[]  = {0,1,1,0,0,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,0,1,0,0};

	ccsds_asm_operator oper(ASM, ASM_LEN, 0, 1);

	unsigned int byte_offset;
	unsigned int bit_offset;

	bool found = oper.search_asm(input, 37, &byte_offset, &bit_offset);

	// check for match (FALSE)
	BOOST_CHECK( found == false );

}

BOOST_AUTO_TEST_CASE(qa_ccsds_asm_operator_search_bpsk_false2){
	const unsigned char ASM[]   = {0x1A,0xCF,0xFC,0x1D};
	const unsigned int  ASM_LEN = 4;

	// BPSK, byte_offset=3 (011), bit_offset=0,  one bit error
	const unsigned char input[]  = {0,1,1,0,0,0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,0,1,0,0};

	// allow for one bit error
	ccsds_asm_operator oper(ASM, ASM_LEN, 1, 1);

	unsigned int byte_offset;
	unsigned int bit_offset;

	bool found = oper.search_asm(input, 37, &byte_offset, &bit_offset);

	// check for match (TRUE)
	BOOST_CHECK( found == true );

	// check for byte offset 1
	BOOST_CHECK( byte_offset == 3 );

	// check for bit offset 1
	BOOST_CHECK( bit_offset == 0 );

}

//////////////////////////////////////////////////////////
//  8-PSK
//////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(qa_ccsds_asm_operator_check_mpsk){
	const unsigned char ASM[]   = {0x1A,0xCF,0xFC,0x1D};
	const unsigned int  ASM_LEN = 4;

	// MPSK, byte_offset=1 (011), bit_offset=1 (0)
	const unsigned char input[]  = {3,0,3,2,6,3,7,7,6,0,3,5,0,0};

	ccsds_asm_operator oper(ASM, ASM_LEN, 0, 3);

	// check for byte offset 0 bit_offset 0 (FALSE)
	BOOST_CHECK( oper.check_for_asm(&input[0], 0) == false );

	// check for byte offset 1 bit_offset 2 (FALSE)
	BOOST_CHECK( oper.check_for_asm(&input[1], 2) == false );

	// check for byte offset 1 bit_offset 1 (TRUE)
	BOOST_CHECK( oper.check_for_asm(&input[1], 1) == true );

	// check for byte offset 1 bit_offset 0 (FALSE)
	BOOST_CHECK( oper.check_for_asm(&input[1], 0) == false );

	// check for byte offset 2 bit_offset 1 (FALSE)
	BOOST_CHECK( oper.check_for_asm(&input[2], 1) == false );
}

BOOST_AUTO_TEST_CASE(qa_ccsds_asm_operator_search_mpsk_right){
	const unsigned char ASM[]   = {0x1A,0xCF,0xFC,0x1D};
	const unsigned int  ASM_LEN = 4;

	// MPSK, byte_offset=1 (011), bit_offset=1 (0)
	const unsigned char input[]  = {3,0,3,2,6,3,7,7,6,0,3,5,0,0};

	ccsds_asm_operator oper(ASM, ASM_LEN, 0, 3);

	unsigned int byte_offset;
	unsigned int bit_offset;

	bool found = oper.search_asm(input, 14, &byte_offset, &bit_offset);

	// check for match
	BOOST_CHECK( found == true );

	// check for byte offset 1
	BOOST_CHECK( byte_offset == 1 );

	// check for bit offset 1
	BOOST_CHECK( bit_offset == 1 );

}


BOOST_AUTO_TEST_CASE(qa_ccsds_asm_operator_search_mpsk_false1){
	const unsigned char ASM[]   = {0x1A,0xCF,0xFC,0x1D};
	const unsigned int  ASM_LEN = 4;

	// MPSK, byte_offset=1 (011), bit_offset=1 (0)  one bit error in ASM
	const unsigned char input[]  = {3,0,3,2,6,3,7,7,6,1,3,5,0,0};

	ccsds_asm_operator oper(ASM, ASM_LEN, 0, 3);

	unsigned int byte_offset;
	unsigned int bit_offset;

	bool found = oper.search_asm(input, 14, &byte_offset, &bit_offset);

	// check for match (FALSE)
	BOOST_CHECK( found == false );

}

BOOST_AUTO_TEST_CASE(qa_ccsds_asm_operator_search_mpsk_false2){
	const unsigned char ASM[]   = {0x1A,0xCF,0xFC,0x1D};
	const unsigned int  ASM_LEN = 4;

	// MPSK, byte_offset=1 (011), bit_offset=1 (0)  one bit error in ASM
	const unsigned char input[]  = {3,0,3,2,6,3,7,7,6,1,3,5,0,0};

	// allow for one bit error
	ccsds_asm_operator oper(ASM, ASM_LEN, 1, 3);

	unsigned int byte_offset;
	unsigned int bit_offset;

	bool found = oper.search_asm(input, 14, &byte_offset, &bit_offset);

	// check for match (TRUE)
	BOOST_CHECK( found == true );

	// check for byte offset 1
	BOOST_CHECK( byte_offset == 1 );

	// check for bit offset 1
	BOOST_CHECK( bit_offset == 1 );

}

