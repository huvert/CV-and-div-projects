#include "Music.h"
#include "Menu.h"

unsigned int melody[24] = {
    NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5,
    NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
    NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5,
    NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
};

unsigned int durations[24] = {
    8, 8, 8, 4, 4, 4,
    4, 5, 8, 8, 8, 8,
    8, 8, 8, 4, 4, 4,
    4, 5, 8, 8, 8, 8
};

Music::Music() {
    current_tone = 0;
    current_duration = 0;
    buzzer_timer = 0;
    state = false;
    index = 0;
}

/*  Function can be called in a while loop. By using millis we can run
    For example PD_controller while running Buzzer at the same time :)      */
void Music::playTune()
{
    if (buzzer_timer + current_duration < millis()) {          
        switch (state) {
        case false:
            // Play Note
            gotoNextIndex();
            current_tone = melody[index];
            current_duration = durations[index];
            current_duration = 1000 / current_duration;
            buzzer.playFrequency(current_tone, current_duration, 15);               // Volume Range: (0...15) 
            break;
        case true:
            // Pause
            current_duration *= 1.5;
            buzzer.playFrequency(0, 0, 0);                                          // Force buzzer to turn off.
            break;
        }
        // Update variables
        state != state;
        buzzer_timer = millis();
    }
}

void Music::gotoNextIndex() {
    index++;
    if (index >= lenghtOfArray) {
        index = 0;
    }
}
