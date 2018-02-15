#include "sched.h"
#include "ss_debug.h"
/*export ss_sched_class ptr for debbugging.. TODO remove :) */
extern void * get_ss_sched_class(void){
        return (void*)&ss_sched_class;
}
SS_EXPORT_IF_DEBUG(get_ss_sched_class);

