/* -*- c++ -*- */

#include "conv_encode27_aux.h"
#include <qa_conv_encode27_aux.h>
#include <cppunit/TestAssert.h>
#include <stdio.h>

namespace gr {
  namespace ccsds {

    ////
    ////////
    ////////////    Rate 1/2
    ////////
    ////

    // Send zeros
    void qa_conv_encode27_aux::test_zeroes_12() {
      const unsigned int len  = 5;

      const uint8_t  input[len]   = {0,0,0,0,0};

      const uint16_t expect[len]     = {0x5555,0x5555,0x5555,0x5555,0x5555};
      const uint8_t  num_expect[len] = {16,16,16,16,16};
	      
      uint16_t output[len];
      uint8_t  num_out[len];

      gr::ccsds::conv_encode27_aux encoder(0x79, 0xDB, conv_puncturing27::NONE);

      for(unsigned int i=0;i<len;i++) {
	encoder.encode_punct(output[i], num_out[i], input[i]);
	
	// check that all output bits match the input bits
	CPPUNIT_ASSERT_EQUAL(output[i], expect[i]);
	
	// check that the output length match the expected ones
	CPPUNIT_ASSERT_EQUAL(num_out[i], num_expect[i]);
      }
    }
	
    // Send ones
    void qa_conv_encode27_aux::test_ones_12() {
      const unsigned int len  = 5;

      const uint8_t  input[len]   = {0xFF,0xFF,0xFF,0xFF,0xFF};

      const uint16_t expect[len]     = {0x8C1A,0xAAAA,0xAAAA,0xAAAA,0xAAAA};
      const uint8_t  num_expect[len] = {16,16,16,16,16};
	      
      uint16_t output[len];
      uint8_t  num_out[len];

      conv_encode27_aux encoder(0x79, 0xDB, conv_puncturing27::NONE);
      
      for(unsigned int i=0;i<len;i++) {
	encoder.encode_punct(output[i], num_out[i], input[i]);

	// check that all output bits match the input bits
	CPPUNIT_ASSERT_EQUAL(output[i], expect[i]);
	
	// check that the output length match the expected ones
	CPPUNIT_ASSERT_EQUAL(num_out[i], num_expect[i]);
      }
    }

    // Send ASM 1ACFFC1D
    void qa_conv_encode27_aux::test_asm_12() {
      const unsigned int len  = 4;

      const uint8_t  input[len] = {0x1A,0xCF,0xFC,0x1D};

      const uint16_t expect[len]     = {0x5608,0x1C97,0x1AA7,0x3D3E};
      const uint8_t  num_expect[len] = {16,16,16,16};
	      
      uint16_t output[len];
      uint8_t  num_out[len];

      conv_encode27_aux encoder(0x79, 0xDB, conv_puncturing27::NONE);

      for(unsigned int i=0;i<len;i++) {
	encoder.encode_punct(output[i], num_out[i], input[i]);
      
	// check that all output bits match the input bits
	CPPUNIT_ASSERT_EQUAL(output[i], expect[i]);
	
	// check that the output length match the expected ones
	CPPUNIT_ASSERT_EQUAL(num_out[i], num_expect[i]);
      }
    }



    ////
    ////////
    ////////////    Rate 2/3
    ////////
    ////

    // Send ASM 1ACFFC1D
    void qa_conv_encode27_aux::test_asm_23() {
      const unsigned int len  = 4;

      const uint8_t  input[len] = {0x1A,0xCF,0xFC,0x1D};

      const uint16_t expect[len]     = {0x005F,0x0570,0x05F8,0x0515};
      const uint8_t  num_expect[len] = {12,12,12,12};
	      
      uint16_t output[len];
      uint8_t  num_out[len];

      conv_encode27_aux encoder(0x79, 0x5B, conv_puncturing27::ECSS_23);
      
      for(unsigned int i=0;i<len;i++) {
	encoder.encode_punct(output[i], num_out[i], input[i]);
      
	// check that all output bits match the input bits
	CPPUNIT_ASSERT_EQUAL(output[i], expect[i]);
	
	// check that the output length match the expected ones
	CPPUNIT_ASSERT_EQUAL(num_out[i], num_expect[i]);
      }
    }



    ////
    ////////
    ////////////    Rate 3/4
    ////////
    ////

    // Send ASM 1ACFFC1D
    void qa_conv_encode27_aux::test_asm_34() {
      const unsigned int len  = 4;

      const uint8_t  input[len] = {0x1A,0xCF,0xFC,0x1D};

      const uint16_t expect[len]     = {0x0077,0x0032,0x02FD,0x029D};
      const uint8_t  num_expect[len] = {11,11,10,11};
	      
      uint16_t output[len];
      uint8_t  num_out[len];

      conv_encode27_aux encoder(0x79, 0x5B, conv_puncturing27::ECSS_34);
      
      for(unsigned int i=0;i<len;i++) {
	encoder.encode_punct(output[i], num_out[i], input[i]);
      
	// check that all output bits match the input bits
	CPPUNIT_ASSERT_EQUAL(output[i], expect[i]);
	
	// check that the output length match the expected ones
	CPPUNIT_ASSERT_EQUAL(num_out[i], num_expect[i]);
      }
    }



    ////
    ////////
    ////////////    Rate 5/6
    ////////
    ////

    // Send ASM 1ACFFC1D
    void qa_conv_encode27_aux::test_asm_56() {
      const unsigned int len  = 4;

      const uint8_t  input[len] = {0x1A,0xCF,0xFC,0x1D};

      const uint16_t expect[len]     = {0x0026,0x02B2,0x017C,0x011D};
      const uint8_t  num_expect[len] = {10,10, 9,10};
	      
      uint16_t output[len];
      uint8_t  num_out[len];

      conv_encode27_aux encoder(0x79, 0x5B, conv_puncturing27::ECSS_56);
      
      for(unsigned int i=0;i<len;i++) {
	encoder.encode_punct(output[i], num_out[i], input[i]);
      
	// check that all output bits match the input bits
	CPPUNIT_ASSERT_EQUAL(output[i], expect[i]);
	
	// check that the output length match the expected ones
	CPPUNIT_ASSERT_EQUAL(num_out[i], num_expect[i]);
      }
    }



    ////
    ////////
    ////////////    Rate 7/8
    ////////
    ////

    // Send ASM 1ACFFC1D
    void qa_conv_encode27_aux::test_asm_78() {
      const unsigned int len  = 5;

      const uint8_t  input[len] = {0x1A,0xCF,0xFC,0x1D,0x00};

      const uint16_t expect[len]     = {0x002D,0x0110,0x017C,0x0189,0x00BC};
      const uint8_t  num_expect[len] = {10, 9, 9, 9, 9};
	      
      uint16_t output[len];
      uint8_t  num_out[len];

      conv_encode27_aux encoder(0x79, 0x5B, conv_puncturing27::ECSS_78);
      
      for(unsigned int i=0;i<len;i++) {
	encoder.encode_punct(output[i], num_out[i], input[i]);
      
	// check that all output bits match the input bits
	CPPUNIT_ASSERT_EQUAL(output[i], expect[i]);
	
	// check that the output length match the expected ones
	CPPUNIT_ASSERT_EQUAL(num_out[i], num_expect[i]);
      }
    }
    
  } /* namespace ccsds */
} /* namespace gr */
