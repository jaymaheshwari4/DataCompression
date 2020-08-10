/* Decoder for Word by Word algorithm by Jay and Kunal for CS 254 Project*/
#include<bits/stdc++.h>
#include<stdio.h>
using namespace std;
long long ord = 0;
long long e=0,r=0;
// class node for making the tree
class node{
public:
  string c;
  int weight; // denotes frequency
  int order; // freq of two node can be same but the order can't be same, hence useful for searching the nodes
  node *left;
  node *right;
  node *parent;
  string code;
  node(string c,long long ord1){
    this->c = c;
    weight = 0;
    order = ord1;
    left = NULL;
    right = NULL;
    parent = NULL;
    code = "";
  }
};

void print(node *header)  // printing of tree if required
{
  if(header == NULL)return;
  print(header->left);
  cout<<header->c<<endl;
  print(header->right);
}

node *c_search(node *header, string c) // searches for a word in tree and return the pointer to it or NULL if node not found
{
  if(header == NULL)return NULL;
  if(header->c == c)return header;
  node *temp = c_search(header->left,c);
  if(temp != NULL && temp->c == c)return temp;
  temp = c_search(header->right,c);
  if(temp != NULL && temp->c == c)return temp;
}
node * f_order(node* header, node *curr)//searches for a node with highest order and with weight greater or equal to the current node 
{
  node *temp = curr;
  if(header->weight>temp->weight && (header->left != NULL && header->right != NULL)){
    node *a = f_order(header->left,temp);
    if(a != NULL)temp = a;
    a = f_order(header->right,temp);
    if(a)temp = a;
  }
  else if(header->weight == temp->weight && header->order > temp->order)temp = header;
  return (temp != curr) ? temp : NULL;
}


void swapNodes(node **x, node **y) // swapping two nodes
{
  node *n1 = *x;
  node *n2 = *y;
  int temp = n1->order;
  n1->order = n2->order;
  n2->order = temp;
  pair<node*,int> p[2];
  if(n1->parent->left == n1){
    p[0].first = n1;
    p[0].second = 0;
  }
  else{
    p[0].first = n1;
    p[0].second = 1;
  }
  if(n2->parent->left == n2){
    p[1].first = n2;
    p[1].second = 0;
  }
  else{
    p[1].first = n2;
    p[1].second = 1;
  }
  if(p[0].second == 0){
    n1->parent->left = n2;
  }
  else n1->parent->right = n2;
  if(p[1].second == 0){
    n2->parent->left = n1;
  }
  else n2->parent->right = n1;
  node *temp1 = n1->parent;
  n1->parent = n2->parent;
  n2->parent = temp1;
}

void update_tree(node **header, node **curr)  // updating the tree after each word is encoded
{
  node *root = *header;
  node *currNode = *curr;
  while (currNode->parent != NULL) {
    node *replaceNode = f_order(root, currNode);
    if (replaceNode && currNode->parent != replaceNode) {
        swapNodes(&currNode, &replaceNode);
    }
    currNode->weight++;
    currNode = currNode->parent;
  }
  (currNode->weight)++; // increasing the freq for the current word by 1
}
int readBit(FILE *fp, unsigned char *buffer, int *bufferSize, long int fileSize, int readHowManyBitsAtTheEnd) { // reads single bit from the current buffer
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

string readByte(vector<pair<string,long long>> dictionary,FILE *fp, unsigned char *buffer, int *bufferSize, long int fileSize, int readHowManyBitsAtTheEnd) { // reads the next e or e+1 bits
  unsigned long long result = 0;
  unsigned long long i, bit;
  for (i = 0; i < e; i++) {
    bit = readBit(fp, buffer, bufferSize, fileSize, readHowManyBitsAtTheEnd);
    bit = bit << (e-i-1);
    result |= bit;
  }

  if(result<r){
    bit = readBit(fp,buffer,bufferSize,fileSize,readHowManyBitsAtTheEnd);
    result <<= 1;
    result |= bit;
  }
  else{
    result += (r);
  }
  return dictionary[result].first;
}
int main()
{
  ofstream outfile;
  outfile.open("output.txt");
  ifstream infile;
  infile.open("temp.txt");
  FILE *fp_in = fopen("encoded.txt","rb");
  
  string data;
  map<string,long> check;
  vector<pair<string, long long>> dictionary;
  // making of the dictionary
  while(infile>>data){
    if(!check.count(data)){
      dictionary.push_back(make_pair(data,0));
      check[data]++;
    }
  }
  long long totalWords = dictionary.size();
  e = log2(totalWords);
  r = totalWords - pow(2,e);
  long long ck = 1;
  for(int i=0;i<dictionary.size();i++){
    if(i+1<=2*r){
      dictionary[i].second = ck-1;
    }
    else{
      dictionary[i].second = ck - r - 1;
    }
    ck++;
  }
  ord = 2*totalWords+100;
  node *head = new node("-",ord--);
  node *NYT = head;
  unsigned char buffer = 0;
  int bufferSize = 0;
  fseek(fp_in,-1,SEEK_END);
  long long fileSize = ftell(fp_in);
  unsigned char readHowManyBitsAtTheEnd;
  fread(&readHowManyBitsAtTheEnd,sizeof(unsigned char),1,fp_in);
  rewind(fp_in);

  // Starts reading the encoded file
  while(!feof(fp_in)){
    node *currNode = head;
    int endOfFile = 0;

    // Traversing the tree
    while(currNode-> left != NULL && currNode->right != NULL && !endOfFile){
      int bit = readBit(fp_in,&buffer,&bufferSize,fileSize,readHowManyBitsAtTheEnd);
      if(bit == 0)currNode=currNode->left;
      else if(bit == 1)currNode=currNode->right;
      else endOfFile = 1;
    }
    if(endOfFile)break;
    string c;
    // adding the node to the tree
    if(currNode == NYT){
      c = readByte(dictionary,fp_in,&buffer,&bufferSize,fileSize,readHowManyBitsAtTheEnd);
      node *a = new node(c,ord--);
      node *b = new node("-",ord--);
      NYT->left = b;
      NYT->right = a;
      NYT->c = "?";
      a->parent = NYT;
      b->parent = NYT;
      NYT = b;
      currNode = a;
    }
    else c = currNode->c;
    outfile<<c<<" "; // writing the decoded word
    
    update_tree(&head,&currNode);
  }
}
