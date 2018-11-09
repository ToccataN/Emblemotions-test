#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "BasicThumbComp.h"
#include "SuperpoweredSimple.h"
#include "superpoweredanalyzer.h"
#include "SuperpoweredBandpassFilterBank.h"


class MultiChannelAudioSource : public PositionableAudioSource,
                                public ChangeBroadcaster

{
public:
    //==============================================================================
    /* Constructor */
    MultiChannelAudioSource() = default;
    
    /* Destructor */
    ~MultiChannelAudioSource() { releaseResources(); };
    
    //==============================================================================
    /* Creates a set of readers for the multi-mono source data */
    void loadAudioAssets(const HashMap<int, File> &audioFileSet)
    {
        if (isRegistered == false)
        {
            formatManager.registerBasicFormats();
            isRegistered = true;
        }
        // This deletes any readers created by a previous call to loadAudioAssets()
        // and clears the array for a fresh load of the incoming audioFileSet
        releaseResources();
        
        for (auto f : audioFileSet)
        {
            auto* reader = formatManager.createReaderFor(f);
            if (reader != nullptr){
               inputReaders.add( new AudioFormatReaderSource(reader, true));
            }else{
                jassertfalse;
            }
        }
    }
    
    //==============================================================================
    /* setPosition */
    void setPosition(double newPosition)
    {
        if (sampleRate > 0.0)
            setNextReadPosition((int64)(newPosition * sampleRate));
    }
    
    /* getCurrentPosition */
    double getCurrentPosition() const
    {
        if (this->sampleRate > 0.0)
            return (double)getNextReadPosition() / this->sampleRate;
        
        return 0.0;
    }
    
    /* getLengthInSeconds */
    double getLengthInSeconds() const
    {
        if (sampleRate > 0.0)
            return (double)getTotalLength() / sampleRate;
        
        return 0.0;
    };
    
    /* hasStreamFinished */
    bool hasStreamFinished() const noexcept { return inputStreamEOF; }
    
    //==============================================================================
    /* start */
    void start()
    {
        if (!playing)
        {
            {
                const ScopedLock sl(callbackLock);
                playing = true;
                stopped = false;
                inputStreamEOF = false;
            }
            
            sendChangeMessage();
        }
    }
    
    /* stop */
    void stop()
    {
        if (playing)
        {
            {
                const ScopedLock sl(callbackLock);
                playing = false;
            }
            
            int n = 500;
            while (--n >= 0 && !stopped)
                Thread::sleep(2);
            
            sendChangeMessage();
        }
    }
    
    /* isPlaying */
    bool isPlaying() const noexcept { return playing; }
    
    //==============================================================================
    /* setGain */
    void setGain(float newGain) noexcept { currentGain = newGain; };
    
    /* getGain */
    float getGain() const noexcept { return currentGain; }
    
    //==============================================================================
    /* prepareToPlay */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        const ScopedLock sl(callbackLock);
        
        this->blockSize = samplesPerBlockExpected;
        this->sampleRate = sampleRate;
        
        for (auto* reader : inputReaders)
            reader->prepareToPlay(samplesPerBlockExpected, sampleRate);
        
        inputStreamEOF = false;
        isPrepared = true;
    }
    
    /* releaseResources */
    void releaseResources() override
    {
        const ScopedLock sl(callbackLock);
        
        for (auto* reader : inputReaders)
            reader->releaseResources();
        
        // Clear the array and delete the readers
        inputReaders.clear(true);
        
        isPrepared = false;
    }
    
    /* getNextAudioBlock */
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
    {
        const ScopedLock sl(callbackLock);
        
        //jassert(inputReaders.size() >= bufferToFill.buffer->getNumChannels());
        
        if (stopped == false)
        {
           /* *** Passing variables into Superpowered from these channels
             appears to cause threading issues, despite successful
             compilation. There seems to be some sort of optimization
             setting that I am having difficulty getting around. ***
            
            auto *buff = bufferToFill.buffer;
           
            float *leftChannel = buff->getWritePointer(0);
            float *rightChannel = buff->getWritePointer(1);
            
            SuperpoweredInterleave(leftChannel, rightChannel, output, buff->getNumSamples());
            
            analyzer->process(output, 8);
            */
            if(inputReaders.size() > 1)
            {
              for (int i = 0; i < bufferToFill.buffer->getNumChannels(); ++i)
              {
                
                  auto writePointer = bufferToFill.buffer->getWritePointer(i);
                  AudioBuffer<float> proxyBuffer(&writePointer, 1, bufferToFill.buffer->getNumSamples());
                  AudioSourceChannelInfo proxyInfo(&proxyBuffer, bufferToFill.startSample, bufferToFill.numSamples);
                  inputReaders.getUnchecked(i)->getNextAudioBlock(proxyInfo);
                
               }
            }
            else
            {
                auto writePointer = bufferToFill.buffer->getWritePointer(0);
                AudioBuffer<float> proxyBuffer(&writePointer, 1, bufferToFill.buffer->getNumSamples());
                AudioSourceChannelInfo proxyInfo(&proxyBuffer, bufferToFill.startSample, bufferToFill.numSamples);
                inputReaders.getFirst()->getNextAudioBlock(proxyInfo);
    
            }
            
            if (playing == false)
            {
                // just stopped playing, so fade out the last block..
                for (int i = bufferToFill.buffer->getNumChannels(); --i >= 0;)
                    bufferToFill.buffer->applyGainRamp(i, bufferToFill.startSample, jmin(256, bufferToFill.numSamples), 1.0f, 0.0f);
                
                if (bufferToFill.numSamples > 256)
                    bufferToFill.buffer->clear(bufferToFill.startSample + 256, bufferToFill.numSamples - 256);
            }
            
            if (inputReaders.getUnchecked(0)->getNextReadPosition() > inputReaders.getUnchecked(0)->getTotalLength() + 1
                && inputReaders.getUnchecked(0)->isLooping() == false)
            {
                playing = false;
                inputStreamEOF = true;
                sendChangeMessage();
            }
            
            stopped = !playing;
            
            for (int i = bufferToFill.buffer->getNumChannels(); --i >= 0;)
                bufferToFill.buffer->applyGainRamp(i, bufferToFill.startSample, bufferToFill.numSamples, previousGain, currentGain);
        }
        
        else
        {
            bufferToFill.clearActiveBufferRegion();
            stopped = true;
        }
        
        previousGain = currentGain;
    }
    
    //==============================================================================
    /* setNextReadPosition */
    void setNextReadPosition(int64 newPosition) override
    {
        for (auto* reader : inputReaders)
            reader->setNextReadPosition(newPosition);
        
        inputStreamEOF = false;
    }
    
    /* getNextReadPosition */
    int64 getNextReadPosition() const override
    {
        if (inputReaders.isEmpty())
            return 0;
        
        return inputReaders.getUnchecked(0)->getNextReadPosition();
    }
    
    /* getTotalLength */
    int64 getTotalLength() const override
    {
        const ScopedLock sl(callbackLock);
        
        if (inputReaders.isEmpty())
            return 0;
        
        return inputReaders.getUnchecked(0)->getTotalLength();
    }
    
    //==============================================================================
    /* setLooping */
    void setLooping(bool shouldLoop) override
    {
        const ScopedLock sl(callbackLock);
        
        for (auto* reader : inputReaders)
            reader->setLooping(shouldLoop);
    }
    
    /* isLooping */
    bool isLooping() const override
    {
        const ScopedLock sl(callbackLock);
        
        if (inputReaders.isEmpty())
            return false;
        
        return inputReaders.getUnchecked(0)->isLooping();
    }
    
private:
    //==============================================================================
    AudioFormatManager formatManager;
    OwnedArray<AudioFormatReaderSource> inputReaders;
    
    //==============================================================================
    CriticalSection callbackLock;
    
    //==============================================================================
    bool isRegistered = false;
    bool isPrepared = false, inputStreamEOF = false;
    bool playing = false, stopped = true;
    float currentGain = 1.0f, previousGain = 1.0f;
    double blockSize, sampleRate;
    
    float *output;
    
    SuperpoweredAnalyzer *analyzer = new SuperpoweredAnalyzer(44100);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiChannelAudioSource)
};
