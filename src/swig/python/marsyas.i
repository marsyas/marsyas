%module marsyas

%include "marsystem.i"
%include "marsystemmanager.i"
%include "marcontrolptr.i"
%include "realvec.i"

%extend realvec {
	mrs_natural __len__() { return $self->getSize(); }
	
	/* this is probably very inefficient, but can't seem to bind the () operator.
	   It also doesn't check to make sure the array index is within bounds. */
	mrs_real __getitem__(mrs_natural i) { return $self->getData()[i]; }
	void __setitem__(mrs_natural i, mrs_real x) { $self->stretchWrite(i, x); }
	
	/* it's be nice to bind all the overloaded operators, but %rename doesn't seem to work.
	   Also, python's "for x in list" iterator doesn't work either. */
};
