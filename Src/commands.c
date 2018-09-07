#include "terminal.h"
#include "commands.h"

extern const sTermEntry_t hEntry;
extern const sTermEntry_t helpEntry;
extern const sTermEntry_t rebootEntry;
extern const sTermEntry_t deviceEntry;
extern const sTermEntry_t netIdEntry;
extern const sTermEntry_t idEntry;
extern const sTermEntry_t channelEntry;
extern const sTermEntry_t nrfEntry;
extern const sTermEntry_t adcEntry;

const sTermEntry_t *term_entries[] =
{
        &hEntry,
        &helpEntry,
        &rebootEntry,
        &deviceEntry,
        &idEntry,
        &netIdEntry,
        &channelEntry,
        &nrfEntry,
        &adcEntry,
        0
};
