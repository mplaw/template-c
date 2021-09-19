#include "test/unit_test.h"


static int expected_signal = 0;
static volatile sig_atomic_t seen_signal = 0;
static void (*old_handler)(int) = SIG_DFL;


void unit_test_signal_test_handler(int sig)
{
    seen_signal = sig;
}


int unit_test_signal_test_get_seen_signal()
{
    return seen_signal;
}


void unit_test_signal_test_setup(int expected_signal_)
{
    old_handler = signal(expected_signal_, unit_test_signal_test_handler);
    if (old_handler == SIG_ERR)
    {
        error("Failed to install signal handler for %i", expected_signal_);
        exit(SKIP_RETURN_CODE);
    }
    expected_signal = expected_signal_;
}


void unit_test_signal_test_reset()
{
    signal(expected_signal, old_handler);
    expected_signal = 0;
    seen_signal = 0;
}
