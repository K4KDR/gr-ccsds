/* -*- c++ -*- */
/* 
 * Copyright 2020 Martin Luelf.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_CCSDS_EXCEPTIONS_H
#define INCLUDED_CCSDS_EXCEPTIONS_H

#include <ccsds/api.h>

#include <exception>
#include <string>

namespace gr {
  namespace ccsds {
    namespace exceptions {

    /*!
     * \brief Base class for all exceptions from the ccsds module.
     * 
     * \ingroup ccsds
     */
    class CCSDS_API BaseException : public std::exception {
    protected:
      const char *msg_;

    public:
      BaseException(const char *msg) : msg_(msg) {}

      const char * what () const throw () {
        return msg_;
      }
    };


    /*!
     * \brief Exception that is raised if a planned feature has been
     * requested that is not implemented yet.
     * 
     * \ingroup ccsds
     */
    class CCSDS_API NotImplemented : public BaseException {
      public:
      NotImplemented(const char *msg) : BaseException(msg) {};
    };

    /*!
     * \brief Exception that is raised if a parameter has an unexpected value.
     * 
     * \ingroup ccsds
     */
    class CCSDS_API InvalidParameter : public BaseException {
      public:
      InvalidParameter(const char *msg) : BaseException(msg) {};
    };

    /*!
     * \brief Exception that is raised if an information is requested that is not available
     * 
     * \ingroup ccsds
     */
    class CCSDS_API NotAvailable : public BaseException {
      public:
      NotAvailable(const char *msg) : BaseException(msg) {};
    };

    /*!
     * \brief Exception that is raised if an element is requested that is out of bounds
     * 
     * \ingroup ccsds
     */
    class CCSDS_API OutOfBounds : public NotAvailable {
      public:
      OutOfBounds(const char *msg) : NotAvailable(msg) {};
    };

    /*!
     * \brief Exception that is raised if the frame_sync_buffer is advanced before the buffer is full
     * 
     * \ingroup ccsds
     */
    class CCSDS_API BufferAdvancedBeforeFull : public BaseException {
      public:
      BufferAdvancedBeforeFull(const char *msg) : BaseException(msg) {};
    };

    /*!
     * \brief Exception that is raised if a symbol based frame
     * synchronization is not possible.
     * 
     * Symbol based frame synchronization requires, that sync marker,
     * payload and entire block length are an integer multiple of the
     * symbol length.
     * 
     * \ingroup ccsds
     */
    class CCSDS_API SymbolSyncNotPossible : public InvalidParameter {
      public:
      SymbolSyncNotPossible(const char *msg) : InvalidParameter(msg) {};
    };

    /*!
     * \brief Exception that is raised if a constellation with a dimensionality
     * of greater than one is provided in a place where a one-dimensional
     * contellation is expected.
     * 
     * \ingroup ccsds
     */
    class CCSDS_API ConstellationOneDimRequired : public InvalidParameter {
      public:
      ConstellationOneDimRequired(const char *msg) : InvalidParameter(msg) {};
    };

    }  // namespace exceptions
  }  // namespace ccsds
}  // namespace gr

#endif /* INCLUDED_CCSDS_FRAME_EXCEPTIONS_H */

