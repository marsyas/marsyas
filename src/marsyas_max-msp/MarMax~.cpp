//Template for Marsyas Max/MSP External
#include "MarMax~.h"

int main(void)
{
  // object initialization, note the use of dsp_free for the freemethod, which is required
  // unless you need to free allocated memory, in which case you should call dsp_free from
  // your custom free function.

  // NEW METHOD
  t_class *c;

  //NOTE that the name specified in class_new() first arg must be the same as the external filename
  c = class_new("ibt~", (method)MarMax_new, (method)dsp_free, (long)sizeof(t_MarMax), (method)0L, A_GIMME, 0);

  class_addmethod(c, (method)MarMax_float,		"float",	A_FLOAT, 0);
  class_addmethod(c, (method)MarMax_dsp,		"dsp",		A_CANT, 0);
  class_addmethod(c, (method)MarMax_assist,	"assist",	A_CANT, 0);
  //for parameters defined as messages in the first inlet
  //class_addmethod(c, (method)MarMax_window,		"window",	A_FLOAT,	A_FLOAT, 0);

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
  int output_tempo, curMedianTempo = 0;

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

  //post("Nozero: %d", Nozero);

  if (Nozero)
  {
    int j;
    for (j = 0; j < vectorSize; j++) {

      r(x->pos) = inL[j];

      /*time to do something */
      if (x->pos == x->hopsize - 1) {
        /* block loop */

        // Load the network with the data
        x->m_MarsyasNetwork->updControl("Series/featureNetwork/RealvecSource/src/mrs_realvec/data", r);
        //post ("network feeded");
        // Tick the network once, which will process one window of data

        x->m_MarsyasNetwork->tick();
        //post ("processing done");

        // Get the data out of the network -> beat events
        output_realvec = x->m_MarsyasNetwork->getControl("Series/featureNetwork/FlowThru/beattracker/mrs_realvec/innerOut")->to<mrs_realvec>();

        // Get the data out of the network -> median tempo
        output_tempo = (int) x->m_MarsyasNetwork->getControl("Series/featureNetwork/FlowThru/beattracker/BeatTimesSink/sink/mrs_natural/curMedianTempo")->to<mrs_natural>();
        if(output_tempo > 0) curMedianTempo = output_tempo;

        //post("tempo: %d", curMedianTempo);
        outlet_int(x->outletTempo, curMedianTempo); // signal outlet (note "signal" rather than NULL)

        //post("output vector copied");

        if (output_realvec(0,0))
          outlet_bang (x->outletBeat);
        //if(output_realvec(0,0))
        //	post("BEAT!");

        /* end of block loop */
        x->pos = -1; /* so it will be zero next j loop */
      }

      x->pos++;
    }
  }

  //you have to return the NEXT pointer in the array OR MAX WILL CRASH
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

void MarMax_setIBTDefaultParams(t_MarMax *x)
{
  x->bufsize = 1024;
  x->hopsize = 512;
  x->inductionTime = 5.0;
  x->minBPM = 81;
  x->maxBPM = 160;
  x->stateRecovery = false;
  x->outPathName = "";
}

void *MarMax_new(t_symbol *s, long argc, t_atom *argv)
{
  t_MarMax *x = NULL;

  // NEW VERSION
  if (x = ((t_MarMax *)object_alloc((t_class*)MarMax_class)))
  {
    object_post((t_object *)x, "v1.0 - implemented by João Lobato Oliveira from the SMCGroup at INESC Porto, Portugal (smc.inescporto.pt)", s->s_name);
    //object_post((t_object *)x, "a new %s object was instantiated: 0x%X", s->s_name, x);
    //object_post((t_object *)x, "%s has %ld arguments", s->s_name, argc);

    //define winSize and hopSize here! (irrelevant of I/O Vector Size and Signal Vector Size in Max)

    //at first define default values for all parameters
    //these will be the assigned values to the parameters not defined via ibt~ arguments
    MarMax_setIBTDefaultParams(x);
    while (argc > 0) //for parameters passing as arguments defined in ibt~ object box itself
    {
      t_symbol *firstarg = atom_getsym(argv);
      //post("arg name: %s", firstarg->s_name);
      if (!strcmp(firstarg->s_name, "@winSize") && argc > 1)
      {
        object_post((t_object *)x, "%s: %f", firstarg->s_name, atom_getfloat(argv+1));
        x->bufsize = (int) atom_getfloat(argv+1);
        argc -= 2; argv += 2;
      }
      else if (!strcmp(firstarg->s_name, "@hopSize") && argc > 1)
      {
        object_post((t_object *)x, "%s: %f", firstarg->s_name, atom_getfloat(argv+1));
        x->hopsize = (int) atom_getfloat(argv+1);
        argc -= 2; argv += 2;
      }
      else if (!strcmp(firstarg->s_name, "@inductionTime") && argc > 1)
      {
        object_post((t_object *)x, "%s: %f", firstarg->s_name, atom_getfloat(argv+1));
        x->inductionTime = atom_getfloat(argv+1);
        argc -= 2; argv += 2;
      }
      else if (!strcmp(firstarg->s_name, "@minBPM") && argc > 1)
      {
        object_post((t_object *)x, "%s: %f", firstarg->s_name, atom_getfloat(argv+1));
        x->minBPM = (int) atom_getfloat(argv+1);
        argc -= 2; argv += 2;
      }
      else if (!strcmp(firstarg->s_name, "@maxBPM") && argc > 1)
      {
        object_post((t_object *)x, "%s: %f", firstarg->s_name, atom_getfloat(argv+1));
        x->maxBPM = (int) atom_getfloat(argv+1);
        argc -= 2; argv += 2;
      }
      else if (!strcmp(firstarg->s_name, "@outPathName") && argc > 1)
      {
        object_post((t_object *)x, "%s: %s", firstarg->s_name, atom_getsym(argv+1)->s_name);
        x->outPathName = atom_getsym(argv+1)->s_name;
        argc -= 2; argv += 2;
      }
      else if (!strcmp(firstarg->s_name, "@stateRecovery") && argc > 1)
      {
        object_post((t_object *)x, "%s: %d", firstarg->s_name, (int) atom_getfloat(argv+1));
        int stateParam = (int) atom_getfloat(argv+1);
        if(stateParam >= 1) x->stateRecovery = true;
        argc -= 2; argv += 2;
      }
      else
      {
        object_post((t_object *)x, "usage is ibt~ [@winSize #] [@hopSize #] [@inductionTime #] [@minBPM #] [@maxBPM #] [@stateRecovery] [@outPathName #]");
        argc = 0;
      }
    }

    r.create(x->hopsize);

    //x->ob.z_misc = Z_NO_INPLACE;
    dsp_setup((t_pxobject *)x, 1);	// MSP inlets: arg is # of inlets and is REQUIRED!
    // use 0 if you don't need inlets
    x->outletTempo = intout(x); // int outlet for tempo
    x->outletBeat = bangout(x);	//an outlet that can only output bangs (nothing else)

    //post("winSize: %d; hopSize: %d; fs: %f; inductionTime: %f; minBPM: %d, maxBPM: %d; outPathName: %s",
    //	x->bufsize, x->hopsize, x->d_SR, x->inductionTime, x->minBPM, x->maxBPM, x->outPathName);

    //Create the MarSystem Network
    x->ibt = new MarMaxIBT(x->bufsize, x->hopsize, 44100.0, x->inductionTime, x->minBPM, x->maxBPM, x->outPathName, x->stateRecovery);
    x->m_MarsyasNetwork = x->ibt->createMarsyasNet();

    x->offset = 0;
  }
  return (x);
}

