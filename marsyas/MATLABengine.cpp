
#ifdef _MATLAB_ENGINE_

#include "realvec.h"
#include "MATLABengine.h"

using namespace std;

MATLABengine * MATLABengine::instance_ = 0;

MATLABengine::MATLABengine()
{
	if (!(engine_ = engOpen("\0")))
	{
		MRSERR("Can't start MATLAB Engine! Check MATLAB install and configuration!\n");
		return;
	}
	instance_ = this;
}

MATLABengine::~MATLABengine()
{
	engClose(engine_);
}

MATLABengine*
MATLABengine::getMatlabEng()
{
	if(!instance_) 
		instance_ = new MATLABengine();
	
	return instance_;
}

void
MATLABengine::evalString(string MATLABcmd)
{
	engEvalString(engine_, MATLABcmd.c_str());
}

void
MATLABengine::putVariable(realvec &value, string MATLABname)
{
	//----------------------------------
	// send a realvec to a MATLAB matrix
	//----------------------------------
	int dims[2]; //realvec is 2D (maximum)

	//realvec is row-wise but
	//MATLAB arrays are column-wise!
	//Must transpose input and then output 
	//in order to pass the array correctly to MATLAB!

	//transpose input!
	dims[0] = value.getCols(); //rows = columns!
	dims[1] = value.getRows(); //columns = rows!

	//realvec are by default double precision matrices => mxDOUBLE_CLASS
	mxArray *mxMatrix = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
	real *data = value.getData();
	memcpy((void *)mxGetPr(mxMatrix), (void *)(data), dims[0]*dims[1]*mxGetElementSize(mxMatrix));
	engPutVariable(engine_, MATLABname.c_str(), mxMatrix);

	//transpose output (already inside MATLAB)
	string tranposeCmd = MATLABname + "=" + MATLABname + "';";
	engEvalString(engine_, tranposeCmd.c_str());

	mxDestroyArray(mxMatrix);
}

void
MATLABengine::putVariable(double *value, unsigned int size, string MATLABname)
{
	//-----------------------------------
	//send C/C++ vector to MATLAB vector
	//-----------------------------------
	int dims[2];
	dims[0] = 1; //row vector
	dims[1] = size;

	mxArray *mxVector = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
	memcpy(mxGetData(mxVector), (void *)value, size*mxGetElementSize(mxVector));
	engPutVariable(engine_, MATLABname.c_str(), mxVector);

	//Convert to MATLAB double type
	//string MatCmd = MATLABname + "=double(" + MATLABname + ");";
	//engEvalString(engine_, MatCmd.c_str());

	mxDestroyArray(mxVector);
}

void
MATLABengine::putVariable(float *value, unsigned int size, string MATLABname)
{
	//-----------------------------------
	//send C/C++ vector to MATLAB vector
	//-----------------------------------
	int dims[2];
	dims[0] = 1; //row vector
	dims[1] = size;

	mxArray *mxVector = mxCreateNumericArray(2, dims, mxSINGLE_CLASS, mxREAL);
	memcpy(mxGetData(mxVector), (void *)value, size*mxGetElementSize(mxVector));
	engPutVariable(engine_, MATLABname.c_str(), mxVector);

	//Convert to MATLAB double type
	string MatCmd = MATLABname + "=double(" + MATLABname + ");";
	engEvalString(engine_, MatCmd.c_str());

	mxDestroyArray(mxVector);
}

void
MATLABengine::putVariable(int *value, unsigned int size, string MATLABname)
{
	//-----------------------------------
	//send C/C++ vector to MATLAB vector
	//-----------------------------------
	int dims[2];
	dims[0] = 1; //row vector
	dims[1] = size;

	mxArray *mxVector = mxCreateNumericArray(2, dims, mxINT32_CLASS, mxREAL);
	memcpy(mxGetData(mxVector), (void *)value, size*mxGetElementSize(mxVector));
	engPutVariable(engine_, MATLABname.c_str(), mxVector);

	//Convert to MATLAB double type
	string MatCmd = MATLABname + "=double(" + MATLABname + ");";
	engEvalString(engine_, MatCmd.c_str());

	mxDestroyArray(mxVector);
}

void
MATLABengine::putVariable(int value, string MATLABname)
{
	putVariable(&value, 1, MATLABname);
}

void
MATLABengine::putVariable(float value, string MATLABname)
{
	putVariable(&value, 1, MATLABname);
}

void
MATLABengine::putVariable(double scalar, string MATLABname)
{
	putVariable(&scalar, 1, MATLABname);
}


//-------------------------------------------------
// C++ STL Types
//-------------------------------------------------
void 
MATLABengine::putVariable(vector<double> &value, string MATLABname)
{
	int dims[2];
	dims[0] = 1; //row vector
	dims[1] = value.size();

	mxArray *mxVector = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
	double *x = mxGetPr(mxVector);

	for(unsigned int i = 0; i < value.size(); i++)
	{
		*(x + i) = value[i];
	}

	engPutVariable(engine_, MATLABname.c_str(), mxVector);

	//Convert to MATLAB double type
	//string MatCmd = MATLABname + "=double(" + MATLABname + ");";
	//engEvalString(engine_, MatCmd.c_str());

	mxDestroyArray(mxVector);

}

// void 
// MATLABengine::putVariable(complex<double> &value, string MATLABname)
// {
// 	int dims[2];
// 	dims[0] = 1; 
// 	dims[1] = 1;
// 
// 	mxArray *mxVector = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxCOMPLEX);
// 	double *xr = mxGetPr(mxVector);
// 	double *xi = mxGetPi(mxVector);
// 
// 	*xr = value.real();
// 	*xi = value.imag();
// 
// 	engPutVariable(engine_, MATLABname.c_str(), mxVector);
// 
// 	//Convert to MATLAB double type
// 	//string MatCmd = MATLABname + "=double(" + MATLABname + ");";
// 	//engEvalString(engine_, MatCmd.c_str());
// 
// 	mxDestroyArray(mxVector);
// }
// 
// void 
// MATLABengine::putVariable(vector< complex<double> > &value, string MATLABname)
// {
// 	int dims[2];
// 	dims[0] = 1; //row vector
// 	dims[1] = value.size();
// 
// 	mxArray *mxVector = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxCOMPLEX);
// 	double *xr = mxGetPr(mxVector);
// 	double *xi = mxGetPi(mxVector);
// 
// 	for(unsigned int i = 0; i < value.size(); i++)
// 	{
// 		*(xr + i) = value[i].real();
// 		*(xi + i) = value[i].imag();
// 	}
// 
// 	engPutVariable(engine_, MATLABname.c_str(), mxVector);
// 
// 	//Convert to MATLAB double type
// 	//string MatCmd = MATLABname + "=double(" + MATLABname + ");";
// 	//engEvalString(engine_, MatCmd.c_str());
// 
// 	mxDestroyArray(mxVector);
// }

//-------------------------------------------------------------------
// getters
//-------------------------------------------------------------------

void
MATLABengine::getVariable(std::string MATLABname, double& value)
{
	int dims[2];
	dims[0] = 1;
	dims[1] = 1;

	mxArray *mxVector = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
	mxVector = engGetVariable(engine_, MATLABname.c_str());
	
	value = mxGetScalar(mxVector);
	
	mxDestroyArray(mxVector);
}

//-------------------------------------------------------------------
// TESTING METHODS
//-------------------------------------------------------------------
void
MATLABengine::test()
{
	//---------------------------------------
	double double_scalar = 3.123456789;
	MATLABengine::getMatlabEng()->putVariable(double_scalar,"double_scalar");
	//---------------------------------------
	realvec marsyas_realvec1D(4);
	marsyas_realvec1D(0) = 3.123456789;
	marsyas_realvec1D(1) = 3.987654321;
	marsyas_realvec1D(2) = 2.0;
	marsyas_realvec1D(3) = 1.1234567890123456789;
	MATLABengine::getMatlabEng()->putVariable(marsyas_realvec1D,"marsyas_realvec1D");
	//---------------------------------------
	realvec marsyas_realvec2D(2,3);
	marsyas_realvec2D.setval(1.123456789);
	MATLABengine::getMatlabEng()->putVariable(marsyas_realvec2D,"marsyas_realvec2D");
	//---------------------------------------
	double double_array[4] = {1.123456789, 2.123456789, 3.123456789, 0.0};
	MATLABengine::getMatlabEng()->putVariable(double_array,4,"double_array");
	//---------------------------------------
// 	complex<double> complex_double(1.123456789, 2.123456789);
// 	MATLABengine::getMatlabEng()->putVariable(complex_double, "complex_double");
	//---------------------------------------
	vector<double> vector_double(4);
	vector_double[0] = 1.123456789;
	vector_double[1] = 2.123456789;
	vector_double[2] = 3.123456789;
	vector_double[3] = 4.123456789;
	MATLABengine::getMatlabEng()->putVariable(vector_double, "vector_double");
	//---------------------------------------
// 	vector< complex<double> > vector_complex_double(4);
// 	vector_complex_double[0] = complex<double> (1.123456789, 2.123456789);
// 	vector_complex_double[1] = complex<double> (3.123456789, 4.123456789);
// 	vector_complex_double[2] = complex<double> (5.123456789, 6.123456789);
// 	vector_complex_double[3] = complex<double> (7.123456789, 8.123456789);
// 	MATLABengine::getMatlabEng()->putVariable(vector_complex_double, "vector_complex_double");
}

#endif //_MATLAB_ENGINE


