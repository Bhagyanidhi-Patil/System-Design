#ifndef PRICINGSTRATEGY_H
#define PRICINGSTRATEGY_H

#include <ctime>

class PricingStrategy {
public:
    virtual double calculateFee(std::time_t entryTime) = 0;
    virtual ~PricingStrategy() {}
};

#endif