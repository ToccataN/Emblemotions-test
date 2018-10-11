/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "ObjCInterface.h"
#include "MainComponent.h"

MainComponent::MainComponent() : audioAnalyser(fftOrder)

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
    
    museValue.setText("Muse Signal Value: ", sendNotification);
    museValue.setColour(Label::backgroundColourId, Colours::darkred);
    museValue.setJustificationType(Justification::centred);
    addAndMakeVisible(museValue);
    
    affectivaValue.setText("Facial Emotion Value: ", sendNotification);
    affectivaValue.setColour(Label::backgroundColourId, Colours::darkred);
    affectivaValue.setJustificationType(Justification::centred);
    addAndMakeVisible(affectivaValue);
    
    if(!connect(5003)){
        std::cout << ShowTheNumber(555) << std::endl;
    } else{
        std::cout << "IM HERE" << std::endl;
    }

    addListener( this, "/muse/elements/blink");
    
    
    

    startTimerHz(10);
    setSize (800, 600);

    // specify the number of input and output channels that we want to open
    
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    lastSampleRate = sampleRate;
    
    dsp::ProcessSpec spec;
    spec.sampleRate = lastSampleRate;
    spec.maximumBlockSize = samplesPerBlockExpected;
    
}

void MainComponent::process(float data)
{
    if(fftIndex == fftSize)
    {
        
      audioAnalyser.performRealOnlyForwardTransform(fftData);
      
      
        
      for(int i= 0 ; i < fftSize ; ++i )
        if(fftData[i] != 0 && fftData[i] >= 0.001) audioFFTValue = fftData[i];
      //if(fftData[i] != 0) std::cout << " current:  " << fftData[i] << std:: endl;
        
      fftIndex= 0;
        
    }
   
    fftData[fftIndex++] = data;

}

bool MainComponent::receiveChecker(const OSCMessage& m){
    int sizeVar = m.size();
    
    if(sizeVar > 0)
    {
        std::cout << sizeVar << std::endl;
        for(int i=0 ; i< sizeVar ; ++i)
        {
            if(m[i].isFloat32()){
                std::cout << m[i].getFloat32() << std::endl;
                
            }
        }
        return true;
    } else {
        std::cout << " Im not here " << std::endl;
        return false;
    }
}

void MainComponent::oscMessageReceived(const OSCMessage& message)
{
    
    if(message.size()==1 && message[0].isInt32()){
        oscMuse = message[0].getInt32();
        std::cout << message[0].getInt32() << std::endl;
    }
    
}

void MainComponent::showConnectionErrorMessage (const String& messageText)
{
    AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                      "Connection error",
                                      messageText,
                                      "OK");
}

void MainComponent::updateLabelText(const String& text, Label& label)
{
    if(&label == &audioValue)
        audioValue.setText("Audio Signal Value (FFT): " + text, sendNotification);
    else if(&label == &museValue)
        museValue.setText("Muse Data Signal (FFT): " + text, sendNotification);
    else if(&label == &affectivaValue)
        affectivaValue.setText(text, sendNotification);
}

void MainComponent::timerCallback()
{
    
    valence = floor(affValence()*100)/100;
    anger = floor(affAnger()*100)/100;
    disgust = floor(affDisgust()*100)/100;
    engage = floor(affEngage()*100)/100;
    att = floor(affAtt()*100)/100;
    joy = floor(affJoy()*100)/100;
    
    String affVals = "Valence: " + String(valence) + " Anger: " + String(anger) + " Disgust: " + String(disgust) + " Engage: " + String(engage) + " Joy: " + String(joy) + " Attention: " + String(att);
    
    updateLabelText(String(audioFFTValue), audioValue);
    updateLabelText(String(oscMuse), museValue);
    updateLabelText(affVals, affectivaValue);
    
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    
    
    for(int i= 0 ; i < bufferToFill.buffer->getNumChannels(); ++i){
        auto* inputChannel = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
        
        for(int samp = 0; samp < bufferToFill.numSamples; ++samp)
            process(inputChannel[samp]);
        
       
        bufferToFill.clearActiveBufferRegion();
        
    }
    
    
    
    
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
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
    museValue.setBounds(leftScreen.removeFromTop(labelHeight).reduced(10));
    affectivaValue.setBounds(leftScreen.removeFromTop(labelHeight).reduced(10));
}
