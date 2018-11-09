/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once
#include "ObjCInterface.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "BasicThumbComp.h"
#include "MultiChannelAudioSource.h"
#include "SuperpoweredBandpassFilterBank.h"
#include "SuperpoweredSimple.h"
#include "UploadCSVComp.h"

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
    
    void process(float data, const float input);
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
    
    //pitch detector (rough version)
    void pitchDetector(float data, int index);
    
    //current file multitrack
    HashMap<int, File> FileArray;
    void openButtonClicked(int index);
    void playButtonClicked();
    void stopButtonClicked();

private:
    
    double lastSampleRate= 44100;
    double audioFFTValue = 0.0;
    
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
    
    UploadCSVComp upCSV;
    
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
    //BasicThumbComp bassThumb, drumThumb, synthThumb, voiceThumb;
    OwnedArray<BasicThumbComp> thumbnails;
    AudioFormatManager afManager;
    
    Array<String> sectionString = {"Bass", "Drums", "Synth", "Voice"};
    OwnedArray<TextButton> openButton;
    TextButton playButton;
    TextButton stopButton;
    
    //this instantiates the multi-track "stem" format funcitonality for wav files.
    MultiChannelAudioSource mixer;
  
    
    //pitch variables
    Array<String> notes ={"A", "A#", "B", "C" ,"C#" , "D", "D#" , "E" , "F","F#", "G", "G#"};
    Array<String> midiTable;
    
    int discreteCount = 0;
    
    SuperpoweredBandpassFilterbank *superFB;
    float frequencies[8] = { 55, 110, 220, 440, 880, 1760, 3520, 7040 };
    float widths[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
    float bands[128][8];
    float* peak, *sum;
    //==============================================================================
    // Your private member variables go here...

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};






