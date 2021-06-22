#include <stdlib.h>
#include "Mathf.h"

// •‚“®¬”“_Zp
float Mathf::RandomRange(float min, float max)
{
    float rnd;
    float RRND;
    rnd = static_cast<float>(rand()) / RAND_MAX;
    RRND = (max + min) - (min * rnd);
    return RRND;
}