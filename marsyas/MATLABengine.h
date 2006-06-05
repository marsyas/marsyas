#ifdef _MATLAB_ENGINE_ 

#if !defined(__MATLABENGINE_H__)
#define __MATLABENGINE_H__

#include "engine.h" //$(MATLAB)\extern\include

#include <string>
#include <vector>
//#include <complex>

class realvec;

class MATLABengine
{
private:
	Engine *engine_;
	static MATLABengine *instance_;
	
	MATLABengine();

public:
	~MATLABengine();

	static MATLABengine *getMatlabEng();

	void evalString(std::string MATLABcmd);

	//-------------------------------------------------
	//Marsyas0.2 types (realvec = matrix of doubles)
	//-------------------------------------------------
	//put realvec
	void putVariable(realvec &value, std::string MATLABname);
	//get realvec
	void getVariable(std::string MATLABname, realvec& value);
	
	
	//-------------------------------------------------
	//Standard C/C++ types
	//-------------------------------------------------
	//put vectors
	void putVariable(double *value, unsigned int size, std::string MATLABname);
	void putVariable(float *value, unsigned int size, std::string MATLABname);
	void putVariable(int *value, unsigned int size, std::string MATLABname);
	
	//put scalars
	void putVariable(int value, std::string MATLABname);
	void putVariable(float value, std::string MATLABname);
	void putVariable(double value, std::string MATLABname);

	//get scalars
	//all MATLAB scalars are represented as doubles
	//so, no need to have getters for ints or floats...
	void getVariable(std::string MATLABname, double& value); 

	//-------------------------------------------------
	// C++ STL Types
	//-------------------------------------------------
 	void putVariable(std::vector<double> &value, std::string MATLABname);
// 	void putVariable(std::complex<double> &value, std::string MATLABname);
// 	void putVariable(std::vector< std::complex<double> > &value, std::string MATLABname);

	
	//-------------------------------------------------
	// TESTING METHODS
	//-------------------------------------------------
	void test();

};

#endif //__MATLABENGINE_H__

#endif //_MATLAB_ENGINE