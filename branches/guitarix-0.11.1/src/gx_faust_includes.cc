/*
 * Copyright (C) 2009-2010 Hermann Meyer, James Warden, Andreas Degert
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * --------------------------------------------------------------------------
 *
 *
 *    definitions for code generated with faust / dsp2cc
 *    part of gx_engine_audio.cpp
 *
 * --------------------------------------------------------------------------
 */

/****************************************************************
 ** functions and variables used by faust dsp files
 */

inline float sigmoid(float x)
{
	return x*(1.5f - 0.5f*x*x);
}

inline float saturate(float x, float t)
{
	if (fabs(x)<t)
		return x;
	else {
		if (x > 0.f)
			return t + (1.f-t)*sigmoid((x-t)/((1-t)*1.5f));
		else
			return -(t + (1.f-t)*sigmoid((-x-t)/((1-t)*1.5f)));
	}
}

inline float hard_cut(float in, float threshold)
{
	if ( in > threshold) {
		in = threshold;
	} else if ( in < -threshold) {
		in = -threshold;
	}

	return in;
}

inline float foldback(float in, float threshold)
{
	if (threshold == 0) threshold = 0.01f;

	if (fabs(in) > threshold) {
		in = fabs(fabs(fmod(in - threshold, threshold*4)) - threshold*2) - threshold;
	}
	return in;
}

inline float fold(float threshold, float v)
{
	// switch between hard_cut or foldback distortion, or plain output
	switch ((int)audio.ffuse) {
	case 0:
		break;
	case 1:
		v = hard_cut(saturate(v,threshold),threshold);
		break;
	case 2:
		v = foldback(v,threshold);
		break;
	}
	return v;
}

inline float add_dc (float val)
{
	return val + 1e-20; // avoid denormals
}

// foreign variable added to faust module feed
// it's set in process_buffers()
namespace feed { float ngate = 1; }  // noise-gate, modifies output gain

/****************************************************************
 **  definitions for code generated with faust / dsp2cc
 */

typedef void (*inifunc)(int);

typedef struct {
	inifunc func;
	const char *name;
} inidef;


static list<inidef>& get_inilist()
{
	static list<inidef> inilist;
	return inilist;
}

static list<gx_gui::Parameter*>& get_paramlist()
{
	static list<gx_gui::Parameter*> paramlist;
	return paramlist;
}

void register_faust_parameters()
{
	list<gx_gui::Parameter*>& paramlist = get_paramlist();
	for (list<gx_gui::Parameter*>::iterator i = paramlist.begin(); i != paramlist.end(); i++) {
		gx_gui::parameter_map.insert(*i);
	}
	paramlist.clear();
}

static gx_gui::Parameter *find_parameter(const char *id)
{
	list<gx_gui::Parameter*>& paramlist = get_paramlist();
	for (list<gx_gui::Parameter*>::iterator i = paramlist.begin(); i != paramlist.end(); i++) {
		if ((*i)->id() == id) {
			return *i;
		}
	}
	return 0;
}

static float& get_alias(const char *id)
{
	static float dummy;
	gx_gui::Parameter *p = find_parameter(id);
	if (!p) {
		gx_system::gx_print_error("engine", string("can't define alias for unknown (or not yet defined) parameter id: ") + id);
		return dummy;
	} else {
		return p->getFloat().value;
	}
}

static void registerVar(const char* id, const char* name, const char* tp,
                        const char* tooltip, float* var, float val=0,
                        float low=0, float up=0, float step=0, bool exp=false)
{
	if (!name[0]) {
		assert(strrchr(id, '.'));
		name = strrchr(id, '.')+1;
	}
	gx_gui::Parameter *p = new gx_gui::FloatParameter(
		id, name, gx_gui::Parameter::Continuous, true, *var, val, low, up, step, true, exp);
	if (tooltip) {
		p->set_desc(tooltip);
	}
	get_paramlist().push_back(p);
}

static void registerEnumVar(const char *id, const char* name, const char* tp,
                            const char* tooltip, const char** values, float *var, float val,
                            float low=0, float up=0, float step=1, bool exp=false)
{
	if (!name[0]) {
		assert(strrchr(id, '.'));
		name = strrchr(id, '.')+1;
	}
	assert(low == 0.0 && step == 1.0);
	gx_gui::FloatEnumParameter *p = new gx_gui::FloatEnumParameter(
		id, name, values, true, *var, (int)round(val), true, exp);
	assert(up == p->upper); // calculated by constructor
	get_paramlist().push_back(p);
}

static inline void registerIntParam(const char*a,const char*b,int*c,int std=0,int lower=0,int upper=1,bool exp=false)
 {
	 get_paramlist().push_back(
		 new gx_gui::IntParameter(a,b,gx_gui::Parameter::Enum,true,*c,std,
		                          lower,upper,true,exp));
 }

static void registerInit(const char *name, inifunc f)
{
	inidef i;
	i.func = f;
	i.name = name;
	get_inilist().push_back(i);
}

static void jack_sync()
{
	while (sem_wait(&gx_jack::jack_sync_sem) == EINTR);
}

#define max(x,y) (((x)>(y)) ? (x) : (y))
#define min(x,y) (((x)<(y)) ? (x) : (y))

//FIXME (temporary)hack to support older compiler versions
inline float  pow(float b, float e) { return ::powf(b, e); }
inline double pow(double b, double e){return ::pow(b, e); }
inline double pow(double b, int e)  { return ::pow(b, (double)e); }
inline double pow(int b, double e)  { return ::pow((double)b, e); }
inline double pow(double b, float e){ return ::pow(b, (double)e); }
inline double pow(float b, double e){ return ::pow((double)b, e); }
inline float  pow(float b, int e)   { return ::powf(b, (float) e); }
inline float  pow(int b, float e)   { return ::powf((float)b,  e); }

template <int N> inline float faustpower(float x)       { return powf(x,N); }
template <int N> inline double faustpower(double x)     { return pow(x,N); }
template <int N> inline int faustpower(int x)           { return faustpower<N/2>(x) * faustpower<N-N/2>(x); }
template <>      inline int faustpower<0>(int x)        { return 1; }
template <>      inline int faustpower<1>(int x)        { return x; }
#define FAUSTFLOAT float

/****************************************************************
 **  include faust/dsp2cc generated files
 */

// amp
#include "faust/amp2.cc"
#include "faust/stage3.cc"
#include "faust/preamp.cc"
#include "faust/inputgain.cc"
#include "faust/noise_shaper.cc"
#include "faust/AntiAlias.cc"
#include "faust/HighShelf.cc"
#include "faust/drive.cc"
#include "faust/osc_tube.cc"
#include "faust/reso_tube.cc"
#include "faust/tube.cc"
#include "faust/tubevibrato.cc"
#include "faust/multifilter.cc"
#include "faust/bassbooster.cc"
#include "faust/feed.cc"
#include "faust/outputgain.cc"
#include "faust/balance.cc"
#include "faust/jconv_post.cc"
#include "faust/balance1.cc"
#include "faust/eq.cc"
#include "faust/tube3.cc"

// tone stack
static struct ToneStackParams { ToneStackParams(); } ToneStackParams;
ToneStackParams::ToneStackParams() {
	static FAUSTFLOAT v1, v2, v3;
	registerVar("amp.tonestack.Treble","","S","",&v1, 0.5, 0.0, 1.0, 0.01);
	registerVar("amp.tonestack.Bass",  "","S","",&v2, 0.5, 0.0, 1.0, 0.01);
	registerVar("amp.tonestack.Middle","","S","",&v3, 0.5, 0.0, 1.0, 0.01);
}
#include "faust/tonestack_default.cc"
#include "faust/tonestack_bassman.cc"
#include "faust/tonestack_twin.cc"
#include "faust/tonestack_princeton.cc"
#include "faust/tonestack_jcm800.cc"
#include "faust/tonestack_jcm2000.cc"
#include "faust/tonestack_mlead.cc"
#include "faust/tonestack_m2199.cc"
#include "faust/tonestack_ac30.cc"

// effects
#include "faust/overdrive.cc"
#include "faust/compressor.cc"
#include "faust/crybaby.cc"
#include "faust/autowah.cc"
#include "faust/echo.cc"
#include "faust/delay.cc"
#include "faust/distortion.cc"
#include "faust/distortion1.cc"
#include "faust/freeverb.cc"
#include "faust/impulseresponse.cc"
#include "faust/chorus.cc"
#include "faust/moog.cc"
#include "faust/biquad.cc"
#include "faust/flanger.cc"
#include "faust/selecteq.cc"
#include "faust/sloop.cc"

static void activate_callback(float val, void *data)
{
	((void (*)(bool,int))data)(!(val == 0.0), gx_jack::jack_sr);
}

static void faust_add_callback(const char* id, void (*func)(bool,int))
{
	new gx_ui::GxUiCallbackItemFloat(gx_gui::GxMainInterface::instance(),
	                                 (float*)gx_gui::parameter_map[id].zone(),
	                                 activate_callback, (void*)func);
}

void faust_init(int samplingFreq)
{
	faust_add_callback("SampleLooper.on_off", sloop::activate);
	faust_add_callback("delay.on_off", delay::activate);
	faust_add_callback("echo.on_off", echo::activate);
	faust_add_callback("chorus.on_off", chorus::activate);
	list<inidef>& inilist = get_inilist();
	for (list<inidef>::iterator i = inilist.begin(); i != inilist.end(); i++) {
		try {
			i->func(samplingFreq);
		} catch (bad_alloc) {
			string name = gx_gui::param_group(i->name, true);
			gx_system::gx_print_error("DSP Module", (boost::format("not enough memory to initialize module %1%") % i->name).str());
		}
	}
}



#ifdef EXPERIMENTAL
/****************************************************************
 ** Support for experimental faust dsp files
 */

#define EXPERIMENTAL_PROCESSING                                  \
    if (exp_on) {                                                \
	    static int exp_upsample_old = 0;                         \
	    int ovs_exp_count, ovs_exp_sr;                           \
	    float *ovs_exp_buffer;                                   \
        if (exp_upsample_on) {                                   \
            exp_upsample = min(8,audio.upsample_mode+1);         \
            if (exp_upsample != exp_upsample_old) {              \
                exp_upsample_old = exp_upsample;                 \
                /*FIXME non-rt*/                                 \
                resampExp.setup(gx_jack::jack_sr, exp_upsample); \
                Exp::init(exp_upsample * gx_jack::jack_sr);      \
            }                                                    \
                resampExp.up(count, output0, oversample);        \
                ovs_exp_sr = exp_upsample * gx_jack::jack_sr;    \
                ovs_exp_count = exp_upsample * count;            \
                ovs_exp_buffer = oversample;                     \
              }                                                  \
             else {                                              \
                ovs_exp_sr = gx_jack::jack_sr;                   \
                ovs_exp_count = count;                           \
                ovs_exp_buffer = output0;                        \
            }                                                    \
                                                                 \
        Exp::compute(ovs_exp_count, ovs_exp_buffer, ovs_exp_buffer);\
                                                                 \
        if (exp_upsample_on) {                                   \
            resampExp.down(count, oversample, output0);          \
        }                                                        \
    }

/****************************************************************
 **  the Eperimental widget
 */

typedef void (*setupfunc)(GtkWidget *);
list<setupfunc> setuplist;

void registerSetup(setupfunc f)
{
	setuplist.push_back(f);
}

#define registerVar(id,name,tp,tooltip,var,val,low,up,step) registerVar(id,name,tp,tooltip,var,val,low,up,step,true)

#include  "faust/Exp.cc"

#undef registerVar

static GtkWidget *exp_window = 0;

static gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_widget_hide(widget);
    return TRUE;
}

static void on_show(GtkWidget *widget, gpointer data)
{
	gx_gui::parameter_map["system.show_exp_window"].getSwitch().set(true);
}

static void on_hide(GtkWidget *widget, gpointer data)
{
	gx_gui::parameter_map["system.show_exp_window"].getSwitch().set(false);
}

volatile int exp_upsample;
volatile int exp_upsample_on;
volatile int exp_on;
volatile bool exp_hs;
GtkWidget *exp_sample_spin;
SimpleResampler resampExp;

static void exp_sr_changed(GtkWidget *widget, gpointer data)
{
	exp_upsample = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
}

static void exp_hs_toggled(GtkWidget *widget, gpointer data)
{
	exp_hs = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
}

static void exp_up_toggled(GtkWidget *widget, gpointer data)
{
	exp_upsample_on = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
}

static void exp_on_toggled(GtkWidget *widget, gpointer data)
{
	exp_on = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
}

static gboolean set_transient(gpointer data)
{
	GtkWindow *exp_window = GTK_WINDOW(data);
	if (GTK_WIDGET_REALIZED (GTK_WIDGET(gx_gui::fWindow))) {
    // replaced for GTK-2.13
	//if (gtk_widget_get_realized(GTK_WIDGET(gx_gui::fWindow))) {
	    gtk_window_set_transient_for(exp_window, GTK_WINDOW(gx_gui::fWindow));
	}
	return FALSE;
}

void faust_setup()
{
	registerIntParam("test.upsample", "Upsample", (int*)&exp_upsample, 4, 1, 8, true);
	gx_gui::registerParam("test.highshelf", "HighShelf", (bool*)&exp_hs, true, true);
    exp_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(exp_window), "Experimental");
    if (GTK_WIDGET_REALIZED (gx_gui::fWindow)) {
    // replaced for GTK-2.13
    //if (gtk_widget_get_realized(gx_gui::fWindow)) {
	    gtk_window_set_transient_for(GTK_WINDOW(exp_window), GTK_WINDOW(gx_gui::fWindow));
    } else {
	    //this is a HACK. recall_state constructs this window before the main window is realized
	    g_idle_add(set_transient, exp_window);
    }
    //FIXME prevents digit entry (-> preset selection)
    //gtk_window_add_accel_group(GTK_WINDOW(exp_window),
    //                           gx_gui::GxMainInterface::instance()->fAccelGroup);
    gtk_signal_connect (GTK_OBJECT (exp_window), "delete_event", GTK_SIGNAL_FUNC(delete_event), NULL);
    gtk_signal_connect (GTK_OBJECT (exp_window), "hide", GTK_SIGNAL_FUNC(on_hide), NULL);
    gtk_signal_connect (GTK_OBJECT (exp_window), "show", GTK_SIGNAL_FUNC(on_show), NULL);
    GtkWidget *vbox = gtk_vbox_new(false, 10);
    g_signal_connect(vbox, "expose-event", G_CALLBACK(gx_cairo::AmpBox_expose), NULL);
    gtk_widget_show(vbox);
    GtkWidget *hbox = gtk_hbox_new(false, 10);
    gtk_widget_show(hbox);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, 0, 0, 5);
    GtkWidget *w = gtk_label_new("Select:");
    gtk_widget_show(w);
    gtk_box_pack_start(GTK_BOX(hbox), w, 0, 0, 5);
	GtkObject *adj = gtk_adjustment_new(4, 1, 8, 1, 2, 0);
	exp_sample_spin = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 1, 0);
    gtk_signal_connect (GTK_OBJECT(exp_sample_spin), "value-changed", GTK_SIGNAL_FUNC(exp_sr_changed), NULL);
    gtk_widget_show(exp_sample_spin);
    w = gtk_check_button_new_with_label("enable");
    gtk_signal_connect(GTK_OBJECT(w), "toggled", GTK_SIGNAL_FUNC(exp_on_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), w, 0, 0, 5);
    gtk_widget_show(w);
    w = gtk_check_button_new_with_label("upsample");
    gtk_signal_connect(GTK_OBJECT(w), "toggled", GTK_SIGNAL_FUNC(exp_up_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), w, 0, 0, 5);
    gtk_widget_show(w);
    w = gtk_check_button_new_with_label("HighShelf");
    gtk_widget_show(w);
    gtk_box_pack_start(GTK_BOX(hbox), w, 0, 0, 20);
    gtk_signal_connect(GTK_OBJECT(w), "toggled", GTK_SIGNAL_FUNC(exp_hs_toggled), NULL);
    hbox = gtk_hbox_new(false, 10);
    gtk_widget_show(hbox);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, 0, 0, 5);
    gtk_container_add(GTK_CONTAINER(exp_window), vbox);
    gtk_window_set_type_hint (GTK_WINDOW(exp_window), GDK_WINDOW_TYPE_HINT_UTILITY);
	for (list<setupfunc>::iterator i = setuplist.begin(); i != setuplist.end(); i++) {
		(*i)(hbox);
	}
}

void toggle_exp_window(bool v)
{
	if (v) {
		if (!GTK_WIDGET_VISIBLE(exp_window)) {
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(exp_sample_spin), exp_upsample);
			gtk_window_present(GTK_WINDOW(exp_window));
		}
	} else {
		if (GTK_WIDGET_VISIBLE(exp_window)) {
			gtk_widget_hide(exp_window);
		}
	}
}


#define IF_HS(fn) {if (exp_hs) { fn; }}
#else
#define EXPERIMENTAL_PROCESSING
#define IF_HS(fn) {fn;}
#endif // EXPERIMENTAL