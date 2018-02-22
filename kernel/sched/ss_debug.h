/*
file ss_debug.h
@version 0.0.1
contains all the debug helper macros
if the SS_DEBUG_MODE is defined , then every symbol is exported for modules to use
disable the debug mode simply commenting the definition of SS_DEBUG_MODE
enjoy!
*/


#define SS_DEBUG_MODE 1
#ifdef SS_DEBUG_MODE
	#define ss_debug_lvl KERN_CRIT
	#define ss_debug(...) printk(ss_debug_lvl"ss_log: " __VA_ARGS__ )
	#define SS_EXPORT_IF_DEBUG(symbol) EXPORT_SYMBOL(symbol)
#else
	#define ss_debug_lvl
	#define ss_debug(...)
	#define SS_EXPORT_IF_DEBUG(symbol)
#endif
#define temp_debug(...) ss_debug(__VA_ARGS__)
