/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent,
                        private Timer,
                        public OSCReceiver,
      public OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void process(float data);
    void updateLabelText(const String& text, Label& label);
    
    
    void oscMessageReceived(const OSCMessage& message) override;
    void timerCallback() override;
    bool receiveChecker(const OSCMessage& m);
    void showConnectionErrorMessage (const String& messageText);
    
    enum{
        fftOrder= 7,
        fftSize = 1 << fftOrder
    };

private:
    double lastSampleRate;
    double audioFFTValue;
    
    dsp::FFT audioAnalyser;
    
    float fifo[fftSize];
    float fftData[fftSize * 2];
    int fftIndex= 0;
    bool fftReady= false;
    
    float oscMuse = 0.0;
    
    float joy, anger, disgust, engage, att, valence;
    
    Label audioValue, appHeader, museValue, affectivaValue;
    
    const int labelHeight = 40;
    
    //==============================================================================
    // Your private member variables go here...

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
