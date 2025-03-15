/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#include "midioutputdevice.h"

#include <RtMidi.h>
#include <exception>
#include <score/dynamic.h>
#include <score/generalmidi.h>
#include <cassert>

#include <QAudioDevice>
#include <QMediaDevices>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QAudioSink>
#include <QBuffer>
#include <QAudioDecoder>
#include <QDebug>
#ifdef __APPLE__
#include "midisoftwaresynth.h"
#endif
double midiNoteToFrequency(int midiNote) {
	return 440.0 * std::pow(2.0, (midiNote - 69) / 12.0);
}
// 生成正弦波
QByteArray generateSineWave(int midiNote) {
	//double frequency = midiNote / 127.0;// midiNoteToFrequency(midiNote);
	double frequency =  midiNoteToFrequency(midiNote);
  //  if (midiNote == 127)
  //  {
  //      frequency = 880*2;
  //  }
  //  else
  //  {
		//frequency = 440*2;
  //  }
	int sampleRate = 8000;  // 采样率 
	int duration = 1;        // 播放 1 秒
	int totalSamples = sampleRate * duration;
    double normFre = 2 * M_PI / sampleRate;
    double scale = frequency * normFre;
	QByteArray byteArray;
    byteArray.reserve(totalSamples);
	// 计算每个采样点的值
	for (int i = 0; i < totalSamples; ++i) {
		// 计算正弦波值
		//double sample = frequency;// std::sin(2 * M_PI * frequency * i / sampleRate);
		double sample = std::sin(scale  * i );

		//short int16Sample = static_cast<short>(sample * 32767); // 16-bit 音频
		short int16Sample = static_cast<short>(sample * 32767); // 16-bit 音频

		// 将数据转为字节流
		byteArray.append(reinterpret_cast<char*>(&int16Sample), sizeof(int16Sample));
    }

	return byteArray;
}
MidiOutputDevice::MidiOutputDevice() /*: myMidiOut(nullptr)*/
{
    // Initialize the OSX software synth.
#ifdef __APPLE__
    try
    {
        static MidiSoftwareSynth synth;
        synth.initialize();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    };
#endif

    myMaxVolumes.fill(Midi::MAX_MIDI_CHANNEL_VOLUME);
    myActiveVolumes.fill(static_cast<uint8_t>(VolumeLevel::fff));

    // Create all MIDI APIs supported on this platform.
    //std::vector<RtMidi::Api> apis;
    //RtMidi::getCompiledApi(apis);

    //for (const RtMidi::Api &api : apis)
    //{
    //    try
    //    {
    //        qDebug()<<"api "<<api<<"";;
    //        myMidiOuts.emplace_back(new RtMidiOut(api));
    //    }
    //    catch (RtMidiError &e)
    //    {
    //        // Continue anyway, another API might work.
    //        e.printMessage();
    //        qDebug()<<"err midi:"<<e.what();
    //    }
    //}
}

MidiOutputDevice::~MidiOutputDevice()
{
    // Make sure there aren't any lingering notes.
    if (myMidiOut)
        stopAllNotes();

	if (myCurOutput)
	{
        stopOutAudio();
		//mfile.close();
		//myCurOutput->stop();

		//while (QAudio::State::StoppedState != myCurOutput->state())
		//{

		//}
		//myCurOutput->moveToThread();
		myCurOutput->deleteLater();
		//delete myCurOutput;
		myCurOutput = nullptr;
	}
}

void
MidiOutputDevice::stopAllNotes()
{
    for (uint8_t channel = 0; channel < Midi::NUM_MIDI_CHANNELS_PER_PORT;
         ++channel)
    {
        // Turn off the pedal in case a "let ring" event was active.
        sendMidiMessage(ControlChange + channel, HoldPedal, 0);
        // Stop all notes.
        sendMidiMessage(ControlChange + channel, AllNotesOff, 0);
    }
    stopOutAudio();

}


void MidiOutputDevice::stopOutAudio()
{
	myCurOutput->stop();
	//mfile.close();
	//mfile.open(QIODevice::ReadOnly);
	while (QAudio::State::StoppedState != myCurOutput->state())
	{

	}
	if (myBuff)
	{
		delete myBuff;
		myBuff = nullptr;
	}
}

bool
MidiOutputDevice::sendMessage(const std::vector<uint8_t> &data)
{
    if (data.empty()) {

        return false;
    }
    if (data.size() == 3)
    {
		if (data[0] == 153)
		{//节拍器音符
            // QAudioDecoder d;
            // d.read();
            uint8_t pitch = data[1];
            if (data[2] != 127)
            {
                pitch = 59;
            }
			QByteArray audioData = generateSineWave(pitch);
			myBuff = new QBuffer();
			myBuff->setData(audioData);
			myBuff->open(QIODevice::ReadOnly);
            myCurOutput->setVolume(data[2]/127.0);
			myCurOutput->start(myBuff);
		}
		else if (data[0] == 128 + 9)
		{//节拍器结束符号
			stopOutAudio();
		}
		else {
			bool what = true;
		}
    }
    return true;
	try
	{
		//myMidiOut->sendMessage(&message);
		myMidiOut->sendMessage(&data);
	}
	catch (RtMidiError& e)
	{
		e.printMessage();
		return false;
	}
    return true;
}

bool MidiOutputDevice::sendMidiMessage(unsigned char a, unsigned char b,
                                       unsigned char c)
{
    // return true;
    std::vector<uint8_t> message;

    message.push_back(a);

    if (b <= 127)
        message.push_back(b);

    if (c <= 127)
        message.push_back(c);

    return sendMessage(message);
    //try
    //{
    //    myMidiOut->sendMessage(&message);
    //}
    //catch (RtMidiError &e)
    //{
    //    e.printMessage();
    //    return false;
    //}

    //return true;
}
bool MidiOutputDevice::initialize(size_t preferredApi,
                                  unsigned int preferredPort)
{
    if (!myCurOutput)
	{
		QAudioFormat format;
		format.setSampleRate(8000); // 采样率 44.1 kHz
		format.setChannelCount(1);   // 单声道
		//format.setSampleFormat(QAudioFormat::UInt8);
		format.setSampleFormat(QAudioFormat::Int16);
        
		//format.setSampleRate(16);    // 16 位深度
		//format.setSampleFormat();
		//format.setByteOrder(QAudioFormat::LittleEndian);
		//format.setSampleType(QAudioFormat::SignedInt);
        //mfile.setFileName("G:/music_projs/pteplayer/music/tick-4-4-120bp.wav");
        //mfile.open(QIODevice::ReadOnly);
        QAudioDevice info(QMediaDevices::defaultAudioOutput());
        if (!info.isFormatSupported(format))
        {
            bool pause = true;
        }

		myCurOutput = new QAudioSink(QMediaDevices::defaultAudioOutput(), format);
        //QByteArray audioData = generateSineWave(440);
        //myBuff = new QBuffer();
        //myBuff->setData(audioData);
        //myBuff->open(QIODevice::ReadOnly);

        //myCurOutput->start(myBuff);
    }
    return true;
    if (myMidiOut)
        myMidiOut->closePort(); // Close any open ports.
    qDebug()<<"myMidiOuts.size() "<<myMidiOuts.size();
    if (preferredApi >= myMidiOuts.size())
        return false;

    myMidiOut = myMidiOuts[preferredApi].get();
    unsigned int num_ports = myMidiOut->getPortCount();

    qDebug()<<"num_ports "<<num_ports;
    if (num_ports == 0)
        return false;
    return true;
    try
    {
        myMidiOut->openPort(preferredPort);
    }
    catch (RtMidiError &e)
    {
        //qDebug()<<"err message "<<e.getMessage();
        e.printMessage();
        return false;
    }

    return true;
}

size_t MidiOutputDevice::getApiCount()
{
     return 0;
    //return myMidiOuts.size();
}

unsigned int MidiOutputDevice::getPortCount(size_t api)
{
    //assert(api < myMidiOuts.size() && "Programming error, api doesn't exist");
    //return myMidiOuts[api]->getPortCount();
     return 0;
}

std::string MidiOutputDevice::getPortName(size_t api, unsigned int port)
{
    //assert(api < myMidiOuts.size() && "Programming error, api doesn't exist");
    //return myMidiOuts[api]->getPortName(port);
    return "";
}

bool MidiOutputDevice::setPatch(int channel, uint8_t patch)
{
    // return true;
    if (patch > 127)
    {
        patch = 127;
    }

    // MIDI program change:
    // - first parameter is 0xC0-0xCF with C being the id and 0-F being the
    //   channel (0-15).
    // - second parameter is the new patch (0-127).
    return sendMidiMessage(ProgramChange + channel, patch, -1);
}

bool MidiOutputDevice::setVolume (int channel, uint8_t volume)
{
    //assert(volume <= 127);

    myActiveVolumes[channel] = volume;

    return sendMidiMessage(
        ControlChange + channel, ChannelVolume,
        static_cast<int>((volume / 127.0) * myMaxVolumes[channel]));
}

bool MidiOutputDevice::setPan(int channel, uint8_t pan)
{
    if (pan > 127)
        pan = 127;

    // MIDI control change
    // first parameter is 0xB0-0xBF with B being the id and 0-F being the channel (0-15)
    // second parameter is the control to change (0-127), 10 is channel pan
    // third parameter is the new pan (0-127)
    return sendMidiMessage(ControlChange + channel, PanChange, pan);
}

bool MidiOutputDevice::setPitchBend (int channel, uint8_t bend)
{
    if (bend > 127)
        bend = 127;

    return sendMidiMessage(PitchWheel + channel, 0, bend);
}

bool MidiOutputDevice::playNote(int channel, uint8_t pitch, uint8_t velocity)
{
    if (pitch > 127)
    {
        pitch = 127;
    }

    if (velocity == 0)
    {
        velocity = 1;
    }
    else if (velocity > 127)
    {
        velocity = 127;
    }

    // MIDI note on
    // first parameter 0x90-9x9F with 9 being the id and 0-F being the channel (0-15)
    // second parameter is the pitch of the note (0-127), 60 would be a 'middle C'
    // third parameter is the velocity of the note (1-127), 0 is not allowed, 64 would be no velocity
    return sendMidiMessage(NoteOn + channel, pitch, velocity);
}

bool MidiOutputDevice::stopNote(int channel, uint8_t pitch)
{
    if (pitch > 127)
        pitch=127;

    // MIDI note off
    // first parameter 0x80-9x8F with 8 being the id and 0-F being the channel (0-15)
    // second parameter is the pitch of the note (0-127), 60 would be a 'middle C'
    return sendMidiMessage(NoteOff + channel, pitch, 127);
}

bool MidiOutputDevice::setVibrato(int channel, uint8_t modulation)
{
    if (modulation > 127)
        modulation = 127;

    return sendMidiMessage(ControlChange + channel, ModWheel, modulation);
}

bool MidiOutputDevice::setSustain(int channel, bool sustainOn)
{
    const uint8_t value = sustainOn ? 127 : 0;
    
    return sendMidiMessage(ControlChange + channel, HoldPedal, value);
}

void MidiOutputDevice::setPitchBendRange(int channel, uint8_t semiTones)
{
    sendMidiMessage(ControlChange + channel, RpnMsb, 0);
    sendMidiMessage(ControlChange + channel, RpnLsb, 0);
    sendMidiMessage(ControlChange + channel, DataEntryCoarse, semiTones);
    sendMidiMessage(ControlChange + channel, DataEntryFine, 0);
}

void MidiOutputDevice::setChannelMaxVolume(int channel, uint8_t newMaxVolume)
{
     return ;
    assert(newMaxVolume <= 127);

    const bool maxVolumeChanged = myMaxVolumes[channel] != newMaxVolume;
    myMaxVolumes[channel] = newMaxVolume;

    // If the new volume is different from the existing volume, send out a MIDI message
    if (maxVolumeChanged)
        setVolume(channel, myActiveVolumes[channel]);
}
