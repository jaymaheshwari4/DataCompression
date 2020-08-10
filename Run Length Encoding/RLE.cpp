/* Run Length Encoding and Decoding code by Jay and Kunal for CS 254 Project */


#include<bits/stdc++.h>

using namespace std;

void encoder() // RLE encoder
{
  FILE *fp_in = fopen("input.txt","rb");
  ofstream of;
  of.open("encoded.txt");
  char c;
  long long count = 1;
  c = fgetc(fp_in);
  of<<c;
  while(!feof(fp_in)){
    char c1 = fgetc(fp_in);
    if(c1 == c)count++;
    else{
      if(c1 != '\n' && count>=2){ // print on;y if count >=2 guarantees the encoded file size won't be greater then input file size 
        of<<count;
      }
      if(c1 == EOF)break;
      if(c1 != '\n')of<<c1;
      c = c1;
      count=1;
    }
  }
  fclose(fp_in);
  of.close();
}
void decoder() // RLE decoder
{
  ifstream infile;
  ofstream of;
  of.open("output.txt");
  infile.open("encoded.txt");
  string text;

  while(getline(infile,text)){
    string number;
    char c;
    if((int)text[0]>=48 && (int)text[0]<=57){
      string enter;
      enter.push_back(text[0]);
      int ck = 1;
      while((int)text[ck]>=48 && (int)text[ck]<=57){
        enter.push_back(text[ck++]);
      }
      reverse(enter.begin(),enter.end());
      long long enter1 = stoi(enter);
      while(enter1--)of<<"\n";
      of<<text[ck];
      c = text[ck];

      for(long long i = ck+1; i<text.size(); i++){
        if(!(text[i] >= '0' && text[i] <= '9')){
          long long x = 0;
          if(number.size()>0)x = stoi(number); // converting the string to integer to print the char x number of times
          while(x>1){
            of<<c;
            x--;
          }
          number.clear();
          c = text[i];
          of<<text[i];
        }
        else number.push_back(text[i]); // storing the number in string
      }
    }

    else{
      of<<text[0];
      c = text[0];
      for(long long i = 1; i<text.size(); i++){
        if(!(text[i] >= '0' && text[i] <= '9')){
          long long x = 0;
          if(number.size()>0)x = stoi(number);
          while(x>1){
            of<<c;
            x--;
          }
          number.clear();
          c = text[i];
          of<<text[i];
        }
        else number.push_back(text[i]);
      }
    }
    long long x = 0;
    if(number.size()>0)x = stoi(number);
    while(x>1){
      of<<c;
      x--;
    }
  }
  infile.close();
  of.close();
}
int main()
{
  
  encoder();
  
  //decoder();
  return 0;
}
