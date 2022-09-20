/* -*- c++ -*- */
/*
 * Copyright 2022 João Faria.
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

#ifndef INCLUDED_PHYAUTH_CFO_ESTIMATES_H
#define INCLUDED_PHYAUTH_CFO_ESTIMATES_H

#include <PHYAuth/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace PHYAuth {

    /*!
     * \brief <+description of block+>
     * \ingroup PHYAuth
     *
     */
    class PHYAUTH_API CFO_estimates : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<CFO_estimates> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of PHYAuth::CFO_estimates.
       *
       * To avoid accidental use of raw pointers, PHYAuth::CFO_estimates's
       * constructor is in a private implementation
       * class. PHYAuth::CFO_estimates::make is the public interface for
       * creating new instances.
       */
      static sptr make(const char * filename, bool debug, bool save_estimates);
    };

  } // namespace PHYAuth
} // namespace gr

#endif /* INCLUDED_PHYAUTH_CFO_ESTIMATES_H */

