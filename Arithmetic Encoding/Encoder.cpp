/* Code for Arithmetic Encoding by Kunal and Jay For CS 254 Project */
#include<bits/stdc++.h>
using namespace std;

const int CODE_VALUE = 16;
const int MAX_VALUE = ((1 << CODE_VALUE) - 1);
const int MAX_FREQ = 16383; // 2^14-1

const int FIRST_QTR = (MAX_VALUE / 4 + 1);
const int HALF = (2 * FIRST_QTR);
const int THIRD_QTR = (3 * FIRST_QTR);

const int NO_OF_CHARS = 256;
const int EOF_SYMBOL = (NO_OF_CHARS + 1);
const int NO_OF_SYMBOLS = (NO_OF_CHARS + 1);

unsigned char index_to_char [NO_OF_SYMBOLS]; // output the respective char according to index
int char_to_index [NO_OF_CHARS]; // gives index of symbol
int cum_freq [NO_OF_SYMBOLS + 1]; // stores cummulative fre of the symbols
int freq [NO_OF_SYMBOLS + 1]; //stores the freq of symbol


int low,high;
int pending_bits;

int buffer = 0;
int bits_in_buf = 0;



FILE * f_in;
ofstream out;
// Wrting single bits in buffer and when we get a packet of 8, buffer is written in encoded file
void write_bit(int bit)
{
	buffer >>= 1;
	if (bit)
	buffer |= 0x80;
	bits_in_buf++;
	if (bits_in_buf == 8)
	{
		cout<<(char)(buffer);
		bits_in_buf = 0;
	}
}
// function fro printing the pending bits
void output_bits(int bit)
{
	write_bit(bit);
	while (pending_bits > 0)
	{
		write_bit(!bit);
		pending_bits--;
	}
}
// at the end od encoding we write the remaining pending bits
void end_encoding(void)
{
  pending_bits++;
  if (low < FIRST_QTR)
    output_bits(0);
  else
    output_bits(1);
	
  cout<<(char)(buffer >> bits_in_buf);
}

// initializing the model
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

// updating the model after each character is encountered
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
// encoding each symbol
void encode_symbol(int symbol)
{
	int range;

	range = high - low;
	// slicing the range according to symbol probability
	high = low + (range * cum_freq [symbol - 1]) / cum_freq [0];
	low = low + (range * cum_freq [symbol]) / cum_freq [0];
	// normalization
	while(true)
	{
	if (high < HALF)
		output_bits(0);
	else if (low >= HALF)
	{
		output_bits(1);
		low -= HALF;
		high -= HALF;
	}
	else if (low >= FIRST_QTR && high < THIRD_QTR)
	{
		pending_bits++;
		low -= FIRST_QTR;
		high -= FIRST_QTR;
	}
	else
		break;
	low = 2 * low;
	high = 2 * high;
	}
}
// function to read the input file and encode
void encode(){

	low=0;
	high=MAX_VALUE;
	pending_bits=0;

	unsigned char currByte;

    while(fread(&currByte,sizeof(unsigned char),1,f_in)){

    	int symbol;
    	//cout<<currByte<<endl;
    	symbol=char_to_index[(int)currByte];
    	//cout<<symbol<<endl;
    	encode_symbol(symbol);
    	update_tables(symbol);// updating the table after 

    }
    encode_symbol(EOF_SYMBOL);
    end_encoding();// writing the remaining bits
    fclose(f_in);
    out.close();

}


int main(){
	
	f_in=fopen("input.txt", "rb");

	freopen("encoded.txt","w",stdout);

	startModel();
	encode();

}
