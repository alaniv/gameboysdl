#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define MUS_PATH "Roland-GR-1-Trumpet-C5.wav"

/*
** PLAYING A SOUND IS MUCH MORE COMPLICATED THAN IT SHOULD BE
*/
int main(int argc, char* argv[]){

	// Initialize SDL.
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
			return 1;
    
  SDL_AudioSpec wavSpec;
  Uint32 wavLength;
  Uint8 *wavBuffer;
  SDL_LoadWAV(MUS_PATH, &wavSpec, &wavBuffer, &wavLength);
  
  SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_PauseAudioDevice(deviceId, 0);
  SDL_Delay(3000);
	
  SDL_CloseAudioDevice(deviceId);
	SDL_FreeWAV(wavBuffer);
  SDL_Quit();
  
  return 0;
}