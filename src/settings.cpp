/*
*	nettop (C) 2017 E. Oriani, ema <AT> fastwebnet <DOT> it
*
*	This file is part of nettop.
*
*	nettop is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	nettop is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with nettop.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "settings.h"
#include <iostream>
#include <getopt.h>
#include <cstring>
#include "utils.h"

namespace {
	// settings/options management
	void print_help(const char *prog, const char *version) {
		using namespace nettop::settings;

		std::cerr <<	"Usage: " << prog << " [options]\nExecutes nettop " << version << "\n\n"
				"-r, --refresh s\t\t\tsets the refresh rate in 's' seconds (default " << REFRESH_SECS << ")\n"
				"-c, --capture (a|s|r)\t\tCapture mode for 'a'll, 's'end and 'r'ecv only (default '" << 'a' << "')\n"
				"-o, --order (a|d)\t\tOrdering of results, 'a'scending, 'd'escending (default '" << 'd' << "')\n"
				"    --filter-zero\t\tSet to filter all zero results (default not set)\n"
				"    --tcp-udp-split\t\tDisplays split of TCP and UDP traffic in % (default not set)\n"
				"-i, --interface (network interface name)\tOnly captures data from the specified network interface (e.g., eth0)\n"
				"-a, --async-log-file (file)\tSets an output file where to store the packets attribued to the 'kernel' (default not set)\n"
				"-l, --limit-hosts-rows\t\tLimits maximum number of hosts rows per pid (default no limit)\n"
				"    --help\t\t\tprints this help and exit\n\n"
				"Press 'q' or 'ESC' inside nettop to quit, 'SPACE' or 'p' to pause nettop\n"
		<< std::flush;
	}
}

namespace nettop { 
	namespace settings {
		size_t		REFRESH_SECS = 3;
		int		CAPTURE_ASR = 3;
		bool		ORDER_TOP = true;
		bool		FILTER_ZERO = false;
		bool		TCP_UDP_TRAFFIC = false;
		std::string	INTERFACE = "";
		std::string	ASYNC_LOG_FILE = "";
		size_t		LIMIT_HOSTS_ROWS = 0;
	}
}

int nettop::parse_args(int argc, char *argv[], const char *prog, const char *version) {
	using namespace nettop::settings;

	int			c;
	static struct option	long_options[] = {
		{"help",		no_argument,	   0,	0},
		{"refresh",		required_argument, 0,	'r'},
		{"capture",		required_argument, 0,	'c'},
		{"order",		required_argument, 0,	'o'},
		{"filter-zero",		no_argument, 	   0,	0},
		{"tcp-udp-split",	no_argument,	   0,	0},
	    {"interface",	required_argument, 0,	'i'},
		{"async-log-file",	required_argument, 0,	'a'},
		{"limit-hosts-rows",	required_argument, 0,	'l'},
		{0, 0, 0, 0}
	};
	
	while (1) {
        	// getopt_long stores the option index here
        	int		option_index = 0;

		if(-1 == (c = getopt_long(argc, argv, "hr:c:o:i:a:l:", long_options, &option_index)))
       			break;

		switch (c) {
		case 0: {
			// If this option set a flag, do nothing else now
           		if (long_options[option_index].flag != 0)
                		break;
			if(!std::strcmp("filter-zero", long_options[option_index].name)) {
				FILTER_ZERO = true;
			} if(!std::strcmp("tcp-udp-split", long_options[option_index].name)) {
				TCP_UDP_TRAFFIC = true;
			} else if(!std::strcmp("help", long_options[option_index].name)) {
				print_help(prog, version);
				std::exit(0);
			}
		} break;

		case 'r': {
			const int	r_res = std::atoi(optarg);
			REFRESH_SECS = (r_res < 0) ? 1 : (r_res > 60) ? 60 : r_res;
		} break;

		case 'c': {
			switch(optarg[0]) {
				case 'a':
					CAPTURE_ASR = CAPTURE_ALL;
					break;
				case 's':
					CAPTURE_ASR = CAPTURE_SEND;
					break;
				case 'r':
					CAPTURE_ASR = CAPTURE_RECV;
					break;
				default:
					throw runtime_error("Invalid capture flag provided (expected 'a', 's' or 'r' but found '") << optarg[0] << "')";
					break;
			}
		} break;

		case 'o': {
			switch(optarg[0]) {
				case 'a':
					ORDER_TOP = false;
					break;
				default:
					ORDER_TOP = true;
					break;
			}
		} break;

		case 'a': {
			ASYNC_LOG_FILE = optarg;
		} break;

		case 'i': {
			INTERFACE = optarg;
		} break;

		case 'l': {
			const int	m_res = std::atoi(optarg);
			LIMIT_HOSTS_ROWS = (m_res > 0) ? m_res : 0;
		} break;

		case '?':
		break;
		
		default:
			throw runtime_error("Invalid option '") << (char)c << "'";
		break;
             	}
	}

	return optind;
}

