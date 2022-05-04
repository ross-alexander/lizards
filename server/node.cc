#include "node.h"
#include <iostream>
#include <string.h>

TextNode::TextNode(int l, char *t)
{
  text = new char[l + 1];
  strcpy(text, t);
};

TextNode::~TextNode()
{
  delete text;
};

void TextNode::Dump(void)
{
  std::cout << text;
}
