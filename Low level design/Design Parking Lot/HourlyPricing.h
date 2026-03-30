#ifndef HOURLYPRICING_H
#define HOURLYPRICING_H

#include "PricingStrategy.h"

class HourlyPricing : public PricingStrategy {
public:
    double calculateFee(std::time_t entryTime) override;
};

#endif