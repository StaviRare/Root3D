#pragma once

class Core 
{
	public:
		static void Initialize();
		static void UnInitialize();
		static void Pause();
		static void Resume();
		static void Tick();
};