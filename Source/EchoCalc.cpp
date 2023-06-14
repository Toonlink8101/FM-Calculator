/*
    Made by Spinning Square Waves#9987

    Started: 5/18/2023
    Last Updated: 5/25/2023
*/

/*
    -------------
        ABOUT
    -------------
    This is a companion tool for Furnace that calculates the multiplier 
        settings needed for FM-based single-channel echoes.
    It was designed for use with the OPN2/YM2612, but it should also work
        with some other 4op FM chips, such as the OPM/YM2151 or the 
        OPNA/YM2608.
    
    It assumes that your instrument is set up with algorithm 4, where each 
        operator has a mult of 8, and op3+op4 is the echo of op1+op2.
    It also assumes that the instument is percussive, meaning that gaps 
        and non-notes will all be treated as a rest.
    Evenly spaced gaps can be removed in Furnace with Edit > Collapse.
    If you want to add gaps back to the result, simply use Edit > Expand.

    Note that the calculator will ignore all instruments, volumes, and 
        effects pasted into it.
    The output will always have 6 columns of effects: 4 for the multiplier
        settings, one for the tuning, and another for the legato.
    The legato is only necessary to prevent retriggering during a rest
        before the echo has caught up with said rest
*/

#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>

using namespace std;

struct intervalValues{
    char
        firstMultiplier,
        secondMultiplier;
    int
        playedNoteValue;
    string
        detuneValue;
};

intervalValues getIntervalRatio(int, int);
int noteToValue(string);
string valueToNote(int);
void getInputData();
void processData(int);

string 
    input = "",
    header = "";

vector<string> patternData;

int main(){
    cout << "\nPlease read the ABOUT section of the source code before using."
        << "\nThank you!\n";

    getInputData();

    cout << "Input the delay in rows (0 to exit): ";
    int delay;
    std::cin >> delay;

    if(delay <= 0){
        return 0;
    }

    processData(delay);

    return 0;
}

void processData(int delay){
    queue<int> delayBuffer;
    
    cout << "\nCopy/paste-mix (ctrl+shift+V) the following into Furnace:\n\n"
        << header
        << "\n0\n";

    for(int i = 0; i < delay; i++){
        delayBuffer.push(noteToValue(patternData[i]));
        
        cout << valueToNote(delayBuffer.back())
                << "...."
                << "1618"
                << "1628"
                << "16.."
                << "16.."
                << "E580"
                << "EA.."
                << "|\n";
    }
    
    intervalValues currentInterval;

    string previousDetune = "80";

    bool previousLegato = false, currentLegato = false;

    for(int i = delay; i < patternData.size(); i++){
        delayBuffer.push(noteToValue(patternData[i]));
        
        currentInterval = getIntervalRatio(delayBuffer.front(), delayBuffer.back());

        currentLegato = delayBuffer.back() >= 120 && delayBuffer.front() < 120;

        cout << valueToNote(currentInterval.playedNoteValue)
            << "...."
            << "161" << currentInterval.firstMultiplier
            << "162" << currentInterval.firstMultiplier
            << "163" << currentInterval.secondMultiplier
            << "164" << currentInterval.secondMultiplier;
        if(currentInterval.detuneValue != previousDetune){
            cout << "E5" << currentInterval.detuneValue;
        }
        else{
            cout << "....";
        }
        if(previousLegato != currentLegato){
            cout << "EA0" << currentLegato;
        }
        else{
            cout << "....";
        }
        cout << "|\n";

        previousDetune = currentInterval.detuneValue;

        previousLegato = currentLegato;

        delayBuffer.pop();
    }

    while(delayBuffer.size() > 0){

        currentInterval = getIntervalRatio(delayBuffer.front(), delayBuffer.back());

        cout << valueToNote(currentInterval.playedNoteValue)
            << "...."
            << "161" << currentInterval.firstMultiplier
            << "162" << currentInterval.firstMultiplier
            << "163" << currentInterval.secondMultiplier
            << "164" << currentInterval.secondMultiplier;
        if(currentInterval.detuneValue != previousDetune){
            cout << "E5" << currentInterval.detuneValue;
        }
        else{
            cout << "....";
        }
        cout << "...."
            << "|\n";

        previousDetune = currentInterval.detuneValue;
        
        delayBuffer.pop();
    }
}

void getInputData(){
    bool validInput = false;
    
    do{
        cout << "\nCopy and paste a column of notes from Furnace into the terminal.\n"
            << "Input \"end\" on a new line when you're done:\n";
        
        std::cin >> input;
        header += input + " ";
        std::cin >> input;
        header += input + " ";
        std::cin >> input;
        header += input + " ";
        std::cin >> input;
        header += input + " ";
        std::cin >> input;
        header += input + " ";

        while(input != "end"){
            if(validInput){
                patternData.push_back(input);
            }
            
            validInput = validInput || input == "0";

            std::cin >> input;
        }

        if(!validInput){
            cout << "\nInvalid input!\n";
        }
        
    } while(!validInput);
}

string valueToNote(int noteValue){
    if(noteValue == 120){
        return "...";
    }


    bool isSuboctave = noteValue < 0;

    if(isSuboctave){
        noteValue *= -1;
    }
    int octave = noteValue / 12 + isSuboctave;

    string noteName;

    // remove octave offset
    noteValue %= 12;

    // flip values if isSuboctave
    if(isSuboctave){
        noteValue -= 12;
        noteValue *= -1;
    }

    switch (noteValue)
    {
    case 0:
        noteName = "C-";
        break;
    case 1:
        noteName = "C#";
        break;
    case 2:
        noteName = "D-";
        break;
    case 3:
        noteName = "D#";
        break;
    case 4:
        noteName = "E-";
        break;
    case 5:
        noteName = "F-";
        break;
    case 6:
        noteName = "F#";
        break;
    case 7:
        noteName = "G-";
        break;
    case 8:
        noteName = "G#";
        break;
    case 9:
        noteName = "A-";
        break;
    case 10:
        noteName = "A#";
        break;
    case 11:
        noteName = "B-";
        break;
    
    default:
        break;
    }

    if(isSuboctave){
        noteName[0] = tolower(noteName[0]);
        if(noteName[1] == '-'){
            noteName[1] = '_';
        }
        else if(noteName[1] == '#'){
            noteName[1] = '+';
        }
    }

    noteName += '0' + octave;

    return noteName;
}

/*
    Encodes notes as integers
    The integer is the number of semitones from C-0
    Negative numbers are notes in negative octaves

    Exceptions:
    Anything that isn't a note (a blank, a release command, etc.) is encoded as 120
    This is because B-9, the highest note supported by Furnace, is 119
*/
int noteToValue(string noteData){
    int noteValue = 0;

    switch (tolower(noteData[0])){
    case 'c':
        noteValue = 0;
        break;
    case 'd':
        noteValue = 2;
        break;
    case 'e':
        noteValue = 4;
        break;
    case 'f':
        noteValue = 5;
        break;
    case 'g':
        noteValue = 7;
        break;
    case 'a':
        noteValue = 9;
        break;
    case 'b':
        noteValue = 11;
        break;

    default:
        // not a note
        return 120;
        break;
    }

    // increment value if note is sharp
    // noteValue += int(noteData[1] == '+' || noteData[1] == '#');
    if(noteData[1] == '+' || noteData[1] == '#'){
        noteValue++;
    }

    int octaveOffset = int(noteData[2] - '0') * 12;

    // // if noteData is lowercase
    // if(tolower(noteData[0]) == noteData[0]){
    //     octaveOffset *= -1;
    // }
    noteValue += octaveOffset;
    
    return noteValue;
}

intervalValues getIntervalRatio(int delayedNoteValue, int currentNoteValue){
    if(delayedNoteValue >= 120){
        if(currentNoteValue >= 120){
            return {'8', '8', 120, "80"}; 
        }
        return {'8', '8', currentNoteValue, "80"}; 
    }
    else if(currentNoteValue >= 120){
        return {'8', '8', delayedNoteValue, "80"}; 
    }
    
    
    int difference = abs(delayedNoteValue - currentNoteValue);

    int lowerNoteValue = min(currentNoteValue, delayedNoteValue);
    int higherNoteValue = max(currentNoteValue, delayedNoteValue);

    intervalValues returnValues;

    switch (difference) {
    case 0:
        returnValues = {'8', '8', lowerNoteValue, "80"}; break;
    case 1:
        returnValues = {'E', 'F', lowerNoteValue - 10, "A0"}; break;
    case 2:
        returnValues = {'8', '9', lowerNoteValue, "80"}; break;
    case 3:
        returnValues = {'A', 'C', lowerNoteValue - 4, "88"}; break;
    case 4:
        returnValues = {'8', 'A', lowerNoteValue, "88"}; break;
    case 5:
        returnValues = {'6', '8', higherNoteValue, "80"}; break;
    case 6:
        returnValues = {'A', 'E', lowerNoteValue - 4, "9C"}; break;
    case 7:
        returnValues = {'8', 'C', lowerNoteValue, "80"}; break;
    case 8:
        returnValues = {'5', '8', higherNoteValue, "88"}; break;
    case 9:
        returnValues = {'6', 'A', higherNoteValue - 4, "88"}; break;
    case 10:
        returnValues = {'5', '9', higherNoteValue - 2, "84"}; break;
    case 11:
        returnValues = {'8', 'F', lowerNoteValue, "88"}; break;
    case 12:
        returnValues = {'4', '8', higherNoteValue, "80"}; break;
    case 13:
        returnValues = {'7', 'F', lowerNoteValue + 2, "90"}; break;
    case 14:
        returnValues = {'4', '9', lowerNoteValue + 12, "8C"}; break;
    case 15:
        returnValues = {'5', 'C', higherNoteValue - 7, "86"}; break;
    case 16:
        returnValues = {'4', 'A', lowerNoteValue + 12, "80"}; break;
    case 17:
        returnValues = {'3', '8', higherNoteValue, "80"}; break;
    case 18:
        returnValues = {'5', 'E', lowerNoteValue + 8, "80"}; break;
    case 19:
        returnValues = {'4', 'C', higherNoteValue - 7, "80"}; break;
    case 21:
        returnValues = {'3', 'A', higherNoteValue - 4, "80"}; break;
    case 22:
        returnValues = {'4', 'E', lowerNoteValue + 12, "80"}; break;
    case 23:
        returnValues = {'4', 'F', lowerNoteValue + 12, "80"}; break;
    case 24:
        returnValues = {'2', '8', higherNoteValue, "80"}; break;
    case 26:
        returnValues = {'2', '9', higherNoteValue - 2, "80"}; break;
    case 27:
        returnValues = {'3', 'E', higherNoteValue, "80"}; break;
    case 28:
        returnValues = {'2', 'A', higherNoteValue - 4, "80"}; break;
    case 31:
        returnValues = {'2', 'C', higherNoteValue - 7, "80"}; break;
    case 34:
        returnValues = {'2', 'E', lowerNoteValue + 24, "80"}; break;
    case 35:
        returnValues = {'2', 'F', lowerNoteValue + 24, "80"}; break;
    case 36:
        returnValues = {'1', '8', higherNoteValue, "80"}; break;
    case 38:
        returnValues = {'1', '9', higherNoteValue - 2, "80"}; break;
    case 40:
        returnValues = {'1', 'A', higherNoteValue - 4, "80"}; break;
    case 43:
        returnValues = {'1', 'C', higherNoteValue - 7, "80"}; break;
    case 46:
        returnValues = {'1', 'E', lowerNoteValue + 36, "80"}; break;
    case 47:
        returnValues = {'1', 'F', lowerNoteValue + 36, "80"}; break;
    case 48:
        returnValues = {'0', '8', higherNoteValue, "80"}; break;
    case 50:
        returnValues = {'0', '9', lowerNoteValue + 48, "80"}; break;
    case 52:
        returnValues = {'0', 'A', lowerNoteValue + 48, "80"}; break;
    case 55:
        returnValues = {'0', 'C', lowerNoteValue + 48, "80"}; break;
    case 58:
        returnValues = {'0', 'E', lowerNoteValue + 48, "80"}; break;
    case 59:
        returnValues = {'0', 'F', lowerNoteValue + 48, "80"}; break;
    
    default:
        cout << "invalid interval: " << difference << " semitones";
        return {'0', '0', 0};
    }

    if(currentNoteValue > delayedNoteValue){
        return {returnValues.secondMultiplier, returnValues.firstMultiplier, returnValues.playedNoteValue, returnValues.detuneValue};
    }
    return returnValues;
}
