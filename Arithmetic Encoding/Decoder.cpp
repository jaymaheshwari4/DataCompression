/* Code for Arithmetic Decoding by Kunal and Jay For CS 254 Project */
#include<bits/stdc++.h>
using namespace std;

const int CODE_VALUE = 16;   // using 16 bit binary form
const int MAX_VALUE = ((1 << CODE_VALUE) - 1);
const int MAX_FREQ = 16383; // 2^14-1

const int FIRST_QTR = (MAX_VALUE / 4 + 1);
const int HALF = (2 * FIRST_QTR);
const int THIRD_QTR = (3 * FIRST_QTR);

const int NO_OF_CHARS = 256; // total number of differenr chars we can encode
const int EOF_SYMBOL = (NO_OF_CHARS + 1);
const int NO_OF_SYMBOLS = (NO_OF_CHARS + 1);

unsigned char index_to_char [NO_OF_SYMBOLS]; // output the respective char according to index
int char_to_index [NO_OF_CHARS]; // gives index of symbol
int cum_freq [NO_OF_SYMBOLS + 1]; // stores cummulative fre of the symbols
int freq [NO_OF_SYMBOLS + 1]; //stores the freq of symbol


int low, high;
int pending_bits;

bool end_decoding=false;

int buffer = 0;
int bits_in_buf = 0;

int value;

ifstream in;
// initiating the model
void startModel(){
	int i;
	for ( i = 0; i < NO_OF_CHARS; i++)
	{
		char_to_index[i] = i + 1;
		index_to_char[i + 1] = i;
	}
	for ( i = 0; i <= NO_OF_SYMBOLS; i++)
	{
		freq[i] = 1;
		cum_freq[i] = NO_OF_SYMBOLS - i;
	}
	freq[0] = 0;
}

// updating the model
void update_tables(int sym_index)
{
  int i;
  if (cum_freq[0] == MAX_FREQ)
  {
    int cum = 0;
    for ( i = NO_OF_SYMBOLS; i >= 0; i--)
    {
      freq[i] = (freq[i] + 1) / 2;
      cum_freq[i] = cum;
      cum += freq[i];
    }
  }

  for ( i = sym_index; freq[i] == freq[i - 1]; i--);

  if (i < sym_index)
  {
	  int ch_i, ch_symbol;

    ch_i = index_to_char[i];

    ch_symbol = index_to_char[sym_index];

    index_to_char[i] = ch_symbol;

    index_to_char[sym_index] = ch_i;

    char_to_index[ch_i] = sym_index;
    
    char_to_index[ch_symbol] = i;
  }
  freq[i]++;
  while (i > 0)
  {
	i--;
    cum_freq[i]++;
  }
}

// reads single bit from encoded stream
int get_bit(void)
{
	int t;
	if (bits_in_buf == 0)
	{
		buffer = in.get();
		if (buffer == EOF)
		{
			end_decoding = true;
			return -1;
		}
		bits_in_buf= 8;
	}
	t = buffer & 1;
	buffer >>= 1;
	bits_in_buf--;
	return t;
}
// returns the decoded symbol
int decode_symbol(void)
{
	int range;
	int cum; 
	int symbol_index;

	range = high - low;

	cum = ((((value - low) + 1) * cum_freq[0] - 1) / range); 
	
	for (symbol_index = 1; cum_freq[symbol_index] > cum; symbol_index++);
	
	
	// slicing the range
	high = low + (range * cum_freq[symbol_index - 1]) / cum_freq[0];
	low = low + (range * cum_freq[symbol_index]) / cum_freq[0];
	// normalization
	for (;;)
	{
		if (high < HALF)
		{
		}
		else if (low >= HALF)
		{
			value -= HALF;
			low -= HALF;
			high -= HALF;
		}
		else if (low >= FIRST_QTR && high < THIRD_QTR)
		{
			value -= FIRST_QTR;
			low -= FIRST_QTR;
			high -= FIRST_QTR;
		}
		else
			break;
		low = 2 * low;
		high = 2 * high;
		value = 2 * value +get_bit();
	}
	return symbol_index;
}
// main function for traversing through the encoded file and decoding the symbol 
void decode(){

	low=0;
	high=MAX_VALUE;
	value = 0;
	for (int i = 1; i <= CODE_VALUE; i++){
		value = 2 * value + get_bit();}
	while (true)
	{
		int ch=59;
		int sym_index;
		sym_index = decode_symbol();

		if ((sym_index == EOF_SYMBOL) || end_decoding){
						break;
		}
		ch = index_to_char[sym_index];
		cout<<(char)ch;
		update_tables(sym_index); // calling for updating the freq table
	}
	
}


int main(){


	in.open("encoded.txt");
	freopen("output.txt", "w",stdout);
	startModel();
	decode();
}
