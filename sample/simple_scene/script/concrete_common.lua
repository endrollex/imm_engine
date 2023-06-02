-- concrete_common.lua
-- read common information to immature engine

-- require
require "script\\library\\csv_dump"

-- csv
csv_audio_bgm    = read_csv("script\\scene_common\\audio_bgm.csv")
csv_audio_effect = read_csv("script\\scene_common\\audio_effect.csv")
csv_hit_box      = read_csv("script\\scene_common\\hit_box.csv")
csv_bound_offset = read_csv("script\\scene_common\\bound_offset.csv")
csv_avatar       = read_csv("script\\scene_common\\avatar.csv")
csv_weapon_map   = read_csv("script\\scene_common\\weapon_map.csv")
csv_npc_skill    = read_csv("script\\scene_common\\npc_skill.csv")
csv_status_data  = read_csv("script\\scene_common\\status_data.csv")
csv_scene_info1  = read_csv("script\\scene_common\\scene_info1.csv")
csv_npc_ability  = read_csv("script\\scene_common\\npc_ability.csv")
csv_ex_texutre   = read_csv("script\\scene_common\\ex_texutre.csv")
