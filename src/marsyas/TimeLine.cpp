/*
** Copyright (C) 1998-2004 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "TimeLine.h"

using namespace std;
using namespace Marsyas;

TimeLine::TimeLine()
{
	srate_ = 0;
	lineSize_ = 0;
	size_ = 0;
	classNames_.reserve(100);
	filename_ = "Unknown";
}

TimeLine::~TimeLine()
{

}

/*
void
TimeLine::color(Feature_map &map)
{
int i;
float_vector f1 = map.get_feat(9);
float_vector f2 = map.get_feat(0);
float_vector f3 = map.get_feat(3);
float r;
float g;
float b;

Gen_Plotter plotter;  
//f1.norm(0.5, 0.2);
//f2.norm(0.5, 0.2);
//f3.norm(0.5, 0.2);

f1.norm(0.00074, 0.0017, 0.5, 0.2);
f2.norm(40.5, 20.5, 0.5, 0.2);
f3.norm(5.63, 2.71, 0.5, 0.2);

//plotter.plot_wait(f1);
//plotter.plot_wait(f2);
//plotter.plot_wait(f3);  

int start;
int end;
for (i=0; i<num_of_regions_; i++)
{
start = region_[i].start;
end = region_[i].end;
r = fabs(f1.mean(start,end));
g = fabs(f2.mean(start,end));
b = fabs(f3.mean(start,end));

//r = 0.5;
//g = 0.5;
//b = 0.5;

// Clip to colors
if (r > 1.0) r = 1.0;
if (g > 1.0) g = 1.0;      
if (b > 1.0) b = 1.0;      
if (r < 0.0) r = 0.0;
if (g < 0.0) g = 0.0;      
if (b < 0.0) b = 0.0;      

region_[i].color.setRGB(r,g,b);
}

}
*/

void 
TimeLine::regular(unsigned int spacing, unsigned int size)
{
	if (size_ != 0)
	{
		cerr << "TimeLine has data already \n";
		return;
	}

	size_ = size;
	int reg_index = 0;
	int i;
	lineSize_ = 2048;
	if ((size_ % spacing) != 0)
		numRegions_ = (size_ / spacing) + 1;
	else
		numRegions_ = (size_ / spacing);


	for (i=0; i < numRegions_; i++)
	{
		TimeRegion region;
		regions_.push_back(region);
	}

	for (i=0; i<size_; i++)
	{
		if ((i % spacing) == 0)
		{
			if (reg_index > 0) regions_[reg_index-1].end = i;
			regions_[reg_index].start = i;
			// regions_[reg_index].classId = reg_index+1;
			regions_[reg_index].classId = 0;
			reg_index++;
		}
	}  
	regions_[numRegions_-1].end = size_;
	regions_[reg_index-1].end = size_;

}

void
TimeLine::scan(realvec segmentation)
{
	int i;
	int peakCount=0;

	if (size_ != 0)
	{
		cerr << "TimeLine has data already \n";
		return;
	}

	size_ = segmentation.getSize();
	for (i=0; i<size_; i++)
	{
		if (segmentation(i) == 1)
			peakCount++;
	}
	numRegions_ = peakCount-1;
	lineSize_ = 512;			// must change to argument [!]

	for (i=0; i < numRegions_; i++)
	{
		TimeRegion region;
		regions_.push_back(region);
	}


	int reg_index = 0;
	for (i=0; i<size_; i++)
	{
		if (segmentation(i) == 1)
		{
			if (reg_index > 0) regions_[reg_index-1].end = i;
			if (reg_index == peakCount -1) break;
			regions_[reg_index].start = i;
			regions_[reg_index].classId = 0;
			reg_index++;
		}
	}
}

/* get_num_of_regions:
Returns the number of regions of this timeline. 
*/
int
TimeLine::numRegions()
{
	return numRegions_;
}

unsigned int
TimeLine::numClasses()
{
	vector<unsigned int> classes;
	bool found = false;
	
	for(mrs_natural i = 0; i < numRegions_; ++i)
	{
		found = false;
		//check if this region's class Id was already counted
		for(unsigned int c = 0; c < (unsigned int)classes.size(); ++c)
		{
			if(classes[c] == regions_[i].classId)
			{
				found = true;
				break;
			}
		}
		//if this is a new classId, count it
		if(!found)
			classes.push_back(regions_[i].classId);
	}

	return (unsigned int)classes.size();
}


int 
TimeLine::start(int regionNum)
{
	if (regionNum < numRegions_)
		return regions_[regionNum].start;
	else return 0;
}

string 
TimeLine::name(int regionNum)
{
	if (regionNum < numRegions_)
		return regions_[regionNum].name;
	else return "";
}

void 
TimeLine::setName(int regionNum, string name)
{
	if (regionNum < numRegions_)
		regions_[regionNum].name = name;
}

void 
TimeLine::setClassId(int regionNum, int classId)
{
	if (regionNum < numRegions_)
	{
		regions_[regionNum].classId = classId;
	}
}

int 
TimeLine::end(int regionNum)
{
	if (regionNum < numRegions_)
		return regions_[regionNum].end;
	else return 0;
}

void 
TimeLine::smooth(unsigned int smoothSize)
{
	TimeRegion region;
	TimeRegion pregion;
	TimeRegion nregion;

	for (int i=1; i < numRegions_-1; i++)
	{
		region = regions_[i];
		pregion = regions_[i-1];
		nregion = regions_[i+1];

		if ((region.end - region.start < smoothSize) && (region.classId == 1))
		{
			// if ((pregion.end - pregion.start) > (nregion.end - nregion.start))
			// {
			remove(i);
			i = i-1;
			// }
			// else 
			// remove(i+1);
		}
	}

	for (int i=1; i < numRegions_; i++)
	{
		region = regions_[i];
		pregion = regions_[i-1];

		if (region.classId == pregion.classId)
		{
			remove(i);
			i = i-1;
		}
	}
}

void 
TimeLine::remove(int regionNum)
{

	if (regionNum >= 1) 
	{
		regions_[regionNum-1].end = regions_[regionNum].end;
		regions_.erase(regions_.begin() + regionNum);
		numRegions_--;
	}
}

int 
TimeLine::getRClassId(int regionNum)
{
	if (regionNum < numRegions_)
		return regions_[regionNum].classId;
	else 
		return 0;
}

unsigned int
TimeLine::getClassId(unsigned int index)
{
	TimeRegion region;
	for (int i=0; i < numRegions_; i++)
	{
		region = regions_[i];
		if ((region.start <= index) && (index < region.end))
		{
			return region.classId;
		}

	}
	return 0;
}

/* load:
Load a TimeLine from a file. 
*/
void
TimeLine::load(string filename)
{
	ifstream in;
	filename_ = filename;

	in.open(filename.c_str());
	if(!in.is_open())
		cerr << "Problem opening file " << filename_;

	in >> numRegions_; // read numRegions
	cerr << "Number of regions is " << numRegions_ << endl;

	in >> lineSize_; //read lineSize
	cerr << "Skip size is " << lineSize_ << endl;

	in >> size_; //read size
	cerr << "Size is " << size_ << endl;

	for (int i=0; i < numRegions_; i++)
	{
		TimeRegion region;
		regions_.push_back(region);
	}

	for (int i=0; i<numRegions_; i++)
	{
		unsigned int token;
		string stoken1, stoken2;
		in >> token;
		regions_[i].start = token;
		in >> token;
		regions_[i].classId = token;
		in >> token;
		regions_[i].end = token;
		in >> stoken1 >> stoken2;
		regions_[i].name = stoken1 +" "+stoken2;
	}
}

void
TimeLine::info()
{
	int i;
	cerr << "Number of regions = " << numRegions_ << endl;
	cerr << "Skip size  = " << lineSize_ << endl;
	cerr << "TimeLine size (# skip_size_ blocks ) = " << size_ << endl;

	for (i=0; i < numRegions_; i++)
	{
		cerr << "Region " << i << " start    = " << regions_[i].start << endl;
		cerr << "Region " << i << " class id = " << regions_[i].classId << endl;
		cerr << "Region " << i << " end      = " << regions_[i].end   << endl;
	}
}

void 
TimeLine::printnew(FILE *fp)
{
	int i;
	fprintf(fp, "%d\n", numRegions_);
	fprintf(fp, "%d\n", lineSize_);
	fprintf(fp, "%d\n", size_);

	for (i=0; i<numRegions_; i++)
	{
		// convert to milliseconds 
		float smsec;
		float emsec;
		smsec = ((regions_[i].start * lineSize_ * 1.0f) / 22050.0f)*10000;
		// fprintf(fp, "%d ", regions_[i].start);
		fprintf(fp, "%6.0f ", smsec);

		fprintf(fp, "%d ", regions_[i].classId);
		emsec = ((regions_[i].end * lineSize_ * 1.0f) / 22050.0f) * 1000;
		// fprintf(fp, "%d ", regions_[i].end);
		fprintf(fp, "%6.0f\n", emsec);

		fprintf(fp, "%s\n", regions_[i].name.c_str());
	}
}

void 
TimeLine::write(string filename)
{
	ofstream os(filename.c_str());
	os << (*this) << endl;
}

ostream&
Marsyas::operator<<(ostream& o, const TimeLine& tline)
{
	o << tline.numRegions_ << endl;
	o << tline.lineSize_ << endl;
	o << tline.size_ << endl;

	for (int i=0; i<tline.numRegions_; i++)
	{
		o << tline.regions_[i].start ;
		o << " " << tline.regions_[i].classId;
		o << " " << tline.regions_[i].end << endl;
		o << "Region " << i+1 << endl;
	}
	return o;
}

void 
TimeLine::print(FILE *fp)
{
	int i;
	fprintf(fp, "%d\n", numRegions_);
	fprintf(fp, "%d\n", lineSize_);
	fprintf(fp, "%d\n", size_);

	for (i=0; i<numRegions_; i++)
	{
		fprintf(fp, "%d ", regions_[i].start);
		fprintf(fp, "%d ", regions_[i].classId);
		fprintf(fp, "%d\n", regions_[i].end);
		fprintf(fp, "Region %d\n", i+1);
	}
}

/*
void 
TimeLine::print_mp3(FILE *fp)
{
verbose("TimeLine::print(FILE *fp)");
int i;
float ratio = 576.0 / 512.0;

fprintf(fp, "%d\n", num_of_regions_);
fprintf(fp, "%d\n", skip_size_);
fprintf(fp, "%d\n", (int)(size_ * ratio));

for (i=0; i<num_of_regions_; i++)
{
fprintf(fp, "%d ", (int)(region_[i].start *ratio));
fprintf(fp, "%d ", region_[i].class_id);
fprintf(fp, "%d\n", (int)(region_[i].end *ratio));
fprintf(fp, "Region %d\n", i+1);
}

}
*/

/*
void 
TimeLine::print_mmf(FILE *fp)
{
int i;
float ratio = 576.0 / 512.0;

fprintf(fp,"# Feature map information file written by MARSYAS\n");
fprintf(fp,"Segment\n");
fprintf(fp,"#Number of features and number of different classes\n");
fprintf(fp,"%d %d\n", num_of_regions_, 0);
fprintf(fp, "#Names of features\n");
for (i=0; i<num_of_regions_; i++)
fprintf(fp,"f%d ", i);
fprintf(fp,"\n\n");
fprintf(fp,"#Name of classes\n");
fprintf(fp,"\n\n");

fprintf(fp,"#Number of feature float_vectors \n");
fprintf(fp,"1\n");
fprintf(fp,"#Feature map data\n");
fprintf(fp,"\n");


for (i=0; i<num_of_regions_; i++)
{
fprintf(fp, "%f ", region_[i].end * ratio - region_[i].start *ratio );
}
fprintf(fp," 0\n\n");
}
*/

void 
TimeLine::receive(Communicator* com)
{
	static char *buf = new char[256];
	int i;
	string message;

	com->receive_message(buf);
	// message = buf;
	numRegions_ = atoi(buf);

	com->receive_message(buf);

	lineSize_ = atoi(buf);

	com->receive_message(buf);
	size_ = atoi(buf);

	for (i=0; i < numRegions_; i++)
	{
		com->receive_message(buf);      
	}
}

void
TimeLine::send(Communicator* com)
{
	static char *buf = new char[256];
	int i;
	string message;

	sprintf(buf, "%d\n", numRegions_);
	message = buf;
	com->send_message(message);

	sprintf(buf, "%d\n", lineSize_);
	message = buf;
	com->send_message(message);

	sprintf(buf, "%d\n", size_);
	message = buf;
	com->send_message(message);

	for (i=0; i<numRegions_; i++)
	{
		sprintf(buf, "%d ", regions_[i].start);
		message = buf;
		com->send_message(message);

		sprintf(buf, "%d ", regions_[i].classId);
		message = buf;
		com->send_message(message);

		/*sprintf(buf, "%f ", region_[i].color.getR());
		message = buf;
		com->send_message(message);

		sprintf(buf, "%f ", region_[i].color.getG());
		message = buf;
		com->send_message(message);

		sprintf(buf, "%f ", region_[i].color.getB());
		message = buf;
		com->send_message(message);

		*/

		sprintf(buf, "%d\n", regions_[i].end);
		message = buf;
		com->send_message(message);      

		sprintf(buf, "Region %d\n", i);
		message = buf;
		com->send_message(message);
	}
}



