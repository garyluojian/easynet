#include <signal.h>

namespace
{
    class IgnoreSigHup
    {
    public:
        IgnoreSigHup()
        {
            (void)signal(SIGHUP, SIG_IGN);
        }
    };

    IgnoreSigHup ignore_sighup;
}