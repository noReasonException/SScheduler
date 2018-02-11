#include "sched.h"
/*export ss_sched_class ptr for debbugging.. TODO remove :) */
extern void * get_ss_sched_class(void){
        return (void*)&ss_sched_class;
}
EXPORT_SYMBOL(get_ss_sched_class);

