//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ ... -DUSE_AURA -DUSE_OZONE/-DUSE_X11 -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/liburl.so /usr/lib/chromium/lib/libcrypto.so /usr/lib/chromium/lib/libgpu_command_buffer.so /usr/lib/chromium/lib/libui_gfx.so /usr/lib/chromium/lib/libui_events.so /usr/lib/chromium/lib/libmedia.so

#include <iostream>				// for << >>

#include "media/base/audio_hardware_config.h"	//  for media::AudioHardwareConfig
#include "media/audio/audio_parameters.h"	//  for media::AudioParameters


 // we create an audio configuration 

int main (int argc, char *argv[])
{
	 // Parameters : type, channels, sample rate, bpp, buffer size

	media::AudioParameters input_params (media::AudioParameters::AUDIO_PCM_LOW_LATENCY, media::CHANNEL_LAYOUT_MONO, 44100, 16, 2048);
	media::AudioParameters output_params (media::AudioParameters::AUDIO_PCM_LOW_LATENCY, media::CHANNEL_LAYOUT_STEREO, 48000, 16, 2048);

	media::AudioHardwareConfig config (input_params, output_params);

	std::cout << "Output sample rate : " << config.GetOutputSampleRate () << std::endl;

	return 0;
}

