class Node
{
public:
  virtual void Dump(void) = 0;
};

class TextNode : public virtual Node {
private:
  char *text;
  int leng;
public:
  TextNode(int l, char * t);
  ~TextNode();
  void Dump(void);
};
