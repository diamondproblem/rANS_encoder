#include <iostream>
#include <vector>
#include <numeric>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>
#include <utility>
#include <fstream>
#include <sstream>
using namespace std;


class RansAlgo {

	public:
		map <char, int> symbol_counts = {
			{' ', 67962112},
			{'a', 24373121},
			{'b', 4762938},
			{'c', 8982417},
			{'d',10805580},
			{'e', 37907119},
			{'f', 7486889},
			{'g', 5143059},
			{'h', 18058207},
			{'i', 21820970},
			{'j', 474021},
			{'k', 1720909},
			{'l', 11730498},
			{'m', 7391366},
			{'n', 21402466},
			{'o', 23215532},
			{'p', 5719422},
			{'q', 297237},
			{'r', 17897352},
			{'s', 19059775},
			{'t', 28691274},
			{'u', 8022379},
			{'v', 2835696},
			{'w', 6505294},
			{'x', 562732},
			{'y', 5910495},
			{'z', 93172}

		}; 

		/*map <char, int> symbol_counts = {
			{' ', 39},
			{'a', 14},
			{'b', 5},
			{'c', 6},
			{'d', 13},
			{'e', 34},
			{'f', 5},
			{'g', 5},
			{'h', 7},
			{'i', 10},
			{'j', 1},
			{'k', 1},
			{'l', 6},
			{'m', 2},
			{'n', 18},
			{'o', 14},
			{'p', 3},
			{'q', 1},
			{'r', 18},
			{'s', 16},
			{'t', 7},
			{'u', 10},
			{'v', 1},
			{'w', 3},
			{'x', 2},
			{'y', 5},
			{'z', 1}

		};	*/
		/*
		map <char, int> symbol_counts = {
			{' ', 3},
			{'a', 14},
			{'b', 5},
			{'c', 6},
			{'d', 13},
			{'e', 3},
			{'f', 5},
			{'g', 5},
			{'h', 7},
			{'i', 10},
			{'j', 1},
			{'k', 1},
			{'l', 6},
			{'m', 2},
			{'n', 1},
			{'o', 2},
			{'p', 3},
			{'q', 1},
			{'r', 1},
			{'s', 1},
			{'t', 1},
			{'u', 1},
			{'v', 1},
			{'w', 3},
			{'x', 2},
			{'y', 1},
			{'z', 1}

		};
		*/
		/*map <char, int> symbol_counts = {
			{' ', 10259},
			{'a', 3933},
			{'b', 627},
			{'c', 1197},
			{'d', 2679},
			{'e', 7372},
			{'f', 988},
			{'g', 1311},
			{'h', 1805},
			{'i', 3952},
			{'j', 114},
			{'k', 228},
			{'l', 2299},
			{'m', 1805},
			{'n', 4066},
			{'o', 3572},
			{'p', 1330},
			{'q', 114},
			{'r', 3819},
			{'s', 3515},
			{'t', 3572},
			{'u', 1710},
			{'v', 836},
			{'w', 855},
			{'x', 209},
			{'y', 1140},
			{'z', 57}

		};*/

		/*map <char, int> symbol_counts = {
			{'a', 1},
			{'d', 1},
			{'n', 1}
		}; */

		/*map <char, int> symbol_counts = {
			{' ', 3},
			{'e', 3},
			{'j', 1},
			{'n', 1},
			{'o', 2},
			{'q', 1},
			{'r', 1},
			{'s', 1},
			{'t', 1},
			{'u', 1},
			{'y', 1},
			{'z', 1}
		};*/

	    map<char, int>letters_cumul_counts = { {'a', 0} };
		uint64_t total_counts = 0;
		uint64_t symbol_counts_len = 0;
		uint64_t s_count = 0;
		uint64_t next_state = 0;
		uint64_t slot = 0;
		string dec_string = "";
		int range_factor = 1;
		vector<int> bitstream;
		int low_level = 1;
		string stre_dec_str = "";
		int bits;
		vector<uint64_t> dec_vec;

		void calc_total_counts() {

			total_counts = accumulate(begin(symbol_counts), end(symbol_counts), 0,
				[](int previous, const pair<char, int>& p)
				{ return previous + p.second; });
		}

		void calc_cumul_map() {

			vector<int>sym_map_values;

			for (map<char, int>::iterator it = symbol_counts.begin(); it != symbol_counts.end(); ++it) {
				sym_map_values.push_back(it->second);
			}


			symbol_counts_len = sym_map_values.size();

			vector<int> cumul_counts_part(symbol_counts_len);
			partial_sum(sym_map_values.begin(), sym_map_values.end(), cumul_counts_part.begin(), plus<int>());
			vector<int> cumul_counts{ 0 };
			cumul_counts.insert(cumul_counts.end(), cumul_counts_part.begin(), cumul_counts_part.end());


			map<char, int>let_cumul_counts{
				{' ', cumul_counts[0]}
			};

			int j = 1;

			for (char i = 'a'; i <= 'z'; i++) {
				let_cumul_counts.insert({ i, cumul_counts[j]});
				j++;
			}

			letters_cumul_counts = let_cumul_counts;
		} 

		void calc_total_counts_int() {

			total_counts = accumulate(begin(symbol_counts), end(symbol_counts), 0,
				[](int previous, const pair<int, int>& p)
				{ return previous + p.second; });

			cout << "result: " << total_counts << endl;
		}


		char cumul_inverse(uint64_t slot) {
			bool flag_done = 0;
			for (auto it = letters_cumul_counts.begin(); it != letters_cumul_counts.end(); ++it) {
				auto prev_ele = prev(it);
				if (slot < it->second) {
					return prev_ele->first;
					flag_done == 1;
				}
				// In case of different alphabet, change this to the last letter of it
				if (it->first == 'z' && flag_done == 0) {
					return it->first;
				}

			}
		}

		uint64_t d_rans(uint64_t state) {

			slot = state % total_counts;
			char dec_sym = cumul_inverse(slot);
			cout << "symbol " << dec_sym << " state " << state << " total_counts " << total_counts << " s_counts " <<
				symbol_counts[dec_sym] << " cum_couts " << letters_cumul_counts[dec_sym] << endl;
			dec_string += dec_sym;
			uint64_t prev_state = (state / total_counts) * symbol_counts[dec_sym] + slot - letters_cumul_counts[dec_sym];
			return prev_state;

		}

		uint64_t c_rans(char s, uint64_t state) {
			s_count = symbol_counts[s];
			uint64_t modu = state % s_count;
			cout << "state " << uint64_t(state) << " s_count " << s_count << " total_counts " << total_counts <<
				" cum_let " << letters_cumul_counts[s] << " modulo " << modu << endl;
			next_state = (state / s_count) * total_counts + letters_cumul_counts[s] + (state % s_count);
			return next_state;
		}

		uint64_t streaming_rans_encoder(string s_input, uint64_t state) {
			state = low_level * total_counts;

			for (uint64_t s : s_input) {
				while (state >= range_factor * symbol_counts[s]) {
					bitstream.push_back(state % 2);
					state = state / 2;
				}
				state = c_rans(s, state);
			}
			return state;
		}

		uint64_t streaming_rans_decoder(uint64_t state) {

			state = d_rans(state);

			while (state < range_factor * total_counts) {
				bits = bitstream.back();
				bitstream.pop_back();
				state = state * 2 + bits;
			}
			
			return state;
		}
};



int main() {

	RansAlgo myRans;

	// Default text line 
	string text_line = "quam pellentesque nec nam aliquam sem et tortor consequat id porta nibh venenatis cras sed felis eget velit aliquet sagittis id consectetur purus ut faucibus pulvinar elementum integer enim neque volutpat ac tincidunt vitae semper quis lectus nulla at volutpat diam ut venenatis tellus in metus vulputate eu scelerisque felis";

	// Read file and prepare text for encoding

	string lines_first_file = "";
	ifstream file_read;
	ofstream file_save;
	string line_from_text;
	file_read.open("C:/Users/karol/Desktop/stream_vs_no_1/short_short_text.txt");

	while (getline(file_read, line_from_text)) {
		lines_first_file += line_from_text;
	}

	file_read.close();

	text_line = lines_first_file;
	uint64_t len_str = text_line.length();

	// Change every letter to lowercase

	transform(text_line.begin(), text_line.end(), text_line.begin(),
		[](unsigned char c) { return tolower(c); });
	int lit = ' ';


	string input_str = text_line;
	uint64_t state = 0;

	// Calculate sum and cumulative sum of all letter frequencies, comment calc_cumul_map() for short alphabets and words 

	myRans.calc_total_counts();
	myRans.calc_cumul_map();

	// Used for test of short alphabets, words:

	/*
	myRans.letters_cumul_counts = {
		{'a', 0},
		{'d', 1},
		{'n', 2}
	};
	*/

	/*myRans.letters_cumul_counts = {{' ', 0},
		{'e', 3},
		{'j', 6},
		{'n', 7},
		{'o', 8},
		{'q', 10},
		{'r', 11},
		{'s', 12},
		{'t', 13},
		{'u', 14},
		{'y', 15},
		{'z', 16} 
	};*/

	// rANS encoding
	/*
	
	cout << "input str: " << input_str << endl;
	
	 for (uint64_t s : input_str) {
		state = myRans.c_rans(s, state);
	}

	cout << "final state: " << state << endl;

	// Save final state to file

	
	string straa_rans = to_string(state);

	file_save.open("C:/Users/karol/Desktop/stream_vs_no_1/encodec_exmaple_short_only.txt");
	file_save << endl;
	file_save << straa_rans;
	file_save.close();

	// rANS decoding for debug and verificiation purpose

	uint64_t prev_state = state;

	for (int i = 0; i < len_str; i++) {
		prev_state = myRans.d_rans(prev_state);
	}

	cout << "final state: " << prev_state << endl;
	reverse(myRans.dec_string.begin(), myRans.dec_string.end());
	*/
	
	// Uncomment for streaming-rANS

	
	
	// streaming-rANS encoding

	uint64_t first_state = 0;

	myRans.range_factor = 2;

	uint64_t last_stat = myRans.streaming_rans_encoder(input_str, first_state);

	reverse(myRans.dec_string.begin(), myRans.dec_string.end());
	cout << "final state: " << last_stat << endl;


	uint64_t stre_dec_state = last_stat;

	myRans.range_factor = 1;

	myRans.dec_string = "";
	cout << "bitstream " << myRans.bitstream[0] << " " << myRans.bitstream[1] << endl;

	// Save final state and bitstream file

	stringstream ss;
	for (auto it = myRans.bitstream.begin(); it != myRans.bitstream.end(); it++) {
		if (it != myRans.bitstream.begin()) {
		}
		ss << *it;
	}

	string straa_bitstream = ss.str();

	file_save.open("C:/Users/karol/Desktop/stream_vs_no_1/encodec_exmaple_rANS_long_ex_stre.txt");
	file_save << last_stat;
	file_save << endl;
	file_save << straa_bitstream;
	file_save.close();

	// streaming-rANS decoding

	for (int i = 0; i < len_str; i++) {
		stre_dec_state = myRans.streaming_rans_decoder(stre_dec_state);
	}

	reverse(myRans.dec_string.begin(), myRans.dec_string.end());

	cout << endl;

	

	// Save decoded text to file for verification purpose

	ofstream myfile;
	myfile.open("C:/Users/karol/Desktop/stream_vs_no_1/decoded_short_rans_stream.txt");
	myfile << myRans.dec_string;
	myfile.close();






	return 0;
}
