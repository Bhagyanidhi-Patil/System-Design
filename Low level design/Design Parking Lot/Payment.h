#ifndef PAYMENT_H
#define PAYMENT_H
#include <ctime>
#include "PricingStrategy.h"

class Payment {
private:
    static PricingStrategy* strategy;
public:
    static void setStrategy(PricingStrategy* newStrategy);
    static double calculateFee(std::time_t entryTime);
};
#endif