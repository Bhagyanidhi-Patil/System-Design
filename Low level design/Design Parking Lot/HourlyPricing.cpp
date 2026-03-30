#include "HourlyPricing.h"
#include <ctime>

double HourlyPricing::calculateFee(std::time_t entryTime) {
    std::time_t now = std::time(nullptr);
    double hours = std::difftime(now, entryTime) / 3600.0;
    if (hours < 0) hours = 0;
    return hours * 25.0;  // ₹25 per hour (different from default)
}