/* Code for Adaptive Huffman Decoder by Jay And Kunal for CS 254 Project */
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
  cout<<header->c<<endl;
  print(header->right);
}

node *c_search(node *header, char c)  // searches for a char in tree and return the pointer to it or NULL if char not found
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
int readBit(FILE *fp, unsigned char *buffer, int *bufferSize, long int fileSize, int readHowManyBitsAtTheEnd) { // read single bit from the current buffer
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
char readByte(FILE *fp, unsigned char *buffer, int *bufferSize, long int fileSize, int readHowManyBitsAtTheEnd) { // reading the whole byte(8 bits)
  char result = 0;
  int i, bit;
  for (i = 0; i < 8; i++) {
    bit = readBit(fp, buffer, bufferSize, fileSize, readHowManyBitsAtTheEnd);
    bit = bit << (7-i);
    result |= bit;
  }
  return result;
}
int main()
{
  FILE *fp_in = fopen("encoded.txt","rb");
  FILE *fp_out = fopen("output.txt","wb");
  node *head = new node('-',ord--);
  node *NYT = head;
  unsigned char buffer= 0;
  int bufferSize = 0;
  fseek(fp_in,-1,SEEK_END);
  long long fileSize = ftell(fp_in);
  unsigned char readHowManyBitsAtTheEnd;

  fread(&readHowManyBitsAtTheEnd,sizeof(unsigned char),1,fp_in);
  rewind(fp_in);
  // starting the decoding
  while(!feof(fp_in)){
    node *currNode = head;
    int endOfFile = 0;
    // traversing the current tree
    while(currNode-> left != NULL && currNode->right != NULL && !endOfFile){
      int bit = readBit(fp_in,&buffer,&bufferSize,fileSize,readHowManyBitsAtTheEnd);
      if(bit == 0)currNode=currNode->left;
      else if(bit == 1)currNode=currNode->right;
      else endOfFile = 1;
    }
    if(endOfFile)break;
    unsigned char c;
    if(currNode == NYT){
      c = readByte(fp_in,&buffer,&bufferSize,fileSize,readHowManyBitsAtTheEnd);
      node *a = new node(c,ord--);
      node *b = new node('-',ord--);
      NYT->left = b;
      NYT->right = a;
      NYT->c = '^';
      a->parent = NYT;
      b->parent = NYT;
      NYT = b;
      currNode = a;
    }
    else c = currNode->c;
    fwrite(&c,sizeof(unsigned char),1,fp_out);
    update_tree(&head,&currNode); // updating the tree after printing the char
  }
}
