#include "SoundManager.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

SoundManager& SoundManager::getInstance() {
	static SoundManager instance;
	return instance;
}

void SoundManager::loadSound(const std::string& name, const std::string& filename) {
	sf::SoundBuffer buffer;
	if (buffer.loadFromFile(filename)) {
		soundBuffers[name] = buffer;
		sounds[name].setBuffer(soundBuffers[name]);
		std::cout << "Loaded sound: " << filename << std::endl; // Debug statement
	}
	else {
		std::cerr << "Failed to load sound: " << filename << std::endl;
	}
}

void SoundManager::playSound(const std::string& name) {
	if (sounds.find(name) != sounds.end()) {
		if (sounds[name].getBuffer() != nullptr) {
			if (randomizedPitchSounds.find(name) != randomizedPitchSounds.end()) {
				float pitch = static_cast<float>(std::rand()) / RAND_MAX * 0.4f + 0.8f; // Random pitch between 0.8 and 1.2
				sounds[name].setPitch(pitch);
			}
			else {
				sounds[name].setPitch(1.0f); // Reset to default pitch
			}
			sounds[name].play();
			std::cout << "Playing sound: " << name << std::endl; // Debug statement
		}
		else {
			std::cerr << "Sound buffer not set for: " << name << std::endl;
		}
	}
	else {
		std::cerr << "Sound not found: " << name << std::endl;
	}
}

void SoundManager::loadMusic(const std::string& name, const std::string& filename) {
	if (!music[name].openFromFile(filename)) {
		std::cerr << "Failed to load music: " << filename << std::endl;
	}
	else {
		std::cout << "Loaded music: " << filename << std::endl; // Debug statement
	}
}

void SoundManager::playMusic(const std::string& name, bool loop) {
	if (music.find(name) != music.end()) {
		float offset = lastPlayingOffsets.count(name) ? lastPlayingOffsets[name] : 0.0f;

		// Stop any existing playback before applying the offset
		music[name].stop();
		music[name].setLoop(loop);
		music[name].play();
		std::cout << "Setting playing offset for " << name << " to " << offset << " seconds." << std::endl; // Debug
		music[name].setPlayingOffset(sf::seconds(offset));

		currentMusic = name;
		std::cout << "Playing music: " << name << " from offset: " << offset << std::endl; // Debug
	}
	else {
		std::cerr << "Music not found: " << name << std::endl;
	}
}

void SoundManager::stopMusic() {
	for (auto& m : music) {
		m.second.stop();
	}
	currentMusic.clear(); // Clear current music state
}

void SoundManager::setRandomizedPitch(const std::string& name, bool randomized) {
	if (randomized) {
		randomizedPitchSounds.insert(name);
	}
	else {
		randomizedPitchSounds.erase(name);
	}
}

void SoundManager::crossfadeMusic(const std::string& from, const std::string& to, float duration) {
	if (music.find(from) != music.end() && music.find(to) != music.end()) {
		// Store the playback position of the 'from' track
		lastPlayingOffsets[from] = music[from].getPlayingOffset().asSeconds();
		std::cout << "Stored playback position for " << from << ": " << lastPlayingOffsets[from] << " seconds." << std::endl; // Debug

		// Stop 'from' track and prepare 'to' track
		crossfadeFrom = from;
		crossfadeTo = to;
		crossfadeDuration = duration;
		isCrossfading = true;
		crossfadeClock.restart();

		music[to].setVolume(0);

		// Resume the 'to' track from its stored position, if available
		float toOffset = lastPlayingOffsets.count(to) ? lastPlayingOffsets[to] : 0.0f;
		music[to].stop(); // Stop any current playback to ensure offset takes effect
		music[to].play();
		std::cout << "Setting playing offset for " << to << " to " << toOffset << " seconds." << std::endl; // Debug
		music[to].setPlayingOffset(sf::seconds(toOffset));

		currentMusic = to;
		std::cout << "Crossfading to music: " << to << " from offset: " << toOffset << std::endl; // Debug
	}
}

void SoundManager::updateCrossfade() {
	if (isCrossfading) {
		float elapsed = crossfadeClock.getElapsedTime().asSeconds();
		if (elapsed < crossfadeDuration) {
			float volume = 100 * (elapsed / crossfadeDuration);
			float fromVolume = musicVolumes[crossfadeFrom];
			float toVolume = musicVolumes[crossfadeTo];
			music[crossfadeFrom].setVolume(fromVolume * (1.0f - (elapsed / crossfadeDuration)));
			music[crossfadeTo].setVolume(toVolume * (elapsed / crossfadeDuration));
		}
		else {
			music[crossfadeFrom].stop();
			music[crossfadeTo].setVolume(musicVolumes[crossfadeTo]);
			isCrossfading = false;
		}
	}
}

void SoundManager::setSoundVolume(const std::string& name, float volume) {
	if (sounds.find(name) != sounds.end()) {
		sounds[name].setVolume(volume);
		std::cout << "Set volume for sound: " << name << " to " << volume << std::endl; // Debug statement
	}
	else {
		std::cerr << "Sound not found: " << name << std::endl;
	}
}

void SoundManager::setMusicVolume(const std::string& name, float volume) {
	if (music.find(name) != music.end()) {
		music[name].setVolume(volume);
		musicVolumes[name] = volume; // Store the target volume
		std::cout << "Set volume for music: " << name << " to " << volume << std::endl; // Debug statement
	}
	else {
		std::cerr << "Music not found: " << name << std::endl;
	}
}

void SoundManager::setSoundPitch(const std::string& name, float pitch) {
	if (sounds.find(name) != sounds.end()) {
		sounds[name].setPitch(pitch);
	}
	else {
		std::cerr << "Sound not found: " << name << std::endl;
	}
}

float SoundManager::getSoundDuration(const std::string& name) const {
	auto it = soundBuffers.find(name);
	if (it != soundBuffers.end()) {
		return it->second.getDuration().asSeconds();
	}
	else {
		std::cerr << "Sound not found: " << name << std::endl;
		return 0.0f;
	}
}