//Template for Marsyas Max/MSP External
#include "MarMax~.h"

int main(void)
{	
	// object initialization, note the use of dsp_free for the freemethod, which is required
	// unless you need to free allocated memory, in which case you should call dsp_free from
	// your custom free function.

	// OLD METHOD
	// setup((t_messlist **)&MarMax_class, (method)MarMax_new, (method)dsp_free, (short)sizeof(t_MarMax), 0L, A_GIMME, 0);
	// addfloat((method)MarMax_float);
	// you need this
	// addmess((method)MarMax_dsp,				"dsp",			A_CANT, 0);
    // addmess((method)MarMax_assist,			"assist",		A_CANT, 0);  
	// you need this
    // dsp_initclass();
	
	// NEW METHOD
	t_class *c;
	
	//NOTE that the name specified in class_new() first arg must be the same as the external filename
	c = class_new("ibt_", (method)MarMax_new, (method)dsp_free, (long)sizeof(t_MarMax), 0L, A_GIMME, 0);
	
	class_addmethod(c, (method)MarMax_float,		"float",	A_FLOAT, 0);
	class_addmethod(c, (method)MarMax_dsp,		"dsp",		A_CANT, 0);
	class_addmethod(c, (method)MarMax_assist,	"assist",	A_CANT, 0);
	
	class_dspinit(c);				// new style object version of dsp_initclass();
	class_register(CLASS_BOX, c);	// register class as a box class
	MarMax_class = c;
	
	return 0;
}

void MarMax_float(t_MarMax *x, double f)
{
	x->offset = f;
}

// this function is called when the DAC is enabled, and "registers" a function
// for the signal chain. in this case, "MarMax_perform"
void MarMax_dsp(t_MarMax *x, t_signal **sp, short *count)
{
	//post("my sample rate is: %f; inPointer: %d; outPointer: %d; vector size: %d", sp[0]->s_sr, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
	x->d_SR = sp[0]->s_sr;
	// dsp_add
	// 1: (t_perfroutine p) perform method
	// 2: (long argc) number of args to your perform method
	// 3...: argc additional arguments, all must be sizeof(pointer) or long
	// these can be whatever, so you might want to include your object pointer in there
	// so that you have access to the info, if you need it.
	//dsp_add(MarMax_perform, 3, x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
	dsp_add(MarMax_perform, 3, x, sp[0]->s_vec, sp[0]->s_n);
}

t_int *MarMax_perform(t_int *w)
{
	// DO NOT CALL post IN HERE, but you can call defer_low (not defer)
	
	// args are in a vector, sized as specified in MarMax_dsp method
	// w[0] contains &MarMax_perform, so we start at w[1]
	t_MarMax *x = (t_MarMax *)(w[1]);
	t_float *inL = (t_float *)(w[2]);
	//t_float *outL = (t_float *)(w[3]);
	int vectorSize = (int)w[3];
	//int vectorSize = (int)w[4];
	int i;
	bool Nozero = false;
	realvec output_realvec;
	realvec r(vectorSize);

	//to check if input is empty -> don't know exactly why when no input is loaded 
	//into Marsystem the network stucks in steady-state
	//(suppose it has to do with how RealvecSource deals with the input)
	for ( i = 0; i < vectorSize; ++i)
	{
		if (inL[i] != 0.)
		{
			Nozero = true;
			break;
		}
	}
	
	if (Nozero)
	{
		// Stuff inputBuffers into a realvec
		for ( i = 0; i < vectorSize; ++i)
			r(i) = inL[i];
	
		// Load the network with the data
		x->m_MarsyasNetwork->updControl("Series/featureNetwork/RealvecSource/src/mrs_realvec/data", r);
		//post ("network feeded");
		// Tick the network once, which will process one window of data

		x->m_MarsyasNetwork->tick();
		//post ("processing done");

		// Get the data out of the networK
		output_realvec = x->m_MarsyasNetwork->getControl("Series/featureNetwork/FlowThru/beattracker/mrs_realvec/innerOut")->to<mrs_realvec>();

		//post("output vector copied");
		
		//copy Marsystem network output to Max output
//		outL[0] = output_realvec(0,0); //BEAT = 1; NON-BEAT = 0 (1response/frame)
//		for (i = 1 ; i < vectorSize ; i++) //fill remaining output vector with 0
//			outL[i] = 0;
		if (output_realvec(0,0))
			outlet_bang (x->outlet);
		//if(outL[0] == 1.0)
		//	post("BEAT!");
	}

	// you have to return the NEXT pointer in the array OR MAX WILL CRASH
	//return w + 5;
	return w + 4;
}

void MarMax_assist(t_MarMax *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
		sprintf(s, "I am inlet %ld", a);
	} 
	else {	// outlet
		sprintf(s, "I am outlet %ld", a); 			
	}
}

// NOT CALLED!, we use dsp_free for a generic free function
void MarMax_free(t_MarMax *x) 
{
	delete x->m_MarsyasNetwork;
}

void *MarMax_new(t_symbol *s, long argc, t_atom *argv)
{
	t_MarMax *x = NULL;

	// OLD VERSION
	// if (x = (t_MarMax *)newobject(MarMax_class)) {
	
	// NEW VERSION
	if (x = (t_MarMax *)object_alloc((t_class*)MarMax_class))
	{
		//x->ob.z_misc = Z_NO_INPLACE;
		dsp_setup((t_pxobject *)x, 1);	// MSP inlets: arg is # of inlets and is REQUIRED! 
										// use 0 if you don't need inlets
		//outlet_new(x, "signal"); // signal outlet (note "signal" rather than NULL)
		x->outlet = bangout(x);//an outlet that can only output bangs (nothing else)
		//Create the MarSystem Network

		x->ibt = new MarMaxIBT();
		x->m_MarsyasNetwork = x->ibt->createMarsyasNet();

		//int xx = fntestdll();
		//bool yy = x->testdll.test(1, 2.3);
		x->offset = 0;
	}
	return (x);
}
