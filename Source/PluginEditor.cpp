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
//    auto backgroundTextColor = Colours::transparentBlack;
//    auto textColor = Colours::white;
    
    // === Rotary Slider === //
    
    //Fill
    //g.setColour(Colours::dimgrey); //Color inside the rotary slider
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
        
        // === Slider Actual Value === //
//        g.setFont(rswl->getTextHeight());
//        auto text = rswl->getDisplayString();
//        auto strWidth = g.getCurrentFont().getStringWidth(text);
//
//        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
//        r.setCentre(bounds.getCentre());
//
//        //Draw text background
//        g.setColour(backgroundTextColor); //Backgound text color -> Set to transparent for white text
//        g.fillRect(r);
//
//        //Draw Text
//        g.setColour(textColor); //Text color
//        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
        
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
        
        auto ang = jmap(pos, 0.f, 1.f, startAng + degreesToRadians(18.f), endAng - degreesToRadians(18.f));
        
        auto c = center.getPointOnCircumference(radius + getTextHeight() * 1.0f + 1, ang);
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
ResponseCurveComponent::ResponseCurveComponent(ZooEQAudioProcessor& p) : audioProcessor(p)
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

void ResponseCurveComponent::timerCallback()
{
    if ( parametersChanged.compareAndSetBool(false, true) )
    {
        //DBG("params changed");
        updateChain();
        //signal a repaint
        repaint();
    }
}

void ResponseCurveComponent::updateChain()
{
    //update the monochain
    auto chainSettings = getChainSettings(audioProcessor.apvts);
    
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
    
    //Display parameters
    float cornerSizeDisplay = 4.f;
    float lineThicknessDisplay = 3.f;
    float strokeThickness = 2.f;
    
    g.fillAll (backgroundColor); //Curve Background Color
    
    g.drawImage(background, getLocalBounds().toFloat()); //Draw the freq&dB lines
    //(for setup go to ResponseCurveComponent::paint resized())
    
//    auto responseArea = getLocalBounds();
//    auto responseArea = getRenderArea();
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
        if(! monoChain.isBypassed<ChainPositions::Peak>())
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);
        
        //Low Cut
        if( !lowcut.isBypassed<0>() )
            mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if( !lowcut.isBypassed<1>() )
            mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if( !lowcut.isBypassed<2>() )
            mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if( !lowcut.isBypassed<3>() )
            mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        
        //High Cut
        if( !highcut.isBypassed<0>() )
            mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if( !highcut.isBypassed<1>() )
            mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if( !highcut.isBypassed<2>() )
            mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if( !highcut.isBypassed<3>() )
            mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        
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
    
    g.setColour(backgroundOutlineColor);
    g.drawRoundedRectangle(getRenderArea().toFloat(), cornerSizeDisplay, lineThicknessDisplay);
 
    g.setColour(responseCurveColor);
    g.strokePath(responseCurve, PathStrokeType(strokeThickness));
    
    
}

void ResponseCurveComponent::resized()
{
    using namespace juce;
    background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);
    Graphics g(background);
    
    //Colours
    auto freqLineColour = Colours::white;
    auto gainLineColour = Colours::lightslategrey;
    auto gain0dBLineColour = Colours::red;
    
    // === Draw curve component vertical lines (Freq) === //
    
    g.setColour(freqLineColour); //Set colour of the vertical lines
    
    Array<float> freqs
    {
        20,30,40,50,100,
        200,300,400,500,1000,
        2000,3000,4000,5000,10000,
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
//        auto normX = mapFromLog10(f, 20.f, 20000.f); //map the freqs in log10 base
//        g.drawVerticalLine(getWidth() * normX, 0.f, getHeight());
//        Draw vertical lines of ONE pixel in log10 from the top(0.f) to the bottom(getHeight)
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
//        g.drawHorizontalLine(y, 0.f, getWidth()); //Draw horizontal line from the left(0.f) to the right(getWidth)
        g.setColour(gdB == 0.f ? gain0dBLineColour : gainLineColour); //Set the horizontal line colours
        g.drawHorizontalLine(y, left, right);
    }
    
//    g.drawRect(getAnalysisArea());
    
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
highCutSlopeSliderAttachment(audioProcessor.apvts, "HighCut Slope", highCutSlopeSlider)
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
    
    setSize (600, 400);
}

ZooEQAudioProcessorEditor::~ZooEQAudioProcessorEditor()
{

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
    
    //Organisation of the display : Placement of the elements
    auto bounds = getLocalBounds();
    float hRatio = 32.f / 100.f;// JUCE_LIVE_CONSTANT(33) / 100.f;
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * hRatio);
    
    responseCurveComponent.setBounds(responseArea);
    
    bounds.removeFromTop(5); //Create a gap between response curve and sliders
    
    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);
    
    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);
    
    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);
    
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
        &responseCurveComponent
    };
}
