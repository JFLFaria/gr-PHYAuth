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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "freq_analysis_sink_impl.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

namespace gr {
namespace PHYAuth {

freq_analysis_sink::sptr freq_analysis_sink::make(int veclen,
		const char *filename, long int samp_rate, bool debug, bool save_peaks) {
	return gnuradio::get_initial_sptr(
			new freq_analysis_sink_impl(veclen, filename, samp_rate, debug,
					save_peaks));
}

/*
 * The private constructor
 */
freq_analysis_sink_impl::freq_analysis_sink_impl(int veclen,
		const char *filename, long int samp_rate, bool debug, bool save_peaks) :
		gr::sync_block("freq_analysis_sink",
				gr::io_signature::make(1, 1, sizeof(float) * veclen),
				gr::io_signature::make(0, 0, 0)) {
	freq_analysis_sink_impl::samp_rate = samp_rate;
	freq_analysis_sink_impl::veclen = veclen;

	this->debug = debug;
	this->save_peaks = save_peaks;

	fp = fopen(filename, "wb+");
}

/*
 * Our virtual destructor.
 */
freq_analysis_sink_impl::~freq_analysis_sink_impl() {
}

int freq_analysis_sink_impl::work(int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items) {

	int ninput = input_items.size() * veclen;
	float fft_vector[veclen];
	int vec_length = 0;
	//std::cout << "ninput: " << ninput << "\n";

	const float *in = (const float*) input_items[0];
	//<+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

	// Do <+signal processing+>
	for (int i = 0; i < ninput; ++i) {

		fft_vector[vec_length] = in[i];
		++vec_length;

		if (vec_length == veclen) {
			searchFFT(fft_vector);
			vec_length = 0;
		}
	}

	// Tell runtime system how many output items we produced.
	return noutput_items;
}

bool freq_analysis_sink_impl::stop() {

	fflush(fp);
	fclose(fp);

	return true;
}

void freq_analysis_sink_impl::searchFFT(float fft_vector[]) {

	//std::ofstream filedump;
	//filedump.open("/home/user/gr_dump/peakFrequencies.bin",
	//		std::ios::out | std::ios::app | std::ios::binary);

	if (fp != NULL) {
		bool notEmpty = false;

		for (int bin = 1; bin < veclen; ++bin) {
			if (fft_vector[bin] > -200.0) { //Search if vector has different values
				notEmpty = true;
				break;
			}
		}

		if (notEmpty) {
			//std::cout << "\nFound non empty FFT\n";
			//std::cout << "\nFreq 0Hz: " << fft_vector[2047];
			float rightPeakPower = -200;
			float rightPeakFrequency = findRightPeak(fft_vector,
					&rightPeakPower);

			float leftPeakPower = -200;
			float leftPeakFrequency = findLeftPeak(fft_vector, &leftPeakPower);
			if (rightPeakFrequency == -1) { //Value is not acceptable
				return;
			}

			if (debug) {

				std::cout << "Left Peak     Power: " << leftPeakPower
						<< "    \t\t" << "Right Peak     Power: "
						<< rightPeakPower << "\n";
				std::cout << "Left Peak Frequency: " << leftPeakFrequency / 1e3
						<< " kHz\t\t" << "Right Peak Frequency: "
						<< rightPeakFrequency / 1e3 << " kHz\n\n";

			}

			if (save_peaks) {
				fwrite(&leftPeakPower, sizeof(leftPeakPower), 1, fp);
				fwrite(&leftPeakFrequency, sizeof(leftPeakFrequency), 1, fp);
				fwrite(&rightPeakPower, sizeof(rightPeakPower), 1, fp);
				fwrite(&rightPeakFrequency, sizeof(rightPeakFrequency), 1, fp);
				fflush(fp);
			}

			notEmpty = false;
		}

	} else {
		std::cout << "\n[searchFFT] Unable to open file\n";
	}
}

float freq_analysis_sink_impl::findRightPeak(float fft_vector[],
		float *maxPower) {

	int index = 0;
	float max = -200;

	for (int i = veclen / 2; i < veclen; ++i) {

		if (fft_vector[i] > max) {
			index = i;
			max = fft_vector[i];
		}

	}

	*maxPower = max;

	/*
	 if( (fft_vector[index] - fft_vector[index + 16] < 5) or (index + 16 > veclen - 1)){ //If round fft or out of bounds
	 return -1;
	 }
	 */
	return (index - veclen / 2) * samp_rate / veclen;
}

float freq_analysis_sink_impl::findLeftPeak(float fft_vector[],
		float *maxPower) {

	int index = 0;
	float max = -200;

	for (int i = 0; i < veclen / 2; ++i) {

		if (fft_vector[i] > max) {
			index = i;
			max = fft_vector[i];
		}

	}

	*maxPower = max;

	/*
	 if( (fft_vector[index] - fft_vector[index + 16] < 5) or (index + 16 > veclen - 1)){ //If round fft or out of bounds
	 return -1;
	 }
	 */
	return (index - veclen / 2) * samp_rate / veclen;
}

} /* namespace PHYAuth */
} /* namespace gr */

