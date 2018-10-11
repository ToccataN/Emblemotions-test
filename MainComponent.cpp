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
    
    addMuseLabels();
    
    if(!connect(5003))
        std::cout << ShowTheNumber(555) << std::endl;

    addListener(this);
    
    startTimerHz(5);
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
    
    for(auto i= 0; i < museLabels.size(); ++i)
      museLabels[i]->setBounds(leftScreen.removeFromTop(labelHeight).reduced(5));
    
    for(auto i= 0; i < affDexLabels.size(); ++i)
      affDexLabels[i]->setBounds(leftScreen.removeFromTop(labelHeight).reduced(5));
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
    
    
    
}



