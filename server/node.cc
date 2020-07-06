#include "node.h"
#include <stream.h>
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
  cout << text;
}
