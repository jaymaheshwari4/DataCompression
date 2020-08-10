/* Encoder for Word by Word algorithm by Jay and Kunal for CS 254 Project*/
#include<bits/stdc++.h>
#include <fstream>
using namespace std;
long long ord = 512;
long long e = 0,r = 0;
//node class to make the tree
class node{
public:
  string c;
  int weight; // denotes frequency
  int order; // freq of two node can be same but the order can't be same, hence useful for searching the nodes
  node *left;
  node *right;
  node *parent;
  node(string c,long long ord1){
    this->c = c;
    weight = 0;
    order = ord1;
    left = NULL;
    right = NULL;
    parent = NULL;
  }
};
vector<pair<string, long long>> med;
map <string ,long long> dictionary;

void print(node *header) // printing of tree if required
{
  if(header == NULL)return;
  print(header->left);
  cout<<header->c<<" "<<header->order<<" "<<header->weight<<endl;
  print(header->right);
}

node *s_search(node *header, string c) // searches for a word in tree and return the pointer to it or NULL if node not found
{
  if(header == NULL)return NULL;
  if(header->c == c)return header;
  node *temp = s_search(header->left,c);
  if(temp != NULL && temp->c == c)return temp;
  temp = s_search(header->right,c);
  if(temp != NULL && temp->c == c)return temp;
  return NULL;
}
node * f_order(node* header, node *curr) //searches for a node with highest order and with weight greater or equal to the current node 
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


void swapNodes(node **x, node **y)  // swapping two nodes
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

void update_tree(node **header, node **curr) // updating the tree after each word is encoded
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
void generate_code(node *head, vector<int> &code) // generates the code
{
  if(head->parent == NULL){
    return;
  }
  if(head->parent->left == head){
    code.push_back(0);
  }
  else{
    code.push_back(1);
  }
  generate_code(head->parent,code);
}

unsigned char writeCode(vector<int> code,unsigned char buffer,int *bufferSize, FILE *fp_out) // writing the code of the word in buffer which then is written to the encoded file 
{
  unsigned char currBuff = buffer;
  for(auto i: code){
    unsigned char bit = ((unsigned char)i)<<(*bufferSize - 1);
    currBuff |= bit;
    (*bufferSize)--;
    if(*bufferSize == 0){
      fwrite(&currBuff, sizeof(unsigned char), 1, fp_out);
      currBuff = 0;
      *bufferSize = 8;
    }
  }
  return currBuff;
}
long long findIndex(string code)  // gets the index of the node from dictionary
{ return dictionary[code];
  
  }
unsigned char writeIndex(string code, unsigned char buffer,int *bufferSize, FILE *fp_out) // writing the index of the word in buffer which then is written to the encoded file
{
  int pt = 0;
  vector<int>index;
  long long in = findIndex(code);
  if(in > 2*r)index.resize(e);
  else index.resize(e+1);
  for(int i=0;i<index.size();i++)index[i]=0;
  long long ck_temp = med[in-1].second;
  while(ck_temp>0){
    index[pt++] = ck_temp%2;
    ck_temp /= 2;
  }
  reverse(index.begin(),index.end());
  unsigned char currbuff = writeCode(index,buffer,bufferSize,fp_out);
  return currbuff;
}
void writeRemainingBits(FILE *fp, unsigned char buffer, int bufferSize) {
  if (bufferSize < 8) fwrite(&buffer, sizeof(unsigned char), 1, fp);
  buffer = (bufferSize == 8) ? 8 : 8 - bufferSize;
  fwrite(&buffer, sizeof(unsigned char), 1, fp);
}
int main()
{

  FILE *fp_out;
  fp_out = fopen("encoded.txt","wb");
  ifstream infile;
  infile.open("input.txt"); // opening the input file for the first time to make dictionary
  string data;
  ofstream intermediate;
  intermediate.open("temp.txt"); // the intermediate file
  map<string,long> check;
  
  while(infile>>data){
    if(!check.count(data)){
      intermediate<<data<<" ";
      med.push_back(make_pair(data,0));
      check[data]++;
    }
  }
  long long totalWords = med.size();
  e = log2(totalWords);
  r = totalWords - pow(2,e);
  long long ck = 1;
  // 
  for(int i=0;i<med.size();i++){
    if(i+1<=2*r){
      med[i].second = ck-1;
    }
    else{
      med[i].second = ck - r - 1;
    }
    ck++;
    dictionary[med[i].first]=i+1; // storing the indexes in the dictionary
  }

  ord = 2*totalWords+100;
  infile.close();
  infile.open("input.txt"); // opening for the second time to encode
  node *head = new node("-",ord--);
  node *NYT = head;
  unsigned char buffer = 0;
  int bufferSize = 8;
  while(infile>>data){
    node *temp = s_search(head,data); //searching for the word
    vector<int> code;
    if(temp == NULL){ // if word not found
      generate_code(NYT,code);
      reverse(code.begin(),code.end());
      buffer = writeCode(code,buffer,&bufferSize,fp_out);
      buffer = writeIndex(data,buffer,&bufferSize,fp_out);
      node *a = new node(data,ord--);
      node *b = new node("-",ord--);
      NYT->left = b;NYT->right = a;
      NYT->c = "?";
      a->parent = b->parent = NYT;
      NYT = b;
      update_tree(&head,&a); // updating the tree after each word
    }
    else{ // if the word is found
      generate_code(temp,code);
      reverse(code.begin(),code.end());
      buffer = writeCode(code,buffer,&bufferSize,fp_out);
      update_tree(&head,&temp); // updating the tree after each word
    }
  }
  writeRemainingBits(fp_out, buffer,bufferSize);
}
