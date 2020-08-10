/*
Code for Huffman Encoding and Decoding by Jay and Kunal For CS 254 Project
*/
#include<bits/stdc++.h>
#include<stdio.h>
using namespace std;

map<char,string> final;
unsigned char addCodeToBuffer(vector<int> code, FILE *fp, unsigned char buffer, int *bufferSize) { // writes code in buffer and write it in encoded files when its size is 8  
  int codeSize = code.size();
  unsigned char currBuffer = buffer;
  int i;
  for (auto i: code) {
    unsigned char bit = ((unsigned char)i) << (*bufferSize - 1);
    currBuffer = currBuffer | bit;
    (*bufferSize)--;
    if (*bufferSize == 0) {
      fwrite(&currBuffer, sizeof(unsigned char), 1, fp);
      currBuffer = 0;
      *bufferSize = 8;
    }
  }
  return currBuffer;
}
void writeRemainingBits(FILE *fp, unsigned char buffer, int bufferSize) { // write the remaining bits (when buffer size is not 8) at the last
  if (bufferSize < 8) fwrite(&buffer, sizeof(unsigned char), 1, fp);
  buffer = (bufferSize == 8) ? 8 : 8 - bufferSize;
  fwrite(&buffer, sizeof(unsigned char), 1, fp);
}


int readBit(FILE *fp, unsigned char *buffer, int *bufferSize, long int fileSize, int readHowManyBitsAtTheEnd) { // read single bit from the stream
  if (*bufferSize == 0) {
    *bufferSize = 8;
    fread(buffer, sizeof(unsigned char), 1, fp);
  }
  if (readHowManyBitsAtTheEnd != 8) {
    if (ftell(fp) == fileSize && readHowManyBitsAtTheEnd <= (8 - *bufferSize)) return -1;
  }
  if (ftell(fp) > fileSize || feof(fp)) return -1;
  (*bufferSize)--;
  return (*buffer >> *bufferSize) & 1;
}
// Class for nodes to make the tree
class HuffmanTreeNode{
public:

	HuffmanTreeNode *left; 

	HuffmanTreeNode *right;

	char data; 

	long long fr;  // Its freq

	string code=""; // the code word to store the code word

	HuffmanTreeNode(char c, long long f) // The current node is initialized
	{
		data=c;
		fr=f;
		code="";
		left=right=NULL;
	}
};


class myComp {
public:
    bool operator()(HuffmanTreeNode* a,HuffmanTreeNode* b)  // operator  overloading
    {
        return a->fr>b->fr;  // prioritize on the basis of frequency(for Min-Heap)
    }
};

HuffmanTreeNode* treeTop(priority_queue<HuffmanTreeNode*, vector<HuffmanTreeNode*>, myComp> prq) {// Generating the tree

		while(prq.size()!=1){
      // popping two min freq and making a new parent node for them
			HuffmanTreeNode* left=prq.top(); 
			prq.pop();

			HuffmanTreeNode* right=prq.top(); 
			prq.pop();

			HuffmanTreeNode* node=new HuffmanTreeNode('~', left->fr+right->fr);

			node->left=left;
			node->right=right;

			prq.push(node);

		}

		return prq.top(); // root node is returned
}


void storeCode(HuffmanTreeNode* root){
	if(root->left){
		root->left->code=root->code+'0';
		storeCode(root->left);
	}

	if(root->right){
		root->right->code=root->code+'1';
		storeCode(root->right);
	}

	if (!root->left && !root->right) {
		final.insert(pair<char,string>(root->data,root->code)); // Finally storing the code word
    }

}

priority_queue<HuffmanTreeNode*, vector<HuffmanTreeNode*>, myComp> pq; // to get the root node;

auto huffmanCodes(map<char ,long long> m){

	priority_queue<HuffmanTreeNode*, vector<HuffmanTreeNode*>, myComp> prq; // Declaring the priority queue

	map<char,long long> :: iterator itr;

	for (itr = m.begin(); itr != m.end(); ++itr){
		HuffmanTreeNode* node=new HuffmanTreeNode(itr->first,itr->second); // making a node for each character
		prq.push(node);
	}
	pq=prq;
	HuffmanTreeNode *root=treeTop(prq);
	storeCode(root); // code is in string format stored in the node itself
	return final;
}
// function to encode the input file
void encodeOut(map<char,string > fin){ 
	FILE *fp_in;
  	FILE *fp_out;
  	fp_in = fopen("input.txt","rb");
  	fp_out = fopen("coded.txt","wb");

  	unsigned char buffer = 0;
  	int bufferSize = 8;
  	unsigned char currByte;

  	 while(fread(&currByte,sizeof(unsigned char),1,fp_in)){
  	 	auto it=fin.find(currByte);
  	 	string code=it->second;
  	 	vector<int> cd;
      // storing code
			for(int j=0; j<code.length(); j++) {
				if(code[j] == '1') cd.push_back(1);
				else cd.push_back(0);
				}
		buffer=addCodeToBuffer(cd,fp_out,buffer,&bufferSize);
  	 }
  	 writeRemainingBits(fp_out, buffer, bufferSize);
  	 fclose(fp_out);
  	 fclose(fp_in);

}

// function to decode the encoded message
void decode(){ 
	FILE *fp_in = fopen("coded.txt","rb");
  	FILE *fp_out = fopen("output.txt","wb");

  	unsigned char buffer= 0;
  	int bufferSize = 0;
  	fseek(fp_in,-1,SEEK_END);
  	long int fileSize = ftell(fp_in);
  	unsigned char readHowManyBitsAtTheEnd;
  	fread(&readHowManyBitsAtTheEnd,sizeof(unsigned char),1,fp_in);
  	rewind(fp_in);
  	
  	HuffmanTreeNode *root=treeTop(pq);

  	while(!feof(fp_in)){
	HuffmanTreeNode *currNode=root;
	int endOfFile = 0;
    // Traversing the tree
  	while(currNode-> left != NULL && currNode->right != NULL && !endOfFile){
      int bit = readBit(fp_in,&buffer,&bufferSize,fileSize,readHowManyBitsAtTheEnd);
      if(bit == 0)currNode=currNode->left;
      else if(bit == 1)currNode=currNode->right;
      else endOfFile = 1;
    }
    if(endOfFile)break;
    char c=currNode->data;
    fwrite(&c,sizeof(unsigned char),1,fp_out);
	}
}





int main(){
  FILE *fp_in;
  fp_in = fopen("input.txt","rb");
  
  map<char,long long> freq;
  map<char,long long> :: iterator itr;
  unsigned char currByte;
  while(fread(&currByte,sizeof(unsigned char),1,fp_in)){
  
  			freq[currByte]++;
  	}
  fclose(fp_in);

  map<char,string> fin=huffmanCodes(freq); 
  encodeOut(fin); // calling the function for encoding
  decode();       // calling the function for decoding


}