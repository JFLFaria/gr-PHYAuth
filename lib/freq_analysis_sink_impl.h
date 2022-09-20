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

#ifndef INCLUDED_PHYAUTH_FREQ_ANALYSIS_SINK_IMPL_H
#define INCLUDED_PHYAUTH_FREQ_ANALYSIS_SINK_IMPL_H

#include <PHYAuth/freq_analysis_sink.h>

namespace gr {
  namespace PHYAuth {

    class freq_analysis_sink_impl : public freq_analysis_sink
    {
     private:
      // Nothing to declare in this block.
    	FILE * fp;
    	int veclen;

    	bool debug;
    	bool save_peaks;
    	//const char * filename = "";
    	long int samp_rate;
     public:
      freq_analysis_sink_impl(int veclen, const char * filename, long int samp_rate, bool debug, bool save_peaks);
      ~freq_analysis_sink_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );

      float findRightPeak(float fft_vector[], float * maxPower);
      float findLeftPeak(float fft_vector[], float * maxPower);
      void searchFFT(float fft_vector[]);
      bool stop() override;
    };

  } // namespace PHYAuth
} // namespace gr

#endif /* INCLUDED_PHYAUTH_FREQ_ANALYSIS_SINK_IMPL_H */

