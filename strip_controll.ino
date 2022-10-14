#include "fix_fft.h"
#define ANALOG_IN A0
#define LEN_AVG 16
//Number of samples for building the average
#define SAMPLES 256
//number of samples taken of analog input for fft
int time = 0;
//------------------------------------------
char im[SAMPLES], data[SAMPLES];
//lists for samples taken from analog input for fft
int avg_list[LEN_AVG];
//list for average over  special frequency range of fft
long avg;
//average of fft over time
uint8_t k, counter;
//to remember which led is on and counter for saving average index
long ts;
//variable for timestamps, used to decide weather a fix threshold is used or the average
//==============================================================
void setup() {
  pinMode(ANALOG_IN, INPUT);
  //Analog Input
  pinMode(3, OUTPUT);
  //Configure Outputs
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
}
//==============================================================
void loop() {
  for (int i = 0; i < SAMPLES; i++) {
    //take 128 samples of analog input for fft
    data[i] = 2 * analogRead(ANALOG_IN);
    //write samples to data
    im[i] = 0;
    //imaginary part of data is always zero
    delayMicroseconds(14);
    //delay to see low frequencies
  }
  //------------------------------------------

  fix_fft(data, im, 7, 0);
  //FFT on data fix_fft(data, im, n, inv) data is the date given, im the imaginary part,
  //n represents the bitlength of the output, inv for 0 the fft and 1 for the inverse fft
  //------------------------------------------
  int on = 0;
  //value to write data in, the integration over low frequencys
  for (int i = 0; i < 8; i++) {
    //use only low frequencies, first 12 entries of fft
    on = on + data[i] * data[i] + im[i] * im[i];
    //calqulate square of abolute value
  }
  //-----------------------------------------
  if (on != 0) {
    avg_list[counter++] = on;
    //write in average list
  } else {
    avg_list[counter++] = avg / 2;
    //reduce the avg slowly, not remain high if the fft is zero
  }
  counter = counter % 128;
  //reset counter to stay in list

  avg = 0;
  //reset average
  //-----------------------------------------
  for (int i = 0; i < LEN_AVG; i++) {
    //iterate over average list and write it into avg
    avg = avg_list[i] + avg;
  }
  //----------------------------------------
  avg = (int)avg / (2 / 3.0 * LEN_AVG);
  //norm average to get a usefull threshold
  switch (millis() - ts < 6000) {
    //case 0 if it is on for enough time use average as theshold,
    // otherwise use a fix threshold
    case 0:
      if (on > avg) {
        //decide weather to change output
        digitalWrite(k, HIGH);
        //turn of old color
        digitalWrite((++k) % 3 + 3, LOW);
        //turn on new color
        k = k % 3 + 3;
        //remember which color is on
        delay(5);
      }
    case 1:
      if (on > 800) {
        //decide weather to change output
        digitalWrite(k, HIGH);
        //turn of old color
        digitalWrite((++k) % 3 + 3, LOW);
        //turn on new color
        k = k % 3 + 3;
        //remember which color is on
        delay(5);
      }
  }
  //norm average

  //---------------------------------------
  if (avg < 5) {
    //turn of if no music is playing
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    ts = millis();
  }
  if (counter == 0) {
    time = millis();
  }
}
