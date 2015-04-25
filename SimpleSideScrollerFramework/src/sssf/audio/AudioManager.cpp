#include "AudioManager.h"

AudioManager::AudioManager() {
}

/*
	Clean up
*/
AudioManager::~AudioManager() {
	if (audioEngine) {
		audioEngine->ShutDown();
		audioEngine->Release();
	}
	audioEngine = nullptr;

	if (streamingWaveBankData != INVALID_HANDLE_VALUE && streamingWaveBankData) {
		CloseHandle(streamingWaveBankData);
	}

	if (soundBankData) delete[] soundBankData;
	soundBankData = nullptr;

	if (waveBankData) UnmapViewOfFile(waveBankData);
	waveBankData = nullptr;

	if (soundBank) soundBank->Destroy();
	if (waveBank) waveBank->Destroy();

	DeleteCriticalSection(&cs);

	CoUninitialize();
}

/*
	Initialize the XACT audio engine
*/
bool AudioManager::initialize() {
	// Initialize the critical section, used for preventing race conditions
	InitializeCriticalSection(&cs);

	testing = L"Yes";

	// Create a new thread
	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) return false;

	// Create the XACT audio engine
	if (FAILED(XACT3CreateEngine(XACT_FLAG_API_AUDITION_MODE, &audioEngine))) return false;
	if (audioEngine == 0) return false;

	// If loading a global settings file, do it here

	// Set the runtime parameters
	XACT_RUNTIME_PARAMETERS params = { 0 };
	params.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT;

	//params.fnNotificationCallback = &AudioManager::notificationCallback;

	// Initialize the audio engine with the parameters
	if (FAILED(audioEngine->Initialize(&params))) return false;
	return true;

	// Register the various XACT notifications
	// Wave bank prepared
	XACT_NOTIFICATION_DESCRIPTION desc = { 0 };
	desc.flags = XACT_FLAG_NOTIFICATION_PERSIST;
	desc.type = XACTNOTIFICATIONTYPE_WAVEBANKPREPARED;
	audioEngine->RegisterNotification(&desc);

	// Sound bank destroyed
	desc.flags = XACT_FLAG_NOTIFICATION_PERSIST;
	desc.type = XACTNOTIFICATIONTYPE_SOUNDBANKDESTROYED;
	audioEngine->RegisterNotification(&desc);

	// Cue stop
	desc.flags = XACT_FLAG_NOTIFICATION_PERSIST;
	desc.type = XACTNOTIFICATIONTYPE_CUESTOP;
	desc.cueIndex = XACTINDEX_INVALID;
	audioEngine->RegisterNotification(&desc);

	// Cue prepared -- not really using this one 
	desc.flags = XACT_FLAG_NOTIFICATION_PERSIST;
	desc.type = XACTNOTIFICATIONTYPE_CUEPREPARED;
	desc.cueIndex = XACTINDEX_INVALID;
	audioEngine->RegisterNotification(&desc);

}

/*
	Load audio from wave bank and sound bank files
*/
bool AudioManager::loadAudio(wstring waveBankFile, wstring streamingWaveBankFile,
		wstring soundBankFile) {
	// Load the wave and sound banks
	if (!loadWaveBank(waveBankFile)) return false;
	if (!loadStreamingWaveBank(streamingWaveBankFile)) return false;
	if (!loadSoundBank(soundBankFile)) return false;	// <-- Don't use memory mapped IO for sound bank

	// Get the sound categories
	musicCategory = audioEngine->GetCategory("Music");
	soundCategory = audioEngine->GetCategory("Sound");
	return true;
}

/*
	Load a wave bank using memory mapped file IO
*/
bool AudioManager::loadWaveBank(wstring waveBankFile) {
	// Load the data file
	unsigned long fileSize = 0;

	// Create a file handle and get the size of the file
	HANDLE file = CreateFile(waveBankFile.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
	if (file == INVALID_HANDLE_VALUE) return false;
	fileSize = GetFileSize(file, 0);
	if (fileSize == -1) {
		CloseHandle(file);
		return false;
	}

	// Create a map to the file's data
	HANDLE mapFile = CreateFileMapping(file, 0, PAGE_READONLY, 0, fileSize, 0);
	if (!mapFile) {
		CloseHandle(file);
		return false;
	}

	// Get a pointer to the buffer, clean up and return the pointer
	waveBankData = MapViewOfFile(mapFile, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(mapFile);
	CloseHandle(file);

	// Create the wave bank using the XACT Engine
	if (FAILED (audioEngine->CreateInMemoryWaveBank(waveBankData, fileSize, 0, 0, &waveBank))) {
		return false;
	}

	return true;
}

/*
	Load a streaming wave bank
*/
bool AudioManager::loadStreamingWaveBank(wstring streamingWaveBankFile) {
	streamingWaveBankData = CreateFile(streamingWaveBankFile.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
	if (streamingWaveBankData == INVALID_HANDLE_VALUE) return false;

	// Set streaming parameters
	XACT_WAVEBANK_STREAMING_PARAMETERS params;
	ZeroMemory(&params, sizeof(XACT_WAVEBANK_STREAMING_PARAMETERS));
	params.file = streamingWaveBankData;
	params.offset = 0;

	// Allocate 64 * 2K streaming buffer and create the streaming wave bank
	params.packetSize = 64;
	if (FAILED(audioEngine->CreateStreamingWaveBank(&params, &streamingWaveBank))) return false;


}

/*
	Load a sound bank
*/
bool AudioManager::loadSoundBank(wstring waveBankFile) {
	// Load the data file
	unsigned long fileSize = 0;
	HANDLE file = CreateFile(waveBankFile.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
	if (file == INVALID_HANDLE_VALUE) return false;
	fileSize = GetFileSize(file, 0);
	if (fileSize == -1) {
		CloseHandle(file);
		return false;
	}

	soundBankData = new BYTE[fileSize];
	if (!soundBankData) {
		CloseHandle(file);
		return false;
	}

	unsigned long bytesRead = 0;
	if (ReadFile(file, soundBankData, fileSize, &bytesRead, NULL) == 0) {
		CloseHandle(file);
		return false;
	}

	CloseHandle(file);
	// Create the sound bank using the XACT Engine
	if (FAILED(audioEngine->CreateSoundBank(soundBankData, fileSize, 0, 0, &soundBank))) {
		return false;
	}

	return true;
}

/*
	Load an audio cue by name. Note using string instead of wstring here.
*/
void AudioManager::loadCue(string cue) { 
	XACTINDEX index = soundBank->GetCueIndex(cue.c_str());
	cues[cue] = index;
}

/*
	Change the volume of sound effects. For example, delta = 0.05 to raise the volume
	by 5%, delta = -0.05 to lower by 5%. 
*/
void AudioManager::changeSoundVolume(float delta) {
	soundVolume += delta;
	if (soundVolume < 0.0f) soundVolume = 0.0f;
	if (soundVolume > 1.0f) soundVolume = 1.0f;
	audioEngine->SetVolume(soundCategory, soundVolume);
}

/*
	Change the volume of music. For example, delta = 0.05 to raise the volume
	by 5%, delta = -0.05 to lower by 5%.
*/
void AudioManager::changeMusicVolume(float delta) {
	musicVolume += delta;
	if (musicVolume < 0.0f) musicVolume = 0.0f;
	if (musicVolume > 1.0f) musicVolume = 1.0f;
	audioEngine->SetVolume(musicCategory, musicVolume);
}

/*
	This handles notifications from XACT, for example, when a song stops playing.
*/
void WINAPI AudioManager::notificationCallback(const XACT_NOTIFICATION* notification) {
	// If the current song stopped
	if (notification->type == XACTNOTIFICATIONTYPE_CUESTOP) {
		if (notification->cue.cueIndex == cues.at("")) {
			EnterCriticalSection(&cs);
			songStopped = true; // Mark this as true, and deal with it in update()
			LeaveCriticalSection;
		}
	}

	// The streaming wave bank is prepared
	if (notification->type == XACTNOTIFICATIONTYPE_WAVEBANKPREPARED &&
		notification->waveBank.pWaveBank == streamingWaveBank) {
		EnterCriticalSection(&cs);
		streamingBankPrepared = true;
		LeaveCriticalSection(&cs);
	}

	if (notification->type == XACTNOTIFICATIONTYPE_SOUNDBANKDESTROYED &&
		notification->soundBank.pSoundBank == soundBank) {
		// Cleanup sound bank memory
		if (soundBankData) {
			delete[] soundBankData;
			soundBankData = nullptr;
		}
	}
}

/*
	Handle any notifications outside of the callback function and call DoWork()
*/
void AudioManager::update() {
	// Enter the critical section before touching any AudioManager data
	EnterCriticalSection(&cs);
	// get copies of some AudioManager data
	bool prepared = streamingBankPrepared;
	bool stopped = songStopped;
	LeaveCriticalSection(&cs);

	// The streaming wave bank is prepared
	if (prepared) {
		EnterCriticalSection(&cs);
		streamingBankPrepared = false;
		LeaveCriticalSection(&cs);

		// Start playing the splash screen music
		soundBank->Play(cues.at(currentSong), 0, 0, nullptr);
	}

	// If the song is stopped, a new one should start
	if (stopped) {
		EnterCriticalSection(&cs);
		songStopped = false;
		LeaveCriticalSection(&cs);

		soundBank->Play(cues.at(currentSong), 0, 0, nullptr);
	}

	// Allow XACT to do work every frame
	if (audioEngine) audioEngine->DoWork();
}