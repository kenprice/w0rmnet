#include <assert.h>
#include <stdlib.h>

/**
 * Returns an integer in the range [0, n).
 */
int randint(int n) {
    int end = RAND_MAX / n; // truncate skew
    assert (end > 0);
    end *= n;

    int r;
    while ((r = rand()) >= end);

    return r % n;
}
