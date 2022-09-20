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
#include "preamble_cutter_impl.h"

namespace gr {
namespace PHYAuth {

preamble_cutter::sptr preamble_cutter::make(int preambleSeqSamples) {
	return gnuradio::get_initial_sptr(
			new preamble_cutter_impl(preambleSeqSamples));
}

/*
 * The private constructor
 */
preamble_cutter_impl::preamble_cutter_impl(int preambleSeqSamples) :
		gr::block("preamble_cutter",
				gr::io_signature::make(1, 1, sizeof(gr_complex)),
				gr::io_signature::make(1, 1, sizeof(gr_complex))) {
	this->preambleSeqSamples = preambleSeqSamples;
}

/*
 * Our virtual destructor.
 */
preamble_cutter_impl::~preamble_cutter_impl() {
}

void preamble_cutter_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required) {
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
	ninput_items_required[0] = noutput_items;
}

int preamble_cutter_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items) {


	const gr_complex *in = (const gr_complex*) input_items[0];
	gr_complex *out = (gr_complex*) output_items[0];

	int ninput = ninput_items[0];
	int produced = 0;

	// Do <+signal processing+>
	//std::cout << "[Preamble cutter] Ninput: " << ninput << "\n";
	for (int i = 0; i < ninput; ++i) {
		float real = std::real(in[i]);
		float imag = std::imag(in[i]);
		if (real != 0.0 || imag != 0.0) {

			havePreamble = true;

			if (nSamples < preambleSeqSamples)
				preamble.push_back(in[i]);

			++nSamples;
			//if (nSamples == preambleSeqSamples)
			//	break;

		} else if (havePreamble) {
			if (nSamples < preambleSeqSamples) {
				havePreamble = false;
				nSamples = 0;
				preamble.clear();
				continue;
			}
			++nPreambles;
			if (debug)
				std::cout << "[Preamble cutter] outputing " << preamble.size()
						<< " samples\n";
			for (int c = 0; c < preambleSeqSamples; ++c) {
				out[c] = preamble[c];
			}
			produced += preambleSeqSamples;
			havePreamble = false;
			nSamples = 0;
			preamble.clear();
			//break;
		}

	}

	// Tell runtime system how many input items we consumed on
	// each input stream.
	consume_each(ninput);

	// Tell runtime system how many output items we produced.
	return produced;
}

bool preamble_cutter_impl::stop() {
	if (debug)
		std::cout << "[Preamble Cutter] Cutted " << nPreambles << "\n";
	return true;
}

} /* namespace PHYAuth */
} /* namespace gr */

