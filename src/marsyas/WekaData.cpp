#include "WekaData.h"

using namespace std;
using namespace Marsyas;

//This class represents a collection of row data read from a weka arff file.
//It is organized as a vector collection of vector pointers.
//It is optimized for fast sorting and shuffling of the data. It is not intended
//that the data change once it is loaded.
//
//It is also assumed that the last column of each row is the class attribute.
//All data items are mrs_real, including the class attribute, however the class
//attribute should be interpreted as an mrs_natural.
WekaData::WekaData():cols_(0),rows_(0)
{
}

WekaData::~WekaData()
{
}

//create the table. Will clear contents first and fix the number of columns.
void WekaData::Create(mrs_natural cols)
{
	MRSASSERT(cols>=0);
	this->Clear();
	cols_ = cols;
	rows_ = 0;
}

//clear all data from the table
//Requires that the vector rows be freed
void WekaData::Clear()
{
  
	for(vector<vector<mrs_real>*>::iterator iter = this->begin(); iter!=this->end(); iter++)
	{
		delete (*iter);
	}
	this->clear();
}//Clear


void 
WekaData::NormMaxMinRow(realvec& in)
{
  int ii;
  for(ii=0; ii<(int)in.getSize()-1; ii++)
    {
      in(ii) =  (in(ii) - minimums_(ii)) / (maximums_(ii) - minimums_(ii));
    }
}

void 
WekaData::NormMaxMin()
{
  minimums_.create(cols_-1);
  maximums_.create(cols_-1);
  maximums_.setval(DBL_MIN);
  minimums_.setval(DBL_MAX);
  
  // find minimums_ and maximums_ 
  for(vector<vector<mrs_real>*>::const_iterator citer = this->begin(); citer!=this->end(); citer++)
    {
      const vector<mrs_real> *row = (*citer);
      int ii;
      for(ii=0; ii<(int)row->size()-1; ii++)
	{
	  if (row->at(ii) > maximums_(ii))
	    maximums_(ii) = row->at(ii);
	  if (row->at(ii) < minimums_(ii))
	    minimums_(ii) = row->at(ii);
	}
    }
  

  // normalize 
  for(vector<vector<mrs_real>*>::const_iterator citer = this->begin(); citer!=this->end(); citer++)
    {
      vector<mrs_real> *row = (*citer);
      int ii;
      for(ii=0; ii<(int)row->size()-1; ii++)
	{
	  row->at(ii) =  ((row->at(ii) - minimums_(ii)) / (maximums_(ii) - minimums_(ii)));
	}
    }




}

mrs_realvec WekaData::GetMinimums() const
{
 	return minimums_;
}
 
mrs_realvec WekaData::GetMaximums() const
{
 	return maximums_;
}


//randomly shuffle the data in the table
//Need only to swap the pointers to row data, nice and fast!
void WekaData::Shuffle()
{
	mrs_natural size = this->size()-1;
	for (mrs_natural ii=0; ii<size; ii++)
	{
		mrs_natural rind = (mrs_natural)(((mrs_real)rand() / (mrs_real)(RAND_MAX))*size);
		//swap row ii with row rind
		swapRows(ii, rind);
	}//for ii
}//Shuffle

//SwapRows will exchange one row for another.
//Just need to swap the 2 vector pointers.
void WekaData::swapRows(mrs_natural l, mrs_natural r)
{
	vector<mrs_real> *temp = this->at(l);
	this->at(l) = this->at(r);
	this->at(r) = temp;
}

mrs_natural WekaData::partition(mrs_natural attIndex, mrs_natural l, mrs_natural r)
{
	mrs_real pivot = this->at((l+r)/2)->at(attIndex);
	while (l < r)
	{
		while ((this->at(l)->at(attIndex) < pivot) && (l < r))
		{
			l++;
		}//while

		while ((this->at(r)->at(attIndex) > pivot) && (l < r))
		{
			r--;
		}//while

		if (l < r)
		{
			swapRows(l, r);
			l++;
			r--;
		}//if
    }
	if ((l == r) && (this->at(r)->at(attIndex) > pivot))
	{
		r--;
	} //if

	return r;
}//partition

  /**
   * Implements quicksort according to Manber's "Introduction to
   * Algorithms".
   *
   * @param attIndex the attribute's index
   * @param left the first index of the subset to be sorted
   * @param right the last index of the subset to be sorted
   */
  //@ requires 0 <= attIndex && attIndex < numAttributes();
  //@ requires 0 <= first && first <= right && right < numInstances();
  //Shamelessly ripped off from the weka library of code. - dale
void WekaData::quickSort(mrs_natural attIndex, mrs_natural left, mrs_natural right)
{
	if (left < right)
	{
		int middle = partition(attIndex, left, right);
		quickSort(attIndex, left, middle);
		quickSort(attIndex, middle + 1, right);
	}//if
}//quicksort

//Sort the instances dataset based on the column attr
//Note that the entire table must be sorted on the attribute,
//not just the attribute itself.
void WekaData::Sort(mrs_natural attr)
{
	MRSASSERT(attr>=0&&attr<cols_);
	quickSort(attr, 0, this->size()-1);
}

//add rows of data to the table
void WekaData::Append(const realvec& in)
{
  MRSASSERT(in.getRows()==cols_);

	mrs_natural count=0;
	//count number of labeled instances 
	for(mrs_natural i=0; i < in.getRows(); ++i)
	{
		if(in(i, in.getRows()-1)>=0)
			count++;
	}
  vector<mrs_real> *data = new vector<mrs_real>(count);
	count = 0;
  for(mrs_natural ii=0; ii<in.getRows(); ii++)
    {
      //only store labeled instances
			if(in(ii, in.getRows()-1)>=0)
			{
				data->at(count++) = in(ii, 0);
			}
    }
  Append(data);
}//Append

//add rows of data to the table
void WekaData::Append(vector<mrs_real> *data)
{
  rows_++;
	MRSASSERT(data!=NULL&&data->size()==cols_);
	this->push_back(data);
}//Append

//get the class attribute for a row and convert to a int
//class attribute is last column of row
mrs_natural WekaData::GetClass(mrs_natural row) const
{
	return (mrs_natural)this->at(row)->at(cols_-1);
}

//debug helper funtion to dump table to an ascii file
void WekaData::Dump(const string& filename, const vector<string>& classNames) const
{
	char buffer[32];

	ofstream *mis = new ofstream;

	mis->open(filename.c_str(), ios_base::out | ios_base::trunc );
	MRSASSERT( mis->is_open() );

	for(vector<vector<mrs_real>*>::const_iterator citer = this->begin(); citer!=this->end(); citer++)
	{
		bool first = true;
		const vector<mrs_real> *row = (*citer);
		int ii;
		for(ii=0; ii<(int)row->size()-1; ii++)
		{
			if(!first)
				mis->write(", ", 2);
			first = false;

			sprintf(buffer, "%09.4f", row->at(ii));
			mis->write(buffer, strlen(buffer));
		}
		mis->write(", ", 2);
		mrs_natural classIndex = (mrs_natural)row->at(ii);
		mis->write(classNames[classIndex].c_str(), strlen(classNames[classIndex].c_str()));
		mis->write("\n", 1);
	}

	mis->close();
}//Dump
