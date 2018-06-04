#include "terminal.h"
#include "commands.h"

extern const sTermEntry_t hEntry;
extern const sTermEntry_t helpEntry;
extern const sTermEntry_t rebootEntry;
extern const sTermEntry_t deviceEntry;
extern const sTermEntry_t addrEntry;
extern const sTermEntry_t idEntry;
extern const sTermEntry_t channelEntry;
extern const sTermEntry_t nrfEntry;

const sTermEntry_t *term_entries[] =
{
        &hEntry,
        &helpEntry,
        &rebootEntry,
        &deviceEntry,
        &addrEntry,
        &idEntry,
        &channelEntry,
        &nrfEntry,
        0
};
