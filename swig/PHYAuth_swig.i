/* -*- c++ -*- */

#define PHYAUTH_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "PHYAuth_swig_doc.i"

%{
#include "PHYAuth/freq_analysis_sink.h"
#include "PHYAuth/CFO_estimates.h"
#include "PHYAuth/preamble_cutter.h"
%}

%include "PHYAuth/freq_analysis_sink.h"
GR_SWIG_BLOCK_MAGIC2(PHYAuth, freq_analysis_sink);
%include "PHYAuth/CFO_estimates.h"
GR_SWIG_BLOCK_MAGIC2(PHYAuth, CFO_estimates);
%include "PHYAuth/preamble_cutter.h"
GR_SWIG_BLOCK_MAGIC2(PHYAuth, preamble_cutter);
