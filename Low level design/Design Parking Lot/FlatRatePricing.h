#ifndef FLATRATEPRICING_H
#define FLATRATEPRICING_H

#include "PricingStrategy.h"

class FlatRatePricing : public PricingStrategy {
public:
    double calculateFee(std::time_t entryTime) override;
};

#endif