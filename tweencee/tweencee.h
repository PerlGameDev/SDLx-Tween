#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "SDL/SDL.h"

/* all times duration deltas in ticks (1ms)
 *
 * TODO
 * should be using sv_setsv(SV*, SV*); to set SV* if it is already set
 * what is correct dance with stack when calling perl from xs?
 * how to use SDL.h portably?
 * check stack size instead of using SvIOK in start
 * error checking in perl
 * "Floating point division with a constant or repeated division with the same value should of course be done by multiplying with the reciprocal"
 * final and initial ticks even if stopped externally with accurate init and final values from solver
 * call a perl cb, set a hash, method call, tick cb is what?
 * each path has an output type
 * each proxy has an input type and they must match
 * e.g. circle, path linear, d=1, proxy-> int proxy rounds,distinct,calls method
 *              path out = double, proxy in  -> double 
 * mutli d, multi linear, fast set, sdl sprite you draw opt
 */

/* ------------------------------ tween ------------------------------ */

typedef struct sdl_tween {

       SV*   register_cb;
       SV*   unregister_cb;

    Uint32   duration;
      bool   forever;
       int   repeat;
      bool   bounce;

      bool   is_reversed;
      bool   is_active;
    Uint32   cycle_start_time;
    Uint32   last_tick_time;
    Uint32   last_cycle_complete_time;

    double   (*ease_func) (double);

     void*   path;
     void*   (*path_build_func ) (SV*);
      void   (*path_free_func  ) (void*);
      void   (*path_solve_func ) (void*, double, double[]);

     void*   proxy;
     void*   (*proxy_build_func) (SV*);
      void   (*proxy_free_func ) (void*);
      void   (*proxy_set_func  ) (void*, double);

} sdl_tween;

typedef sdl_tween* SDLx__Tween;

/* ------------------------------ easing ----------------------------- */

double ease_linear        (double t);
double ease_swing         (double t);
double ease_out_bounce    (double t);
double ease_in_bounce     (double t);
double ease_in_out_bounce (double t);

/* ------------------------------ path ------------------------------- */

typedef struct sdl_tween_path_linear_1D {

    double  from[4];
    double  to[4];
    int     dim;

} sdl_tween_path_linear_1D;

typedef sdl_tween_path_linear_1D* SDLx__Tween__Path__Linear1D;

void*  path_linear_1D_build (SV* path_args);
void   path_linear_1D_free  (void* thisp);
void   path_linear_1D_solve (void* thisp, double t, double solved[]);

/* ------------------------------ proxy ------------------------------- */

/* in = double, out = call method with distinct int */
typedef struct sdl_tween_proxy_method {

    SV*    target;
    char*  method;
    bool   round;
    int    last_value;
    bool   is_init;

} sdl_tween_proxy_method;

typedef sdl_tween_proxy_method* SDLx__Tween__Proxy__Method;

void*  proxy_method_build (SV* proxy_args);
void   proxy_method_free  (void* thisp);
void   proxy_method_set   (void* thisp, double inval);

