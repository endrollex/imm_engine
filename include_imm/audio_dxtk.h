////////////////
// audio_dxtk.h
// by Huang Yiting
////////////////
////////////////
#ifndef AUDIO_DXTK_H
#define AUDIO_DXTK_H
#include "stru_load_help.h"
#include <Audio.h>
////////////////
// audio_play_bgm
////////////////
////////////////
struct audio_play_bgm
{
	audio_play_bgm();
	std::string current;
	bool is_executed;
	bool is_loop;
};
//
audio_play_bgm::audio_play_bgm():
	current(),
	is_executed(true),
	is_loop(false)
{
	;	
}
////////////////
// audio_dxtk
////////////////
////////////////
namespace imm
{
struct audio_dxtk
{
	audio_dxtk();
	void init_load();
	void update();
	void play_bgm(const std::string &name, const bool &is_loop);
	void play_bgm_update();
	void stop_bgm();
	void play_effect(const std::string &name);
	void set_effect_inst_volume(float volume);
	void set_wave_bank_volume(float volume);
	void suspend(const bool &is_stop);
	float wave_bank_volume;
	std::map<std::string, std::wstring> map_bgm;
	std::map<std::string, std::string> map_effect_bank;
	std::map<std::string, int> map_effect_ix;
	std::map<std::string, std::unique_ptr<WaveBank>> map_wave_bank;
	std::map<std::string, std::unique_ptr<SoundEffect>> map_sound_effect;
	std::map<std::string, std::unique_ptr<SoundEffectInstance>> map_effect_inst;
	std::unique_ptr<AudioEngine> aud_engine;
	audio_play_bgm bgm_task;
};
//
audio_dxtk::audio_dxtk():
	wave_bank_volume(0.7f),
	aud_engine(nullptr)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif
	aud_engine = std::unique_ptr<AudioEngine>(new AudioEngine(eflags));
}
//
void audio_dxtk::init_load()
{
	std::string concrete = IMM_PATH["script"]+"concrete_common.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::vector<std::vector<std::string>> vec2d_audio_bgm;
	l_reader.vec2d_str_from_table("csv_audio_bgm", vec2d_audio_bgm);
	std::wstring path_med(txtutil::str_to_wstr(IMM_PATH["media"]));
	for (size_t ix = 1; ix < vec2d_audio_bgm.size(); ++ix) {
		map_bgm[vec2d_audio_bgm[ix][0]] = path_med+txtutil::str_to_wstr(vec2d_audio_bgm[ix][1]);
	}
	std::vector<std::vector<std::string>> vec2d_audio_effect;
	l_reader.vec2d_str_from_table("csv_audio_effect", vec2d_audio_effect);
	for (size_t ix = 1; ix < vec2d_audio_effect.size(); ++ix) {
		map_effect_bank[vec2d_audio_effect[ix][0]] = vec2d_audio_effect[ix][1];
		map_effect_ix[vec2d_audio_effect[ix][0]] = std::stoi(vec2d_audio_effect[ix][2]);
		if (!map_wave_bank.count(vec2d_audio_effect[ix][1])) {
			std::wstring path_wave_bank = path_med+txtutil::str_to_wstr(vec2d_audio_effect[ix][1]);
			data_check_file_exist(path_wave_bank);
			map_wave_bank[vec2d_audio_effect[ix][1]] =
				 std::unique_ptr<WaveBank>(new WaveBank(aud_engine.get(), path_wave_bank.c_str()));
		}
	}
}
//
void audio_dxtk::update()
{
	aud_engine->Update();
	play_bgm_update();
}
//
void audio_dxtk::play_bgm(const std::string &name, const bool &is_loop = false)
{
	if (!map_bgm.count(name)) {
		std::string err_str("Audio bgm file not found: ");
		err_str += name;
		ERROR_MESA(err_str.c_str());
	}
	if (!map_sound_effect.count(name)) {
		data_check_file_exist(map_bgm[name]);
		map_sound_effect[name] = std::unique_ptr<SoundEffect>(new SoundEffect(aud_engine.get(), map_bgm[name].c_str()));
		map_effect_inst[name] = map_sound_effect[name]->CreateInstance();
	}
	bgm_task.current = name;
	bgm_task.is_executed = false;
	bgm_task.is_loop = is_loop;
}
//
void audio_dxtk::play_bgm_update()
{
	if (bgm_task.is_executed) return;
	map_effect_inst[bgm_task.current]->Play(bgm_task.is_loop);
	bgm_task.is_executed = true;
}
//
void audio_dxtk::stop_bgm()
{
	if (!map_effect_inst.count(bgm_task.current)) return;
	map_effect_inst[bgm_task.current]->Stop(true);
}
//
void audio_dxtk::play_effect(const std::string &name)
{
	if (name == sfx::Empty) return;
	assert(map_effect_bank.count(name));
	map_wave_bank[map_effect_bank[name]]->Play(map_effect_ix[name], wave_bank_volume, 0.0f, 0.0f);
}
//
void audio_dxtk::set_effect_inst_volume(float volume)
{
	if (!map_effect_inst.count(bgm_task.current)) return;
	map_effect_inst[bgm_task.current]->SetVolume(volume);
}
//
void audio_dxtk::set_wave_bank_volume(float volume)
{
	wave_bank_volume = volume;
}
//
void audio_dxtk::suspend(const bool &is_stop)
{
	if (is_stop) aud_engine->Suspend();
	else aud_engine->Resume();
}
//
}
#endif