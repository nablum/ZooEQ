/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void LookAndFeel::drawRotarySlider(juce::Graphics & g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider & slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    
    //Color Set up
    auto outlineRotarySliderColor = Colour(43u, 36u, 48u);
    auto rotarySliderColorGradient1 = Colours::lightslategrey;
    auto rotarySliderColorGradient2 = Colours::slategrey;
    auto sliderColor = Colours::lightgoldenrodyellow;
    
    // === Rotary Slider === //
    
    //Fill
    auto rotarySliderColorGradient = ColourGradient().vertical(rotarySliderColorGradient1, rotarySliderColorGradient2, bounds);
    g.setGradientFill(rotarySliderColorGradient);
    g.fillEllipse(bounds);
    
    //Draw
    g.setColour(outlineRotarySliderColor); //Color around the rotary slider
    g.drawEllipse(bounds, 1.5f);
    
    if ( auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider) )
    {
        // === Slider === //
        g.setColour(sliderColor); //Set the color
        
        //Set up
        auto center = bounds.getCentre();
        Path p;
        Rectangle<float> r;
        
        //Set Slider Position & size
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        //r.setBottom(center.getY() - rswl->getTextHeight() * 1.5); //With Slider Value in the middle
        r.setBottom(center.getY());
        p.addRoundedRectangle(r, 2.f);
        
        //Rotation
        jassert(rotaryStartAngle < rotaryEndAngle);
        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);  //Normalized angle values
        p.applyTransform(AffineTransform().rotation(sliderAngRad, center.getX(), center.getY())); //Rotation transformation
        g.fillPath(p); //Fill the graphics
    }
}

void LookAndFeel::drawToggleButton(juce::Graphics &g,
                                   juce::ToggleButton &toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown)
{
    using namespace juce;
    
    //Coulours
    auto powerButtonColourOn = Colour(215u, 43u, 71u);
    auto powerButtonColourOff = Colours::dimgrey;
    
    //Button Thickness
    float thicknessLinePowerButton = 1.f;
    float thicknessLineOutlinePowerButton = 1.5f;
    float thicknessLineAnalyserEnableButton = 1.f;
    
    //Check if the button to draw is a LowCut, Peak or HighCut Bypass button
    if (auto* pb = dynamic_cast<PowerButton*>(&toggleButton))
    {
        Path powerButton;
        
        //Set the position of the bypass button
        auto bounds = toggleButton.getLocalBounds();
        auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 5 /*JUCE_LIVE_CONSTANT(6)*/;
        auto r = bounds.withSizeKeepingCentre(size, size).toFloat(); //Put the button in the middle of the slider area
        
        //Set the power button on the left side of the toggle button area
        r.setBounds(bounds.getX() + 36, bounds.getY() + 4 /*JUCE_LIVE_CONSTANT(10)*/, size, size);
        
        //Desing the power button
        float ang = 33.f; //JUCE_LIVE_CONSTANT(30);
        size -= 8; //JUCE_LIVE_CONSTANT(6);
        
        powerButton.addCentredArc(r.getCentreX(),
                                  r.getCentreY(),
                                  size * 0.5,
                                  size * 0.5, 0.f,
                                  degreesToRadians(ang),
                                  degreesToRadians(360.f -ang),
                                  true);
        
        powerButton.startNewSubPath(r.getCentreX(), r.getY());
        powerButton.lineTo(r.getCentre());
        PathStrokeType pst(thicknessLinePowerButton, PathStrokeType::JointStyle::curved);
        
        //Apply different colours depending on the on/off power button state
        auto powerButtonColor = toggleButton.getToggleState() ? powerButtonColourOff : powerButtonColourOn;
        
        g.setColour(powerButtonColor); //Set the colour
        g.strokePath(powerButton, pst); //Draw the power sign
        g.drawEllipse(r, thicknessLineOutlinePowerButton); //Draw a circle around the power sign
    }
    //Check if the button is a analyser enable button
    else if (auto* analyserButton = dynamic_cast<AnalyserButton*>(&toggleButton))
    {
        auto color = ! toggleButton.getToggleState() ? powerButtonColourOff : powerButtonColourOn;
        g.setColour(color);
        
        auto bounds = toggleButton.getLocalBounds();
        g.drawRect(bounds);
        
        g.strokePath(analyserButton->randomPath, PathStrokeType(thicknessLineAnalyserEnableButton));
    }
}

//==============================================================================
void RotarySliderWithLabels::paint(juce::Graphics &g)
{
    using namespace juce;
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
    auto range = getRange();
    auto sliderBounds = getSliderBounds();
    
    //Rotary Slider areas debug
//    g.setColour(Colours::red);
//    g.drawRect(getLocalBounds());
//    g.setColour(Colours::yellow);
//    g.drawRect(sliderBounds);
    
    // === Slider Value === //
    //Colors
    auto backgroundTextColor = Colours::transparentWhite;
    auto outlineTextColor = Colours::transparentWhite;
    auto textColor = Colours::black;
    //Font Size
    g.setFont(getTextHeight()-1);
    
    Rectangle<float> r;
    auto text = getDisplayString();
    auto strWidth = g.getCurrentFont().getStringWidth(text);
    
    r.setSize(strWidth + 4, getTextHeight() + 2);
    r.setCentre(getLocalBounds().getCentreX(), getLocalBounds().getBottom()-10); //Slider value position
    
    //Draw rectangle background
    g.setColour(backgroundTextColor); //Backgound text color -> Set to transparent for white text
    g.fillRect(r);
    
    //Draw the background outline
    g.setColour(outlineTextColor);
    g.drawRect(r);
    
    //Draw Text
    g.setColour(textColor); //Text color
    g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
    
    // === Rotary Slider === //
    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAng,
                                      endAng,
                                      *this);
    
    // === Slider Labels === //
    //Set up
    auto labelColor = Colours::dimgrey; //Color of the slider labels
    g.setFont(getTextHeight()-2); //Set the font size
    
    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;
    
    g.setColour(labelColor); //Label color
    
    auto numChoices = labels.size();
    for ( int i = 0; i < numChoices; ++i)
    {
        auto pos = labels[i].pos;
        
        jassert(0.f <= pos);
        jassert(pos <= 1.f);
        
        float rad = 26.f; //JUCE_LIVE_CONSTANT(18.f);
        
        auto ang = jmap(pos, 0.f, 1.f, startAng + degreesToRadians(rad), endAng - degreesToRadians(rad));
        
        float mod = 1.f; //JUCE_LIVE_CONSTANT(1.f);
        
        auto c = center.getPointOnCircumference(radius + getTextHeight() * mod + 1, ang);
        //Get away from the center of the slider at the right angle
        
        Rectangle<float> r;
        
        auto str = labels[i].label;
        r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight()); //Move a little bit from the slider bound
        
        r.setCentre(c);
        r.setY(r.getY() + getTextHeight());
        
        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }
    
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size,size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(2);
    
    return r;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    //This function refoactred the way that the values are display
    
    if( auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param) )
        return choiceParam->getCurrentChoiceName();
    
    juce::String str;
    bool addK = false;
    
    //Refactor the value if it's over 999Hz to 1kHz
    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
    {
        float val = getValue();
        if (val > 999.f)
        {
            val /= 1000.f; //1001 / 1000 = 1.001
            addK = true;
        }
        str = juce::String(val, (addK ? 2 : 0)); //1001 / 1000 = 1.00
    }
    else
    {
        jassertfalse;  //This shouldn't happen!
    }
    
    //Add kHz if necessary
    if (suffix.isNotEmpty())
    {
        str << " ";
        if(addK)
            str << "k";
        
        str << suffix;
    }
    
    return str;
    
}
//==============================================================================
ResponseCurveComponent::ResponseCurveComponent(ZooEQAudioProcessor& p) :
audioProcessor(p),
//leftChannelFifo(&audioProcessor.leftChannelFifo)
leftPathProducer(audioProcessor.leftChannelFifo),
rightPathProducer(audioProcessor.rightChannelFifo)
{
    const auto& params = audioProcessor.getParameters();
    for( auto params : params )
    {
        params->addListener(this);
    }
    
    
    
    updateChain();
    
    startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = audioProcessor.getParameters();
    for( auto params : params )
    {
        params->removeListener(this);
    }
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomingBuffer;
    
    while (leftChannelFifo->getNumCompleteBuffersAvailable() > 0)
    {
        if ( leftChannelFifo->getAudioBuffer(tempIncomingBuffer) )
        {
            auto size = tempIncomingBuffer.getNumSamples();
            
            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0),
                                              monoBuffer.getReadPointer(0, size),
                                              monoBuffer.getNumSamples() - size);
            
            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
                                              tempIncomingBuffer.getReadPointer(0, 0),
                                              size);
            
            leftChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, -48.f);
        }
    }
    
    /**
     if there are FFT data buffers to pull
        if we can pull a buffer
            generate a path
     */
    const auto fftSize = leftChannelFFTDataGenerator.getFFTSize();
    /**
        4800 / 2048 = 23Hz <- this is bind width
     */
    const auto binWidth = sampleRate / (double) fftSize;
    
    while ( leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0 )
    {
        std::vector<float> fftData;
        if (leftChannelFFTDataGenerator.getFFTData(fftData))
        {
            pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -48.f);
        }
    }
    /**
        while there are paths that we can pull
            pull as many as we can
                display the most recent path
     */
    while (pathProducer.getNumPathsAvailable() )
    {
        pathProducer.getPath(leftChannelFFTPath);
    }
}

void ResponseCurveComponent::timerCallback()
{
    auto fftBounds = getAnalysisArea().toFloat();
    auto sampleRate = audioProcessor.getSampleRate();
    
    leftPathProducer.process(fftBounds, sampleRate);
    rightPathProducer.process(fftBounds, sampleRate);
    
    if ( parametersChanged.compareAndSetBool(false, true) )
    {
        //DBG("params changed");
        updateChain();
        //signal a repaint
//        repaint();
    }
    
    repaint();
    
}

void ResponseCurveComponent::updateChain()
{
    //update the monochain
    auto chainSettings = getChainSettings(audioProcessor.apvts);
    
    monoChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypassed);
    monoChain.setBypassed<ChainPositions::Peak>(chainSettings.peakBypassed);
    monoChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypassed);
    
    //Apply PeakCut Filter changes on the white line
    auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
    updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    
    //Apply LowCut Filter changes on the white line
    auto lowCutCoefficients = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
    updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
    
    //Apply HighCut Filter changes on the white line
    auto highCutCoefficients = makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());
    updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);
}

void ResponseCurveComponent::paint (juce::Graphics& g)
{
    using namespace juce;
    
    //Colors
    auto responseCurveColor = Colours::white;
    auto backgroundOutlineColor = Colour(43u, 36u, 48u);
    auto backgroundColor = Colour(140u, 200u, 190u);
    auto fftLeftColor = Colours::goldenrod;
    auto fftRightColor = Colours::yellow;
    
    //Display parameters
    float cornerSizeDisplay = 4.f;
    float lineThicknessDisplay = 3.f;
    float strokeThickness = 2.f;
    
    //Draw the background render area
    g.setColour(backgroundColor);
    g.fillRect(getRenderArea());
    
    g.drawImage(background, getLocalBounds().toFloat()); //Draw the freq&dB lines
    //(for setup go to ResponseCurveComponent::paint resized())
    
    auto responseArea = getAnalysisArea();
    
    auto w = responseArea.getWidth();
    
    auto& lowcut = monoChain.get<ChainPositions::LowCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& highcut = monoChain.get<ChainPositions::HighCut>();
    
    auto sampleRate = audioProcessor.getSampleRate();
    
    std::vector<double> mags;
    mags.resize(w);
    
    for ( int i = 0; i < w; ++i )
    {
        double mag = 1.f;
        auto freq = mapToLog10((double(i)/double(w)), 20.0, 20000.0);
        
        //Peak
        if(! monoChain.isBypassed<ChainPositions::Peak>())
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        
        //Low Cut
        if (! monoChain.isBypassed<ChainPositions::LowCut>() )
        {
            if( !lowcut.isBypassed<0>() )
                mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if( !lowcut.isBypassed<1>() )
                mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if( !lowcut.isBypassed<2>() )
                mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if( !lowcut.isBypassed<3>() )
                mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        
        //High Cut
        if (! monoChain.isBypassed<ChainPositions::HighCut>() )
        {
            if( !highcut.isBypassed<0>() )
                mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if( !highcut.isBypassed<1>() )
                mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if( !highcut.isBypassed<2>() )
                mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if( !highcut.isBypassed<3>() )
                mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        
        mags[i] = Decibels::gainToDecibels(mag);
    }
    
    Path responseCurve;
    
    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin,outputMax](double input)
    {
        return jmap(input, -24.0, 24.0, outputMin, outputMax);
    };
    
    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));
    
    for ( size_t i = 1; i<mags.size(); ++i )
    {
        responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }
    
    //Draw the FFT
        //Left channel
    auto leftChannelFFTPath = leftPathProducer.getPath();
    leftChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));
    g.setColour(fftLeftColor);
    g.strokePath(leftChannelFFTPath, PathStrokeType(2.f));
    
        //right channel
    auto rightChannelFFTPath = rightPathProducer.getPath();
    rightChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));
    g.setColour(fftRightColor);
    g.strokePath(rightChannelFFTPath, PathStrokeType(2.f));
    
    //Draw the render area outline
    g.setColour(backgroundOutlineColor);
    g.drawRoundedRectangle(getRenderArea().toFloat(), cornerSizeDisplay, lineThicknessDisplay);
 
    //Draw the reponse curve
    g.setColour(responseCurveColor);
    g.strokePath(responseCurve, PathStrokeType(strokeThickness));
    
    
}

void ResponseCurveComponent::resized()
{
    using namespace juce;
    background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);
    Graphics g(background);
    
    //Colours
    auto freqLineColour = Colours::whitesmoke;
    auto gainLineColour = Colours::lightslategrey;
    auto gain0dBLineColour = Colours::red;
    auto freqLabelColour = Colours::dimgrey;
    auto gainLabelColour = Colours::dimgrey;
    auto gain0dBLabelColour = Colours::red;
    
    //Labels text size
    const int fontHeighFreqLabel = 10;
    const int fontHeighGainLabel = 9;
    
    // === Draw curve component vertical lines (Freq) === //
    
    g.setColour(freqLineColour); //Set colour of the vertical lines
    
    Array<float> freqs
    {
        20,/*30,40,*/50,100,
        200,/*300,400,*/500,1000,
        2000,/*3000,4000,*/5000,10000,
        20000
    };
    
    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();
    
    Array<float> xs;
    for (auto f : freqs)
    {
        auto normX = mapFromLog10(f, 20.f, 20000.f); //map the freqs in log10 base
        xs.add(left + width * normX);
        
    }
    
    for (auto x : xs)
    {
        g.drawVerticalLine(x, top, bottom);
    }
    
    // === Draw curve component horizontal lines (Gain) === //
    Array<float> gain
    {
        -24,-12,0,12,24
    };
    
    for (auto gdB : gain)
    {
        auto y = jmap(gdB, -24.f, 24.f, float(bottom), float(top)); //Map the -24dB to the bottom and +24dB to the top
        g.setColour(gdB == 0.f ? gain0dBLineColour : gainLineColour); //Set the horizontal line colours
        g.drawHorizontalLine(y, left, right);
    }
    
    // === Draw analysis area labels === //
    
    //Frequency label
    //Setup
    g.setColour(freqLabelColour);
    g.setFont(fontHeighFreqLabel);
    
    for (int i = 0; i< freqs.size(); ++i)
    {
        auto f = freqs[i];
        auto x = xs[i];
        
        //Add kHz if necessary
        bool addK = false;
        String str;
        if (f > 999.f)
        {
            addK = true;
            f /= 1000.f;
        }
        str << f;
        if (addK)
            str << "k";
        str << "Hz";
        
        //Build a rectangle around the label
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        Rectangle<int> r;
        r.setSize(textWidth, fontHeighFreqLabel);
        r.setCentre(x, 0);
        r.setY(1);
        g.drawFittedText(str, r, Justification::centred, 1);
    }
    
    //Gain label
    //Setup
    g.setFont(fontHeighGainLabel);
    
    for (auto gdB : gain)
    {
        //Draw the curve gain (right labels)
        auto y = jmap(gdB, -24.f, 24.f, float(bottom), float(top)); //Map the -24dB to the bottom and +24dB to the top
        String str;
        
        if (gdB > 0)
            str << "+";
        str << gdB;
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        Rectangle<int> r;
        r.setSize(textWidth, fontHeighGainLabel);
        r.setX(getWidth() - textWidth);
        r.setCentre(r.getCentreX(), y);
        
        g.setColour(gdB == 0.f ? gain0dBLabelColour : gainLabelColour); //Set the gain label colours
        //The colour is different if the Gain is 0dB
        
        g.drawFittedText(str, r, Justification::centred, 1);
        
        //Draw the analyser gain (left labels)
        str.clear();
        str << (gdB - 24.f); //From 0dB to -48dB
        
        r.setX(1); //gap between analysis area and text
        textWidth = g.getCurrentFont().getStringWidth(str);
        r.setSize(textWidth, fontHeighGainLabel);
        
        g.setColour(gainLabelColour);
        g.drawFittedText(str, r, Justification::centred, 1);
    }
    
}

juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
    auto bounds = getLocalBounds();
    
//    bounds.reduce(10, //JUCE_LIVE_CONSTANT(5),
//                  8 //JUCE_LIVE_CONSTANT(5)
//                  );

    //Resized the curve reponse component
    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);
    
    return bounds;
    
}

juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();
    
    //Define the Analysis Area (where the Freq/Gain values are display
    //Make a gape to avoid colision between text and response curve bounds
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    
    return bounds;
}

//==============================================================================
ZooEQAudioProcessorEditor::ZooEQAudioProcessorEditor (ZooEQAudioProcessor& p):
AudioProcessorEditor (&p),
audioProcessor (p),

peakFreqSlider(*audioProcessor.apvts.getParameter("Peak Freq"), "Hz"),
peakGainSlider(*audioProcessor.apvts.getParameter("Peak Gain"), "dB"),
peakQualitySlider(*audioProcessor.apvts.getParameter("Peak Quality"), ""),
lowCutFreqSlider(*audioProcessor.apvts.getParameter("LowCut Freq"), "Hz"),
highCutFreqSlider(*audioProcessor.apvts.getParameter("HighCut Freq"), "Hz"),
lowCutSlopeSlider(*audioProcessor.apvts.getParameter("LowCut Slope"), "dB/Oct"),
highCutSlopeSlider(*audioProcessor.apvts.getParameter("HighCut Slope"), "dB/Oct"),

responseCurveComponent(audioProcessor),

peakFreqSliderAttachment(audioProcessor.apvts, "Peak Freq", peakFreqSlider),
peakGainSliderAttachment(audioProcessor.apvts, "Peak Gain", peakGainSlider),
peakQualitySliderAttachment(audioProcessor.apvts, "Peak Quality", peakQualitySlider),
lowCutFreqSliderAttachment(audioProcessor.apvts, "LowCut Freq", lowCutFreqSlider),
highCutFreqSliderAttachment(audioProcessor.apvts, "HighCut Freq", highCutFreqSlider),
lowCutSlopeSliderAttachment(audioProcessor.apvts, "LowCut Slope", lowCutSlopeSlider),
highCutSlopeSliderAttachment(audioProcessor.apvts, "HighCut Slope", highCutSlopeSlider),

lowcutBypassButtonAttachment(audioProcessor.apvts, "LowCut Bypassed", lowcutBypassButton),
peakBypassButtonAttachment(audioProcessor.apvts, "Peak Bypassed", peakBypassButton),
highcutBypassButtonAttachment(audioProcessor.apvts, "HighCut Bypassed", highcutBypassButton),
analyserEnableButtonAttachment(audioProcessor.apvts, "Analyser Enable", analyserEnableButton)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    peakFreqSlider.labels.add({0.f, "20Hz"});
    peakFreqSlider.labels.add({1.f, "20kHz"});
    
    peakGainSlider.labels.add({0.f, "-24dB"});
    peakGainSlider.labels.add({1.f, "+24dB"});
    
    peakQualitySlider.labels.add({0.f, "0.1"});
    peakQualitySlider.labels.add({1.f, "10"});
    
    lowCutFreqSlider.labels.add({0.f, "20Hz"});
    lowCutFreqSlider.labels.add({1.f, "20kHz"});
    
    lowCutSlopeSlider.labels.add({0.f, "12"});
    lowCutSlopeSlider.labels.add({1.f, "48"});
    
    highCutFreqSlider.labels.add({0.f, "20Hz"});
    highCutFreqSlider.labels.add({1.f, "20kHz"});
    
    highCutSlopeSlider.labels.add({0.f, "12"});
    highCutSlopeSlider.labels.add({1.f, "48"});
    
    
    //Set the custom rotary slider 
    for( auto* comp : getComps() )
    {
        addAndMakeVisible(comp);
    }
    
    lowcutBypassButton.setLookAndFeel(&lnf);
    peakBypassButton.setLookAndFeel(&lnf);
    highcutBypassButton.setLookAndFeel(&lnf);
    analyserEnableButton.setLookAndFeel(&lnf);
    
    auto safePtr = juce::Component::SafePointer<ZooEQAudioProcessorEditor>(this);
    peakBypassButton.onClick = [safePtr]()
    {
        if (auto* comp = safePtr.getComponent())
        {
            auto bypassed = comp->peakBypassButton.getToggleState();
            comp->peakFreqSlider.setEnabled(!bypassed);
            comp->peakGainSlider.setEnabled(!bypassed);
            comp->peakQualitySlider.setEnabled(!bypassed);
            
        }
    };
    
    setSize (600, 400);
}

ZooEQAudioProcessorEditor::~ZooEQAudioProcessorEditor()
{
    lowcutBypassButton.setLookAndFeel(nullptr);
    peakBypassButton.setLookAndFeel(nullptr);
    highcutBypassButton.setLookAndFeel(nullptr);
    analyserEnableButton.setLookAndFeel(nullptr);
}

//==============================================================================
void ZooEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    
    auto Color1 = Colours::white;
    auto Color2 = Colour(190u, 190u, 190u);
    
    auto backgroundColorGradient = ColourGradient().vertical(Color1, Color2, getLocalBounds());
    
    g.setGradientFill(backgroundColorGradient);
    g.fillAll();
    
}

void ZooEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    //=== Organisation of the display : Placement of the elements === //
    
    auto bounds = getLocalBounds();
    
    auto analyzerEnableArea = bounds.removeFromTop(25);
    analyzerEnableArea.setWidth(40 /*JUCE_LIVE_CONSTANT(50)*/);
    analyzerEnableArea.setX( 20 /*JUCE_LIVE_CONSTANT(5)*/); //To don't be glue to the left bound window
    analyzerEnableArea.removeFromTop(2); //To don't be glue to the top bound window
    analyserEnableButton.setBounds(analyzerEnableArea);
    
    bounds.removeFromTop(5);
    
    float hRatio = 32.f / 100.f;// JUCE_LIVE_CONSTANT(33) / 100.f;
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * hRatio);
    responseCurveComponent.setBounds(responseArea);
    
    bounds.removeFromTop(5); //Create a gap between response curve and sliders
    
    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);
    
    lowcutBypassButton.setBounds(lowCutArea.removeFromTop(25)); //Set the area for the bypassed button
    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5)); //Set the area of freq slider
    lowCutSlopeSlider.setBounds(lowCutArea); //Set area for the slope slider
    
    highcutBypassButton.setBounds(highCutArea.removeFromTop(25));
    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);
    
    peakBypassButton.setBounds(bounds.removeFromTop(25));
    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds(bounds);
}

std::vector<juce::Component*> ZooEQAudioProcessorEditor::getComps()
{
    return
    {   &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider,
        &responseCurveComponent,
        &lowcutBypassButton,
        &peakBypassButton,
        &highcutBypassButton,
        &analyserEnableButton
    };
}
