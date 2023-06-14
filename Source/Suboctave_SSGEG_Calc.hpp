/*

Suboctave and QoL modifications by Spinning Square Waves#9987

""""made""""" by genatar_i#6807

INFO:
http://gendev.spritesmind.net/forum/viewtopic.php?p=5716#p5716
which was wrong for a lot of it :(

for help, thanks to:
オップナー2608#6983 - Puna
ValleyBell#8930
breakthetargets#0888

*/

#pragma once

#include <Suboctave_SSGEG_Calc.hpp>

struct ADSRvalues {
    unsigned int 
        attack = 0, 
        decay = 0, 
        sustain = 0, 
        decay2 = 0;

    ADSRvalues(unsigned int attackIn, unsigned int decayIn, unsigned int sustainIn, unsigned int decay2In) {
        attack = attackIn;
        decay = decayIn;
        sustain = sustainIn;
        decay2 = decay2In;
    }
};

int main(void);
unsigned int calcDecayTime(const unsigned int& ADSR, const unsigned int& keyRate, const unsigned int& sustainLevel, const unsigned int& decay2, unsigned int& d1Time, unsigned int& d2Time);
unsigned int calcAttackTime(const unsigned int& ADSR, const unsigned int& keyRate);
unsigned int findCounterShift(const unsigned int& rate);
unsigned int findWaitTime(const unsigned int& rate);
unsigned int findAttenuationIncrement(const unsigned int& rate, const unsigned int& input);
unsigned int findRate(const unsigned int& R, const unsigned int& Rks);
unsigned int calcSustainLevel(const unsigned int& ADSR);
void wantGotAttackCycles(const int& wantedCycles, const int& keyRate);
void wantGotDecayCycles(const int& wantedCycles, const int& keyRate);
void wantGotAttackDecayCycles(const int& wantedCycles, const int& keyRate);
void wantGotAttackDecaySustainCycles(const int& wantedCycles, const int& keyRate);
void wantGotAll(const int& wantedCycles, const int& keyRate);

void suboctaveMode(const int& wantedCycles);