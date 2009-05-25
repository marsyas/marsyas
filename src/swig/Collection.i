%module Collection

%{
#include <marsyas/Collection.h>

using namespace Marsyas;

%}

%include "types.i"


class Collection
{
private:
  std::vector<std::string> collectionList_;
  std::vector<std::string> labelList_;
  std::vector<std::string> labelNames_;
  std::string name_;
  bool hasLabels_;
public:
  Collection();
  ~Collection();
  mrs_natural size();
  void setName(std::string name);
  std::string entry(unsigned int i);
  std::string labelEntry(unsigned int i);
  mrs_natural getSize();
  mrs_natural getNumLabels();
  mrs_string getLabelNames();
  mrs_natural labelNum(mrs_string label);
  mrs_string labelName(mrs_natural i);
  mrs_bool hasLabels();
  void add(std::string entry);
  void add(std::string entry, std::string label);
  std::string name();
  void shuffle();
  void read(std::string filename);
  void write(std::string filename);
  void labelAll(std::string label);
  std::string toLongString();

  void concatenate(std::vector<Collection> cls);

//  friend std::ostream& operator<<(std::ostream&, const Collection&);
//  friend std::istream& operator>>(std::istream&, Collection&);
};



