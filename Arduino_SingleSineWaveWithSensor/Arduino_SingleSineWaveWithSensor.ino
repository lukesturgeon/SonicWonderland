#include <CapacitiveSensor.h>
#include <SPI.h>
#include <GD.h>

/*
 * Single input capacitive touch input with sine wave audio output
 *
 * Built for Bompass & Parr - Sony Sonic Wonderland Installation
 * Tangible musical interface with green plants as the interface
 *
 * Luke Sturgeon <luke.sturgeon@network.rca.ac.uk>
 */

class Sine {
  public:
    int maxVolume;
    int minSensorValue;
    int maxSensorValue;
    int minFreq;
    int maxFreq;
    float minTheta;
    float maxTheta;
    int sensorValue;
    float thetaAdjuster;
    int freqAdjuster;
    int volumeAdjuster;
    float theta;
    int freqOut;

    Sine(int _maxVolume, int _minSensorValue, int _maxSensorValue, int _minFreq, int _maxFreq, float _minTheta, float _maxTheta)
    {
      // settings
      maxVolume =       _maxVolume;
      minSensorValue =  _minSensorValue;
      maxSensorValue =  _maxSensorValue;
      minFreq =         _minFreq;
      maxFreq =         _maxFreq;
      minTheta =        _minTheta;
      maxTheta =        _maxTheta;
      //defaults
      sensorValue = 0;
      thetaAdjuster = 0.001;
      freqAdjuster = 0;
      volumeAdjuster = 0;
      theta = 0.0;
      freqOut = 0;
    }

    void updateSensor(int _v)
    {
      sensorValue = _v;
      thetaAdjuster = mapf(sensorValue, minSensorValue, maxSensorValue, minTheta, maxTheta);
      freqAdjuster = map(sensorValue, minSensorValue, maxSensorValue, 0, maxFreq - minFreq);
    }

    void updateVolume()
    {
      // adjust the volume fader/amplitude
      if (sensorValue < minSensorValue && volumeAdjuster > 0)
      {
        volumeAdjuster -= 2;
        if (volumeAdjuster < 0) volumeAdjuster = 0;
      }
      else if (sensorValue > minSensorValue && volumeAdjuster < maxVolume)
      {
        volumeAdjuster += 75;
        if (volumeAdjuster > maxVolume) volumeAdjuster = maxVolume;
      }
    }

    void updateFreq()
    {
      //increment
      theta += thetaAdjuster;

      // map values to correct audio frequencies
      freqOut = map( sin(theta) * 100, -100, 100, minFreq, minFreq + freqAdjuster);
    }

    void makeTone( int _channel )
    {
      // generate tone
      GD.voice(_channel, 0, freqOut, volumeAdjuster, volumeAdjuster);
    }
};

unsigned long lastTime;

CapacitiveSensor sensor1 = CapacitiveSensor(2, 3);
CapacitiveSensor sensor2 = CapacitiveSensor(5, 6);
CapacitiveSensor sensor3 = CapacitiveSensor(7, 8);


//Sine(int _maxVolume, int _minSensorValue, int _maxSensorValue, int _minFreq, int _maxFreq, float _minTheta, float _maxTheta);
Sine s1( 255/4, 1000, 2000,   1200, 2000, 0.005, 0.0001);
Sine s2( 255/4, 1000, 2000,   1500, 1300, 0.001, 0.1);
Sine s3( 255/4, 1000, 2000,   2500, 2000, 0.001, 0.1);

Sine s4( 200/4, 100, 150, 300, 600, 0.1, 0.01);


void setup()
{
  GD.begin();
  Serial.begin(9600);
}


void loop()
{
  unsigned long time = millis();

  if (time - lastTime > 55)
  {
    // update sensor values
    s1.updateSensor( sensor1.capacitiveSensor(30) );
    s1.updateVolume();

    s2.updateSensor( sensor2.capacitiveSensor(30) );
    s2.updateVolume();
    
    s3.updateSensor( sensor3.capacitiveSensor(30) );
    s3.updateVolume();
    
    // base sound
    s4.updateSensor(110);
    s4.updateVolume();
    

    Serial.print(s1.sensorValue);
    Serial.print('\t');
    Serial.print(s2.sensorValue);
    Serial.print('\t');
    Serial.println(s3.sensorValue);

    lastTime = time;
  }

  s1.updateFreq();
  s1.makeTone( 0 );

  s2.updateFreq();
  s2.makeTone( 1 );
  
  s3.updateFreq();
  s3.makeTone( 2 );
  
  s4.updateFreq();
  s4.makeTone( 3 );
}


static float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
