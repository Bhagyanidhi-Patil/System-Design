#include "FlatRatePricing.h"
#include <ctime>

double FlatRatePricing::calculateFee(std::time_t entryTime) {
    // Flat rate of ₹50 regardless of time
    return 50.0;
}