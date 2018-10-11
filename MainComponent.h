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
      public OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>,
                       private AsyncUpdater
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
    void timerCallback() override;
    
    
    
    void oscMessageReceived(const OSCMessage& message) override;
    void oscBundleReceived(const OSCBundle& bundle) override;
    
    bool receiveChecker(const OSCMessage& m);
    void addOSCBundle (const OSCBundle& bundle, int level = 0);
    void addOSCMessageArgument (const OSCArgument& arg , String m);
    
    void handleAsyncUpdate() override
    {
        
    }
    
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
    
    float joy, anger, disgust, engage, att, valence;
    
    Label audioValue, appHeader, museValue, affectivaValue;
    const int labelHeight = 40;
    
    const String alphaMessage = "/muse/elements/alpha_absolute",
                 thetaMessage = "/muse/elements/theta_absolute",
                 gammaMessage = "/muse/elements/gamma_absolute",
                 betaMessage =  "/muse/elements/beta_absolute",
                 deltaMessage = "/muse/elements/delta_absolute";
    
    String museAlpha, museBeta, museDelta, museTheta, museGamma;
    String museText;
    
    //==============================================================================
    // Your private member variables go here...

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
