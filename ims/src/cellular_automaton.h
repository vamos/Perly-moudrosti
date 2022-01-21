/*	VUT FIT IMS Project:
 *
 *	Modeling voles population in Cellular automaton
 *
 * 	File: cellular_automaton.h
 * 	Authors: David Vodak	(xvodak05)
 * 		 Libor Dvoracek	(xdvora2t)
 */

#include <exception>
#include <stdint.h>
#include <vector>
#include <string>
#include <stdlib.h>  // abs

#define CELL_ARR_SIZE	 60 
#define ITERATIONS       15
#define TIME_STEP	 25

class ca_create_exception : public std::exception {
private:
	std::string msg;
public:
	ca_create_exception(std::string new_msg);
	const char * what () const throw ();
};


struct ca_stats {
	int on_road_cnt;     

	int voles_cnt;       
	int is_female_cnt;   
	int with_calf_cnt;
	int is_adult_cnt;

	int days;

	int predators;
};

struct attr {

	uint32_t voles		: 1;

	uint32_t is_female	: 1;
	uint32_t with_calf 	: 1;
	uint32_t is_adult 	: 1;
};

class cellular_automaton;

class cell {

protected:
	/*
	 * The state of cell consist of struct attr state
	 * and flag on_road which is separate and therefore
	 * cant be overwritten that easily (road doesn't change).
	 */
	struct attr state;
	bool on_road;
public:
	cell();
	cell(struct attr a);

	/*
	 * The rule for updating state, it is used 
	 * for every cell in cellular automaton.
	 */
	struct attr get_next_state(cellular_automaton &ca,
					int x, int y);

	struct attr get_info() const;
	bool is_on_road() const;

	friend class cellular_automaton;
};

class cellular_automaton {

private:

	/*
	 *	Cellular automaton's cells
	 *
	 *	There are two arrays, to ensure that
	 *	all cells states are updated synchronously.
	 */
	cell previous_state[CELL_ARR_SIZE][CELL_ARR_SIZE];
	cell current_state[CELL_ARR_SIZE][CELL_ARR_SIZE];

	/*
	 *	Time parameters
	 *
	 *	There are many of them to provide features
	 *	needed for proper simulation such as winter
	 *	or special kind of winter, which happens
	 *	according to statistics every 3-4 year.
	 *	This winter, which is here called 'weak_winter'
	 *	is very pleasant for voles and their multiplication.
	 */
	int days;
	int weak_winter_cnt;
	bool was_winter;

	bool weak_winter;
	bool winter;

	/*
	 *	Predators can be set by user.
	 *	They are not implemented in CA
	 *	since they move too fast.
	 */
	int predators_num;

	/*
	 *	Compute the time so winter can be detected,
	 *	needs to be called every each step().
	 */
	void calc_winter();

	/*
	 *	Saves 'current_state' to 'previous_state'
	 *	so cell states can be computed synchronously.
	 *	It needs to be called every each step().
	 */
	void save_state();
public:
	cellular_automaton();

	void step();

	const cell &get_previous_cell(int x, int y) const;
	const cell *get_cells() const;
	void get_statistics(struct ca_stats &s);

	int get_predators_num();
	void set_predators_num(int predators_num);

	/*
	 *	Tells if its winter time (bad for vole population or not).
	 *	It is based on 'days' attribute which is counts days from
	 *	the start of CA which equals the 1. day of March
	 */
	bool is_winter(bool *weak_winter);
};
