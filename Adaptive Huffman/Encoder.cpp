/* Code for Adaptive Huffman Encoder by Jay And Kunal for CS 254 Project */
#include<bits/stdc++.h>
using namespace std;
long long ord = 512;
// class to make the tree
class node{
public:
  char c;
  int weight; // denotes frequency
  int order; // order is used for geeting the best node for swapping while updating the tree
  node *left;
  node *right;
  node *parent;
  string code;
  node(char c,long long ord1){
    this->c = c;
    weight = 0;
    order = ord1;
    left = NULL;
    right = NULL;
    parent = NULL;
    code = "";
  }
};
void print(node *header) // printing the tree if required
{
  if(header == NULL)return;
  print(header->left);
  cout<<header->c<<" "<<header->order<<" "<<header->weight<<endl;
  print(header->right);
}

node *c_search(node *header, char c) // searches for a char in tree and return the pointer to it or NULL if the char not found
{
  if(header == NULL)return NULL;
  if(header->c == c)return header;
  node *temp = c_search(header->left,c);
  if(temp != NULL && temp->c == c)return temp;
  temp = c_search(header->right,c);
  if(temp != NULL && temp->c == c)return temp;
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


void swapNodes(node **x, node **y) // swapping of nodes
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

void update_tree(node **header, node **curr) // updating the tree
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
  (currNode->weight)++; // increasing the freq of current char by 1
}
void generate_code(node *head, vector<int> &code) // genrating code 
{
  if(head->parent == NULL)return;
  if(head->parent->left == head)code.push_back(0);
  else code.push_back(1);
  generate_code(head->parent,code);
}

unsigned char addCodeToBuffer(vector<int> code, FILE *fp, unsigned char buffer, int *bufferSize) { //writing the code of char in buffer which then is written to the encoded file
  unsigned char currBuffer = buffer;
  int i;
  for (auto i: code) {
    //cout<<i<<endl;
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
unsigned char addByteToBuffer(char byte, FILE *fp, unsigned char buffer, int *bufferSize) { 
  unsigned char currBuffer = buffer;
  int howManyBitsWillBeWritten = *bufferSize;
  int shiftSize = 8 - howManyBitsWillBeWritten;
  unsigned char tempByte = ((unsigned char) byte) >> shiftSize;
  currBuffer = currBuffer | tempByte;
  fwrite(&currBuffer, sizeof(unsigned char), 1, fp);
  currBuffer = byte << howManyBitsWillBeWritten;
  return (*bufferSize == 8) ? 0 : currBuffer;
}
void writeRemainingBits(FILE *fp, unsigned char buffer, int bufferSize) {
  if (bufferSize < 8) fwrite(&buffer, sizeof(unsigned char), 1, fp);
  buffer = (bufferSize == 8) ? 8 : 8 - bufferSize;
  fwrite(&buffer, sizeof(unsigned char), 1, fp);
}
int main()
{
  FILE *fp_in;
  FILE *fp_out;
  fp_in = fopen("input.txt","rb");
  fp_out = fopen("encoded.txt","wb");
  unsigned char buffer = 0;
  int bufferSize = 8;
  unsigned char currByte;
  node *header = NULL;
  header = new node('-',ord--);
  node *NYT = header;
  map<unsigned char,int> mp;
  // starts encoding
  while(fread(&currByte,sizeof(unsigned char),1,fp_in)){
    vector<int> output;
    if(mp.count(currByte)){
      node *temp = c_search(header,currByte); 
      generate_code(temp,output);
      reverse(output.begin(),output.end());
      buffer = addCodeToBuffer(output,fp_out,buffer,&bufferSize);
      update_tree(&header,&temp); // updating the tree after every char
    }
    else{
      mp[currByte]++;
      generate_code(NYT,output);
      reverse(output.begin(),output.end());
      buffer = addCodeToBuffer(output,fp_out,buffer,&bufferSize);
      buffer = addByteToBuffer(currByte,fp_out,buffer,&bufferSize);
      node *a = new node(currByte,ord--);
      node *b = new node('-',ord--);
      NYT->right = a;
      NYT->left = b;
      NYT->c = '^';
      a->parent = NYT;
      b->parent = NYT;
      NYT = b;
      update_tree(&header,&a); // updating the tree after every char
    }
  }
  writeRemainingBits(fp_out, buffer, bufferSize);
}
