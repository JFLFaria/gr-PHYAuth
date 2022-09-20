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

#ifndef INCLUDED_PHYAUTH_CFO_ESTIMATES_IMPL_H
#define INCLUDED_PHYAUTH_CFO_ESTIMATES_IMPL_H

#include <PHYAuth/CFO_estimates.h>

namespace gr {
namespace PHYAuth {

class CFO_estimates_impl: public CFO_estimates {
private:
	// Nothing to declare in this block.
	std::vector<gr_complex> preamble;
	bool havePreamble = false;
	int nSamples = 0;

	bool debug;
	bool save_estimates;

	FILE * fp;

public:

	CFO_estimates_impl(const char * filename, bool debug, bool save_estimates);
	~CFO_estimates_impl();

	float estim_CFO(/*std::vector<gr_complex> preamble*/);
	bool stop() override;
	/*
	 template <typename T, typename A>
	 int arg_max(std::vector<T, A> const& vec);
	 */
	// Where all the action really happens
	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);

};

} // namespace PHYAuth
} // namespace gr

#endif /* INCLUDED_PHYAUTH_CFO_ESTIMATES_IMPL_H */

