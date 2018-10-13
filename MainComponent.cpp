/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "ObjCInterface.h"
#include "MainComponent.h"

extern "C"{
    #include "aubio/aubio.h"
}


MainComponent::MainComponent() : audioAnalyser(fftOrder),
                                 atCache(5),
                                 thumbComp(512, afManager, atCache),
                                 playstate(Stopped)

{
    
    auto audioDevice = deviceManager.getCurrentAudioDevice();
    auto numInputChannels = jmax(audioDevice != nullptr ? audioDevice->getActiveInputChannels() .countNumberOfSetBits() : 1, 1);
    auto outputChannels = audioDevice != nullptr ? audioDevice->getActiveOutputChannels() .countNumberOfSetBits() : 2;
    setAudioChannels (numInputChannels, outputChannels);
    
    appHeader.setText("Emblemotions App", dontSendNotification);
    appHeader.setColour(Label::textColourId, Colours::darkred);
    appHeader.setColour(Label::backgroundColourId, Colours::black);
    addAndMakeVisible(appHeader);
    
    audioValue.setText("Audio Signal Value: ", sendNotification);
    audioValue.setColour(Label::backgroundColourId, Colours::darkred);
    audioValue.setJustificationType(Justification::centred);
    addAndMakeVisible(audioValue);
    
    addMuseLabels();
    
    afManager.registerBasicFormats();
    transportSource.addChangeListener (this);
    
    if(!connect(5003))
        std::cout << ShowTheNumber(555) << std::endl;

    addListener(this);
    
    startTimerHz(5);
    setSize (800, 600);

    // specify the number of input and output channels that we want to open
    for(int i=0; i < 128; ++i){
        if(i < 21) midiTable.insert(i, "none") ;
        int in = (i % 12);
        if(in >= notes.size()) in -= 12;
        midiTable.insert(i, notes[in]);
        std::cout << midiTable[i] << std::endl;
    }
    
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    transportSource.releaseResources();
}

//==============================================================================

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    lastSampleRate = sampleRate;
    
    dsp::ProcessSpec spec;
    spec.sampleRate = lastSampleRate;
    spec.maximumBlockSize = samplesPerBlockExpected;
    
    transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void MainComponent::process(float data)
{
    if(fftIndex == fftSize)
    {
    
      audioAnalyser.performRealOnlyForwardTransform(fftData);
      float temp = 0.0;
      int index = 0;
        for(int i= 0 ; i < fftSize ; ++i ){
          if(fftData[i] != 0 && fftData[i] >= 0.001) audioFFTValue = fftData[i];
         
            
            if(temp == 0.0){
                temp = fftData[i];
            } else {
                if(fftData[i] > temp){
                  temp = fftData[i];
                  index = i;
                }
            }
            
        }

      pitchDetector(temp, index);
      fftIndex= 0;
        
    }
   
    fftData[fftIndex++] = data;

}

void MainComponent::oscMessageReceived(const OSCMessage& message)
{
    String m = message.getAddressPattern().toString();
    if (!message.isEmpty() && ( m == alphaMessage || m == betaMessage ||
            m == thetaMessage || m == gammaMessage || m == deltaMessage))
    {
        for (OSCArgument* arg = message.begin(); arg != message.end(); ++arg)
            addOSCMessageArgument (*arg, m);
    }
}

void MainComponent::oscBundleReceived(const juce::OSCBundle &bundle)
{
    for (int i = 0; i < bundle.size(); ++i)
    {
        auto elem = bundle[i];
        if (elem.isMessage())
            oscMessageReceived (elem.getMessage());
        else if (elem.isBundle())
            oscBundleReceived (elem.getBundle());
    }
}

void MainComponent::addOSCMessageArgument (const OSCArgument& arg, String m)
{
    String typeAsString;
    String value;
    
    if (arg.isFloat32())
    {
        typeAsString = "float32";
        if (m == alphaMessage)
            museAlpha =  museStrings[0] + String (arg.getFloat32());
        else if (m == betaMessage)
            museBeta = museStrings[1] + String(arg.getFloat32());
        else if (m == deltaMessage)
            museDelta = museStrings[2] + String(arg.getFloat32());
        else if (m == gammaMessage)
            museGamma = museStrings[3] + String(arg.getFloat32());
        else if (m == thetaMessage)
            museTheta = museStrings[4] + String(arg.getFloat32());
        
    }
    else if (arg.isInt32())
    {
        typeAsString = "int32";
        value = m + " : " + String (arg.getInt32());
    }
    else if (arg.isString())
    {
        typeAsString = "string";
        value = arg.getString();
    }
    else if (arg.isBlob())
    {
        typeAsString = "blob";
        auto& blob = arg.getBlob();
        value = String::fromUTF8 ((const char*) blob.getData(), (int) blob.getSize());
    }
    else
    {
        typeAsString = "(unknown)";
    }
    
    museText =  value;
    std::cout << value << std::endl;
}

void MainComponent::showConnectionErrorMessage (const String& messageText)
{
    AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                      "Connection error",
                                      messageText,
                                      "OK");
}

void MainComponent::updateLabelText(const String& text,const Label& label, int index)
{
    if(&label == &audioValue)
        audioValue.setText("Audio Signal Value (FFT): " + text, sendNotification);
    else if(&label == museLabels[index])
        museLabels[index]->setText(text, sendNotification);
    else if(&label == affDexLabels[index])
        affDexLabels[index]->setText(text, sendNotification);
    else if(&label == &noteValue)
        noteValue.setText(currentNote, sendNotification);
    
}

void MainComponent::timerCallback()
{
    
    valence = floor(affValence()*100)/100;
    anger = floor(affAnger()*100)/100;
    disgust = floor(affDisgust()*100)/100;
    engage = floor(affEngage()*100)/100;
    att = floor(affAtt()*100)/100;
    joy = floor(affJoy()*100)/100;
    
    updateLabelText(String(audioFFTValue), audioValue);
   
    updateLabelText(museAlpha, *museLabels[0], 0);
    updateLabelText(museBeta, *museLabels[1], 1);
    updateLabelText(museDelta, *museLabels[2], 2);
    updateLabelText(museGamma, *museLabels[3], 3);
    updateLabelText(museTheta, *museLabels[4], 4);
    
    updateLabelText(affDexStrings[0] + String(joy), *affDexLabels[0], 0);
    updateLabelText(affDexStrings[1] + String(anger), *affDexLabels[1], 1);
    updateLabelText(affDexStrings[2] + String(disgust), *affDexLabels[2], 2);
    updateLabelText(affDexStrings[3] + String(engage), *affDexLabels[3], 3);
    updateLabelText(affDexStrings[4] + String(att), *affDexLabels[4], 4);
    updateLabelText(affDexStrings[5] + String(valence), *affDexLabels[5], 5);
    
    updateLabelText(currentNote, noteValue);
    
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    
        bufferToFill.clearActiveBufferRegion();
        transportSource.getNextAudioBlock(bufferToFill);

        
        for(int i= 0 ; i < bufferToFill.buffer->getNumChannels(); ++i){
            auto* inputChannel = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
            
            for(int samp = 0; samp < bufferToFill.numSamples; ++samp){
                process(inputChannel[samp]);
                
            }
            
        }
    
}

void MainComponent::pitchDetector(float data, int index)
{
    if (fftSampleIndex < 44100){
       fftSampleIndex += 512;
    } else{
        double f = ((index * 44100) / 512);
        double m = round(log(f/440.0)/log(2) * 12 + 67);
        if((m > 20 || m < 128 )&& notes.indexOf(midiTable[m]) != -1){
            currentNote= "Pitch: " + String(midiTable[m]);
        }
        
        fftSampleIndex = 0;
    }
    
    
}

void MainComponent::releaseResources()
{

    
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
  
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    g.setColour(Colours::darkred);
    g.setFont(Font(40.0f));
    
}

void MainComponent::resized()
{
    Rectangle<int> mainview = getLocalBounds();
    Rectangle<int> leftScreen;
    leftScreen.setBounds(0, labelHeight, mainview.getWidth()/2, mainview.getHeight());
    
    appHeader.setBounds(mainview.removeFromTop(labelHeight));
    audioValue.setBounds(leftScreen.removeFromTop(labelHeight).reduced(10));
    
    for(auto i= 0; i < museLabels.size(); ++i)
      museLabels[i]->setBounds(leftScreen.removeFromTop(labelHeight).reduced(5));
    
    for(auto i= 0; i < affDexLabels.size(); ++i)
      affDexLabels[i]->setBounds(leftScreen.removeFromTop(labelHeight).reduced(5));
    
    //Right side
    Rectangle<int> rightScreen;
    rightScreen.setBounds(mainview.getWidth()/2, labelHeight, mainview.getWidth()/2, mainview.getHeight());
    
    openButton.setBounds (rightScreen.removeFromTop(labelHeight).reduced(10));
    playButton.setBounds (rightScreen.removeFromTop(labelHeight).reduced(10));
    stopButton.setBounds (rightScreen.removeFromTop(labelHeight).reduced(10));
    
    Rectangle<int> thumbnailBounds (rightScreen.getX()+10,rightScreen.getY() , rightScreen.getWidth() - 20, rightScreen.getHeight() - 220);
    
    thumbComp.setBounds(thumbnailBounds);
    
    Rectangle<int> lowerVals = leftScreen.removeFromTop(labelHeight);
    
    noteValue.setBounds(lowerVals.removeFromLeft(leftScreen.getWidth()/2).reduced(5));
    
}



void MainComponent::addMuseLabels()
{
    for(auto i = 0 ; i < 5 ; ++i)
    {
        addAndMakeVisible(museLabels.add(new Label(museStrings[i])));
        museLabels[i]->setColour(Label::backgroundColourId, Colours::darkred);
        museLabels[i]->setJustificationType(Justification::centred);
    }
    
    for(auto i = 0 ; i < affDexStrings.size() ; ++i)
    {
        addAndMakeVisible(affDexLabels.add(new Label(affDexStrings[i])));
        affDexLabels[i]->setColour(Label::backgroundColourId, Colours::darkred);
        affDexLabels[i]->setJustificationType(Justification::centred);
    }
    
    
    noteValue.setText("Pitch: ", sendNotification);
    noteValue.setColour(Label::backgroundColourId, Colours::darkred);
    noteValue.setJustificationType(Justification::centred);
    addAndMakeVisible(noteValue);
    
    addAndMakeVisible (&openButton);
    openButton.setButtonText ("Open...");
    openButton.onClick = [this] { openButtonClicked(); };
    
    addAndMakeVisible (&playButton);
    playButton.setButtonText ("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour (TextButton::buttonColourId, Colours::green);
    playButton.setEnabled (false);
    
    addAndMakeVisible (&stopButton);
    stopButton.setButtonText ("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour (TextButton::buttonColourId, Colours::red);
    stopButton.setEnabled (false);
    
    addAndMakeVisible (&thumbComp);
    
}

//File Uploader
void MainComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &transportSource)
        transportSourceChanged();
    
}


