/*	VUT FIT IMS Project:
 *
 *	Modeling voles population in Cellular automaton
 *
 * 	File: debug.cc
 * 	Authors: David Vodak	(xvodak05)
 * 		 Libor Dvoracek	(xdvora2t)
 */
#include <iostream>
#include "debug.h"

#ifdef DEBUG

void db_print(struct attr state)
{
	std::cout << "\n\n"
		  << "voles: "  <<	 (state.voles ? "true" : "false") << "\n"
		  << "toxins: " <<	 (state.toxins ? "true" : "false") << "\n"
		  << "deep_tillage: " << (state.deep_tillage ? "true" : "false") << "\n"
		  << "predators: " <<	 (state.predators ? "true" : "false") << "\n"
		  << "is_female: " <<	 (state.is_female ? "true" : "false") << "\n"
		  << "with_calf: " <<	 (state.with_calf ? "true" : "false") << "\n"
		  << "is_adult: " <<	 (state.is_adult ? "true" : "false") << "\n"
		  << "children_born: " << state.children_born << "\n";
}

#else 

void db_print(struct attr state)
{
	state;
}

#endif
