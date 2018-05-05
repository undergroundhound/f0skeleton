#include "terminal.h"
#include "commands.h"

extern const sTermEntry_t hEntry;
extern const sTermEntry_t helpEntry;
extern const sTermEntry_t rebootEntry;
extern const sTermEntry_t addrEntry;
extern const sTermEntry_t idEntry;
extern const sTermEntry_t channelEntry;
extern const sTermEntry_t sendEntry;

const sTermEntry_t *term_entries[] =
{
        &hEntry,
        &helpEntry,
        &rebootEntry,
        &addrEntry,
        &idEntry,
        &channelEntry,
        &sendEntry,
        0
};
