/*	VUT FIT IMS Project:
 *
 *	Modeling voles population in Cellular automaton
 *
 * 	File: cellular_automaton.cc
 * 	Authors: David Vodak	(xvodak05)
 * 		 Libor Dvoracek	(xdvora2t)
 */

#include <math.h>
#include <random>
#include "debug.h"

#define RADIUS_OF_NEIGHBOURHOOD			16
#define RADIUS_OF_MOTHERHOOD			2

#define BASE_MORTALITY_PERCENT 			41
/* predators stick around refugia 2 times less */
#define ROAD_MORTALITY_PERCENT 			21

#define WINTER_MORTALITY_PERCENT		58

#define ROAD_START				10
#define ROAD_END				13


const struct attr empty_cell =  {
	.voles = false,

	.is_female = false,
	.with_calf = false,
	.is_adult = false,
};

const struct attr female_cell =  {
	.voles = true,

	.is_female = true,
	.with_calf = false,
	.is_adult = true,
};

const struct attr male_cell =  {
	.voles = true,

	.is_female = false,
	.with_calf = false,
	.is_adult = true,
};

struct neighbour {
	struct attr state;
	bool on_road;
	uint16_t x;
	uint16_t y;
};

/* exception class methods */

ca_create_exception::ca_create_exception(std::string new_msg)
{
	msg = new_msg;
}

const char *ca_create_exception::what () const throw ()
{
	return msg.c_str();
}

/* cell class's methods */

bool cell::is_on_road() const
{
	return on_road;
}

static bool percent_rng(int percent)
{	
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> dis(0, 100);

	if (percent > 100 || percent < 0)
		throw ca_create_exception("Wrong percent number!\n");

	return dis(gen) < percent;	
}

static inline bool cell_is_on_the_border(int x, int y)
{
	return x == 0 || y == 0 ||
	       x == (CELL_ARR_SIZE - 1) || y == (CELL_ARR_SIZE - 1);
}

static void find_neigbours(cellular_automaton &ca, int x, int y,
	      			std::vector<struct neighbour> *neighbours,
				bool include_center,
				int radius)
{
	int dx = x - radius;                // iterating index x set to start. position
	int dy = y - radius;                // iterating index y set to start. position
	int boundary = (2*radius);          // rectangle boundary
	int col_cnt = 0;                    // column counter
	int row_cnt = 0;                    // row counter
	cell c;
	struct neighbour n;

	while ( row_cnt <= boundary) {      // iterate through all indices of the rectangle
		while ( col_cnt <= boundary) {   // and determine which are inside of the circle

		if (!include_center && dx == x && dy == y) {
			dx++;
			continue;
		}

		// cell in the grid of CA
		if (dx < CELL_ARR_SIZE && dy < CELL_ARR_SIZE &&
		    dx >= 0            && dy >= 0             ) { 

			// compute if cell is within desired circle
			int p = abs(pow((x - dx),2))+ abs(pow((y - dy),2));

			if ( p <= pow(radius,2)){ 
				c = ca.get_previous_cell(dx, dy);
				n.state = c.get_info();
				n.x = dx;
				n.y = dy;
				n.on_road = c.is_on_road();
				neighbours->push_back(n);
			}
		}
		// take next index and next column
		col_cnt++;
		dx++;
		}
		// get next row and reset columns
		dx = x - radius;
		col_cnt = 0;
		row_cnt++;
		dy++;
	}	
}

static bool can_get_pregnant(struct attr state,
				std::vector<struct neighbour> &neighbours) 
{
	if (state.voles    &&  state.is_female && 
	    state.is_adult && !state.with_calf) {
		for (auto n : neighbours) { /* find parter */
			if (n.state.voles && n.state.is_adult && !n.state.is_female)
				return true;
		}
	}

	return false;
}

static bool will_get_pregnant(struct attr state,
				std::vector<struct neighbour> &neighbours,
				bool winter) 
{
	if (winter)
		return false;

	if (percent_rng(100))
		return can_get_pregnant(state, neighbours);
	else
		return false;
}

static bool will_be_born(struct attr state,
			std::vector<struct neighbour> &neighbours)
{
	if (state.voles)
		return false;

	for (auto n : neighbours) { /* find mother */
		if (n.state.voles && n.state.is_female && n.state.with_calf) {
			return true;
		}
	}

	return false;
}

static bool road_is_close(std::vector <struct neighbour> &neighbours)
{
	for (auto neighbour : neighbours)
		if (neighbour.on_road)
			return true;

	return false;
}

static bool vole_died(struct attr state,
				std::vector<struct neighbour> &neighbours,
				bool is_winter,
				int predators_num)
{
	int percent;
		
	percent = road_is_close(neighbours) ?
		predators_num * ROAD_MORTALITY_PERCENT : 
		predators_num * BASE_MORTALITY_PERCENT;

	if (percent > 100)
		percent = 100;

	if (percent_rng(percent))
		return true;
	
	if (is_winter) {
		if (percent_rng(WINTER_MORTALITY_PERCENT)) {
			return true;
		}
	}

	return false;	
}

struct attr cell::get_next_state(cellular_automaton &ca, int x, int y)
{
	std::vector <struct neighbour> neighbours;
	std::vector <struct neighbour> motherhood;
	struct attr state;
	bool weak_winter;
	bool winter;	
	
	winter = ca.is_winter(&weak_winter);

	if (winter && weak_winter)
		winter = false;

	/*
	 *  Get the current state and the states of
	 *  neighbouring cells
	 */
 
	find_neigbours(ca, x, y, &neighbours, false, RADIUS_OF_NEIGHBOURHOOD);
	find_neigbours(ca, x, y, &motherhood, false, RADIUS_OF_MOTHERHOOD);

	state = ca.get_previous_cell(x, y).get_info();

	/*
	 *  decide the next state based on current state and
	 *  state of neighbors
	 */

	/* mortality*/

	if (state.voles) {
		if (vole_died(state, neighbours, winter, ca.get_predators_num()))
			return empty_cell;
	}

	/* natality */

	if (state.with_calf) {
		state.with_calf = false;
		return state;
	} 

	if (will_get_pregnant(state, neighbours, winter)) {
		state.with_calf = true;
		return state;
	}

	if (will_be_born(state, motherhood)) {
		state.voles = true;
		state.is_female = percent_rng(50);
		state.is_adult = false;	
		return state;
	}

	if (state.voles) {
		state.is_adult = true;
		return state;
	}

	return state;
}

struct attr cell::get_info() const
{
	return state;
}

cell::cell()
{
	state = empty_cell;
}

cell::cell(struct attr a)
{
	state.voles         = a.voles; 
                                             
	state.is_female     = a.is_female;
	state.with_calf     = a.with_calf;
	state.is_adult      = a.is_adult;
}


/* cellular automaton class's methods */

cellular_automaton::cellular_automaton()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 100);
 
	struct attr a;

	/*
	 *	The cells are randomly generated
	 *	1 : 50 (vole : empty cell )ratio 
	 *	which results in typical vole 
	 *	population at the first day of march
	 *	which is the start of simulation 
	 */
	for (auto &cells_1d : current_state) {
		for (auto &c : cells_1d) {
			switch (dis(gen)) {
			case 0:
				a = female_cell;
				break;	
			case 1:	
				a = male_cell;
				break;	
			default:
				a = empty_cell;
				break;	
			}

			c.state = a;
			c.on_road = false;
		}
	}

	/* create road */
	for (int i = 0; i < CELL_ARR_SIZE; i++)
		for (int j = ROAD_START; j < ROAD_END; j++)
			current_state[i][j].on_road = true;

	days = 31 + 28;	 /* 1st day of march */
	weak_winter_cnt = 0;
	was_winter = false;

	predators_num = 0;
}

void cellular_automaton::save_state()
{
	for (int x = 0; x < CELL_ARR_SIZE; x++) {

		for (int y = 0; y < CELL_ARR_SIZE; y++) {

			previous_state[x][y] = current_state[x][y];
		}
	}
}

bool cellular_automaton::is_winter(bool *weak_winter)
{
	*weak_winter = this->weak_winter;
	return winter;
}

void cellular_automaton::calc_winter() {
	const int days_in_year = 356;
	const int end_of_winter = 31 + 28;	 /* 1st day of March */
	const int start_of_winter = 31 + 28 +    /* 1st day of December */
		31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30;

	weak_winter = false;

	if (!(end_of_winter < (days % days_in_year) && (days % days_in_year) < start_of_winter)) {

		was_winter = true;

		if (weak_winter_cnt % 4 == 0)
			weak_winter = true;

		winter = true;
		return;
	}

	if (was_winter) {
		was_winter = false;
		weak_winter_cnt++;
	}

	winter = false;
	return; 
}

int cellular_automaton::get_predators_num()
{
	return predators_num;
}

void cellular_automaton::set_predators_num(int predators_num)
{
	this->predators_num = predators_num;
}

void cellular_automaton::get_statistics(struct ca_stats &s)
{
	s.on_road_cnt = 0;      
        s.voles_cnt = 0;        
        s.is_female_cnt = 0;    
        s.with_calf_cnt = 0;    
        s.is_adult_cnt = 0;     

	s.days = days;
	s.predators = predators_num;

	for (auto &cells_1d : previous_state) {
		for (auto &c : cells_1d) {
			s.on_road_cnt      	+= c.on_road;      
			                                              
                        s.voles_cnt        	+= c.state.voles;        
			s.is_female_cnt    	+= c.state.is_female;    	
			s.with_calf_cnt    	+= c.state.with_calf;    	
			s.is_adult_cnt     	+= c.state.is_adult;     	
		}
	}
}

void cellular_automaton::step()
{
	int x = 0, y = 0;
	int number_of_voles = 0;

	save_state();
	this->calc_winter();

	/* Apply the rule for updating state to every cell in CA */
	for (auto &cells_1d : current_state) {
		y = 0;

		for (auto &c : cells_1d) {

			c.state = c.get_next_state(*this, x, y);

			db_print(c.state);

			y++;
		}
		x++;
	}

	days += TIME_STEP; 
}

const cell &cellular_automaton::get_previous_cell(int x, int y) const
{
	if (x >= CELL_ARR_SIZE || y >= CELL_ARR_SIZE ||
	    x < 0              || y < 0             )
		throw ca_create_exception("Index out of range!\n");

	return previous_state[x][y];
}

const cell *cellular_automaton::get_cells() const
{
	return &(current_state[0][0]);
}
