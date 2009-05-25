%module realvec

%{
#include <marsyas/realvec.h>

using namespace Marsyas;

%}

%include "types.i"

/* Class description for realvecs */
class realvec {
	public: /* Constructors */
	    realvec();
	    realvec( const realvec &a );
		realvec(mrs_natural size);
        realvec(mrs_natural rows, mrs_natural cols);
	    ~realvec();


        /** \name Getting information */
        mrs_natural getSize() const;
        mrs_natural getCols() const;
        mrs_natural getRows() const;


        mrs_real *getData() const;


	    /* Vector/Matrix Algebra and Statistics */
	    mrs_real maxval();
		mrs_real minval();
		mrs_real mean();
		mrs_real median();
		mrs_real sum();
		mrs_real std();
		mrs_real var();
		void sort();
		void abs();
		void sqr();
		void sqroot();
		void norm();
		void normMaxMin();

		/* Functions that modify the array */
		void stretch(mrs_natural size);
		void setval(mrs_natural start, mrs_natural end, mrs_real val);
		void setval(mrs_real val);
		void appendRealvec(const realvec newValues);
		void apply(mrs_real (*func) (mrs_real));
		void norm(mrs_real mean, mrs_real std);
		void renorm(mrs_real old_mean, mrs_real old_std, mrs_real new_mean, mrs_real new_std);

		/* get a subset of a realvec */
		realvec getSubVector(mrs_natural startPos, mrs_natural length);

		/* some more functions */
		void covariance(realvec& res) const; //Typical covariance calculation, as performed by MATLAB cov().
		void covariance2(realvec& res) const;//Marsyas0.1 covariance calculation (assumes standardized data, and uses biased estimation)

		mrs_natural search(mrs_real val);


};


%extend realvec {

    std::string __str__() {
        std::ostringstream oss;
        $self->dumpDataOnly(oss);
        return oss.str();
    }
    void print_() {
        std::cout << *$self;
    }

    mrs_natural __len__() {
        return $self->getSize();
    }

    /* this is probably very inefficient, but can't seem to bind the () operator.
       It also doesn't check to make sure the array index is within bounds. */
    mrs_real __getitem__(mrs_natural i) { return $self->getData()[i]; }
    void __setitem__(mrs_natural i, mrs_real x) { $self->stretchWrite(i, x); }

    /* it's be nice to bind all the overloaded operators, but %rename doesn't seem to work.
       Also, python's "for x in list" iterator doesn't work either. */
};


