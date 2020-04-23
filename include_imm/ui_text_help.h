////////////////
// ui_text_help.h
// by Huang Yiting
////////////////
////////////////
#ifndef UI_TEXT_HELP
#define UI_TEXT_HELP
#include "stru_lua_help.h"
namespace imm
{
////////////////
// http://stackoverflow.com/questions/236129/split-a-string-in-c
////////////////
////////////////
void text_split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}
////////////////
// ui_text_lines
////////////////
////////////////
struct ui_text_lines
{
	ui_text_lines();
	std::map<std::string, std::string> speaker;
	std::map<std::string, std::vector<std::wstring>> lines;
};
//
ui_text_lines::ui_text_lines()
{
	;
}
////////////////
// ui_text_chunk
////////////////
////////////////
struct ui_text_chunk
{
	ui_text_chunk();
	void remove_all();
	void reset();
	void insert_chunk(
		const std::string &chunk_name,
		const std::map<std::string, std::string> &chunk_raw);
	bool get_lines(
		const std::string &chunk_name,
		std::string &speaker,
		std::wstring &out_wstr);
	std::map<std::string, ui_text_lines> map;
	std::map<std::string, std::vector<std::wstring>>::iterator it_dialogue;
	bool it_dialogue_is_null;
	size_t dialogue_ix;
};
//
ui_text_chunk::ui_text_chunk():
	it_dialogue_is_null(true),
	dialogue_ix(0)
{
	;
}
//
void ui_text_chunk::remove_all()
{
	map.clear();
	reset();
}
//
void ui_text_chunk::reset()
{
	it_dialogue_is_null = true;
	dialogue_ix = 0;
}
//
void ui_text_chunk::insert_chunk(
	const std::string &chunk_name,
	const std::map<std::string, std::string> &chunk_raw)
{
	map.emplace(chunk_name, ui_text_lines());
	for (auto &raw: chunk_raw) {
		std::vector<std::string> elems;
		text_split(raw.second, '\n', elems);
		size_t found = raw.first.find_first_of("_");
		map[chunk_name].speaker[raw.first] = raw.first.substr(found+1);
		for (size_t ix = 0; ix < elems.size(); ix += 3) {
			if (ix > elems.size()-1) break;
			std::string lines;
			size_t row = 0;
			while (row < 3) {
				lines += elems[ix+row];
				row += 1;
				if (ix+row > elems.size()-1) break;
				if (row < 3) lines += '\n';
			}
			map[chunk_name].lines[raw.first].push_back(txtutil::str_to_wstr(lines));
		}
	}
}
//
bool ui_text_chunk::get_lines(
	const std::string &chunk_name,
	std::string &speaker,
	std::wstring &out_wstr)
{
	size_t *ix = &dialogue_ix;
	if (it_dialogue_is_null) {
		it_dialogue = map[chunk_name].lines.begin();
		it_dialogue_is_null = false;
	}
	if (it_dialogue == map[chunk_name].lines.end()) return false;
	assert(*ix < it_dialogue->second.size());
	out_wstr = it_dialogue->second[*ix];
	speaker = map[chunk_name].speaker[it_dialogue->first];
	++(*ix);
	if (*ix >= it_dialogue->second.size()) {
		*ix = 0;
		++it_dialogue;
	}
	return true;
}
//
}
#endif