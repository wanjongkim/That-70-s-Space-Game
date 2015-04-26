/*
	An AudioManager class, all audio interaction through the sssf or the
	game should happen through here.
*/
#pragma once

#include <xact3.h>
#include <string>
#include <map>

using std::wstring;
using std::string;
using std::map;

class AudioManager {
private:
	IXACT3Engine* audioEngine;
	IXACT3WaveBank* waveBank;		// The wave bank
	IXACT3WaveBank* streamingWaveBank;
	IXACT3SoundBank* soundBank;		// The sound bank
	void* waveBankData;			
	void* soundBankData;
	HANDLE streamingWaveBankData;
	XACTCATEGORY musicCategory;
	XACTCATEGORY soundCategory;
	map<string, XACTINDEX> cues;
	wstring testing;

	float musicVolume;				// Music volume
	float soundVolume;				// Sound volume

	// Used to make the notification callback function thread safe
	CRITICAL_SECTION cs;

	bool songStopped;
	bool streamingBankPrepared;
	string currentSong;

public:
	AudioManager(wstring settingsFile);
	~AudioManager();
	void changeMusicVolume(float delta);
	bool loadAudio(wstring waveBankFile, wstring streamingWaveBankFile, wstring soundBankFile);
	void loadCue(string cue);
	void changeSoundVolume(float delta);
	void update();

	// Inlined methods
	void playSound(string cue)	{ soundBank->Play(cues.at(cue), 0, 0, NULL); }
	void stopSound(string cue)	{ soundBank->Stop(cues.at(cue), 0); }

private:
	// Private helper methods
	bool initialize(wstring settingsFile);
	bool loadWaveBank(wstring waveBankFile);
	bool loadStreamingWaveBank(wstring streamingWaveBankFile);
	bool loadSoundBank(wstring waveBankFile);
	void WINAPI notificationCallback(const XACT_NOTIFICATION* notification);

};