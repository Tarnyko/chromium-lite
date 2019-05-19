//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ ... -DUSE_AURA -DUSE_OZONE/-DUSE_X11 -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/liburl.so /usr/lib/chromium/lib/libcrypto.so /usr/lib/chromium/lib/libgpu_command_buffer.so /usr/lib/chromium/lib/libui_gfx.so /usr/lib/chromium/lib/libui_events.so /usr/lib/chromium/lib/libmedia.so

#include <iostream>					// for << >>

#include "media/audio/audio_manager.h"			// for media::ScopedAudioManagerPtr
#include "media/audio/alsa/audio_manager_alsa.h"	// for media::AudioManagerAlsa
#include "media/audio/audio_device_description.h"	// for media::AudioDeviceDescriptions


 // we create an audio configuration 

int main (int argc, char *argv[])
{
	media::ScopedAudioManagerPtr audio_manager;
	audio_manager.reset ();
	//audio_manager = media::TestAudioManagerFactory<AudioManagerAlsa>::Create(&fake_audio_log_factory_);

	if (!audio_manager->HasAudioOutputDevices ()) {
		std::cout << "No audio output devices ! Exiting..." << std::endl;
		return -1;
	}

	 // Enumerate output devices

	media::AudioDeviceDescriptions descriptions;
	audio_manager->GetAudioOutputDeviceDescriptions (&descriptions);

	for (const auto& description : descriptions)
		std::cout << "Audio device : " << description.device_name << std::endl; 

	return 0;
}

