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
#include <chrono>
using namespace std;
using namespace std::chrono;


class RansAlgo {

	public:

		map <char, int> symbol_counts = {
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

		};

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
		int len_last;

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


		char cumul_inverse(uint64_t slot) {
			bool flag_done = 0;
			for (auto it = letters_cumul_counts.begin(); it != letters_cumul_counts.end(); ++it) {
				auto prev_ele = prev(it);
				if (slot < it->second) {
					return prev_ele->first;
					flag_done == 1;
				}
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


		vector<string> to_substr(const string& str)
		{
			vector<string> substrings;

			for (size_t i = 0; i < str.size(); i += 4) {
				substrings.push_back(str.substr(i, 4));
			}

			for (string in_sub : substrings) {
				cout << in_sub << "/";
			}

			len_last = substrings.back().length();

			return substrings;
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
	file_read.open("C:/Users/karol/Desktop/stream_vs_no/short_short_text.txt");  // Path fo file, should be changed when run on different machine

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

	// Calculate sum and cumulative sum of all letter frequencies

	myRans.calc_total_counts();
	myRans.calc_cumul_map();

	// Block rANS encoding with execution time measurement
	
	cout << "input str: " << input_str << endl;
	
	

	vector<string> parted_input_str;

	vector<uint64_t> parted_encoded;

	auto start = high_resolution_clock::now();

	parted_input_str = myRans.to_substr(input_str);

	for (string input_part : parted_input_str) {
		state = 0;
		for (uint64_t s : input_part) {
			state = myRans.c_rans(s, state);
		}
		parted_encoded.push_back(state);
		cout << "final state: " << state << endl;

	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);

	cout << "Time taken by block function : "
		 << duration.count() << " microseconds" << endl;

	// Save results to file

	stringstream ss_blocks;
	for (auto it = parted_encoded.begin(); it != parted_encoded.end(); it++) {
		if (it != parted_encoded.begin()) {
			ss_blocks << " ";
		}
		ss_blocks << *it;
	}

	string straa_blocks = ss_blocks.str();

	file_save.open("C:/Users/karol/Desktop/stream_vs_no/encodec_exmaple_rANS_blocks.txt");
	file_save << endl;
	file_save << straa_blocks;
	file_save.close();

	cout << "final state: " << state << endl;

	// Block rANS decoding for debug purpose

	uint64_t prev_state ;
	 
	for (int i = parted_encoded.size() - 1; i >= 0; i--) {
		prev_state = parted_encoded[i];
		if (i == parted_encoded.size() - 1) {
			for (int i = 0; i < myRans.len_last; i++) {
				prev_state = myRans.d_rans(prev_state);
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				prev_state = myRans.d_rans(prev_state);
			}
		}
	}

	// Save decoded text to file for verification purpose

	cout << "final state: " << prev_state << endl;
	reverse(myRans.dec_string.begin(), myRans.dec_string.end());

	ofstream myfile;
	myfile.open("C:/Users/karol/Desktop/stream_vs_no/decoded_rANS_blocks.txt");
	myfile << myRans.dec_string;
	myfile.close();


	

	// Uncomment for streaming-rANS

	/*
	uint64_t first_state = 0;

	myRans.range_factor = 2;

	auto start = high_resolution_clock::now();

	uint64_t last_stat = myRans.streaming_rans_encoder(input_str, first_state);

	auto stop = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(stop - start);

	cout << "Time taken by streaming: "
	<< duration.count() << " microseconds" << endl;

	reverse(myRans.dec_string.begin(), myRans.dec_string.end());
	cout << "final state: " << last_stat << endl;


	uint64_t stre_dec_state = last_stat;

	myRans.range_factor = 1;

	myRans.dec_string = "";
	cout << "bitstream " << myRans.bitstream[0] << " " << myRans.bitstream[1] << endl;

	stringstream ss;
	for (auto it = myRans.bitstream.begin(); it != myRans.bitstream.end(); it++) {
	if (it != myRans.bitstream.begin()) {
	}
	ss << *it;
	}

	string straa_bitstream = ss.str();

	file_save.open("C:/Users/karol/Desktop/stream_vs_no/encodec_exmaple_rANS_block_stre.txt");
	file_save << last_stat;
	file_save << endl;
	file_save << straa_bitstream;
	file_save.close();

	for (int i = 0; i < len_str; i++) {
	stre_dec_state = myRans.streaming_rans_decoder(stre_dec_state);
	}

	reverse(myRans.dec_string.begin(), myRans.dec_string.end());

	cout << endl;
	*/

	





	return 0;
}
