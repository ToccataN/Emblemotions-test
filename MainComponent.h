/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "BasicThumbComp.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent,
                        private Timer,
                        public OSCReceiver,
      public OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>,
                       private AsyncUpdater,
                       public ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    void changeListenerCallback (ChangeBroadcaster* source) override;
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void process(float data);
    void updateLabelText(const String& text, const Label& label, int index = 0 );
    void timerCallback() override;
    
    void addMuseLabels();
    
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
    
    //pitch detector aubio
    void pitchDetector(float data, int index);
    

private:
    
    double lastSampleRate;
    double audioFFTValue;
    
    dsp::FFT audioAnalyser;
    
    float fifo[fftSize];
    float fftData[fftSize * 2];
    int fftIndex= 0;
    int fftSampleIndex = 0;
    bool fftReady= false;
    
    float joy, anger, disgust, engage, att, valence;
    OwnedArray<Label> affDexLabels;
    Array<String> affDexStrings = {"Joy: ", "Anger: ", "Disgust: ", "Engagement: ",
        "Attention: ", "Valence: "};
    
    
    
    Label audioValue, appHeader, noteValue;
    String currentNote= "Pitch: ";
    const int labelHeight = 40;
    
    //OSC variables for Muse
    OwnedArray<Label> museLabels;
    Label alphaLabel;
    Array<String> museStrings ={"Alpha: ", "Beta: ", "Delta: ", "Gamma: ", "Theta: "};
    
    const String alphaMessage = "/muse/elements/alpha_absolute",
                 thetaMessage = "/muse/elements/beta_absolute",
                 gammaMessage = "/muse/elements/delta_absolute",
                 betaMessage =  "/muse/elements/gamma_absolute",
                 deltaMessage = "/muse/elements/theta_absolute";
    
    String museAlpha, museBeta, museDelta, museTheta, museGamma;
    String museText;
    
    //File Uploader Variables and functions
    AudioThumbnailCache atCache;
    BasicThumbComp thumbComp;
    AudioFormatManager afManager;
    AudioTransportSource transportSource;
    
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };
    
    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;
    
    TransportState playstate;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    
    void changeState (TransportState newState)
    {
        if (playstate != newState)
        {
            playstate = newState;
            
            switch (playstate)
            {
                case Stopped:
                    stopButton.setEnabled (false);
                    playButton.setEnabled (true);
                    transportSource.setPosition (0.0);
                    break;
                    
                case Starting:
                    playButton.setEnabled (false);
                    transportSource.start();
                    break;
                    
                case Playing:
                    stopButton.setEnabled (true);
                    break;
                    
                case Stopping:
                    transportSource.stop();
                    break;
                    
                default:
                    jassertfalse;
                    break;
            }
        }
    }
    
    void transportSourceChanged()
    {
        if (transportSource.isPlaying())
            changeState (Playing);
        else
            changeState (Stopped);
    }
    
    void openButtonClicked()
    {
        FileChooser chooser ("Select a Wave file to play...",
                             File::nonexistent,
                             "*.wav");
        
        if (chooser.browseForFileToOpen())
        {
            File file (chooser.getResult());
            
            if (auto* reader = afManager.createReaderFor (file))
            {
                std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true));
                transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
                playButton.setEnabled (true);
                thumbComp.setFile (file);
                readerSource.reset (newSource.release());
            }
        }
    }
    
    void playButtonClicked()
    {
        changeState (Starting);
    }
    
    void stopButtonClicked()
    {
        changeState (Stopping);
    }
    
    //pitch variables
    Array<String> notes ={"A", "A#", "B", "C" ,"C#" , "D", "D#" , "E" , "F","F#", "G", "G#"};
    Array<String> midiTable;
    
    
    //==============================================================================
    // Your private member variables go here...

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
