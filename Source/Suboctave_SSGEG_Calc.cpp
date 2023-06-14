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

#include <stdlib.h>
#include <iostream>
#include <vector>
#include "include/Suboctave_SSGEG_Calc.hpp"

// 10 bits = 0-1023
const unsigned int VOLUME_MAX = 0b1111111111;
const unsigned int SSGEG_VOLUME_MAX = 0b0111111111;	// during Decay/Sustain/Release, SSG-EG will go back to Attack at 0x200
// 6 bits - 0-63
const unsigned int RATE_MAX = 0b111111;

double CLOCK_HZ = 53693175.0 / 7;


const unsigned int FM_PRESCALER = 6;
double FM_CLOCK = CLOCK_HZ / FM_PRESCALER;
double OUTPUT_FREQUENCY_HZ = FM_CLOCK / 24;
double EG_CLOCK = OUTPUT_FREQUENCY_HZ / 3;

const unsigned int counterShiftValues[RATE_MAX + 1] = { // eg_rate_shift
  11, 11, 11, 11,  //  0 -  3
  10, 10, 10, 10,  //  4 -  7
   9,  9,  9,  9,  //  8 - 11
   8,  8,  8,  8,  // 12 - 15
   7,  7,  7,  7,  // 16 - 19
   6,  6,  6,  6,  // 20 - 23
   5,  5,  5,  5,  // 24 - 27
   4,  4,  4,  4,  // 28 - 31
   3,  3,  3,  3,  // 32 - 35
   2,  2,  2,  2,  // 36 - 39
   1,  1,  1,  1,  // 40 - 43
   0,  0,  0,  0,  // 44 - 47
   0,  0,  0,  0,  // 48 - 51
   0,  0,  0,  0,  // 52 - 55
   0,  0,  0,  0,  // 56 - 59
   0,  0,  0,  0,  // 60 - 63
};

const unsigned int attenuationIncrementValues[(RATE_MAX + 1) * 8] = {
   0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,  0,1,0,1,0,1,0,1,  0,1,0,1,0,1,0,1,  //  0 -  3
   0,1,0,1,0,1,0,1,  0,1,0,1,0,1,0,1,  0,1,1,1,0,1,1,1,  0,1,1,1,0,1,1,1,  //  4 -  7
   0,1,0,1,0,1,0,1,  0,1,0,1,1,1,0,1,  0,1,1,1,0,1,1,1,  0,1,1,1,1,1,1,1,  //  8 - 11
   0,1,0,1,0,1,0,1,  0,1,0,1,1,1,0,1,  0,1,1,1,0,1,1,1,  0,1,1,1,1,1,1,1,  // 12 - 15
   0,1,0,1,0,1,0,1,  0,1,0,1,1,1,0,1,  0,1,1,1,0,1,1,1,  0,1,1,1,1,1,1,1,  // 16 - 19
   0,1,0,1,0,1,0,1,  0,1,0,1,1,1,0,1,  0,1,1,1,0,1,1,1,  0,1,1,1,1,1,1,1,  // 20 - 23
   0,1,0,1,0,1,0,1,  0,1,0,1,1,1,0,1,  0,1,1,1,0,1,1,1,  0,1,1,1,1,1,1,1,  // 24 - 27
   0,1,0,1,0,1,0,1,  0,1,0,1,1,1,0,1,  0,1,1,1,0,1,1,1,  0,1,1,1,1,1,1,1,  // 28 - 31
   0,1,0,1,0,1,0,1,  0,1,0,1,1,1,0,1,  0,1,1,1,0,1,1,1,  0,1,1,1,1,1,1,1,  // 32 - 35
   0,1,0,1,0,1,0,1,  0,1,0,1,1,1,0,1,  0,1,1,1,0,1,1,1,  0,1,1,1,1,1,1,1,  // 36 - 39
   0,1,0,1,0,1,0,1,  0,1,0,1,1,1,0,1,  0,1,1,1,0,1,1,1,  0,1,1,1,1,1,1,1,  // 40 - 43
   0,1,0,1,0,1,0,1,  0,1,0,1,1,1,0,1,  0,1,1,1,0,1,1,1,  0,1,1,1,1,1,1,1,  // 44 - 47
   1,1,1,1,1,1,1,1,  1,1,1,2,1,1,1,2,  1,2,1,2,1,2,1,2,  1,2,2,2,1,2,2,2,  // 48 - 51
   2,2,2,2,2,2,2,2,  2,2,2,4,2,2,2,4,  2,4,2,4,2,4,2,4,  2,4,4,4,2,4,4,4,  // 52 - 55
   4,4,4,4,4,4,4,4,  4,4,4,8,4,4,4,8,  4,8,4,8,4,8,4,8,  4,8,8,8,4,8,8,8,  // 56 - 59
   8,8,8,8,8,8,8,8,  8,8,8,8,8,8,8,8,  8,8,8,8,8,8,8,8,  8,8,8,8,8,8,8,8,  // 60 - 63
};

using namespace std;
using std::cout;
using std::cin;

int main(void) {

    int clockOption = 0;
    cout << endl << "Enter Ext Clock Rate (Chip Clock * 7)" << endl;
    cout << "Enter 0 to exit, 1 for NTSC (53693175.0), 2 for PAL(53,203,424), or enter Custom Clock" << endl;
    cin >> clockOption;

    switch (clockOption) {
    case 0:
        exit(0);
        break;

    case 1:
        CLOCK_HZ = 53693175.0 / 7;
        FM_CLOCK = CLOCK_HZ / FM_PRESCALER;
        OUTPUT_FREQUENCY_HZ = FM_CLOCK / 24;
        EG_CLOCK = OUTPUT_FREQUENCY_HZ / 3;
        break;

    case 2:
        CLOCK_HZ = 53203424.0 / 7;
        FM_CLOCK = CLOCK_HZ / FM_PRESCALER;
        OUTPUT_FREQUENCY_HZ = FM_CLOCK / 24;
        EG_CLOCK = OUTPUT_FREQUENCY_HZ / 3;
        break;

    default:
        CLOCK_HZ = clockOption / 7.0;
        FM_CLOCK = CLOCK_HZ / FM_PRESCALER;
        OUTPUT_FREQUENCY_HZ = FM_CLOCK / 24;
        EG_CLOCK = OUTPUT_FREQUENCY_HZ / 3;
        break;
    }

    double wantedCycles = 0;
    double wantedFrequency = 0;
    while (true) {
        cout << endl << "Enter Wanted Frequency or -1 to go back: " << endl;
        cout << "Upper Frequency: " << (EG_CLOCK / 16) << "Hz" << endl;
        cin >> wantedCycles;
        
        if (wantedCycles < 0) {
            // go back to the start and forget the rest
            main();
            return 0;
        }
        
        wantedFrequency = wantedCycles;
        wantedCycles = int(EG_CLOCK / wantedCycles);

        if (wantedCycles < 16) {
            cout << "Too high Hz Value!" << endl << endl;
        }
        else {
            break;
        }
    }

    if (abs((EG_CLOCK / (wantedCycles - 1)) - wantedFrequency) < abs((EG_CLOCK / wantedCycles) - wantedFrequency)) {
        wantedCycles--;
    }
    else if (abs((EG_CLOCK / (wantedCycles + 1)) - wantedFrequency) < abs((EG_CLOCK / wantedCycles) - wantedFrequency)) {
        wantedCycles++;
    }



    cout << endl << "Closest Frequency to target: " << EG_CLOCK / wantedCycles << "Hz" << endl;
    cout << "ADSR Cycles for wanted Frequency: " << wantedCycles << " cycles" << endl;
    cout << "One Over: " << EG_CLOCK / (wantedCycles - 1) << "Hz | One Under: " << EG_CLOCK / (wantedCycles + 1) << "Hz" << endl;

    cout << endl << endl << "Activate Suboctave mode?" << endl
        << "\n(Key Rate = 0, Decay + Sustain + D2 for pitch, and Attack for timbre)" << endl
        << "1 for yes, 0 for no" << endl;
    int suboctave = 0;
    cin >> suboctave;

    if (suboctave == 1) {
        suboctaveMode(wantedCycles);

        // loop back
        main();
        return 0;
    }

    cout << endl << endl << "Calculate ADS+D2 rate? (will take much longer)" << endl;
    cout << "1 for yes, 0 for no" << endl;
    int calcAll = 0;
    cin >> calcAll;

    cout << endl << "Attack only" << endl;
    wantGotAttackCycles(wantedCycles, 0);
    wantGotAttackCycles(wantedCycles, 1);
    wantGotAttackCycles(wantedCycles, 2);
    wantGotAttackCycles(wantedCycles, 3);

    cout << endl << "Decay only" << endl;
    wantGotDecayCycles(wantedCycles, 0);
    wantGotDecayCycles(wantedCycles, 1);
    wantGotDecayCycles(wantedCycles, 2);
    wantGotDecayCycles(wantedCycles, 3);

    cout << endl << "Attack + Decay" << endl;
    wantGotAttackDecayCycles(wantedCycles, 0);
    wantGotAttackDecayCycles(wantedCycles, 1);
    wantGotAttackDecayCycles(wantedCycles, 2);
    wantGotAttackDecayCycles(wantedCycles, 3);


    cout << endl << "Attack + Decay + Sustain + D2: 31" << endl;
    wantGotAttackDecaySustainCycles(wantedCycles, 0);
    wantGotAttackDecaySustainCycles(wantedCycles, 1);
    wantGotAttackDecaySustainCycles(wantedCycles, 2);
    wantGotAttackDecaySustainCycles(wantedCycles, 3);

    if (calcAll == 1) {
        cout << endl << "All (This will take a while lmao)" << endl;
        wantGotAll(wantedCycles, 0);
        wantGotAll(wantedCycles, 1);
        wantGotAll(wantedCycles, 2);
        wantGotAll(wantedCycles, 3);
    }

    // loop to start of main
    main();

    return 0;
}

void suboctaveMode(const int& wantedCycles) {
    while (true) {
        int attackVal = 0;

        do {
            cout << endl << endl << "Enter desired Attack value (31 - 0) or -1 to go back:" << endl;
            cin >> attackVal;

            if (attackVal < 0) {
                // loop back
                main();
                return;
            }

        } while (attackVal > 31);

        unsigned unused1, unused2;

        int keyRate = 0;

        int closestCycleVal = 0;
        //unsigned closestDecayVal = 0;
        //unsigned closestSustainVal = 0;
        //unsigned closestD2Val = 0;
        int currentCycleVal = 0;

        vector<ADSRvalues> closestCombos;

        // stopping at decay2 10 saves time since 10 and lower is always below the range of perceivable frequency
        // stopping at decay 8 saves time since 8 and lower is always below the range of perceivable frequency

        for (unsigned d2Val = 31; d2Val > 10; --d2Val) {
            for (unsigned sustainVal = 0; sustainVal < 16; ++sustainVal) {
                for (unsigned decayVal = 31; decayVal > 8; --decayVal) {

                    currentCycleVal = calcAttackTime(attackVal, keyRate) + calcDecayTime(decayVal, keyRate, sustainVal, d2Val, unused1, unused2);
                    
                    if (currentCycleVal == closestCycleVal) {
                        closestCombos.push_back(ADSRvalues(attackVal, decayVal, sustainVal, d2Val));
                    }
                    else if (abs(currentCycleVal - wantedCycles) < abs(closestCycleVal - wantedCycles)) {
                        closestCycleVal = currentCycleVal;
                        //closestDecayVal = decayVal;
                        //closestSustainVal = sustainVal;
                        //closestD2Val = d2Val;
                        
                        closestCombos.clear();
                        closestCombos.push_back(ADSRvalues(attackVal, decayVal, sustainVal, d2Val));
                    }
                }
            }
        }

        cout << "Wanted: " << (EG_CLOCK / wantedCycles) << " Got: " << (EG_CLOCK / closestCycleVal) << endl;

        cout << endl << "Input a value for cleanup, -1 to skip. Results with a decay difference higher than this will be removed" << endl;
        int cleanUp = 0;
        cin >> cleanUp;

        if (cleanUp == -1) {
            for (int i = 0; i < closestCombos.size(); i++) {
                cout << " With A:" << closestCombos[i].attack << ", D:" << closestCombos[i].decay << ", S:" << closestCombos[i].sustain << ", and D2:" << closestCombos[i].decay2 << endl;
            }
            suboctaveMode(wantedCycles);
        }
        
        for (int i = 0; i < closestCombos.size(); i++) {
            if (abs(int(closestCombos[i].decay) - int(closestCombos[i].decay2)) <= cleanUp) {
                cout << " With A:" << closestCombos[i].attack << ", D:" << closestCombos[i].decay << ", S:" << closestCombos[i].sustain << ", and D2:" << closestCombos[i].decay2 << endl;
            }
        }

        suboctaveMode(wantedCycles);
    }
}



void wantGotAll(const int& wantedCycles, const int& keyRate) {
    cout << "Key Rate: " << keyRate << " ";

    unsigned unused1, unused2;

    int closestCycleVal = 0;
    unsigned closestAttackVal = 0;
    unsigned closestDecayVal = 0;
    unsigned closestSustainVal = 0;
    unsigned closestD2Val = 0;
    int currentCycleVal = 0;

    // stopping at 10 saves time since 10 and lower is always below the range of perceivable frequency
    for (unsigned d2Val = 31; d2Val > 10; --d2Val) {
        for (unsigned sustainVal = 0; sustainVal < 16; ++sustainVal) {
            // stopping at 8 saves time since 8 and lower is always below the range of perceivable frequency
            for (unsigned decayVal = 31; decayVal > 8; --decayVal) {
                // stopping at 15 saves time since 15 and lower is always below the range of perceivable frequency
                for (unsigned attackVal = 31; attackVal > 15; --attackVal) {
                    currentCycleVal = calcAttackTime(attackVal, keyRate) + calcDecayTime(decayVal, keyRate, sustainVal, d2Val, unused1, unused2);
                    if (abs(currentCycleVal - wantedCycles) < abs(closestCycleVal - wantedCycles)) {
                        closestCycleVal = currentCycleVal;
                        closestAttackVal = attackVal;
                        closestDecayVal = decayVal;
                        closestSustainVal = sustainVal;
                        closestD2Val = d2Val;
                    }
                }
            }
        }
    }

    cout << "Wanted: " << (EG_CLOCK / wantedCycles);
    cout << " " << "Got: " << (EG_CLOCK / closestCycleVal) << " With A:" << closestAttackVal << ", D:" << closestDecayVal << ", S:" << closestSustainVal << ", and D2:" << closestD2Val << endl;
}

vector<unsigned> makeAttackVector(const int& keyRate) {
    vector<unsigned> bruh;
    for (unsigned ADSRVal = 31; ADSRVal > 0; --ADSRVal) {
        bruh.push_back(calcAttackTime(ADSRVal, keyRate));
    }

    return bruh;
}

void wantGotAttackDecaySustainCycles(const int& wantedCycles, const int& keyRate) {
    cout << "Key Rate: " << keyRate << " ";

    unsigned unused1, unused2;

    int closestCycleVal = 0;
    unsigned closestAttackVal = 0;
    unsigned closestDecayVal = 0;
    unsigned closestSustainVal = 0;
    int currentCycleVal = 0;

    for (unsigned sustainVal = 0; sustainVal < 16; ++sustainVal) {
        // stopping at 8 saves time since 8 and lower is always below the range of perceivable frequency
        for (unsigned decayVal = 31; decayVal > 8; --decayVal) {
            // stopping at 15 saves time since 15 and lower is always below the range of perceivable frequency
            for (unsigned attackVal = 31; attackVal > 15; --attackVal) {
                currentCycleVal = calcAttackTime(attackVal, keyRate) + calcDecayTime(decayVal, keyRate, sustainVal, 31, unused1, unused2);//fastest D2
                if (abs(currentCycleVal - wantedCycles) < abs(closestCycleVal - wantedCycles)) {
                    closestCycleVal = currentCycleVal;
                    closestAttackVal = attackVal;
                    closestDecayVal = decayVal;
                    closestSustainVal = sustainVal;
                }
            }
        }
    }

    cout << "Wanted: " << (EG_CLOCK / wantedCycles);
    cout << " " << "Got: " << (EG_CLOCK / closestCycleVal) << " With A:" << closestAttackVal << ", D:" << closestDecayVal << ", and S:" << closestSustainVal << endl;
}

void wantGotAttackDecayCycles(const int& wantedCycles, const int& keyRate) {
    cout << "Key Rate: " << keyRate << " ";

    unsigned unused1, unused2;

    int closestCycleVal = 0;
    unsigned closestAttackVal = 0;
    unsigned closestDecayVal = 0;
    int currentCycleVal = 0;

    // stopping at 8 saves time since 8 and lower is always below the range of perceivable frequency
    for (unsigned decayVal = 31; decayVal > 8; --decayVal) {
        for (unsigned attackVal = 31; attackVal > 0; --attackVal) {
            currentCycleVal = calcAttackTime(attackVal, keyRate) + calcDecayTime(decayVal, keyRate, 15, 31, unused1, unused2);//fastest D2
            if (abs(currentCycleVal - wantedCycles) < abs(closestCycleVal - wantedCycles)) {
                closestCycleVal = currentCycleVal;
                closestAttackVal = attackVal;
                closestDecayVal = decayVal;
            }
        }
    }

    cout << "Wanted: " << (EG_CLOCK / wantedCycles);
    cout << " " << "Got: " << (EG_CLOCK / closestCycleVal) << " With A:" << closestAttackVal << " and D:" << closestDecayVal << endl;
}

void wantGotDecayCycles(const int& wantedCycles, const int& keyRate) {
    cout << "Key Rate: " << keyRate << " ";

    unsigned unused1, unused2;

    int closestCycleVal = 0;
    unsigned closestADSRVal = 0;
    int currentCycleVal = 0;

    //Decay Only
    // stopping at 8 saves time since 8 and lower is always below the range of perceivable frequency
    for (unsigned ADSRVal = 31; ADSRVal > 8; --ADSRVal) {
        currentCycleVal = calcAttackTime(31, keyRate) + calcDecayTime(ADSRVal, keyRate, 15, 31, unused1, unused2);//fastest decay
        if (abs(currentCycleVal - wantedCycles) < abs(closestCycleVal - wantedCycles)) {
            closestCycleVal = currentCycleVal;
            closestADSRVal = ADSRVal;
        }
    }

    cout << "Wanted: " << (EG_CLOCK / wantedCycles);
    cout << " " << "Got: " << (EG_CLOCK / closestCycleVal) << " With Value " << closestADSRVal << endl;
}

void wantGotAttackCycles(const int& wantedCycles, const int& keyRate) {
    cout << "Key Rate: " << keyRate << " ";

    unsigned unused1, unused2;

    int closestCycleVal = 0;
    unsigned closestADSRVal = 0;
    int currentCycleVal = 0;

    //Attack Only
    for (unsigned ADSRVal = 31; ADSRVal > 0; --ADSRVal) {
        currentCycleVal = calcAttackTime(ADSRVal, keyRate) + calcDecayTime(31, keyRate, 15, 31, unused1, unused2);//fastest decay
        if (abs(currentCycleVal - wantedCycles) < abs(closestCycleVal - wantedCycles)) {
            closestCycleVal = currentCycleVal;
            closestADSRVal = ADSRVal;
        }
    }

    cout << "Wanted: " << (EG_CLOCK / wantedCycles);
    cout << " " << "Got: " << (EG_CLOCK / closestCycleVal) << " With Value " << closestADSRVal << endl;
}

unsigned int calcSustainSwitchLevel(const unsigned int& ADSR) {
    if (ADSR == 15) {
        return 992;
    }

    return ADSR * 32;

}

unsigned int calcDecayTime(const unsigned int& ADSR, const unsigned int& keyRate, const unsigned int& sustainLevel, const unsigned int& decay2, unsigned int& d1Time, unsigned int& d2Time) {
    unsigned int rate = findRate(ADSR, keyRate);

    unsigned int currentVolume = 0; // assume currentVolume starts at 0;

    unsigned int attenuationIndex = 0;
    unsigned int waitTime = findWaitTime(rate); // in table 1, how long to wait for volume change
    unsigned int currentCounterShift = waitTime;	// delayed trigger (due to coming from attack phase)
    unsigned int cycles = 0;

    while (currentVolume < min(calcSustainSwitchLevel(sustainLevel), SSGEG_VOLUME_MAX) /*to sustain level*/) {

        //decay 1 untill sustain level reached
        if (currentCounterShift == 0) {
            // if decay:
            unsigned int attenuationDelta = findAttenuationIncrement(rate, attenuationIndex) * 4; // *4 because in SSG-EG

            currentVolume += min(VOLUME_MAX - currentVolume, attenuationDelta);



            //goes through attenVector
            attenuationIndex = (attenuationIndex + 1) % 8;

            //reset the wait time
            currentCounterShift = waitTime;
        }
        currentCounterShift--;
        cycles++;
    }

    rate = findRate(decay2, keyRate);
    waitTime = findWaitTime(rate);
    currentCounterShift = currentCounterShift % waitTime;

    d1Time = cycles - 1;

    //decay 2

    while (currentVolume < SSGEG_VOLUME_MAX) {

        if (currentCounterShift == 0) {
            // if decay:
            unsigned int attenuationDelta = findAttenuationIncrement(rate, attenuationIndex) * 4; // *4 because in SSG-EG

            currentVolume += min(VOLUME_MAX - currentVolume, attenuationDelta);



            //goes through attenVector
            attenuationIndex = (attenuationIndex + 1) % 8;

            //reset the wait time
            currentCounterShift = waitTime;
        }
        currentCounterShift--;
        cycles++;
    }

    d2Time = cycles - d1Time;

    return cycles - 1;
}

unsigned int calcAttackTime(const unsigned int& ADSR, const unsigned int& keyRate) {
    unsigned int rate = findRate(ADSR, keyRate);

    // NOTE: After triggering KeyOn, it will start at VOLUME_MAX (0x3FF).
    //       However when switching from decay/sustain back to attack, it will start at 0x200
    int currentVolume = SSGEG_VOLUME_MAX;

    unsigned int attenuationIndex = 0;  // (OPN->eg_cnt>>SLOT->eg_sh_ar) & 7
    unsigned int waitTime = findWaitTime(rate); //in table 1, how long to wait for volume change
    unsigned int currentCounterShift = waitTime;
    unsigned int cycles = 0;

    // special condition: if rate shifted back to 5-bits == 31, 1-cycle attack
    if (rate / 2 == 31)
        return 1;


    while (currentVolume > 0) {
        if (currentCounterShift == 0) {
            // if attack:
            int attenuationDelta = (findAttenuationIncrement(rate, attenuationIndex) * (currentVolume + 1) + 15) / 16;

            currentVolume -= min(currentVolume, attenuationDelta);

            //goes through attenVector
            attenuationIndex = (attenuationIndex + 1) % 8;

            //reset the wait time
            currentCounterShift = waitTime;
        }
        currentCounterShift--;
        cycles++;
    }
    return cycles - 1;
}

unsigned int findCounterShift(const unsigned int& rate) {
    return counterShiftValues[rate];
}

unsigned int findWaitTime(const unsigned int& rate) {
    return 1 << findCounterShift(rate);
}

unsigned int findAttenuationIncrement(const unsigned int& rate, const unsigned int& update) {
    return attenuationIncrementValues[(rate * 8) + update];
}

unsigned int findRate(const unsigned int& R, const unsigned int& Rks) {
    return (R == 0) ? 0 : min((R * 2) + Rks, RATE_MAX);
}
