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
#include "CFO_estimates_impl.h"
#include <numeric>

namespace gr {
namespace PHYAuth {

CFO_estimates::sptr CFO_estimates::make(const char *filename, bool debug, bool save_estimates) {
	return gnuradio::get_initial_sptr(new CFO_estimates_impl(filename, debug, save_estimates));
}

/*
 * The private constructor
 */
CFO_estimates_impl::CFO_estimates_impl(const char *filename, bool debug, bool save_estimates) :
		gr::block("CFO_estimates",
				gr::io_signature::make(1, 1, sizeof(gr_complex)),
				gr::io_signature::make(0, 0, 0)) {

	this->debug = debug;
	this->save_estimates = save_estimates;

	fp = fopen(filename, "wb+");
}

/*
 * Our virtual destructor.
 */
CFO_estimates_impl::~CFO_estimates_impl() {
}

void CFO_estimates_impl::forecast(int noutput_items,
		gr_vector_int &ninput_items_required) {
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
	ninput_items_required[0] = noutput_items;
}

int CFO_estimates_impl::general_work(int noutput_items,
		gr_vector_int &ninput_items, gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items) {
	const gr_complex *in = (const gr_complex*) input_items[0];
	//<+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

	int ninput = ninput_items[0];

	float CFO_estimate = 0;

	// Do <+signal processing+>

	for (int i = 0; i < ninput; ++i) {
		float real = std::real(in[i]);
		float imag = std::imag(in[i]);
		if (real != 0.0 || imag != 0.0) {
			havePreamble = true;
			++nSamples;
			preamble.push_back(in[i]);
		} else if (havePreamble) {
			//CFO = estim_CFO(/*preamble*/) / 1e3;

			float pi = 3.14159265359;
			float fs = 4e6;
			float ts = 1 / fs;
			int N = 64;
			int I = 2;

			//estCFO = ef + v*delta. (ef is fractional part and v is an Integer)
			float delta = 62.5e3;

			std::vector<int> vset;
			for (int i = -I; i < I + 1; ++i) {
				vset.push_back(i);
			}

			// Fractional Part

			//std::vector < gr_complex > x = std::vector < gr_complex
			//		> (preamble.end() - 5 * N + 1, preamble.end());

			//x1 = preamble[n1 - 7*N:n1 - 2*N];

			std::vector < gr_complex > x1 = std::vector < gr_complex
					> (preamble.end() - 7 * N, preamble.end() - 2 * N);

			std::vector < gr_complex > x2 = std::vector < gr_complex
					> (preamble.end() - 8 * N, preamble.end() - 3 * N);

			for (auto &x : x2)
				x = std::conj(x);

			//int n0 = x.size();

			//The transpose of 1D is the same vector
			//float ef = (std::arg(x[n0 - 2 * N] * x_conj[n0 - 3 * N]))
			//		/ (2 * pi * N * ts);

			float ef = (std::arg(
					std::inner_product(x1.begin(), x1.end(), x2.begin(),
							gr_complex(0, 0)))) / (2 * pi * N * ts);

			// Integer Part

			std::vector < std::vector<gr_complex> > cfoCompSeq;

			int n0 = preamble.size() - 2 * N;
			int n1 = preamble.size();

			std::vector < gr_complex > y_v;
			for (auto v : vset) {
				y_v.clear();
				for (int n = 0; n < n1; ++n) {
					gr_complex tmp = preamble[n]
							* std::exp(
									std::complex<float>(0, -2) * pi
											* (ef + v * delta) * (1.0f * n)
											* ts);
					y_v.push_back(tmp);
				}

				y_v = std::vector < gr_complex
						> (y_v.end() - 4 * N + 1, y_v.end() - 2 * N);

				cfoCompSeq.push_back(y_v);

			}

			std::vector<float> aux;
			std::vector < gr_complex > da7 = std::vector < gr_complex
					> (preamble.end() - 2 * N, preamble.end());
			for (auto v : vset) {
				aux.push_back(
						std::abs(
								std::inner_product(cfoCompSeq[v + I].begin(),
										cfoCompSeq[v + I].end(), da7.begin(),
										gr_complex(0, 0))));
			}

			int v = static_cast<int>(std::distance(aux.begin(),
					max_element(aux.begin(), aux.end()))) - I;

			float CFO = ef + v * delta;

			CFO_estimate = CFO / 1e3;

			if (debug)
				std::cout << "CFO of preamble with " << nSamples << " samples: "
						<< CFO_estimate << " kHz\n";

			if (save_estimates) {
				if (fp != NULL) {
					fwrite(&CFO, sizeof(CFO), 1, fp);
					fflush(fp);
				} else {
					std::cout << "\n[CFO_estimates] Unable to open file\n";
				}
			}

			havePreamble = false;
			nSamples = 0;
			preamble.clear();
		}

	}

	if (false)
		if (havePreamble) {
			if (true) {	// If I call estim_CFO, the attribute CFO_estimates is not found in the module PHYAuth...
				float pi = 3.14159265359;
				float fs = 4e6;
				float ts = 1 / fs;
				int N = 64;
				int I = 2;

				//estCFO = ef + v*delta. (ef is fractional part and v is an Integer)
				float delta = 62.5e3;

				std::vector<int> vset;
				for (int i = -I; i < I + 1; ++i) {
					vset.push_back(i);
				}

				std::vector < gr_complex > x = std::vector < gr_complex
						> (preamble.end() - 5 * N + 1, preamble.end());

				std::vector < gr_complex > x_conj = x;
				for (auto &x : x_conj)
					x = std::conj(x);

				int n0 = x.size();

				//The transpose of 1D is the same vector
				float ef = (std::arg(x[n0 - 2 * N] * x_conj[n0 - 3 * N]))
						/ (2 * pi * N * ts);

				std::vector < std::vector<gr_complex> > cfoCompSeq;

				n0 = preamble.size();

				std::vector < gr_complex > y_v;
				for (auto v : vset) {
					y_v.clear();
					for (int n = 0; n < n0; ++n) {
						gr_complex tmp = preamble[n]
								* std::exp(
										std::complex<float>(0, -2) * pi
												* (ef + v * delta) * (1.0f * n)
												* ts);
						y_v.push_back(tmp);
					}

					y_v = std::vector < gr_complex
							> (y_v.end() - 2 * N + 1, y_v.end());

					cfoCompSeq.push_back(y_v);

				}

				std::vector<float> aux;
				for (auto v : vset) {
					aux.push_back(std::abs(cfoCompSeq[v + I][y_v.size() - 1]));
				}

				int v = static_cast<int>(std::distance(aux.begin(),
						max_element(aux.begin(), aux.end()))) - I;

				float CFO = ef + v * delta;

				CFO_estimate = CFO / 1e3;

				/*
				 if (fp != NULL) {
				 fwrite(&CFO, sizeof(CFO), 1, fp);
				 fflush(fp);
				 } else {
				 std::cout << "\n[CFO_estimates] Unable to open file\n";
				 }
				 */

				std::cout << "CFO of preamble with " << nSamples << " samples: "
						<< CFO_estimate << " kHz\n";

				havePreamble = false;
				nSamples = 0;
				preamble.clear();

			}
		}

// Tell runtime system how many input items we consumed on
// each input stream.
	consume_each(noutput_items);

// Tell runtime system how many output items we produced.
	return noutput_items;
}

bool CFO_estimates_impl::stop() {
	fflush(fp);
	fclose(fp);
	return true;
}

float estim_CFO(/*std::vector<gr_complex> preamble*/) {
	/*
	 float pi = 3.14159265359;
	 float fs = 4e6;
	 float ts = 1 / fs;
	 int N = 64;
	 int I = 2;

	 //estCFO = ef + v*delta. (ef is fractional part and v is an Integer)
	 float delta = 62.5e3;

	 std::vector<int> vset;
	 for (int i = -I; i < I + 1; ++i) {
	 vset.push_back(i);
	 }

	 std::vector < gr_complex > x = std::vector < gr_complex
	 > (preamble.end() - 5 * N + 1, preamble.end());

	 std::vector < gr_complex > x_conj = x;
	 for (auto &x : x_conj)
	 x = std::conj(x);

	 int n0 = x.size();

	 //The transpose of 1D is the same vector
	 float ef = (std::arg(x[n0 - 2 * N] * x_conj[n0 - 3 * N]))
	 / (2 * pi * N * ts);

	 std::vector < std::vector<gr_complex> > cfoCompSeq;

	 n0 = preamble.size();

	 std::vector < gr_complex > y_v;
	 for (auto v : vset) {
	 y_v.clear();
	 for (int n = 0; n < n0; ++n) {
	 gr_complex tmp = preamble[n]
	 * std::exp(
	 std::complex<float>(0, -2) * pi * (ef + v * delta)
	 * (1.0f * n) * ts);
	 y_v.push_back(tmp);
	 }

	 y_v = std::vector < gr_complex > (y_v.end() - 2 * N + 1, y_v.end());

	 cfoCompSeq.push_back(y_v);

	 }

	 std::vector<float> aux;
	 for (auto v : vset) {
	 aux.push_back(std::abs(cfoCompSeq[v + I][y_v.size() - 1]));
	 }

	 int v = static_cast<int>(std::distance(aux.begin(),
	 max_element(aux.begin(), aux.end()))) - I;

	 float CFO = ef + v * delta;

	 return CFO;
	 */
	return 0;
}

/*
 template <typename T, typename A>
 int arg_max(std::vector<T, A> const& vec) {
 return static_cast<int>(std::distance(vec.begin(), max_element(vec.begin(), vec.end())));
 }
 */

} /* namespace PHYAuth */
} /* namespace gr */

