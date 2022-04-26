#ifndef Music_h
#define Music_h
#include "Notes.h"
#include <Zumo32U4.h>
#include <Arduino.h>
#define lenghtOfArray 24

class Music                         // Called this class for Music instead of Buzzer to avoid errors.
{
private:
    char index;
    unsigned long buzzer_timer;
    unsigned int current_tone;
    unsigned int current_duration;
    void gotoNextIndex();
    Zumo32U4Buzzer buzzer;

public:
    bool state;
    Music();
    void playTune();
};

extern unsigned int durations[24];
extern unsigned int melody[24];


#endif
