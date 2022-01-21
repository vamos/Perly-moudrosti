/*	VUT FIT IMS Project:
 *
 *	Modeling voles population in Cellular automaton
 *
 * 	File: main.cc
 * 	Authors: David Vodak	(xvodak05)
 * 		 Libor Dvoracek	(xdvora2t)
 */

#include "cellular_automaton.h"
#include <iostream>
#include <unistd.h>

#define ARGUMENTS                "hcsi:p:d:"

void print_legend(bool dont_print)
{
	if (!dont_print) {
		std::cout << "\'_\'	->	empty cell\n"
			     "\'V\'	->	female adult vole\n"
			     "\'W\'	->	female adult vole with calf\n"
			     "\'v\'	->	female young vole\n"
			     "\'T\'	->	male adult vole\n"
			     "\'t\'	->	male young vole\n"
			     "\'+\' 	->	road\n";
	}
}


void print_cell(const cell *cell_p)
{
	const struct attr s = cell_p->get_info();
	using namespace std;

	if (cell_p->is_on_road()) {
		cout << "+ ";
		return;
	}

	if (s.voles){
		if (s.is_female){
			if (s.is_adult) {
				if (s.with_calf)
					cout << 'W';
				else
					cout << 'V';
			} else {
				cout << 'v';

			}
		} else {
			if (s.is_adult) {
				cout << 'T';
			} else {
				cout << 't';
			}
		}
	} else {
		cout << '_';
	}

	cout << ' ';
}


void print_ca(const cell *cell_p, bool dont_print)
{
	if (!dont_print) {
		std::cout << "\n";

		for (int i = 0; i < CELL_ARR_SIZE;  i++) {

			std::cout << '\n';

			for (int j = 0; j < CELL_ARR_SIZE; j++) {
				print_cell(cell_p);
				cell_p++;
			}
		}

		std::cout << '\n';
	}
}

void print_winter_time(bool weak_winter, bool winter, bool dont_print)
{
	if (!dont_print) {

		if (winter && weak_winter) {
			std::cout << "WEAK WINTER TIME\n";
			return;
		}

		std::cout << (winter ? "WINTER TIME" : "NORMAL SEASON") << std::endl;
	}
}

void print_stats(struct ca_stats &s, bool dont_print)
{

	if (!dont_print) {
		std::cout << "-----------------------------------\n";

		std::cout
			<< "on_road:		" << s.on_road_cnt       			<< "\n"
												<< "\n"
			<< "voles:		 	" << s.voles_cnt         		<< "\n"
			<< "	females:		" << s.is_female_cnt     		<< "\n"
			<< "	females with calf:	" << s.with_calf_cnt     		<< "\n"
			<< "	males:			" << s.voles_cnt - s.is_female_cnt 	<< "\n"
			<< "	adults:			" << s.is_adult_cnt      		<< "\n"
			<< "	young voles:		" << s.voles_cnt - s.is_adult_cnt  	<< "\n"
												<< "\n"
			<< "Time passed: " << s.days/30 <<  " months, " << s.days%30 << "days"  << "\n"
			<< "Predators:   " << s.predators 					<< "\n";
		std::cout << "-----------------------------------\n";
	}
}

void print_vole_stat(std::vector<uint32_t> &vole_stat)
{
	int days = 0;

	std::cout << "\n\n";
	std::cout << "number of voles\t|\tdays\n";
	std::cout << "-----------------------------------\n";

	for (auto v : vole_stat) {
		std::cout << v << "\t\t|\t" << days;
		days += TIME_STEP;
		std::cout << " - " << days << std::endl;
	}
}

void print_help()
{
        std::cout << "Vole cellular automaton\n"
                  << "Usage: ./ca [options]\n"
                  << "Options:\n"
                  << "  -h              Prints this info\n"
                  << "  -i <iter_num>   Specifies the number of iterations\n"
                  << "  -p <pred_num>   Specifies the additional number of predators\n"
		  << "  -d <days_num>   Specifies the number of additional days to\n"
		  << "                  send predators to the CA (it needs -p flag)\n"
                  << "  -c              Disables printing CA every iteration\n"
                  << "  -s              Disables printing stats every iteration\n"
                  << "\n";

}

int main(int argc, char **argv)
{
	cellular_automaton ca;
	bool winter;
	bool weak_winter;
	struct ca_stats s;
	std::vector<uint32_t>vole_stat;
	std::string iterations_str, predators_str, days_str;
	int predators_nb;
	int iterations;
	int days;
	bool rm_iter_ca = false;
	bool rm_iter_stats = false;
	int c;

        while ((c = getopt(argc, argv, ARGUMENTS)) != -1) {

                switch (c) {
                case 'h':
                        print_help();
                        return 0;
                case 'i':
                        iterations_str = optarg;
                        break;
                case 'p':
                        predators_str = optarg;
                        break;
                case 'd':
                        days_str = optarg;
                        break;
                case 'c':
			rm_iter_ca = true;
                        break;
                case 's':
			rm_iter_stats = true;
                        break;
                default:
                        std::cerr << "Invalid arguments!\n"
                                  << "Try using -h argument for help.\n";
                        return 1;
                }
        }

	if (iterations_str.empty()) {
		iterations = ITERATIONS;
	} else {
		try {
			iterations = std::stoi(iterations_str);
		} catch (std::invalid_argument& err) {
			std::cerr << "Invalid arguments!\n"
				  << "Try using -h argument for help.\n";
			return 1;
		}
	}

	if (!days_str.empty()) {
		try {
			days = std::stoi(days_str);
		} catch (std::invalid_argument& err) {
			std::cerr << "Invalid arguments!\n"
				  << "Try using -h argument for help.\n";
			return 1;
		}
	}

	/* add additional predators */
	if (!predators_str.empty()) {
		try {
			predators_nb = std::stoi(predators_str);
		} catch (std::invalid_argument& err) {
			std::cerr << "Invalid arguments!\n"
				  << "Try using -h argument for help.\n";
			return 1;
		}

		if (days_str.empty())
			ca.set_predators_num(predators_nb);
	}

	/* print stats */
	print_legend(rm_iter_ca);
	print_ca(ca.get_cells(), rm_iter_ca);

	for (int i = 0; i < iterations; i++) {

		/* send predators to ca after time specified in cmd line */
		if (!days_str.empty())
			if (days < (i * TIME_STEP))
				ca.set_predators_num(predators_nb);

		/* one step in cellular automaton */
		ca.step();

		/* print stats */
		ca.get_statistics(s);
		vole_stat.push_back(s.voles_cnt);
		winter = ca.is_winter(&weak_winter);
		print_stats(s, rm_iter_stats);
		print_winter_time(weak_winter, winter, rm_iter_stats);
		print_ca(ca.get_cells(), rm_iter_ca);
	}

	print_vole_stat(vole_stat);

	return 0;
}
