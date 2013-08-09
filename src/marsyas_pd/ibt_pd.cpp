/**
 *
 * a puredata wrapper for aubio onset detection functions
 *
 * Thanks to Johannes M Zmolnig for writing the excellent HOWTO:
 *       http://iem.kug.ac.at/pd/externals-HOWTO/
 *
 * */

#include "ibt_pd.h"

char ibt_pd_version[] = "ibt_pd version 0.3";

static t_class *ibt_pd_class;

void ibt_pd_setup (void);


typedef struct _ibt_pd
{
  t_object x_obj;
  t_sample f;
  t_outlet *outletBeat;
  t_outlet *outletTempo;
  MarMaxIBT *ibt;
  MarSystem* m_MarsyasNetwork;
  float offset;
  t_int pos;                    /*frames%dspblocksize */
  t_int bufsize;
  t_int hopsize;
  t_float inductionTime;
  t_int minBPM;
  t_int maxBPM;
  bool stateRecovery;
  char *outPathName;

  //unused for now
  t_int onlineFlag;
  t_int metricalChangesFlag;

} t_ibt_pd;


static t_int *
ibt_pd_perform (t_int * w)
{
  t_ibt_pd *x = (t_ibt_pd *) (w[1]);
  t_sample *inL = (t_sample *) (w[2]);
  int vectorSize = (int) (w[3]);
  // args are in a vector, sized as specified in MarMax_dsp method
  // w[0] contains &MarMax_perform, so we start at w[1]


  int i;
  bool Nozero = false;
  realvec output_realvec;
  t_int output_tempo;
  t_float curMedianTempo = 0.0;

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
        output_tempo = (t_int) x->m_MarsyasNetwork->getControl("Series/featureNetwork/FlowThru/beattracker/BeatTimesSink/sink/mrs_natural/curMedianTempo")->to<mrs_natural>();
        if(output_tempo > 0) curMedianTempo = (t_float) output_tempo;

        //post("tempoI: %d; tempoF: %f", output_tempo, curMedianTempo);

        outlet_float(x->outletTempo, curMedianTempo); // signal outlet (note "signal" rather than NULL)

        //post("output vector copied");

        if (output_realvec(0,0))
          outlet_bang(x->outletBeat);
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

static void
ibt_pd_dsp (t_ibt_pd * x, t_signal ** sp)
{
  dsp_add (ibt_pd_perform, 3, x, sp[0]->s_vec, sp[0]->s_n);
}

static void
ibt_pd_debug (t_ibt_pd * x)
{
  post ("ibt_pd bufsize:\t%d", x->bufsize);
  post ("ibt_pd hopsize:\t%d", x->hopsize);
}

// NOT CALLED!, we use dsp_free for a generic free function
static void ibt_pd_free(t_ibt_pd *x)
{
  //delete x->m_MarsyasNetwork;
}

static void ibt_pd_setIBTDefaultParams(t_ibt_pd *x)
{
  x->bufsize = 1024;
  x->hopsize = 512;
  x->inductionTime = 5.0;
  x->minBPM = 81;
  x->maxBPM = 160;
  x->stateRecovery = false;
  x->outPathName = "";
}


static void *
ibt_pd_new (t_symbol *s, int argc, t_atom *argv)
{
  t_ibt_pd *x =
    (t_ibt_pd *) pd_new (ibt_pd_class);

  ibt_pd_setIBTDefaultParams(x);
  while (argc > 0) //for parameters passing as arguments defined in ibt_pd object box itself
  {
    t_symbol *firstarg = atom_getsymbol(argv);

    //post("arg name: %s", firstarg->s_name);
    if (!strcmp(firstarg->s_name, "@winSize") && argc > 1)
    {
      post("ibt_pd %s: %f", firstarg->s_name, atom_getfloat(argv+1));
      x->bufsize = (int) atom_getfloat(argv+1);
      argc -= 2; argv += 2;
    }
    else if (!strcmp(firstarg->s_name, "@hopSize") && argc > 1)
    {
      post("ibt_pd %s: %f", firstarg->s_name, atom_getfloat(argv+1));
      x->hopsize = (int) atom_getfloat(argv+1);
      argc -= 2; argv += 2;
    }
    else if (!strcmp(firstarg->s_name, "@inductionTime") && argc > 1)
    {
      post("ibt_pd %s: %f", firstarg->s_name, atom_getfloat(argv+1));
      x->inductionTime = atom_getfloat(argv+1);
      argc -= 2; argv += 2;
    }
    else if (!strcmp(firstarg->s_name, "@minBPM") && argc > 1)
    {
      post("ibt_pd %s: %f", firstarg->s_name, atom_getfloat(argv+1));
      x->minBPM = (int) atom_getfloat(argv+1);
      argc -= 2; argv += 2;
    }
    else if (!strcmp(firstarg->s_name, "@maxBPM") && argc > 1)
    {
      post("ibt_pd %s: %f", firstarg->s_name, atom_getfloat(argv+1));
      x->maxBPM = (int) atom_getfloat(argv+1);
      argc -= 2; argv += 2;
    }
    else if (!strcmp(firstarg->s_name, "@outPathName") && argc > 1)
    {
      post("ibt_pd %s: %s", firstarg->s_name, atom_getsymbol(argv+1)->s_name);
      x->outPathName = atom_getsymbol(argv+1)->s_name;
      argc -= 2; argv += 2;
    }
    else if (!strcmp(firstarg->s_name, "@stateRecovery") && argc > 1)
    {
      post("ibt_pd %s: %d", firstarg->s_name, (int) atom_getfloat(argv+1));
      int stateParam = (int) atom_getfloat(argv+1);
      if(stateParam >= 1) x->stateRecovery = true;
      argc -= 2; argv += 2;
    }
    else
    {
      post("usage is ibt_pd [@winSize #] [@hopSize #] [@inductionTime #] [@minBPM #] [@maxBPM #] [@stateRecovery] [@outPathName #]");
      argc = 0;
    }
  }
  r.create(x->hopsize);



  x->outletBeat = outlet_new (&x->x_obj, &s_bang);
  x->outletTempo = outlet_new (&x->x_obj, &s_float);

  //Create the MarSystem Network
  x->ibt = new MarMaxIBT(x->bufsize, x->hopsize, 44100.0, x->inductionTime, x->minBPM, x->maxBPM, x->outPathName, x->stateRecovery);
  x->m_MarsyasNetwork = x->ibt->createMarsyasNet();


  return (void *) x;
}


// main function to set up ibt_pd_class
// this function must always be exported
extern "C" EXPORT void ibt_pd_setup(void)
{
  ibt_pd_class = class_new (gensym ("ibt_pd"),
                            (t_newmethod) ibt_pd_new,
                            0,
                            sizeof (t_ibt_pd), CLASS_DEFAULT, A_GIMME, A_NULL);
  class_addmethod (ibt_pd_class,
                   (t_method) ibt_pd_dsp, gensym ("dsp"), (t_atomtype)0);

  CLASS_MAINSIGNALIN (ibt_pd_class, t_ibt_pd,f);
}
