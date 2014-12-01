#include "global.h"
#include "sys/init.h"
#include "app/main.h"

extern void (*_init_array[])(), (*_init_array_end[])();

void sys_init()
{
    // Call constructors of global objects
    for (void (**i)() = _init_array; i < _init_array_end; i++) (*i)();

    // Run main application program
    main();
}
