#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>

class SoundManager {
public:
    static SoundManager& getInstance();

    void loadSound(const std::string& name, const std::string& filename);
    void playSound(const std::string& name);
    void loadMusic(const std::string& name, const std::string& filename);
    void playMusic(const std::string& name, bool loop = true);
    void stopMusic();
    void setRandomizedPitch(const std::string& name, bool randomized);
    void crossfadeMusic(const std::string& from, const std::string& to, float duration);
    void updateCrossfade();

    void setMusicVolume(const std::string& name, float volume);
    void setSoundVolume(const std::string& name, float volume);

private:
    SoundManager() = default;
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    std::unordered_map<std::string, sf::Sound> sounds;
    std::unordered_map<std::string, sf::Music> music;
    std::unordered_set<std::string> randomizedPitchSounds;
    std::unordered_map<std::string, float> musicVolumes; // Store target volume levels

    sf::Clock crossfadeClock;
    std::string crossfadeFrom;
    std::string crossfadeTo;

    float crossfadeDuration;
    bool isCrossfading = false;
    std::string currentMusic; // Track current music state

    std::unordered_map<std::string, float> lastPlayingOffsets;
};

#endif // SOUNDMANAGER_H

