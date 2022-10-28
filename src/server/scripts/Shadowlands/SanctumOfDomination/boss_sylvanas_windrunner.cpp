/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "DB2Stores.h"
#include "CellImpl.h"
#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "G3DPosition.hpp"
#include <G3D/Box.h>
#include <G3D/CoordinateFrame.h>
#include "InstanceScript.h"
#include "Map.h"
#include "MotionMaster.h"
#include "MovementTypedefs.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellAuras.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "TemporarySummon.h"
#include "UpdateFields.h"
#include "G3D/Vector2.h"
#include "sanctum_of_domination.h"
#include "Unit.h"



enum Spells
{

    // Stances
    SPELL_RANGER_BOW_STANCE = 347560,
    SPELL_RANGER_DAGGERS_STANCE = 348010,

    // Ranger Shot
    SPELL_RANGER_SHOT = 347548,

    // Ranger Strike
    SPELL_RANGER_STRIKE = 348299,

    // Windrunner
    SPELL_WINDRUNNER = 347504,
    SPELL_WINDRUNNER_DISAPPEAR_01 = 352303, // 2 SpellXSpellVisual
    SPELL_WINDRUNNER_DISAPPEAR_02 = 358975, // 2 SpellXSpellVisual
    SPELL_WINDRUNNER_SPIN = 351187,
    SPELL_WINDRUNNER_MOVE = 347606,
    SPELL_WINDRUNNER_SHOT_01 = 347604, // Not on the sniff
    SPELL_WINDRUNNER_JUMP_02 = 347605, // Not on the sniff
    SPELL_WINDRUNNER_DAGGER_01 = 350737, // Not on the sniff
    SPELL_WINDRUNNER_BANSHEE = 352211,

    // Shadow Dagger
    SPELL_SHADOW_DAGGER_COPY = 358964,
    SPELL_SHADOW_DAGGER = 347670,
    SPELL_SHADOW_DAGGER_AOE = 353935, // 2 SpellXSpellVisual
    SPELL_SHADOW_DAGGER_MISSILE = 348089, // Phase 2
    SPELL_SHADOW_DAGGER_PHASE_TWO_AND_THREE = 353935,

    // Withering Fire
    SPELL_WITHERING_FIRE_COPY = 358981,
    SPELL_WITHERING_FIRE = 347928,
    SPELL_BARBED_ARROW = 347807,

    // Desecrating Shot
    SPELL_DESECRATING_SHOT_AREATRIGGER = 348626,
    SPELL_DESECRATING_SHOT = 350652,
    SPELL_DESECRATING_SHOT_JUMP_FRONT = 347957,
    SPELL_DESECRATING_SHOT_JUMP_LEFT = 356191,
    SPELL_DESECRATING_SHOT_JUMP_STRAIGHT_ARROW = 358993,
    SPELL_DESECRATING_SHOT_TRIGGERED = 348627,

    // Ranger Heartseeker
    SPELL_RANGER_HEARTSEEKER_AURA = 352649,
    SPELL_RANGER_HEARTSEEKER_CHARGE = 352650,
    SPELL_RANGER_HEARTSEEKER = 352663,
    SPELL_RANGER_HEARTSEEKER_PHYSICAL_DAMAGE = 352651,
    SPELL_RANGER_HEARTSEEKER_SHADOW_DAMAGE = 352652,
    SPELL_BANSHEE_MARK = 347607,

    // Banshee Heartseeker
    SPELL_BANSHEE_HEARTSEEKER_AURA = 353966,
    SPELL_BANSHEE_HEARTSEEKER_CHARGE = 353965,
    SPELL_BANSHEE_HEARTSEEKER_PHYSICAL_DAMAGE = 353968,
    SPELL_BANSHEE_HEARTSEEKER_SHADOW_DAMAGE = 353967,
    SPELL_BANSHEE_HEARTSEEKER = 353969,

    // Banshee Blades
    SPELL_BANSHEE_BLADES_AURA = 358184,
    SPELL_BANSHEE_BLADES_CHARGE = 358185,
    SPELL_BANSHEE_BLADES = 358181,
    SPELL_BANSHEE_BLADES_PHYSICAL_DAMAGE = 358182,
    SPELL_BANSHEE_BLADES_SHADOW_DAMAGE = 358183,

    // Wailing Arrow
    SPELL_WAILING_ARROW_POINTER = 348064,
    SPELL_WAILING_ARROW = 347609,

    // Domination Chains
    SPELL_DOMINATION_CHAINS_JUMP = 347602,
    SPELL_DOMINATION_CHAINS = 349419,
    SPELL_DOMINATION_ARROW_SHOT_VISUAL = 350426,
    SPELL_DOMINATION_ARROW_FALL = 352317,
    SPELL_DOMINATION_ARROW_FALL_AND_VISUAL = 352319,
    SPELL_DOMINATION_ARROW_ACTIVATE = 356650,
    SPELL_DOMINATION_ARROW_CALAMITY_VISUAL = 356769,
    SPELL_DOMINATION_ARROW_CALAMITY_AREATRIGGER = 356624, // 2 SpellXSpellVisual
    SPELL_DOMINATION_ARROW_CALAMITY_DAMAGE = 356649,

    SPELL_DOMINATION_CHAIN_PLAYER = 349451,
    SPELL_DOMINATION_CHAIN_PERIODIC = 349458,


    // Banshee Shroud
    SPELL_BANSHEE_SHROUD = 350857,

    // Rive
    SPELL_RIVE_DISAPPEAR = 353519,
    SPELL_RIVE = 353417,
    SPELL_RIVE_FAST = 353418,
    SPELL_RIVE_MARKER = 353419,
    SPELL_RIVE_CHAIN = 353504,
    SPELL_RIVE_CHAIN_FAST = 353510,
    SPELL_RIVE_AREATRIGGER = 353375,
    SPELL_RIVE_DAMAGE = 348145,
    SPELL_RIVEN_DEBRIS = 353413,
    SPELL_RIVE_MYTHIC_COPY = 358431,

    // Banshee Wail
    SPELL_BANSHEE_WAIL = 348094, // 3 SpellXSpellVisual
    SPELL_BANSHEE_WAIL_TRIGGERED_MISSILE = 348108,
    SPELL_BANSHEE_WAIL_MISSILE = 348133,
    SPELL_BANSHEE_WAIL_SILENCE = 351253,
    SPELL_BANSHEE_WAIL_MARKER = 357719,
    SPELL_BANSHEE_WAIL_EXPIRE = 355489,

    // Banshee Scream
    SPELL_BANSHEE_SCREAM = 353952,
    SPELL_BANSHEE_SCREAM_SILENCE = 353957,
    SPELL_BANSHEE_SCREAM_MARKER = 357720,
    SPELL_BANSHEE_SCREAM_EXPIRE = 353958,
    SPELL_BANSHEE_SCREAM_MISSILE = 353951,
    SPELL_BANSHEE_SCREAM_INTERRUPT = 353956,
    SPELL_BANSHEE_SCREAM_TRIGGERED_MISSILE = 348108,
    SPELL_BANSHEE_SCREAM_DAMAGE_ADJUSTMENT = 353955,

    SPELL_TELEPORT_TO_PHASE_TWO = 350903,
    SPELL_TELEPORT_NO_IDEA = 350905, // TARGET_DEST_DB, not sniffed

    // Banshee Form
    SPELL_BANSHEE_FORM = 348146,
    SPELL_BANSHEE_READY_STANCE = 357758,

    // Haunting Wave
    SPELL_HAUNTING_WAVE = 352271,
    SPELL_HAUNTING_WAVE_DAMAGE = 351870,
        SPELL_DISABLE_GRAVITY = 355490,
    SPELL_CALL_EARTH_DEBRIS = 355491,

    // Ruin
    SPELL_RUIN = 355540,
    SPELL_RUIN_VISUAL = 358990,
    SPELL_RUIN_DAMAGE = 355545, // 46 same damage 

    // Veil of Darkness (Generic)
    SPELL_VEIL_OF_DARKNESS_DESELECT = 354366,
    SPELL_VEIL_OF_DARKNESS_SCREEN_FOG = 354580,
    SPELL_VEIL_OF_DARKNESS_ABSORB_AURA = 347704,
    SPELL_VEIL_OF_DARKNESS_VISUAL_SPREAD = 355749, // Black circle
    SPELL_VEIL_OF_DARKNESS_POINTER = 357876,
    SPELL_VEIL_OF_DARKNESS_DAMAGE = 350777,

    // Veil of Darkness (Phase 1)
    SPELL_VEIL_OF_DARKNESS_PHASE_1_FADE = 352470,
    SPELL_VEIL_OF_DARKNESS_PHASE_1_GROW = 350335,
     // Also damage
    SPELL_VEIL_OF_DARKNESS_PHASE_1 = 347726, // 4 SpellXSpellVisual


    // Veil of Darkness (Phase 2)
    SPELL_VEIL_OF_DARKNESS_PHASE_2_FADE = 353273, // 107979
    SPELL_VEIL_OF_DARKNESS_PHASE_2_GROW = 352239,
    SPELL_VEIL_OF_DARKNESS_PHASE_2_AREA = 352225, // black smoke
    SPELL_VEIL_OF_DARKNESS_PHASE_2 = 347741,
    SPELL_ABSORB_VEIL_2 = 352206, // i have no clue.

    // Veil of Darkness (Phase 3)
    SPELL_VEIL_OF_DARKNESS_PHASE_3_FADE = 354168,
    SPELL_VEIL_OF_DARKNESS_PHASE_3_GROW = 354143,
    SPELL_VEIL_OF_DARKNESS_PHASE_3 = 354142, // Hit cast
    SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED = 357876,
    SPELL_VEIL_OF_DARKNESS_PHASE_1_AREA = 357726, // damage from debuff
    SPELL_VEIL_OF_DARKNESS_PHASE_THREE_EMPTY = 354141,
    SPELL_VEIL_OF_DARKNESS_P3_ONLY = 347720,


    // Invigorating Field
    SPELL_INVIGORATING_FIELD_ACTIVATE = 353660,
    SPELL_INVIGORATING_FIELD = 354175,
    SPELL_INVIGORATING_FIELD_JUMP = 353642,
    SPELL_INVIGORATING_ZOVAAL_FINAL_SCENE_HIT = 108167,
    SPELL_INVIGORATING_FIELD_VISUAL = 107979,
    SPELL_X_SPELL_VISUAL = 329911,

    // Platform Change
    SPELL_SWITCH_PLATFORM_STOMP = 354141, // 27887 areatrigger forces MovementForce on Players for 150ms

    // Banshee's Bane
    SPELL_BANSHEES_BANE = 353929,
    SPELL_BANSHEES_BANE_AREATRIGGER = 353930,

    // Bane Arrows
    SPELL_BANE_ARROWS = 354011,
    SPELL_BANE_ARROWS_DAMAGE = 353972,

    // Banshee's Blades
    SPELL_BANSHEE_BLADE_PHYSICAL_DAMAGE = 358182,
    SPELL_BANSHEE_BLADE_SHADOW_DAMAGE = 358183,




    //SPELL_VEIL_MOUNT = 353687,

    // Banshee's Fury
    SPELL_BANSHEES_FURY = 354068,
    SPELL_BANSHEES_FURY_EXPLODE = 357526,

    // Raze
    SPELL_RAZE = 354147,
    SPELL_RAZE_PERIODIC = 354146,

    // The Jailer
    SPELL_JAILER_GROW_SOULS = 355347,
    SPELL_JAILER_CAST_SOULS = 355348,

    // Anduin Wrynn
    SPELL_FOCUSING_PRISM_AURA = 358166, // Related to the achievement

    // Miscelanea
    SPELL_PORTAL_BIG_p2 = 354968,
    SPELL_PORTAL_INTERACT = 357098,
        SPELL_APPEARED_PORTAL = 357101,
        SPELL_ABLE_TO_FLY = 357758,
        SPELL_STOP_MOVEMENT = 358550, // during first intermission
        SPELL_DARK_SENTINEL_SPAWN = 358703,


    SPELL_ANCHOR_HERE = 45313,
    SPELL_SYLVANAS_ROOT = 347608,
    SPELL_SYLVANAS_DISPLAY_POWER_SUFFERING = 352311,
    SPELL_DUAL_WIELD = 42459,
    SPELL_SYLVANAS_POWER_ENERGIZE_AURA = 352312,
    SPELL_CHAMPIONS_MOD_FACTION = 355537,

    SPELL_INTERMISSION_STUN = 355488,
            SPELL_SERVERSIDE_STUN = 25900,
    SPELL_INTERMISSION_SCENE = 359062,
    SPELL_PLATFORMS_SCENE = 350943,
    SPELL_FINAL_SCENE = 358806,
    SPELL_FINAL_CINEMATIC = 358985,
    SPELL_SYLVANAS_FEIGN_DEATH = 358844,

    SPELL_HEALTH_PCT_CHECK_INTERMISSION = 358794,
    SPELL_ACTIVATE_INTERMISSION = 359429,
    SPELL_HEALTH_PCT_CHECK_FINISH = 359430,
    SPELL_ACTIVATE_FINISH = 359431,

    // Mawforged Goliath
    SPELL_GOLIATH_FURY = 351672,
    SPELL_GOLIATH_FURY_01 = 350580,
    SPELL_GOLIATH_FURY_02 = 351670,



    // Mawforged Summoner
    SPELL_SUMMON_DECREPIT_ORBS = 351353,
    SPELL_DETONATION = 351317, // aura and cast
    SPELL_DETONATION_ORB_VISUAL = 351327,
    SPELL_DETONATION_DEATH = 351323,
    SPELL_DETONATION_PERIODIC_AOE = 351324,

    // Mawsworn Hopebreaker
        SPELL_ENFLAME = 351109,
        SPELL_DESTABILIZE = 351091,


    // Mawsworn Vanguard

    






};

enum Phases
{
    PHASE_NONE = 0,
    PHASE_ONE = 1,
    PHASE_INTERMISSION = 4,
    PHASE_TWO = 2,
    PHASE_THREE = 3,
    PHASE_INTERMISSION_WORLD_STATE = 11
};

enum Events
{
    EVENT_WINDRUNNER = 1,
    EVENT_EVADE_CHECK,
    EVENT_WITHERING_FIRE,
    EVENT_SHADOW_DAGGERS,
    EVENT_DESECRATING_SHOT,
    EVENT_DESECRATING_SHOT_LAUNCH,
    EVENT_DOMINATION_CHAINS,
    EVENT_DOMINATION_CHAINS_TRANSITION,
    EVENT_WAILING_ARROW,
    EVENT_HAUNTING_WAVE_COMBAT,
    EVENT_CHECK_INTERMISSION,
    EVENT_WAILING_HOWL,
    EVENT_VEIL_OF_DARKNESS,
    EVENT_VEIL_OF_DARKNESS_P2_2,
    EVENT_VEIL_OF_DARKNESS_P2_3,
    EVENT_VEIL_OF_DARKNESS_P2_4,
    EVENT_VEIL_OF_DARKNESS_MAIN,
    EVENT_VEIL_OF_DARKNESS_MAIN2,
    EVENT_VEIL_OF_DARKNESS_MAIN3,
    EVENT_VEIL_OF_DARKNESS_MAIN4,
    EVENT_VEIL_OF_DARKNESS_MAIN5,
    EVENT_RIVE,
    EVENT_FINISH_INTERMISSION,
    EVENT_BANE_ARROWS,
    EVENT_RAZE,
    EVENT_RAZE_02,
    EVENT_RAZE_03,
    EVENT_RAZE_04,
    EVENT_BANSHEE_SCREAM,
    EVENT_BANSHEE_SCREAM_2_2,
    EVENT_BANSHEE_SCREAM_2_3,
    EVENT_BANSHEES_FURY,
    EVENT_BANSHEES_FURY_02,
    EVENT_BANSHEES_FURY_03,
    EVENT_BANSHEES_FURY_04,
    EVENT_BANSHEES_FURY_05,
    EVENT_BANSHEES_FURY_06,
    EVENT_CONTINUE_ATTACK,
    EVENT_TRANSITION_ONE,
    EVENT_TRANSITION_TWO,
    EVENT_TRANSITION_THREE,
};

enum Actions
{
    ACTION_WINDRUNNER_MODEL_ACTIVATE = 1,
    ACTION_WINDRUNNER_MODEL_DEACTIVATE,
    ACTION_START_ENCOUNTER,
    ACTION_JAINA_START_ATTACKING,
    ACTION_RESET_MELEE_KIT,
    ACTION_RANGER_SHOT,
    ACTION_ACTIVATE_DOMINATION_ARROW,
    ACTION_RANGER_HEARTSEEKER,
    ACTION_CALCULATE_ARROWS,
    ACTION_WAILING_ARROW,
    ACTION_PREPARE_INTERMISSION,
    ACTION_CHECK_INTERMISSION,
    ACTION_OPEN_PORTAL_TO_PHASE_TWO,
    ACTION_HAUNTING_WAVE_SECOND_CHAIN,
    ACTION_HAUNTING_WAVE_THIRD_CHAIN,
    ACTION_HAUNTING_WAVE_FOURTH_CHAIN,
    ACTION_HAUNTING_WAVE_FIFTH_CHAIN,
    ACTION_GOLIATH_ALIVE,
    ACTION_GOLIATH_DIED,
    ACTION_SOULJUDGE_DIED,
    ACTION_SUMMONER_DIED,
    ACTION_SECOND_WAVE_VEIL,
    ACTION_RUIN_2,
    ACTION_RUIN_3,
    ACTION_RUIN_4,
    ACTION_WINDS_OF_ICECROWN_PRE,
    ACTION_PREPARE_EARTH_BRIDGE_02,
    ACTION_START_PHASE_TWO_ONE,
    ACTION_PREPARE_JAINA,
    ACTION_JAINA_TWO_ONE,
    ACTION_PREPARE_JAINA_02,
    ACTION_JAINA_TWO_TWO,
    ACTION_START_PHASE_TWO_TWO,
    ACTION_PREPARE_PHASE_THREE,
    ACTION_OPEN_PORTAL_TO_PHASE_THREE,
    ACTION_INITIATE_PHASE_THREE,
    ACTION_START_PHASE_THREE,
    ACTION_ANDUIN_READY,
    ACTION_PREPARE_FINISH_BOSS
};

enum Texts
{
    SAY_AGGRO = 0,
    SAY_SLAY = 1,
    SAY_DISENGAGE = 2,
    SAY_ANNOUNCE_WINDRUNNER = 3,
    SAY_SHADOW_DAGGER = 4,
    SAY_DESECRATING_SHOT = 5,
    SAY_ANNOUNCE_WAILING_ARROW = 6,
    SAY_ANNOUNCE_WAILING_ARROW_AFFECTED = 7,
    SAY_WAILING_ARROW = 8,
    SAY_ANNOUNCE_DOMINATION_CHAINS = 9,
    SAY_DOMINATION_CHAINS = 10,
    SAY_ANNOUNCE_VEIL_OF_DARKNESS = 11,
    SAY_VEIL_OF_DARKNESS_PHASE_ONE = 12,
    SAY_INTERMISSION_BEGIN = 13,
    SAY_ANNOUNCE_BANSHEE_WAIL = 14,
    SAY_INTERMISSION_END = 15,
    SAY_ANNOUNCE_HAUNTING_WAVE = 16,
    SAY_ANNOUNCE_RUIN = 17,
    SAY_WINDS_OF_ICECROWN_AFTER_01 = 18,
    SAY_VEIL_OF_DARKNESS_PHASE_TWO_AND_THREE = 19,
    SAY_ANNOUNCE_BANSHEE_SCREAM = 20,
    SAY_BANSHEE_SCREAM = 21,
    SAY_WINDS_OF_ICECROWN_AFTER_02 = 22,
    SAY_START_PHASE_THREE = 23,
    SAY_BANSHEES_FURY = 24,
    SAY_ANNOUNCE_RAZE = 25,
    SAY_BANSHEES_SCREAM = 27,
    SAY_SLAY02 = 28,
    SAY_VEIL_OF_DARKNESS_PHASE_THREE_ONLY = 29,
    SAY_CHAMPIONS_SCOUT_AHEAD = 31,
    SAY_YOUR_CRUSADE_ENDS_HERE = 32,
    SAY_ANNOUNCE_BANSHEE_WAIL_P2 = 33,
    SAY_VEIL_OF_DARKNESS_PHASE_TWO_ONLY = 34,


};

enum Conversations
{
    CONVERSATION_INTRO = 17368
};

enum SpawnGroups
{
    SPAWN_GROUP_INITIAL = 0
};

enum Points
{
    POINT_INTRO_01 = 1,
    POINT_INTRO_02 = 2,
    POINT_INTRO_03 = 3,
    POINT_INTRO_FINISHED = 4
};

enum SpellVisualKits
{
    SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_01 = 150067,
    SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_02 = 150068,
    SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_03 = 150069,
    SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_04 = 150071,
    SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_05 = 150072,
    SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_06 = 150070,
    SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_07 = 150074,
    SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_08 = 150077,
    SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_09 = 150076,
    SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_10 = 150075,
    SPELL_VISUAL_KIT_BOLVAR_INTRODUCTION_TALK_01 = 150073,

    SPELL_VISUAL_KIT_SYLVANAS_NORMAL_CAST = 142405,
    SPELL_VISUAL_KIT_SYLVANAS_UNSHEATHE_BOW = 143939,
    SPELL_VISUAL_KIT_SYLVANAS_UNSHEATHE_BOW_SPIN = 142389,
    SPELL_VISUAL_KIT_SYLVANAS_UNSHEATHE_DAGGERS = 143940,
    SPELL_VISUAL_KIT_SYLVANAS_UNSHEATHE_DAGGERS_SPIN = 142388, // prolly the best option for p2 shadow daggers
    SPELL_VISUAL_KIT_SYLVANAS_WAILING_ARROW_CHARGE = 142447,
    SPELL_VISUAL_KIT_SYLVANAS_WAILING_ARROW_JUMP = 145145,
    SPELL_VISUAL_KIT_SYLVANAS_WAILING_ARROW_EFFECT = 146199, // Make her play it on p3 as nameplate jumps
    SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS = 142021, // animation for summoning souljudge
    SPELL_VISUAL_KIT_SYLVANAS_RIVE_BREAK = 145541,
    SPELL_VISUAL_KIT_SYLVANAS_RIVE_BREAK_FAST = 145608,
    SPELL_VISUAL_KIT_SYLVANAS_TRANSFORM_INTO_BANSHEE = 146063,
    SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_SCREAM_EXPIRE = 148298, // Cast on all players when banshee screams ends
    SPELL_VISUAL_KIT_SYLVANAS_WINDS_01 = 149160,
    SPELL_VISUAL_KIT_SYLVANAS_RUIN_INTERRUPTED = 145486,
    SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_TELEPORT = 148270,
    SPELL_VISUAL_KIT_SYLVANAS_TRANSFORM_INTO_HUMAN = 146626,
    SPELL_VISUAL_KIT_SYLVANAS_SHADOW_DAGGER = 145482, // combine with below
    SPELL_VISUAL_KIT_SYLVANAS_SHADOW_DAGGER_SOUND = 145850, // legit just sound
    SPELL_VISUAL_KIT_SYLVANAS_WITHERING_FIRE_PHASE_TWO = 149795,
    SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_VEIL_OF_DARKNESS21 = 144927, // Veil for p2 shadow casts on top
    SPELL_VISUAL_KIT_SYLVANAS_WINDS_02 = 149161,
    SPELL_VISUAL_KIT_SYLVANAS_DARK_FOG = 150066,
    SPELL_VISUAL_KIT_SYLVANAS_STOMP = 148200,
    SPELL_VISUAL_KIT_SYLVANAS_SWITCH_PLATFORM = 149580, // cast on the center
    SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_SCREAM = 146059, // DEFINITELY PHASE 3
    SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_BANE_ARROWS = 145872,
    SPELL_VISUAL_KIT_SYLVANAS_BLACKEN_WITH_DAGGERS = 149487, // NOT SNIFFED
    SPELL_VISUAL_KIT_SYLVANAS_DISAPPEAR_MODEL = 149491, // NOT SNIFFED
    SPELL_VISUAL_KIT_SYLVANAS_TELEPORT = 150078, // Can use on raze cast finished
    SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01 = 148191,
    SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02 = 148193,
    SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_03 = 148192,
    SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_FURY_END = 148130,

    SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_VEIL_OF_DARKNESS2 = 144927, // Same as 17 Shadow above that is casting the veil

    SPELL_VISUAL_KIT_JAINA_KNEEL_THEN_STAND = 146285,
    SPELL_VISUAL_KIT_JAINA_FROZEN_BRIDGE = 142472, // NOT ON SNIFF

    SPELL_VISUAL_KIT_THRALL_KNEEL_THEN_STAND = 147632,
    SPELL_VISUAL_KIT_THRALL_EARTHEN_BRIDGE = 142473,

    SPELL_VISUAL_KIT_ANDUIN_FINISH_BLASPHEMY = 148315,

    SPELL_VISUAL_KIT_JAILER_IDK_01 = 149329, // Finishing blast to Arbiter

    SPELL_VISUAL_KIT_PLAYER_ACTIVATE_FIELDS = 144754,

    SPELL_VISUAL_KIT_GENERIC_TELEPORT = 147629,
    SPELL_VISUAL_KIT_GENERIC_CHEERS = 147631,
};

enum SpellVisuals
{
    SPELL_VISUAL_UNK = 106112,
    SPELL_VISUAL_RANGER_STRIKE_RIGHT = 106153, // At 1.0f
    SPELL_VISUAL_RANGER_STRIKE_LEFT = 106160, // At 1.0f
    SPELL_VISUAL_RANGER_STRIKE_01 = 106165, // At 1.0f
    SPELL_VISUAL_RANGER_STRIKE_03 = 106161, // At 1.0f
    SPELL_VISUAL_WINDRUNNER_SPIN = 105759,
    SPELL_VISUAL_WINDRUNNER_01 = 107722, // At 0.5f
    SPELL_VISUAL_WINDRUNNER_02 = 110312, // At 0.25f
    SPELL_VISUAL_WINDRUNNER_03 = 107920, // At 0.25f
    SPELL_VISUAL_DESECRATING_ARROW = 106363, // At 0.40f - 0.12f
    SPELL_VISUAL_WITHERING_FIRE_PHASE_ONE = 105944, // At 48.0f, false as time
    SPELL_VISUAL_HEARTSEEKER = 107607, // At 36f, false as time
    SPELL_VISUAL_DOMINATION_ARROW = 107501, // At 2.5f
    SPELL_VISUAL_DOMINATION_ARROW_SPAWN = 109620, // At 2.5f
    SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_01_NM = 109159, // At 5.0f
    SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_01_HC = 107567, // At 5.0f
    SPELL_VISUAL_TORGHAST_SPIRE_BREAK = 108070,
    SPELL_VISUAL_TORGHAST_SPIRE_BREAK_FAST = 108071,
    SPELL_VISUAL_RIVEN_DEBRIS = 107877, // At 1.744643688201904296f - 1.616904616355895996f, dynamic
    SPELL_VISUAL_WITHERING_FIRE_PHASE_TWO = 110435, // At 3.5f - 2.7f, dynamic
    SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_02_03 = 109371, // At 3.0f
    SPELL_VISUAL_BANE_ARROW = 108112, // At 3.2f - 2.04f, dynamic
    SPELL_VISUAL_BANSHEE_FURY = 108130, // At 0.100000001490116119f
    SPELL_VISUAL_RAZE = 110336, // At 0.100000001490116119f
    SPELL_VISUAL_RAZE_MISSILE = 108154, // At 0.33f - 0.14f
    SPELL_VISUAL_BLASPHEMY = 109680, // At 1.0f
    SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_3_HC = 105852, // At 3.0f
    SPELL_VISUAL_JAILER_BOLT = 107337, // At 90.0f, false as time
    SPELL_VISUAL_BANSHEES_BANE_ABSORB = 108093, // At 0.5f
    SPELL_VISUAL_BANSHEES_BANE_DROP = 107839, // At 0.349999994039535522f

    SPELL_VISUAL_UNK_02 = 108094, // At 1.5f // Shadow Daggers P3 Visual line
    SPELL_VISUAL_SHADOW_DAGGER = 108096, // At 100.0f, false PHASE_2 Dagger
    SPELL_VISUAL_MOST_LIKELY_SPREAD_OUT_FOG = 108092, // At 0.64f - 0.52f Could be windrunner
    SPELL_VISUAL_UNK01_177054 = 107069, // At 1.0f
    SPELL_VISUAL_BANSHEE_FURY_IDK = 107476, // At 1.5f
    SPELL_VISUAL_UNK01_177787 = 107063, // At 1.0f // bolts to the arbiter
};

enum WorldStates
{
    //WORLD_STATE_SYLVANAS_UNK_01 = 20346,
    //WORLD_STATE_SYLVANAS_ENCOUNTER_PHASE = 20348
};

enum MiscData
{
    MUSIC_SANCTUM_OF_DOMINATION = 184695,
    MUSIC_ZOVAAL_FINAL_PHASE = 185404,
    MUSIC_SYLVANAS_FREE_WILL = 176124,
    MUSIC_THE_CRUCIBLE = 176359,
    SOUND_WINDRUNNER = 184496,
    SOUND_WAILING_ARROW = 183953,
    SOUND_BOLVAR_MUSIC = 147952,
};

enum Miscellanea
{
    DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL = 101311,
    DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL = 100930,

    DATA_MELEE_COMBO_SWITCH_TO_MELEE = 0,
    DATA_MELEE_COMBO_RANGER_STRIKE_01 = 1,
    DATA_MELEE_COMBO_RANGER_STRIKE_02 = 2,
    DATA_MELEE_COMBO_SWITCH_TO_RANGED = 3,
    DATA_MELEE_COMBO_FINISH = 4,

    DATA_CHANGE_SHEATHE_UNARMED = 0,
    DATA_CHANGE_SHEATHE_DAGGERS = 1,
    DATA_CHANGE_SHEATHE_BOW = 2,
    DATA_CHANGE_NAMEPLATE_TO_COPY = 3,
    DATA_CHANGE_NAMEPLATE_TO_SYLVANAS = 4,

    DATA_EVENT_TYPE_SHADOWCOPY = 1,
    DATA_EVENT_COPY_NO_EVENT = 1,
    DATA_EVENT_COPY_DOMINATION_CHAIN_EVENT = 2,
    DATA_EVENT_COPY_RIVE_EVENT = 3,
    DATA_EVENT_COPY_FINISH_INTERMISSION_EVENT = 4,
    DATA_EVENT_COPY_DOMINATION_CHAIN_BEFORE_RIVE_EVENT = 5,

    DATA_AREATRIGGER_DOMINATION_ARROW = 27683,
    DATA_AREATRIGGER_RIVE_MARKER = 6197,
    DATA_AREATRIGGER_SHADOW_DAGGER_P3 = 179262,

    DATA_SPLINEPOINT_RIVE_MARKER_DISAPPEAR = 1,

    DATA_DESECRATING_SHOT_PATTERN_STRAIGHT = 1,
    DATA_DESECRATING_SHOT_PATTERN_SCATTERED = 2,
    DATA_DESECRATING_SHOT_PATTERN_WAVE = 3,
    DATA_DESECRATING_SHOT_PATTERN_SPIRAL = 4,
    DATA_DESECRATING_SHOT_PATTERN_JAR = 5,
    DATA_DESECRATING_SHOT_PATTERN_SPIRAL2 = 6,
    DATA_DESECRATING_SHOT_PATTERN_WAVE_LEFT_1 = 7,
    DATA_DESECRATING_SHOT_PATTERN_WAVE_LEFT_2 = 8,
    DATA_DESECRATING_SHOT_PATTERN_WAVE_RIGHT_1 = 9,
    DATA_DESECRATING_SHOT_PATTERN_WAVE_RIGHT_2 = 10,
    DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_1 = 11,
    DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_2 = 12,


    DATA_BRIDGE_PHASE_TWO_1 = 1,
    DATA_BRIDGE_PHASE_TWO_2 = 2,
    DATA_BRIDGE_PHASE_TWO_3 = 3,

    DATA_BRIDGE_PHASE_TWO_COUNT_1 = 1,
    DATA_BRIDGE_PHASE_TWO_COUNT_2 = 2,
    DATA_BRIDGE_PHASE_TWO_COUNT_3 = 3,
    DATA_BRIDGE_PHASE_TWO_COUNT_4 = 4,
    DATA_BRIDGE_PHASE_TWO_COUNT_5 = 5,
    DATA_BRIDGE_PHASE_TWO_COUNT_6 = 6,

    DATA_MALDRAXXI_PLATFORM = 0,
    DATA_NIGHTFAE_PLATFORM = 1,
    DATA_KYRIAN_PLATFORM = 2,
    DATA_VENTHYR_PLATFORM = 3,

    DATA_MIDDLE_POS_OUTTER_PLATFORM = 0,
    DATA_TOP_RIGHT_POS_VERTEX_PLATFORM = 1,
    DATA_BOTTOM_LEFT_POS_VERTEX_PLATFORM = 2
};

Position const SylvanasFirstPhasePlatformCenter = { 234.9542f, -829.9804f, 4104.986f };

constexpr float PLATFORM_RADIUS = 75.0f;
constexpr float PLATFORM_RADIUS2 = 150.0f;

Position const SylvanasIntroPos[4] =
{
    { 231.15799f, -832.816f,   4105.0386f          },
    { 242.00348f, -840.51215f, 4105.0386f          },
    { 241.23091f, -830.0955f,  4105.0386f          },
    { 225.73611f, -844.0746f,  4104.9882f, 1.3613f }
};

Position const SylvanasVeilOnePos = { 255.0392f, -824.6999f, 4205.122f };

Position const DesecratingShotArrowShape[5] =
{
    { -0.43609f,  4.98095f, 4105.0386f  },
    {  2.38811f,  4.82665f, 4105.0386f  },
    {  5.21233f,  4.67245f, 4105.0386f  },
    {  5.05808f,  1.84821f, 4105.0386f  },
    {  4.90381f, -0.97601f, 4105.0386f  }
};

static constexpr float Magnitude = 1.0f;  // Maximum wave outbreak

static Position GetSineWavePoint(Position const& origin, float angle, uint32 tickNumber, float frequency)
{
    float distance = static_cast<float>(tickNumber);
    Position point = origin;
    point.m_positionX += std::cos(angle) * distance;
    point.m_positionY += std::sin(angle) * distance;

    G3D::Vector2 perpendicularDirection(point.GetPositionY() - origin.GetPositionY(), -(point.GetPositionX() - origin.GetPositionX()));

    point.m_positionX += perpendicularDirection.direction().x * std::sin(tickNumber * frequency) * Magnitude;
    point.m_positionY += perpendicularDirection.direction().y * std::sin(tickNumber * frequency) * Magnitude;

    return point;
}

Position LastFrontSpiralPoint = { 0.0f, 0.0f, 0.0f };
Position LastLeftSpiralPoint = { 0.0f, 0.0f, 0.0f };
Position LastRightSpiralPoint = { 0.0f, 0.0f, 0.0f };

Position const RiveThrowPos[8] =
{
    { 166.617f, -856.656f, 4113.2285f },
    { 208.542f, -761.602f, 4113.2285f },
    { 303.332f, -803.056f, 4113.2285f },
    { 302.793f, -858.021f, 4113.2285f },
    { 183.836f, -777.633f, 4113.2285f },
    { 206.884f, -897.526f, 4113.2285f },
    { 261.473f, -898.365f, 4113.2285f },
    { 287.948f, -778.238f, 4113.2285f }
};

Position const RiveFinishPos = { 235.1163f, -829.901f,  4105.0386f, 5.4824f };
// End of P1

// Start of P2
Position const FirstBridge[10] =
{
    {  46.740200f, -1025.729980f, 4999.983398f, 1.407430f }, // SoulJudge
    {  47.600700f, -1005.809998f, 4999.977051f, 3.702740f }, // Goliath
    {  55.577599f, -1002.260010f, 4999.977539f, 2.511290f }, // Vanguard
    {  56.550762f, -1013.864502f, 5000.120117f, 2.946787f }, // Hopebreaker
    {  55.444599f, -1025.030029f, 5000.119629f, 2.255250f }, // Vanguard
    {  49.148201f, -1032.372070f, 4999.983398f, 1.512632f }, // Vanguard
    {  46.616501f, -1019.039978f, 4999.985352f, 2.126440f }, // Vanguard
    {  41.430756f, -1026.425659f, 4999.985352f, 0.991931f }, // Hopebreaker
    {  41.753078f, -1014.469299f, 4999.995605f, 2.266619f } // Vanguard
};

Position const SecondBridge[10] =
{
    {  -55.440277f, -1096.165405f, 4999.985840f, 5.954844f }, // Summoner
    {  -50.487041f, -1101.272827f, 4999.975098f, 0.602360f }, // Goliath
    {  -60.098106f, -1100.975098f, 4999.992676f, 0.287412f }, // Vanguard
    {  -67.824638f, -1100.404053f, 4999.991699f, 0.087131f }, // Hopebreaker
    {  -62.797031f, -1110.498779f, 4999.992188f, 0.515173f }, // Vanguard
    {  -76.990112f, -1118.534424f, 4999.994141f, 0.515173f }, // Vanguard
    {  -81.976562f, -1132.675659f, 4999.974609f, 0.711523f }  // Vanguard
};

Position const ThirdBridge[10] =
{
    {   -188.866608f, -1136.805908f, 4999.990234f, 0.596102f  }, // Summoner
    {   -182.152740f, -1129.249146f, 4999.989258f, 1.113678f  }, // Souljudge
    {   -177.517303f, -1115.974243f, 4999.976074f, 0.720983f  }, // Vanguard
    {   -172.431870f, -1133.668701f, 4999.993652f, 1.396426f  }, // Vanguard
    {   -179.818802f, -1134.514893f, 4999.991699f, 1.156880f  }, // Vanguard
    {   -183.686249f, -1141.364990f, 4999.991211f, 1.194579f  }, // Vanguard
    {   -191.483139f, -1125.979004f, 5000.120605f, 0.330639f  }, // Hopebreaker
    {   -204.768539f, -1149.272461f, 4999.974609f, 0.507353f  }, // Hopebreaker
    {   -226.381165f, -1160.713745f, 4999.980469f, 0.601601f  }  // Hopebreaker
};

Position const FinalBridgeIntermission1[10] =
{
    {   -162.560196f, -1217.309204f, 5000.011230f, 4.199506f  }, //Vanguard
    {   -146.273514f, -1226.458496f, 4999.983887f, 3.579036f  }, //Vanguard
    {   -158.112366f, -1235.938110f, 5000.119629f, 2.149613f  }, //Vanguard
    {   -166.514297f, -1237.435059f, 5000.120117f, 3.464368f  } // Vanguard

};

Position const FinalBridgeIntermission2[10] =
{
    {    -180.164139f, -1231.117310f, 4999.976074f, 4.074629f }, // Hopebreaker
    {    -194.625153f, -1234.159668f, 4999.986328f, 4.666021f }, // Vanguard
    {    -201.289429f, -1234.129639f, 4999.966797f, 4.936996f }, // Hopebreaker
    {    -216.683258f, -1241.382690f, 5000.110840f, 5.646197f }, // Vanguard
    {    -226.047562f, -1256.024902f, 5000.158203f, 0.172768f }, // Hopebreaker
    {    -216.245270f, -1262.280640f, 4999.990723f, 0.377746f }, // Vanguard
    {    -220.860825f, -1272.457275f, 4999.974121f, 0.706043f }, // Vanguard
    {    -207.532928f, -1267.622681f, 5000.119629f, 1.719216f }, // Hopebreaker
    {    -197.940414f, -1261.690552f, 5000.119629f, 1.981541f },  // Vanguard
    {    -180.057983f, -1243.780151f, 5000.120117f, 3.154926f } // Vanguard

};




Position const SylvanasPhase2PrePos = { -14.5625f, -943.441f,   4999.990f,  0.8928f };
Position const SylvanasPhase2PrePos2 = { 46.3907f, -1021.473f, 4999.985f, 2.3660f };
Position const Goliath1 = { 52.4856f, -1005.6083f, 4999.985f, 3.4813f };
Position const SylvanasPhase2PrePos3 = { -50.9061f, -1099.3021f, 4999.9775f, 0.3477f };
Position const SylvanasPhase2PrePos4 = { -177.1530f, -1126.3653f, 4999.9854f, 0.4899f };
Position const SylvanasRuin2 = { -11.1916f, -1082.3433f, 4999.988f, 3.9464f };
Position const SylvanasRuin3 = { -160.4722f, -1088.9347f, 4999.983f, 5.7167f }; // -160.9323 Y: -1088.0469 Z: 4999.983 
Position const SylvanasRuin4 = { -158.3602f, -1228.7094f, 4999.975f, 2.7281f };
Position const HauntingPull = { 78.889130f, -790.001526f, 5000.390137f, 3.249162f };

Position const Veil2Pos1 = { -18.3626f, -942.6083f, 5075.8633f, 5.7316f };
Position const Veil2Pos2 = { -159.1123f, -1089.3759f, 5046.6069f, 5.9122f };
Position const Veil2Pos3 = { -162.407242f, -1228.5635f, 5059.4814f, 2.0481f };

Position const BansheeWailBeforeThirdRuin = { -166.21698f, -1109.0841f, 4999.9775f, 4.142766f };
Position const SylvanasWavePos[10] =
{
    { 133.5104f, -829.4792f, 4999.968f, 0.4383f },
    { 91.66666f, -835.0868f, 4999.968f, 0.4383f },
    { 92.41319f, -883.3455f, 4999.968f, 1.2723f },
    { 56.42536f, -902.6979f, 4999.968f, 1.1349f },
    { 19.02257f, -892.4983f, 4999.968f, 0.3612f },
    { 44.3125f, -991.03644f, 4999.991f, 2.7784f },
    { 44.3125f, -991.03644f, 4999.991f, 0.7843f },
    {-198.2257f, -1113.9983f, 4999.991, 1.7638f }, // summoner and souljudge

};



Position const SylvanasWitheringP2 = { -160.9323f, -1088.0469f, 4999.983f, 0.2769f };
Position const SylvanasWitheringP2Final = { -158.3598f, -1228.7115f, 4999.977f, 0.8070f };
/* {
    { -160.9323f -1088.0469f, 4999.983f, 0.2769f },
    { -158.3598f, -1228.7115f, 4999.977f, 0.8070f },
    { -158.3602f, -1228.7094f, 4999.975f, 2.7281f }
    // -160.9323 Y: -1088.0469 Z: 4999.983
}; */

Position const SylvanasPhase3PrePos = { -258.991f, -1265.996f,  5667.114f,  0.3118f };

Position const SylvanasPhase3Pos = { -280.646f, -1245.48f,   5672.13f,   2.3046f };

// Middle is 0, top right is 1, bottom left is 2
Position const CovenantPlatformPos[4][3] =
{
    // Maldraxxi
    {
        { -289.9608f, -1236.4189f, 5671.9052f, 0.0f },
        { -271.2141f, -1255.5200f, 5671.6704f },
        { -308.6874f, -1218.0043f, 5671.6704f }
    },

    // Nightfae
    {
        { -290.2621f, -1316.9971f, 5671.9067f, 0.0f },
        { -271.2889f, -1335.1152f, 5671.6704f },
        { -308.4179f, -1297.5373f, 5671.6704f }
    },

    // Kyrian
    {
        { -209.5206f, -1316.5284f, 5671.9052f, 0.0f },
        { -191.8095f, -1334.5333f, 5671.6704f },
        { -228.8465f, -1297.1842f, 5671.6704f }
    },

    // Venthyr
    {
        { -210.2180f, -1236.2922f, 5671.9067f, 0.0f },
        { -192.2238f, -1254.8327f, 5671.6704f },
        { -229.1900f, -1217.7504f, 5671.6704f }
    }
};


Position InvigoratingFieldPos[8] =
{
    { -268.229f, -1236.99f, 5671.67f, 0.0f     }, // Necrolord left side
    { -289.297f, -1258.25f, 5671.67f, 4.71239f }, // Necrolord right side
    { -289.569f, -1294.9f,  5671.67f, 1.5708f  }, // 2nd platform front
    { -268.229f, -1316.38f, 5671.67f, 0.0f     }, // 2nd platform left
    { -231.528f, -1315.5f,  5671.67f, 3.14159f }, // Third platform front
    { -210.512f, -1294.48f, 5671.67f, 1.5708f  }, // Third platform left
    { -210.795f, -1257.88f, 5671.67f, 4.71239f }, // Fourth platform front
    { -231.528f, -1236.39f, 5671.67f, 3.14159f } // Forth platform jailer
};

Position RazePlatform1[10] =
{
    { -286.80286f, -1203.163f, 5686.91f, 4.798263f },
    { -256.65234f, -1233.9247f, 5690.91f, 3.216263f },
    { -255.76732f, -1227.7351f, 5692.91f, 3.9757133f },
    { -324.39288f, -1234.2755f, 5695.91f, 0.054995082f },
    { -318.4202f, -1245.0446f, 5691.91f, 6.081147f },
    { -292.44464f, -1206.1954f, 5694.91f, 5.0815907f },
    { -259.59827f, -1239.4385f, 5686.91f, 3.3812764f },
};

Position Platform1Center = { -289.960785f, -1236.418945f, 5671.905273f, 0.00f };
Position Platform2Center = { -290.262085f, -1316.997070f, 5671.906738f, 0.00f };
Position Platform3Center = { -209.520599f, -1316.528442f, 5671.905273f, 0.00f };
Position Platform4Center = { -210.218002f, -1236.292236f, 5671.906738f, 0.00f };

Position Platform2Sky = { -290.262085f, -1316.997070f, 5733.854492f,  0.000000f };
Position Platform3Sky = { -209.520599f, -1316.528442f, 5733.905273f, 0.00f };
Position Platform4Sky = { -210.218002f, -1236.292236f, 5733.906738f, 0.00f };



bool HasBane(Unit const* unit)
{
        if (unit->HasAura(SPELL_BANSHEES_BANE))

            return true;
        else
            return false;
}


static Position GetRandomPointInCovenantPlatform(Position const& a, Position const& b, float c)
{
    float x = frand(std::min(a.GetPositionX(), b.GetPositionX()), std::max(a.GetPositionX(), b.GetPositionX()));
    float y = frand(std::min(a.GetPositionY(), b.GetPositionY()), std::max(a.GetPositionY(), b.GetPositionY()));
    float z = c;

    return Position(x, y, z);
}

Position const SylvanasVeilThreePos = { -286.978f, -1245.2378f, 5772.0347f, 0.0f };

Position const SylvanasEndPos = { -249.876f, -1252.4791f, 5667.1157f, 3.3742f };

class SylvanasNonMeleeSelector
{
public:
    SylvanasNonMeleeSelector(Unit const* obj) : _sourceObj(obj) { }

    bool operator()(Unit* unit) const
    {
        if (!unit->ToPlayer()->IsPlayer() || sChrSpecializationStore.AssertEntry(unit->ToPlayer()->GetPrimarySpecialization())->Flags & CHR_SPECIALIZATION_FLAG_MELEE)
            return false;
        return true;
    }

private:
    Unit const* _sourceObj;
};

class PauseAttackState : public BasicEvent
{
public:
    PauseAttackState(Unit* actor, bool paused) : _actor(actor), _paused(paused) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        if (_paused)
        {
            _actor->InterruptNonMeleeSpells(true);
            _actor->GetMotionMaster()->Clear();
            _actor->StopMoving();
            _actor->AttackStop();

            _actor->ToCreature()->SetReactState(REACT_PASSIVE);
        }
        else
        {
            _actor->ToCreature()->SetReactState(REACT_AGGRESSIVE);

            if (_actor->GetEntry() == BOSS_SYLVANAS_WINDRUNNER)
            {
                if (Unit* target = _actor->GetAI()->SelectTarget(SelectTargetMethod::MaxThreat, 0, 250.0f, true))
                    _actor->GetAI()->AttackStart(target);

                _actor->GetAI()->DoAction(ACTION_RESET_MELEE_KIT);
            }
        }

        return true;
    }

private:
    Unit* _actor;
    bool _paused;
};



class SetSheatheStateOrNameplate : public BasicEvent
{
public:
    SetSheatheStateOrNameplate(Unit* actor, uint8 event, uint8 copyIndex) : _actor(actor), _event(event), _copyIndex(copyIndex) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        switch (_event)
        {
        case DATA_CHANGE_SHEATHE_UNARMED:
            _actor->SetSheath(SHEATH_STATE_UNARMED);
            break;

        case DATA_CHANGE_SHEATHE_DAGGERS:
            _actor->SetSheath(SHEATH_STATE_MELEE);
            break;

        case DATA_CHANGE_SHEATHE_BOW:
            _actor->SetSheath(SHEATH_STATE_RANGED);
            break;

        case DATA_CHANGE_NAMEPLATE_TO_COPY:
            if (Creature* shadowCopy = _actor->GetInstanceScript()->instance->GetCreature(_actor->GetInstanceScript()->GetGuidData(DATA_SYLVANAS_SHADOWCOPY_01 + _copyIndex)))
                _actor->SetNameplateAttachToGUID(shadowCopy->GetGUID());
            break;

        case DATA_SYLVANAS_SHADOW_COPY_RIDING:
            if (Creature* ridingCopy = _actor->GetInstanceScript()->GetCreature(DATA_SYLVANAS_SHADOW_COPY_RIDING))
                _actor->SetNameplateAttachToGUID(ridingCopy->GetGUID());
            break;

        case DATA_CHANGE_NAMEPLATE_TO_SYLVANAS:
            _actor->SetNameplateAttachToGUID(ObjectGuid::Empty);
            break;

        default:
            break;
        }

        return true;
    }

private:
    Unit* _actor;
    uint8 _event;
    uint8 _copyIndex;
};

class SetDisplayIdBanshee : public BasicEvent
{
public:
    SetDisplayIdBanshee(Unit* actor, uint8 event) : _actor(actor), _event(event) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        switch (_event)
        {
        case DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL:
            _actor->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);

            break;

        default:
            break;
        }

        return true;
    }

private:
    Unit* _actor;
    uint8 _event;
};

class SetDisplayIdElf : public BasicEvent
{
public:
    SetDisplayIdElf(Unit* actor, uint8 event) : _actor(actor), _event(event) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        switch (_event)
        {
        case DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL:
            _actor->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
            break;

        default:
            break;
        }

        return true;
    }

private:
    Unit* _actor;
    uint8 _event;
};

// Sylvanas Shadowcopy (Fight) - 176369
struct npc_sylvanas_windrunner_shadowcopy : public ScriptedAI
{
    npc_sylvanas_windrunner_shadowcopy(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()),
        _onPhaseOne(true), _onDominationChains(false), _onDominationChainsBeforeRive(false), _onRiveEvent(false), _sayDaggers(0),
        _sayDesecrating(0), _jumpCount(0) { }

    void JustAppeared() override
    {
        me->SetReactState(REACT_PASSIVE);

        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
        me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
    }

    void Reset() override
    {
        _onPhaseOne = true;
        _onDominationChains = false;
        _onDominationChainsBeforeRive = false;
        _onRiveEvent = false;
        _sayDaggers = 0;
        _sayDesecrating = 0;
        _jumpCount = 0;
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type == DATA_EVENT_TYPE_SHADOWCOPY)
        {
            switch (value)
            {
            case DATA_EVENT_COPY_NO_EVENT:
            {
                _onPhaseOne = true;
                _onDominationChains = false;
                _onDominationChainsBeforeRive = false;
                _onRiveEvent = false;
                break;
            }

            case DATA_EVENT_COPY_DOMINATION_CHAIN_EVENT:
            {
                _onPhaseOne = false;
                _onDominationChains = true;
                _onDominationChainsBeforeRive = false;
                _onRiveEvent = false;
                break;
            }

            case DATA_EVENT_COPY_DOMINATION_CHAIN_BEFORE_RIVE_EVENT:
            {
                _onDominationChainsBeforeRive = true;
                break;
            }

            case DATA_EVENT_COPY_RIVE_EVENT:
            {
                _onPhaseOne = false;
                _onDominationChains = false;
                _onDominationChainsBeforeRive = false;
                _onRiveEvent = true;
                break;
            }

            case DATA_EVENT_COPY_FINISH_INTERMISSION_EVENT:
            {
                _onPhaseOne = false;
                _onDominationChains = false;
                _onDominationChainsBeforeRive = false;
                _onRiveEvent = false;
                break;
            }

            default:
                break;
            }

        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == EFFECT_MOTION_TYPE)
        {
            if (id == EVENT_JUMP)
            {
                if (_onPhaseOne || _onRiveEvent)
                    DoCastSelf(SPELL_ANCHOR_HERE, true);

                if (_onDominationChains)
                {
                    _scheduler.Schedule(150ms, [this](TaskContext /*task*/)
                        {
                            DoCastSelf(SPELL_DOMINATION_ARROW_SHOT_VISUAL, true);

                            DoCastSelf(SPELL_ANCHOR_HERE, true);
                        });

                    _scheduler.Schedule(200ms, [this](TaskContext /*task*/)
                        {
                            if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                            {
                                for (uint8 i = 0; i < 5; i++)
                                {
                                    Position const falseArrowPos = me->GetRandomPoint(SylvanasFirstPhasePlatformCenter, frand(2.5f, 55.0f));

                                    me->SendPlaySpellVisual(falseArrowPos, 0.0f, SPELL_VISUAL_DOMINATION_ARROW, 0, 0, 2.0f, true);

                                    _scheduler.Schedule(2s, [sylvanas, falseArrowPos](TaskContext /*task*/)
                                        {
                                            sylvanas->CastSpell(falseArrowPos, SPELL_DOMINATION_ARROW_FALL, true);
                                        });
                                }

                                if (me->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                                {
                                    // TODO: on mythic, there are 4 spots on which arrows fall on
                                }
                                else
                                {
                                    for (uint32 i = 0; i < _selectedArrowCountsPerJump[_jumpCount]; ++i)
                                    {
                                        Position const arrowPos = me->GetRandomPoint(SylvanasFirstPhasePlatformCenter, frand(2.5f, 55.0f));

                                        if (Creature* dominationArrow = sylvanas->SummonCreature(NPC_DOMINATION_ARROW, arrowPos, TEMPSUMMON_MANUAL_DESPAWN))
                                        {
                                            me->SendPlaySpellVisual(arrowPos, 0.0f, SPELL_VISUAL_DOMINATION_ARROW_SPAWN, 0, 0, 2.0f, true);

                                            _scheduler.Schedule(2s, [this, sylvanas, dominationArrow](TaskContext /*task*/)
                                                {
                                                    sylvanas->CastSpell(dominationArrow, SPELL_DOMINATION_ARROW_FALL_AND_VISUAL, true);

                                                    if (me->GetMap()->GetDifficultyID() == DIFFICULTY_HEROIC_RAID || me->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                                                        dominationArrow->CastSpell(dominationArrow, SPELL_DOMINATION_ARROW_CALAMITY_VISUAL, false);
                                                });
                                        }
                                    }

                                    ++_jumpCount;
                                }
                            }
                        });
                }


            }
        }
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
        case ACTION_CALCULATE_ARROWS:
        {
            // Number of arrows spawned is dependent on raid's difficulty and size: min. 4, max. 10 (unless on intermission, which is every player alive)
            uint8 arrowsToSpawn = _onDominationChainsBeforeRive ? me->GetMap()->GetPlayersCountExceptGMs() :
                std::min<uint8>(std::max<uint8>(std::ceil(float(me->GetMap()->GetPlayersCountExceptGMs() / 3.0f)), 4), 10);
            _selectedArrowCountsPerJump = SplitArrowCasts(arrowsToSpawn);

            _jumpCount = 0;
            break;
        }

        default:
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void WitheringFire(uint32 witheringFires)
    {
        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {
            std::list<Player*> targetList;
            GetPlayerListInGrid(targetList, me, 350.0f);
            Trinity::Containers::RandomResize(targetList, witheringFires);

            for (Unit* target : targetList)
            {
                me->SendPlaySpellVisual(target, SPELL_VISUAL_WITHERING_FIRE_PHASE_ONE, 0, 0, 48.0f, false);
                uint32 timeToTarget = me->GetDistance(target) * 0.0208 * 1000;
                _scheduler.Schedule(Milliseconds(timeToTarget), [sylvanas, target](TaskContext /*task*/)
                    {
                        sylvanas->CastSpell(target, SPELL_WITHERING_FIRE, true);
                    });
            }
        }
    }

    void Veil2(bool chooseMe, uint8 index, Creature* nextIndex)
    {
        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {
            std::list<Player*> targetList;
            GetPlayerListInGrid(targetList, me, 250.0f);

            Trinity::Containers::RandomResize(targetList, 1);

            for (Player* target : targetList)
            {
                Position const veil2Pos = target->GetNearPosition(frand(2.5f, 3.0f), frand(0.0f, 6.0f));

                              
                    me->NearTeleportTo(veil2Pos, false);

                _scheduler.Schedule(202ms, [this, target](TaskContext /*task*/)
                    {
                        me->SetFacingToObject(target);
                    });

                _scheduler.Schedule(250ms, [veil2Pos, sylvanas, this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_GROW, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 3200));
                        sylvanas->SendPlayOrphanSpellVisual(veil2Pos, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_3_HC, 3.0f, true, false);
                        //sylvanas->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_VEIL_OF_DARKNESS2, 0, 0);

                    });

                _scheduler.Schedule(1000ms, [this](TaskContext /*task*/)
                    {
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_VEIL_OF_DARKNESS2, 0, 0);
                    });

                _scheduler.Schedule(3008ms, [veil2Pos, sylvanas](TaskContext /*task*/)
                    {
                        sylvanas->CastSpell(veil2Pos, SPELL_VEIL_OF_DARKNESS_PHASE_2_AREA, false);


                    });
            }
        }
    }

    void StartWitheringFireP3Event(bool chooseMe)
    {
        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {
            uint8 castTimes = 1;

            Position const witheringCastPos = me->GetNearPosition(frand(30.0f, 50.0f), frand(0.0f, 3.5f));




            _scheduler.Schedule(50ms, [this, witheringCastPos, sylvanas, chooseMe](TaskContext /*task*/)
                {
                    sylvanas->SendPlayOrphanSpellVisual(witheringCastPos, SPELL_VISUAL_WINDRUNNER_01, 0.25f, true, false);

                    if (chooseMe)
                        me->CastSpell(witheringCastPos, SPELL_WINDRUNNER_MOVE, false);
                    else
                        me->NearTeleportTo(witheringCastPos, false);
                });

            _scheduler.Schedule(350ms, [this, sylvanas](TaskContext /*task*/)
                {
                    me->SetFacingToObject(sylvanas);
                });

            _scheduler.Schedule(400ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_WITHERING_FIRE_COPY, true);
                });

            _scheduler.Schedule(750ms, [this, castTimes, sylvanas, chooseMe](TaskContext /*task*/)
                {
                    std::list<Player*> targetList;
                    GetPlayerListInGrid(targetList, me, 250.0f);

                    Trinity::Containers::RandomResize(targetList, chooseMe ? castTimes - 3 : castTimes);

                    for (Player* target : targetList)
                    {
                        me->SendPlaySpellVisual(target, SPELL_VISUAL_WITHERING_FIRE_PHASE_ONE, 0, 0, 48.0f, false);

                        uint32 timeToCast = me->GetDistance(target) * 0.0208;

                        _scheduler.Schedule(Seconds(timeToCast), [sylvanas, target](TaskContext /*task*/)
                            {
                                sylvanas->CastSpell(target, SPELL_WITHERING_FIRE, true);
                            });
                    }
                });

            _scheduler.Schedule(800ms, [this, sylvanas](TaskContext /*task*/)
                {
                    //me->SendPlayOrphanSpellVisual(sylvanas->GetPosition(), SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);

                    //me->CastSpell(sylvanas->GetPosition(), SPELL_WINDRUNNER_MOVE, true);
                });

            _scheduler.Schedule(2s + 62ms, [sylvanas, chooseMe](TaskContext /*task*/)
                {
                    if (chooseMe)
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_DEACTIVATE);
                    }
                });
        }
    }

    void StartShadowDaggersEvent(bool chooseMe, uint8 index, Creature* nextIndex)
    {
        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {
            std::list<Player*> targetList;
            GetPlayerListInGrid(targetList, me, 250.0f);

            Trinity::Containers::RandomResize(targetList, 1);

            for (Player* target : targetList)
            {
                Position const shadowdaggerPos = target->GetNearPosition(frand(2.5f, 3.0f), frand(0.0f, 6.0f));

                if (index == 0)
                    sylvanas->SendPlayOrphanSpellVisual(shadowdaggerPos, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);

                if (chooseMe)
                {
                    if (_sayDaggers == 0)
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(SAY_SHADOW_DAGGER);

                        _sayDaggers = 1;

                        _scheduler.Schedule(20s, [this](TaskContext /*task*/)
                            {
                                _sayDaggers = 0;
                            });
                    }

                    me->CastSpell(shadowdaggerPos, SPELL_WINDRUNNER_MOVE, false);

                    sylvanas->SetNameplateAttachToGUID(me->GetGUID());
                }
                else
                    me->NearTeleportTo(shadowdaggerPos, false);

                _scheduler.Schedule(202ms, [this, target](TaskContext /*task*/)
                    {
                        me->SetFacingToObject(target);
                    });

                _scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_SHADOW_DAGGER_COPY, true);
                    });

                _scheduler.Schedule(550ms, [target, sylvanas](TaskContext /*task*/)
                    {
                        sylvanas->CastSpell(target, SPELL_SHADOW_DAGGER, true);
                    });

                _scheduler.Schedule(750ms, [this, sylvanas, nextIndex, chooseMe, index](TaskContext /*task*/)
                    {
                        if (index < 3)
                            me->SendPlayOrphanSpellVisual(*nextIndex, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);
                        else
                            me->SendPlayOrphanSpellVisual(*sylvanas, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);

                        if (chooseMe)
                            me->CastSpell(*sylvanas, SPELL_WINDRUNNER_MOVE, false);
                        else
                            me->NearTeleportTo(*sylvanas, false);
                    });

                _scheduler.Schedule(1s + 250ms, [sylvanas, chooseMe](TaskContext /*task*/)
                    {
                        if (chooseMe)
                        {
                            if (sylvanas->IsAIEnabled())
                                sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_DEACTIVATE);
                        }
                    });
            }
        }
    }

    void StartVeilOfDarknessP2Event(bool chooseMe, uint8 index, Creature* nextIndex)
    {
        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {
            std::list<Player*> targetList;
            GetPlayerListInGrid(targetList, me, 250.0f);

            Trinity::Containers::RandomResize(targetList, 1);

            for (Player* target : targetList)
            {

                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 250.0f, true, true))
                {
                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_GROW, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 5000));
                    me->NearTeleportTo(target->GetPosition(), false);
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_3_HC, 0, 0);

                    float x = target->GetPositionX();
                    float y = target->GetPositionY();
                    float z = target->GetPositionZ() + 0.5f;

                    me->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_3_HC, 3.0f, true, false);
                    _scheduler.Schedule(3308ms, [x, y, z, this](TaskContext /*task*/)
                        {
                            me->CastSpell(Position{ x, y, z }, SPELL_VEIL_OF_DARKNESS_PHASE_2_AREA, false);
                            me->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_02_03, 3.0f, true, false);
                            me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_VEIL_OF_DARKNESS2, 0, 0);

                        });

                }


                _scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 3208));

                    });
            }

            _scheduler.Schedule(1000ms, [this, sylvanas, nextIndex, chooseMe, index](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_GROW, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 5000));


                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 250.0f, true, true))
                    {
                        me->NearTeleportTo(target->GetPosition(), false);
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_VEIL_OF_DARKNESS2, 0, 0);

                        float x = target->GetPositionX();
                        float y = target->GetPositionY();
                        float z = target->GetPositionZ() + 0.5f;

                        sylvanas->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_02_03, 3.0f, true, false);


                        _scheduler.Schedule(3308ms, [x, y, z, sylvanas](TaskContext /*task*/)
                            {
                                sylvanas->CastSpell(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_3_HC, false);
                                sylvanas->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_02_03, 3.0f, true, false);
                                sylvanas->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_VEIL_OF_DARKNESS2, 0, 0);

                            });

                    }


                });




        }
    }




    void StartDesecratingShotEvent(uint8 pattern, Position pos)
    {
        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {

            sylvanas->CastSpell(sylvanas, SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 2550));

            switch (pattern)
            {
            case DATA_DESECRATING_SHOT_PATTERN_STRAIGHT:
            case DATA_DESECRATING_SHOT_PATTERN_SCATTERED:
            {
                if (sylvanas->IsAIEnabled())
                    sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_ACTIVATE);

                me->NearTeleportTo(sylvanas->GetNearPosition(5.0f, float(M_PI)), false);

                _scheduler.Schedule(50ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 5.0f, sylvanas->GetOrientation(), false);
                    });

                _scheduler.Schedule(100ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        if (sylvanas->IsAIEnabled())
                        {
                            if (_sayDesecrating == 0)
                            {
                                sylvanas->AI()->Talk(SAY_DESECRATING_SHOT);

                                _sayDesecrating = 1;

                                _scheduler.Schedule(20s, [this](TaskContext /*task*/)
                                    {
                                        _sayDesecrating = 0;
                                    });
                            }
                        }
                    });

                _scheduler.Schedule(350ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        sylvanas->SetNameplateAttachToGUID(me->GetGUID());

                        DoCastSelf(SPELL_DESECRATING_SHOT_JUMP_FRONT, false);
                    });

                _scheduler.Schedule(850ms, [sylvanas](TaskContext /*task*/)
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_DEACTIVATE);
                    });
                break;
            }

            case DATA_DESECRATING_SHOT_PATTERN_WAVE:
            {
                sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_ACTIVATE);
                me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ() + 10.0f, false);

                _scheduler.Schedule(400ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        sylvanas->SetNameplateAttachToGUID(me->GetGUID());

                        DoCastSelf(SPELL_DESECRATING_SHOT_JUMP_LEFT, false);
                    });

                _scheduler.Schedule(450ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        if (sylvanas->IsAIEnabled())
                        {
                            if (_sayDesecrating == 0)
                            {
                                sylvanas->AI()->Talk(SAY_DESECRATING_SHOT);

                                _sayDesecrating = 1;

                                _scheduler.Schedule(20s, [this](TaskContext /*task*/)
                                    {
                                        _sayDesecrating = 0;
                                    });
                            }
                        }
                    });

                _scheduler.Schedule(400ms, [this, sylvanas](TaskContext /*task*/)
                    {

                        sylvanas->SetNameplateAttachToGUID(me->GetGUID());

                        DoCastSelf(SPELL_DESECRATING_SHOT_JUMP_LEFT, false);
                    });

                _scheduler.Schedule(400ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        sylvanas->SetNameplateAttachToGUID(me->GetGUID());

                        DoCastSelf(SPELL_DESECRATING_SHOT_JUMP_LEFT, false);
                    });

                _scheduler.Schedule(1200ms, [sylvanas](TaskContext /*task*/)
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_DEACTIVATE);
                    });
                break;
            }

            case DATA_DESECRATING_SHOT_PATTERN_WAVE_LEFT_1:
            {
                sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_ACTIVATE);
                me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY() + 5.0f, pos.GetPositionZ() + 10.0f, false);

                _scheduler.Schedule(350ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        sylvanas->SetNameplateAttachToGUID(me->GetGUID());

                        DoCastSelf(SPELL_DESECRATING_SHOT_JUMP_LEFT, false);
                    });

                _scheduler.Schedule(400ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        if (sylvanas->IsAIEnabled())
                        {
                            if (_sayDesecrating == 0)
                            {
                                sylvanas->AI()->Talk(SAY_DESECRATING_SHOT);

                                _sayDesecrating = 1;

                                _scheduler.Schedule(20s, [this](TaskContext /*task*/)
                                    {
                                        _sayDesecrating = 0;
                                    });
                            }
                        }
                    });
                break;
            }

            case DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_1:
            {
                sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_ACTIVATE);
                me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ() + 10.0f, false);

                _scheduler.Schedule(500ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        sylvanas->SetNameplateAttachToGUID(me->GetGUID());

                        DoCastSelf(SPELL_DESECRATING_SHOT_JUMP_LEFT, false);
                    });
                break;
            }

            case DATA_DESECRATING_SHOT_PATTERN_WAVE_RIGHT_1:
            {
                sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_ACTIVATE);
                me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY() - 5.0f, pos.GetPositionZ() + 10.0f, false);

                _scheduler.Schedule(650ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        sylvanas->SetNameplateAttachToGUID(me->GetGUID());

                        DoCastSelf(SPELL_DESECRATING_SHOT_JUMP_LEFT, false);
                    });

                _scheduler.Schedule(1500ms, [sylvanas](TaskContext /*task*/)
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_DEACTIVATE);
                    });
                break;
            }

            case DATA_DESECRATING_SHOT_PATTERN_SPIRAL:
            {
                sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_ACTIVATE);

                me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ() + 10.0f, false);

                _scheduler.Schedule(400ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        sylvanas->SetNameplateAttachToGUID(me->GetGUID());

                        DoCastSelf(SPELL_DESECRATING_SHOT_JUMP_LEFT, false);
                    });

                _scheduler.Schedule(450ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        if (sylvanas->IsAIEnabled())
                        {
                            if (_sayDesecrating == 0)
                            {
                                sylvanas->AI()->Talk(SAY_DESECRATING_SHOT);

                                _sayDesecrating = 1;

                                _scheduler.Schedule(20s, [this](TaskContext /*task*/)
                                    {
                                        _sayDesecrating = 0;
                                    });
                            }
                        }
                    });

                _scheduler.Schedule(900ms, [sylvanas](TaskContext /*task*/)
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->DoAction(ACTION_WINDRUNNER_MODEL_DEACTIVATE);
                    });

                break;
            }

            case DATA_DESECRATING_SHOT_PATTERN_JAR:
            {
                break;
            }

            default:
                break;
            }
        }
    }

    

    std::array<uint32, 3> SplitArrowCasts(uint32 totalArrows)
    {
        std::array<uint32, 3> arrowsPerWave{ };

        uint32 baseArrowsPerWave = totalArrows / 3;

        arrowsPerWave.fill(baseArrowsPerWave);

        uint32 remainder = totalArrows - baseArrowsPerWave * 3;

        for (uint32 i = 0; i < remainder; ++i)
            ++arrowsPerWave[i];

        Trinity::Containers::RandomShuffle(arrowsPerWave);

        return arrowsPerWave;
    }


private:
    InstanceScript* _instance;
    TaskScheduler _scheduler;
    bool _onPhaseOne;
    bool _onDominationChains;
    bool _onDominationChainsBeforeRive;
    bool _onRiveEvent;
    uint8 _sayDaggers;
    uint8 _sayDesecrating;
    std::array<uint32, 3> _selectedArrowCountsPerJump;
    uint8 _jumpCount;

};

// Sylvanas Shadowcopy (Riding) - 178355
struct npc_sylvanas_windrunner_shadowcopy_riding : public ScriptedAI
{
    npc_sylvanas_windrunner_shadowcopy_riding(Creature* creature) : ScriptedAI(creature),
        _instance(creature->GetInstanceScript()) { }

    void JustAppeared() override
    {
        me->SetReactState(REACT_PASSIVE);

        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
        me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

private:
    InstanceScript* _instance;
    TaskScheduler _scheduler;
};

// Sylvanas Windrunner - 175732
struct boss_sylvanas_windrunner : public BossAI
{
    boss_sylvanas_windrunner(Creature* creature) : BossAI(creature, DATA_SYLVANAS_WINDRUNNER), _rangerShotOnCD(false),
        _maldraxxiDesecrated(false), _nightfaeDesecrated(false), _kyrianDesecrated(false), _venthyrDesecrated(false),
        _meleeKitCombo(0), _windrunnerCastTimes(0), _disecratingShotCastTimes(0), _riveCastTimes(0), _hauntingWaveTimes(0),
        _ruinCastTimes(0), _sayKilled(0), _chainsCastTimes(0), _veilCastTimes(0), _wailingArrowCastTimes(0), windsCasted(0), hasPhased(0), anduinReady(0) { }


    void JustAppeared() override
    {
        scheduler.ClearValidator();
        me->RemoveUnitFlag(UNIT_FLAG_NOT_ATTACKABLE_1);
        me->SetImmuneToAll(false);
        me->SetSpeed(UnitMoveType::MOVE_RUN, 14.0f);
        DoCastSelf(SPELL_DUAL_WIELD, true);
        DoCastSelf(SPELL_SYLVANAS_DISPLAY_POWER_SUFFERING, true);
    }

    void RemoveFirstAT()
    {
        me->RemoveAreaTrigger(SPELL_INVIGORATING_FIELD_ACTIVATE);
    }


    void DoCheckEvade()
    {
        Map::PlayerList const& players = me->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            if (Player* player = i->GetSource())
                if (player->IsAlive())
                    return;

        EnterEvadeMode(EVADE_REASON_NO_HOSTILES);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        Talk(SAY_DISENGAGE);

        _EnterEvadeMode();
        summons.DespawnAll();
        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BARBED_ARROW);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BANSHEE_MARK);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BANSHEES_BANE);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DOMINATION_CHAIN_PLAYER);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DOMINATION_CHAIN_PERIODIC);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VEIL_OF_DARKNESS_ABSORB_AURA);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VEIL_OF_DARKNESS_VISUAL_SPREAD);

        if (Creature* bolvar = instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
            bolvar->DespawnOrUnsummon();

        if (Creature* jaina = instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
            jaina->DespawnOrUnsummon();

        if (Creature* thrall = instance->GetCreature(DATA_THRALL_PINNACLE))
            thrall->DespawnOrUnsummon();

        if (Creature* anduin = instance->GetCreature(DATA_ANDUIN_CRUCIBLE))
            anduin->DespawnOrUnsummon();

        if (Creature* goliath = instance->GetCreature(DATA_MAWFORGED_GOLIATH))
            goliath->DespawnOrUnsummon();

        if (Creature* souljudge = instance->GetCreature(DATA_MAWFORGED_SOULJUDGE))
            souljudge->DespawnOrUnsummon();

        if (Creature* summoner = instance->GetCreature(DATA_MAWFORGED_SUMMONER))
            summoner->DespawnOrUnsummon();

        if (Creature* hopebreaker = instance->GetCreature(DATA_MAWSWORN_HOPEBREAKER))
            hopebreaker->DespawnOrUnsummon();

        if (Creature* vanguard = instance->GetCreature(DATA_MAWSWORN_VANGUARD))
            vanguard->DespawnOrUnsummon();

        if (Creature* orbs = instance->GetCreature(DATA_DECREPIT_ORB))
            orbs->DespawnOrUnsummon();


        _DespawnAtEvade();
    }

    void Reset() override
    {
        _Reset();

        me->SummonCreatureGroup(SPAWN_GROUP_INITIAL);

        me->GetInstanceScript()->DoUpdateWorldState(WORLD_STATE_SYLVANAS_ENCOUNTER_PHASE, PHASE_ONE);
        me->GetMap()->SetZoneMusic(AREA_SANCTUM_OF_DOMINATION, MUSIC_SYLVANAS_FREE_WILL);

        events.Reset();
        _specialEvents.Reset();

        // NOTE: apparently Sylvanas stays 750ms roughly without acting.
        me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));

        _rangerShotOnCD = false;
        _maldraxxiDesecrated = false;
        _nightfaeDesecrated = false;
        _kyrianDesecrated = false;
        _venthyrDesecrated = false;
        _meleeKitCombo = 0;
        _windrunnerCastTimes = 0;
        _ruinCastTimes = 0;
        _disecratingShotCastTimes = 0;
        _riveCastTimes = 0;
        _hauntingWaveTimes = 0;
        _chainsCastTimes = 0;
        _veilCastTimes = 0;
        windsCasted = 0;

    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        switch (summon->GetEntry())
        {
        case NPC_SYLVANAS_SHADOW_COPY_FIGHTERS:
            _shadowCopyGUID.push_back(summon->GetGUID());
            break;

        case NPC_MAWSWORN_VANGUARD:
            summon->GetGUID();
            break;

        case NPC_MAWSWORN_HOPEBREAKER:
            summon->GetGUID();
            break;

        default:
            break;
        }
    }

    void JustRegisteredAreaTrigger(AreaTrigger* areaTrigger) override
    {
        switch (areaTrigger->GetSpellId())
        {
        case SPELL_INVIGORATING_FIELD_ACTIVATE:
            _invigoratingFieldGUID.push_back(areaTrigger->GetGUID());
            break;
        default:
            break;
        }
    }

    void KilledUnit(Unit* victim) override
    {
        if (!victim->IsPlayer())
            return;

        if (_sayKilled == 0)
        {
            if (events.IsInPhase(PHASE_ONE))
                Talk(SAY_SLAY);
            else
                Talk(SAY_SLAY02);

            _sayKilled = 1;

            scheduler.Schedule(7s, [this](TaskContext /*task*/)
                {
                    _sayKilled = 0;
                });
        }

        // TODO: there should be an internal CD for this.

    }

    void OnSpellFailed(SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_RUIN)
        {


            switch (_ruinCastTimes)
            {
            case 1:
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetCanFly(false);
                me->SetDisableGravity(false);
                me->SetAnimTier(AnimTier::Ground);

                events.SetPhase(PHASE_TWO);
                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(5s));
                if (Creature* bolvar = instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                {
                    if (bolvar->IsAIEnabled())
                        bolvar->AI()->DoAction(ACTION_WINDS_OF_ICECROWN_PRE);
                    if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                    {
                        sylvanas->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(5750ms));
                        sylvanas->AI()->DoAction(ACTION_WINDS_OF_ICECROWN_PRE);
                    }
                    //DoAction(ACTION_START_PHASE_TWO_ONE);


                    //DoAction(ACTION_START_PHASE_TWO);
                }
                break;
            case 2:
                DoAction(ACTION_HAUNTING_WAVE_FOURTH_CHAIN);
                break;
            case 3:
                DoAction(ACTION_HAUNTING_WAVE_FIFTH_CHAIN);
                break;
            case 4:
                if (Creature* bolvar = instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                {
                    if (bolvar->IsAIEnabled())
                        bolvar->AI()->DoAction(ACTION_WINDS_OF_ICECROWN_PRE);
                    if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                    {
                        sylvanas->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(5750ms));
                        sylvanas->AI()->DoAction(ACTION_WINDS_OF_ICECROWN_PRE);
                    }

                    if (Creature* jaina = instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
                    {
                        if (jaina->IsAIEnabled());
                        //jaina->AI()->DoAction(ACTION_PREPARE_PHASE_THREE);

                    }
                }


                break;
            case 5:

                break;
            default:
                break;
            }


        }
    }







    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        instance->DoUpdateWorldState(WORLD_STATE_SYLVANAS_ENCOUNTER_PHASE, PHASE_ONE);
        me->GetMap()->SetZoneMusic(AREA_SANCTUM_OF_DOMINATION, MUSIC_SANCTUM_OF_DOMINATION);
        //DoAction(ACTION_PREPARE_PHASE_THREE);
        DoAction(ACTION_START_ENCOUNTER);
        instance->SetData(DATA_CENTER_PLATFORM, IN_PROGRESS);
        //me->GetMap()->SetZoneMusic(AREA_SANCTUM_OF_DOMINATION, MUSIC_ZOVAAL_FINAL_PHASE);
        // NOTE: it is safe to assume that these are summoned by a serverside spell.
        for (uint8 i = 0; i < 4; i++)
            me->SummonCreature(NPC_SYLVANAS_SHADOW_COPY_FIGHTERS, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);

        Talk(SAY_AGGRO);

        events.SetPhase(PHASE_ONE);
        //DoCastAOE(SPELL_ABSORB_VEIL_2);
        //events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_MAIN, 7s, 1, PHASE_ONE);
        //events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 5s, 1, PHASE_TWO);
        //events.ScheduleEvent(EVENT_DESECRATING_SHOT, 3s, 1, PHASE_ONE);

        //scheduler.Schedule(1s, [this](TaskContext task)
            //{
                //events.ScheduleEvent(EVENT_WITHERING_FIRE, 1ms, 1, PHASE_ONE);

                //task.Repeat(4s);
            //}); 

        events.ScheduleEvent(EVENT_WINDRUNNER, 7s, 1, PHASE_ONE);
        events.ScheduleEvent(EVENT_DOMINATION_CHAINS, 23200ms, 1, PHASE_ONE);
        events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS, 44900ms, 1, PHASE_ONE);
        events.ScheduleEvent(EVENT_EVADE_CHECK, 5s);

        // NOTE: we need a separated event handler for this because Wailing Arrow is triggered even if Sylvanas is casting.
        _specialEvents.SetPhase(PHASE_ONE);
        _specialEvents.ScheduleEvent(EVENT_WAILING_ARROW, 28s, 1, PHASE_ONE);
        DoCastSelf(SPELL_SYLVANAS_POWER_ENERGIZE_AURA, true);
        DoCastSelf(SPELL_RANGER_HEARTSEEKER_AURA, true);
        DoCastSelf(SPELL_HEALTH_PCT_CHECK_INTERMISSION, true);
        DoCastSelf(SPELL_HEALTH_PCT_CHECK_FINISH, true);




        me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(750ms));
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
        case ACTION_START_SYLVANAS_INTRO:
        {
            scheduler.Schedule(22s + 328ms, [this](TaskContext /*task*/)
                {
                    Conversation::CreateConversation(CONVERSATION_INTRO, me, me->GetPosition(), ObjectGuid::Empty);
                });

            scheduler.Schedule(23s + 828ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_01, 0, 0);

                    if (Creature* bolvar = instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        me->SetFacingToObject(bolvar);
                });

            scheduler.Schedule(24s + 968ms, [this](TaskContext /*task*/)
                {
                    me->GetMotionMaster()->MovePoint(POINT_INTRO_01, SylvanasIntroPos[0], false);
                });

            scheduler.Schedule(26s + 468ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_02, 0, 0);
                });

            scheduler.Schedule(28s + 203ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_ANCHOR_HERE, true);
                });

            scheduler.Schedule(29s + 828ms, [this](TaskContext /*task*/)
                {
                    me->GetMotionMaster()->MovePoint(POINT_INTRO_02, SylvanasIntroPos[1], false);
                });

            scheduler.Schedule(30s + 297ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_03, 0, 0);
                });

            scheduler.Schedule(33s + 797ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_04, 0, 0);
                });

            scheduler.Schedule(35s + 797ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_05, 0, 0);
                });

            scheduler.Schedule(40s + 797ms, [this](TaskContext /*task*/)
                {
                    if (Creature* bolvar = instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        me->SetFacingToObject(bolvar);
                });

            scheduler.Schedule(41s + 547ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_06, 0, 0);
                });

            scheduler.Schedule(42s, [this](TaskContext /*task*/)
                {
                    me->GetMotionMaster()->MovePoint(POINT_INTRO_03, SylvanasIntroPos[2], false);
                });

            scheduler.Schedule(47s + 89ms, [this](TaskContext /*task*/)
                {
                    if (Creature* bolvar = instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                    {
                        bolvar->SetFacingToObject(me);

                        bolvar->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_BOLVAR_INTRODUCTION_TALK_01, 0, 0);
                    }
                });

            scheduler.Schedule(50s + 484ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_ANCHOR_HERE, true);
                });

            scheduler.Schedule(60s + 656ms, [this](TaskContext /*task*/)
                {
                    if (Creature* bolvar = instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        me->SetFacingToObject(bolvar);
                });

            scheduler.Schedule(61s + 140ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_07, 0, 0);
                });

            scheduler.Schedule(66s + 656ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_08, 0, 0);
                });

            scheduler.Schedule(68s + 172ms, [this](TaskContext /*task*/)
                {
                    me->NearTeleportTo(SylvanasIntroPos[3], false);
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_TELEPORT, 0, 0);
                });

            scheduler.Schedule(69s + 437ms, [this](TaskContext /*task*/)
                {
                    if (Creature* bolvar = instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        me->SetFacingToObject(bolvar);
                });

            scheduler.Schedule(70s + 406ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_09, 0, 0);
                });

            scheduler.Schedule(75s + 172ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_INTRODUCTION_TALK_10, 0, 0);
                });

            scheduler.Schedule(78s + 422ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_RANGER_BOW_STANCE, false);
                });

            scheduler.Schedule(78s + 437ms, [this](TaskContext /*task*/)
                {
                    me->SetHomePosition(me->GetPosition());

                    instance->SetData(DATA_SYLVANAS_INTRO, DONE);
                });

            break;
        }

        case ACTION_START_ENCOUNTER:
        {


            if (Creature* jaina = instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
            {
                if (jaina->IsAIEnabled())
                    jaina->AI()->DoAction(ACTION_JAINA_START_ATTACKING);
                    
            }

            break;


        }

        case ACTION_ANDUIN_READY:
        {
            anduinReady = 1;
            break;
        }

        case ACTION_WINDRUNNER_MODEL_ACTIVATE:
            DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_01, false);

            break;

        case ACTION_WINDRUNNER_MODEL_DEACTIVATE:
            me->SetNameplateAttachToGUID(ObjectGuid::Empty);
            me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_01);

            break;

        case ACTION_RESET_MELEE_KIT:
        {
            _meleeKitCombo = 0;
            break;
        }


        case ACTION_RANGER_SHOT:
        {
            if (IsHeartseekerReady() == true)
                DoCastVictim(SPELL_RANGER_HEARTSEEKER, false);

            else if (IsBansheesHeartseekerReady() == true)
                DoCastVictim(SPELL_BANSHEE_HEARTSEEKER, false);

            else if (AreBladesReady() == true)
                DoCastVictim(SPELL_BANSHEE_BLADES, false);
            else
                DoCastVictim(SPELL_RANGER_SHOT, false);

            _rangerShotOnCD = true;

            scheduler.Schedule(750ms, [this](TaskContext /*task*/)
                {
                    me->resetAttackTimer();

                    _rangerShotOnCD = false;
                });

            DoAction(ACTION_RESET_MELEE_KIT);
            break;
        }

        case ACTION_CHECK_INTERMISSION:
        {
            _specialEvents.CancelEventGroup(1);
            _specialEvents.CancelEvent(EVENT_WAILING_ARROW);
            events.CancelEvent(EVENT_WAILING_ARROW);
            events.CancelEventGroup(1);
            events.CancelEvent(EVENT_DOMINATION_CHAINS);
            _specialEvents.SetPhase(PHASE_ONE);
            events.SetPhase(PHASE_ONE);
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();



            events.ScheduleEvent(EVENT_CHECK_INTERMISSION, 3s, 1);


            break;
        }

        case ACTION_PREPARE_INTERMISSION:
        {
            
            _specialEvents.CancelEventGroup(1);
            _specialEvents.CancelEvent(EVENT_WAILING_ARROW);
            events.CancelEvent(EVENT_WAILING_ARROW);
            events.CancelEventGroup(1);
            events.CancelEvent(EVENT_DOMINATION_CHAINS);
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
            me->InterruptNonMeleeSpells(true);


            me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));

            me->GetInstanceScript()->DoUpdateWorldState(WORLD_STATE_SYLVANAS_ENCOUNTER_PHASE, PHASE_INTERMISSION_WORLD_STATE);

            _specialEvents.SetPhase(PHASE_INTERMISSION);
            events.SetPhase(PHASE_INTERMISSION);
            hasPhased = 1;
            events.ScheduleEvent(EVENT_DOMINATION_CHAINS_TRANSITION, 1s, PHASE_INTERMISSION);
            break;
        }

        case ACTION_HAUNTING_WAVE_SECOND_CHAIN:
        {
            if (_hauntingWaveTimes < 5)
            {
                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                        DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 5500));
                    });

                scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                    {
                        me->SendPlayOrphanSpellVisual(SylvanasWavePos[_hauntingWaveTimes], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                    });

                scheduler.Schedule(750ms, [this](TaskContext /*task*/)
                    {
                        me->NearTeleportTo(SylvanasWavePos[_hauntingWaveTimes], false);
                        me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_02);
                    });

                scheduler.Schedule(1s, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_HAUNTING_WAVE, false);

                        if (_hauntingWaveTimes == 0)
                            Talk(SAY_ANNOUNCE_HAUNTING_WAVE);
                    });

                scheduler.Schedule(5s + 500ms, [this](TaskContext /*task*/)
                    {
                        _hauntingWaveTimes++;

                        DoAction(ACTION_HAUNTING_WAVE_SECOND_CHAIN);
                    });
            }
            else
            {
                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                    });

                scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                    {
                        me->SendPlayOrphanSpellVisual(SylvanasPhase2PrePos, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                    });

                scheduler.Schedule(750ms, [this](TaskContext /*task*/)
                    {
                        me->NearTeleportTo(SylvanasPhase2PrePos, false);
                        me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_02);
                    });

                scheduler.Schedule(5s, [this](TaskContext /*task*/)
                    {
                        me->SetCanFly(false);
                        me->SetDisableGravity(false);
                        DoCastSelf(SPELL_RUIN, false);
                        _ruinCastTimes++;
                        //me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveAura(SPELL_RANGER_HEARTSEEKER_AURA);
                        me->RemoveAura(SPELL_RANGER_HEARTSEEKER_CHARGE);
                        //DoCastSelf(SPELL_RANGER_HEARTSEEKER_AURA);
                        Talk(SAY_ANNOUNCE_RUIN);
                    });
            }
            break;
        }



        case ACTION_START_PHASE_TWO_ONE:
        {



            scheduler.Schedule(1ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_RANGER_HEARTSEEKER_AURA, true);

                });




            scheduler.Schedule(7s + 500ms, [this](TaskContext /*task*/)
                {
                    events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 1s + 500ms, 1, PHASE_TWO);

                });



            scheduler.Schedule(20s + 500ms, [this](TaskContext /*task*/)
                {
                    events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_P2_2, 500ms, 1, PHASE_TWO);
                });



            scheduler.Schedule(29s + 800ms, [this](TaskContext /*task*/)
                {

                    events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 1s + 500ms, 1, PHASE_TWO);

                });

            scheduler.Schedule(37s + 800ms, [this](TaskContext /*task*/)
                {

                    _rangerShotOnCD = true;

                });

            scheduler.Schedule(39s + 1500ms, [this](TaskContext /*task*/)
                {
                    _specialEvents.ScheduleEvent(EVENT_WAILING_HOWL, 1ms + 500ms, 1, PHASE_TWO);


                });

            scheduler.Schedule(47s + 500ms, [this](TaskContext /*task*/)
                {
                    DoAction(ACTION_HAUNTING_WAVE_THIRD_CHAIN);
                    
                });

            scheduler.Schedule(74s + 500ms, [this](TaskContext /*task*/)
                {
                    me->AttackStop();
                    _rangerShotOnCD = true;
                    me->SetReactState(REACT_PASSIVE);
                    events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_P2_2, 500ms, 1, PHASE_TWO);

                });

            scheduler.Schedule(81s, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                    DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 26000));


                });

            scheduler.Schedule(81s, [this](TaskContext /*task*/)
                {

                    me->SendPlayOrphanSpellVisual(SylvanasWitheringP2Final, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                    me->NearTeleportTo(SylvanasWitheringP2Final, false);

                });




            scheduler.Schedule(84s, [this](TaskContext /*task*/)
                {
                    SetCombatMovement(false);

                    me->AttackStop();
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_AURA);
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_CHARGE);
                    me->RemoveAura(SPELL_BANSHEE_FORM);
                    me->RemoveAura(SPELL_BANSHEE_SHROUD);
                    me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
                    me->SetPlayHoverAnim(false);
                    me->SetAnimTier(AnimTier::Ground);
                    _rangerShotOnCD = true;
                    events.SetPhase(PHASE_TWO);
                    events.ScheduleEvent(EVENT_WITHERING_FIRE, 3s + 500ms, 1, PHASE_TWO);

                });


            break;
        }

        case ACTION_START_PHASE_TWO_TWO:
        {



            scheduler.Schedule(50ms + 50ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_RANGER_HEARTSEEKER_AURA, true);
                    

                });


            scheduler.Schedule(2s + 1500ms, [this](TaskContext /*task*/)
                {
                    events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 1s + 500ms, 1, PHASE_TWO);

                });


            scheduler.Schedule(10s + 1000ms, [this](TaskContext /*task*/)
                {
                    _specialEvents.ScheduleEvent(EVENT_HAUNTING_WAVE_COMBAT, 50ms + 500ms, 1, PHASE_TWO);
                });


            scheduler.Schedule(22s + 500ms, [this](TaskContext /*task*/)
                {
                    events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_P2_3, 500ms, 1, PHASE_TWO);
                });



            scheduler.Schedule(25s + 800ms, [this](TaskContext /*task*/)
                {

                    events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 1s + 500ms, 1, PHASE_TWO);

                });

            scheduler.Schedule(40s + 800ms, [this](TaskContext /*task*/)
                {
                    _specialEvents.ScheduleEvent(EVENT_WAILING_HOWL, 1ms + 500ms, 1, PHASE_TWO);


                });

            scheduler.Schedule(43s + 1100ms, [this](TaskContext /*task*/)
                {

                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, FinalBridgeIntermission1[0], TEMPSUMMON_MANUAL_DESPAWN);
                    //me->SummonCreature(NPC_MAWSWORN_VANGUARD, FinalBridgeIntermission1[1], TEMPSUMMON_MANUAL_DESPAWN);
                    //me->SummonCreature(NPC_MAWSWORN_VANGUARD, FinalBridgeIntermission1[2], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, FinalBridgeIntermission1[3], TEMPSUMMON_MANUAL_DESPAWN);

                });

            scheduler.Schedule(45s + 1100ms, [this](TaskContext /*task*/)
                {

                    //me->SummonCreature(NPC_MAWSWORN_HOPEBREAKER, FinalBridgeIntermission2[0], TEMPSUMMON_MANUAL_DESPAWN);
                    //me->SummonCreature(NPC_MAWSWORN_HOPEBREAKER, FinalBridgeIntermission2[2], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_HOPEBREAKER, FinalBridgeIntermission2[4], TEMPSUMMON_MANUAL_DESPAWN);
                    //me->SummonCreature(NPC_MAWSWORN_HOPEBREAKER, FinalBridgeIntermission2[7], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, FinalBridgeIntermission2[1], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, FinalBridgeIntermission2[3], TEMPSUMMON_MANUAL_DESPAWN);
                    //me->SummonCreature(NPC_MAWSWORN_VANGUARD, FinalBridgeIntermission2[5], TEMPSUMMON_MANUAL_DESPAWN);
                    //me->SummonCreature(NPC_MAWSWORN_VANGUARD, FinalBridgeIntermission2[6], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, FinalBridgeIntermission2[8], TEMPSUMMON_MANUAL_DESPAWN);
                    //me->SummonCreature(NPC_MAWSWORN_VANGUARD, FinalBridgeIntermission2[9], TEMPSUMMON_MANUAL_DESPAWN);
                });

            break;
        }

        case ACTION_HAUNTING_WAVE_THIRD_CHAIN:
        {
            if (_hauntingWaveTimes < 6)
            {
                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                        DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 6000));
                        me->AttackStop();
                        _rangerShotOnCD = true;
                    });

                scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                    {
                        me->SendPlayOrphanSpellVisual(SylvanasWavePos[_hauntingWaveTimes], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                    });

                scheduler.Schedule(750ms, [this](TaskContext /*task*/)
                    {
                        me->NearTeleportTo(SylvanasWavePos[_hauntingWaveTimes], false);
                        me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_02);
                    });

                scheduler.Schedule(1s, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_HAUNTING_WAVE, false);

                        if (_hauntingWaveTimes == 0)
                            Talk(SAY_ANNOUNCE_HAUNTING_WAVE);
                    });

                scheduler.Schedule(5s + 500ms, [this](TaskContext /*task*/)
                    {
                        _hauntingWaveTimes++;

                        DoAction(ACTION_HAUNTING_WAVE_THIRD_CHAIN);

                    });
            }
            else
            {
                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                        DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 6000));
                    });

                scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                    {
                        me->SendPlayOrphanSpellVisual(SylvanasPhase2PrePos2, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                    });

                scheduler.Schedule(750ms, [this](TaskContext /*task*/)
                    {
                        me->NearTeleportTo(SylvanasPhase2PrePos2, false);
                        me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_02);

                        if (Creature* mawforgedGoliath = me->SummonCreature(NPC_MAWFORGED_GOLIATH, FirstBridge[1], TEMPSUMMON_MANUAL_DESPAWN))
                        {

                        }

                        if (Creature* soulJudge = me->SummonCreature(NPC_MAWFORGED_SOULJUDGE, FirstBridge[0], TEMPSUMMON_MANUAL_DESPAWN))
                        {

                        }

                        me->SummonCreature(NPC_MAWSWORN_VANGUARD, FirstBridge[2], TEMPSUMMON_MANUAL_DESPAWN);
                        me->SummonCreature(NPC_MAWSWORN_HOPEBREAKER, FirstBridge[3], TEMPSUMMON_MANUAL_DESPAWN);
                        me->SummonCreature(NPC_MAWSWORN_VANGUARD, FirstBridge[4], TEMPSUMMON_MANUAL_DESPAWN);
                        me->SummonCreature(NPC_MAWSWORN_VANGUARD, FirstBridge[5], TEMPSUMMON_MANUAL_DESPAWN);
                        me->SummonCreature(NPC_MAWSWORN_VANGUARD, FirstBridge[6], TEMPSUMMON_MANUAL_DESPAWN);
                        me->SummonCreature(NPC_MAWSWORN_HOPEBREAKER, FirstBridge[7], TEMPSUMMON_MANUAL_DESPAWN);
                        me->SummonCreature(NPC_MAWSWORN_VANGUARD, FirstBridge[8], TEMPSUMMON_MANUAL_DESPAWN);





                    });

                scheduler.Schedule(1s, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                        DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 26000));


                    });

                scheduler.Schedule(1250ms, [this](TaskContext /*task*/)
                    {

                        me->SendPlayOrphanSpellVisual(SylvanasWitheringP2, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                        me->NearTeleportTo(SylvanasWitheringP2, false);

                    });




                scheduler.Schedule(2s, [this](TaskContext /*task*/)
                    {
                        SetCombatMovement(false);
                        me->AttackStop();
                        me->RemoveAura(SPELL_RANGER_HEARTSEEKER_AURA);
                        me->RemoveAura(SPELL_RANGER_HEARTSEEKER_CHARGE);
                        me->RemoveAura(SPELL_BANSHEE_FORM);
                        me->RemoveAura(SPELL_BANSHEE_SHROUD);
                        me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
                        me->SetPlayHoverAnim(false);
                        me->SetAnimTier(AnimTier::Ground);
                        _rangerShotOnCD = true;
                        events.SetPhase(PHASE_TWO);
                        events.ScheduleEvent(EVENT_WITHERING_FIRE, 3s + 500ms, 1, PHASE_TWO);



                    });

            }
            break;
        }

        case ACTION_SECOND_WAVE_VEIL:
        {
            scheduler.Schedule(30s + 500ms, [this](TaskContext /*task*/)
                {
                    me->AttackStop();
                    _rangerShotOnCD = true;
                    me->SetReactState(REACT_PASSIVE);
                    events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_P2_3, 500ms, 1, PHASE_TWO);

                });

            scheduler.Schedule(37s, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                    DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 26000));


                });

            scheduler.Schedule(37s, [this](TaskContext /*task*/)
                {

                    me->SendPlayOrphanSpellVisual(SylvanasWitheringP2Final, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                    me->NearTeleportTo(SylvanasWitheringP2Final, false);

                });

            scheduler.Schedule(37s, [this](TaskContext /*task*/)
                {
                    SetCombatMovement(false);

                    me->AttackStop();
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_AURA);
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_CHARGE);
                    me->RemoveAura(SPELL_BANSHEE_FORM);
                    me->RemoveAura(SPELL_BANSHEE_SHROUD);
                    me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
                    me->SetPlayHoverAnim(false);
                    me->SetAnimTier(AnimTier::Ground);
                    _rangerShotOnCD = true;
                    events.SetPhase(PHASE_TWO);
                    events.ScheduleEvent(EVENT_WITHERING_FIRE, 3s + 500ms, 1, PHASE_TWO);



                });

            break;
        }

        case ACTION_RUIN_2:
        {
            events.CancelEvent(EVENT_WITHERING_FIRE);
            scheduler.Schedule(6500ms, [this](TaskContext /*task*/)
                {
                    me->AttackStop();
                    _rangerShotOnCD = true;
                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                    DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 10000));
                });

            scheduler.Schedule(6750ms, [this](TaskContext /*task*/)
                {
                    me->SendPlayOrphanSpellVisual(SylvanasRuin2, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                });


            scheduler.Schedule(7000ms, [this](TaskContext /*task*/)
                {
                    me->NearTeleportTo(SylvanasRuin2, false);
                    me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_02);
                    me->SetReactState(REACT_PASSIVE);
                });


            scheduler.Schedule(10500ms, [this](TaskContext /*task*/)
                {
                    me->SetCanFly(false);
                    me->SetDisableGravity(false);
                    
                    DoCastSelf(SPELL_RUIN, false);
                    _ruinCastTimes++;
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_AURA);
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_CHARGE);
                    me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);
                    DoCastSelf(SPELL_BANSHEE_SHROUD, true);
                    //Talk(SAY_ANNOUNCE_RUIN);
                });

            break;
        }

        case ACTION_HAUNTING_WAVE_FOURTH_CHAIN:
        {
            events.CancelEvent(EVENT_WITHERING_FIRE);
            me->AttackStop();
            _rangerShotOnCD = true;
            scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                    DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 6000));
                });

            scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                {
                    me->SendPlayOrphanSpellVisual(SylvanasPhase2PrePos3, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                });

            scheduler.Schedule(750ms, [this](TaskContext /*task*/)
                {
                    me->NearTeleportTo(SylvanasPhase2PrePos3, false);
                    me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_02);

                    if (Creature* mawforgedGoliath = me->SummonCreature(NPC_MAWFORGED_SUMMONER, SecondBridge[0], TEMPSUMMON_MANUAL_DESPAWN))
                    {

                    }

                    if (Creature* soulJudge = me->SummonCreature(NPC_MAWFORGED_GOLIATH, SecondBridge[1], TEMPSUMMON_MANUAL_DESPAWN))
                    {

                    }

                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, SecondBridge[4], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, SecondBridge[5], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, SecondBridge[6], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_HOPEBREAKER, SecondBridge[3], TEMPSUMMON_MANUAL_DESPAWN);


                });

            scheduler.Schedule(2s, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                    DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 26000));


                });

            scheduler.Schedule(2250ms, [this](TaskContext /*task*/)
                {

                    me->SendPlayOrphanSpellVisual(SylvanasWitheringP2Final, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                    me->NearTeleportTo(SylvanasWitheringP2Final, false);

                });




            scheduler.Schedule(3s, [this](TaskContext /*task*/)
                {
                    SetCombatMovement(false);

                    me->AttackStop();
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_AURA);
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_CHARGE);
                    me->RemoveAura(SPELL_BANSHEE_FORM);
                    me->RemoveAura(SPELL_BANSHEE_SHROUD);
                    me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
                    me->SetPlayHoverAnim(false);
                    me->SetAnimTier(AnimTier::Ground);
                    _rangerShotOnCD = true;
                    events.SetPhase(PHASE_TWO);
                    events.ScheduleEvent(EVENT_WITHERING_FIRE, 3s + 500ms, 1, PHASE_TWO);



                });


            break;
        }


        case ACTION_RUIN_3:
        {
            events.CancelEvent(EVENT_WITHERING_FIRE);
            scheduler.Schedule(6500ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                    DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 7000));
                });

            scheduler.Schedule(6750ms, [this](TaskContext /*task*/)
                {
                    me->SendPlayOrphanSpellVisual(SylvanasRuin3, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                });


            scheduler.Schedule(7000ms, [this](TaskContext /*task*/)
                {
                    me->NearTeleportTo(SylvanasRuin3, false);
                    me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_02);
                    me->SetReactState(REACT_PASSIVE);
                });


            scheduler.Schedule(11500ms, [this](TaskContext /*task*/)
                {
                    me->SetCanFly(false);
                    me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);
                    me->SetDisableGravity(false);
                    DoCastSelf(SPELL_RUIN, false);
                    _ruinCastTimes++;
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_AURA);
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_CHARGE);
                    me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);
                    DoCastSelf(SPELL_BANSHEE_SHROUD, true);
                    DoCastSelf(SPELL_BANSHEE_FORM, true);
                    //Talk(SAY_ANNOUNCE_RUIN);
                });
            break;
        }




        case ACTION_HAUNTING_WAVE_FIFTH_CHAIN:
        {
            events.CancelEvent(EVENT_WITHERING_FIRE);
            scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                    DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 6000));
                });

            scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                {
                    me->SendPlayOrphanSpellVisual(SylvanasPhase2PrePos4, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                });

            scheduler.Schedule(750ms, [this](TaskContext /*task*/)
                {
                    me->NearTeleportTo(SylvanasPhase2PrePos4, false);
                    me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_02);

                    if (Creature* mawforgedGoliath = me->SummonCreature(NPC_MAWFORGED_SUMMONER, ThirdBridge[0], TEMPSUMMON_MANUAL_DESPAWN))
                    {

                    }

                    if (Creature* soulJudge = me->SummonCreature(NPC_MAWFORGED_SOULJUDGE, ThirdBridge[1], TEMPSUMMON_MANUAL_DESPAWN))
                    {

                    }

                    me->SummonCreature(NPC_MAWSWORN_HOPEBREAKER, ThirdBridge[6], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_HOPEBREAKER, ThirdBridge[7], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_HOPEBREAKER, ThirdBridge[8], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, ThirdBridge[2], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, ThirdBridge[3], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, ThirdBridge[4], TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_MAWSWORN_VANGUARD, ThirdBridge[5], TEMPSUMMON_MANUAL_DESPAWN);

                });

            scheduler.Schedule(2s, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                    DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 26000));


                });

            scheduler.Schedule(2250ms, [this](TaskContext /*task*/)
                {

                    me->SendPlayOrphanSpellVisual(SylvanasWitheringP2Final, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                    me->NearTeleportTo(SylvanasWitheringP2Final, false);

                });




            scheduler.Schedule(3s, [this](TaskContext /*task*/)
                {
                    SetCombatMovement(false);

                    me->AttackStop();
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_AURA);
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_CHARGE);
                    me->RemoveAura(SPELL_BANSHEE_FORM);
                    me->RemoveAura(SPELL_BANSHEE_SHROUD);
                    me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
                    me->SetPlayHoverAnim(false);
                    me->SetAnimTier(AnimTier::Ground);
                    _rangerShotOnCD = true;
                    events.SetPhase(PHASE_TWO);
                    events.ScheduleEvent(EVENT_WITHERING_FIRE, 3s + 500ms, 1, PHASE_TWO);



                });
            break;

        }


        case ACTION_RUIN_4:
        {
            events.CancelEvent(EVENT_WITHERING_FIRE);
            scheduler.Schedule(8500ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);
                    DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 15000));
                });

            scheduler.Schedule(8750ms, [this](TaskContext /*task*/)
                {
                    me->SendPlayOrphanSpellVisual(SylvanasRuin4, SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                });


            scheduler.Schedule(9000ms, [this](TaskContext /*task*/)
                {
                    me->NearTeleportTo(SylvanasRuin4, false);
                    me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_02);
                    me->SetReactState(REACT_PASSIVE);
                });


            scheduler.Schedule(14500ms, [this](TaskContext /*task*/)
                {
                    me->SetCanFly(false);
                    me->SetDisableGravity(false);
                    DoCastSelf(SPELL_RUIN, false);
                    _ruinCastTimes++;
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_AURA);
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_CHARGE);
                    me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);
                    DoCastSelf(SPELL_BANSHEE_SHROUD, true);
                    DoCastSelf(SPELL_BANSHEE_FORM, true);
                    //Talk(SAY_ANNOUNCE_RUIN);
                    SetCombatMovement(false);
                });
            break;
        }

        case ACTION_PREPARE_JAINA:
        {
            if (Creature* jaina = instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
            {
                if (jaina->IsAIEnabled())
                    jaina->AI()->DoAction(ACTION_JAINA_TWO_ONE);
            }
            break;
        }

        case ACTION_PREPARE_JAINA_02:
        {
            if (Creature* jaina = instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
            {
                if (jaina->IsAIEnabled())
                    jaina->AI()->DoAction(ACTION_JAINA_TWO_TWO);
            }
            break;
        }

        case ACTION_PREPARE_PHASE_THREE:
        {
            events.CancelEvent(EVENT_WITHERING_FIRE);
            me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(10000ms));
            me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
            me->GetMap()->SetZoneMusic(AREA_SANCTUM_OF_DOMINATION, MUSIC_ZOVAAL_FINAL_PHASE);

            if (Creature* jaina = instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
            {
                if (jaina->IsAIEnabled())
                    jaina->AI()->DoAction(ACTION_OPEN_PORTAL_TO_PHASE_THREE);
            }
            break;
        }

        case ACTION_INITIATE_PHASE_THREE:
        {
            me->NearTeleportTo(SylvanasPhase3PrePos, false);

            scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_TELEPORT, 0, 0);
                });

            scheduler.Schedule(275ms, [this](TaskContext /*task*/)
                {
                    me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
                    me->SetPlayHoverAnim(false);
                    me->RemoveAura(SPELL_BANSHEE_FORM);
                    me->RemoveAura(SPELL_BANSHEE_SHROUD);
                });

            scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                {
                    TeleportShadowcopiesToMe();

                    DoCastSelf(SPELL_RANGER_BOW_STANCE, true);
                });
            break;
        }



        case ACTION_START_PHASE_THREE:
        {
            DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 650));
            DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 1750));

            if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
            {
                me->SetNameplateAttachToGUID(_shadowCopyGUID[0]);

                shadowCopy->CastSpell(SylvanasPhase3Pos, SPELL_DOMINATION_CHAINS_JUMP, true);

                me->SendPlayOrphanSpellVisual(SylvanasPhase3Pos, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);
            }

            scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                {
                    Talk(SAY_START_PHASE_THREE);
                });

            scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                {
                    me->NearTeleportTo(SylvanasPhase3Pos, false);
                    me->SetNameplateAttachToGUID(ObjectGuid::Empty);

                });

            scheduler.Schedule(1s, [this](TaskContext /*task*/)
                {
                    me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));
                    me->GetInstanceScript()->DoUpdateWorldState(WORLD_STATE_SYLVANAS_ENCOUNTER_PHASE, PHASE_THREE);
                    

                    TeleportShadowcopiesToMe();
                    //me->GetMap()->SetZoneMusic(AREA_SANCTUM_OF_DOMINATION, MUSIC_THE_CRUCIBLE);
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_AURA);
                    me->RemoveAura(SPELL_RANGER_HEARTSEEKER_CHARGE);
                    me->RemoveAura(SPELL_BANSHEE_FORM);
                    me->RemoveAura(SPELL_BANSHEE_SHROUD);
                    me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
                    me->SetPlayHoverAnim(false);
                    me->SetReactState(REACT_AGGRESSIVE);
                    SetCombatMovement(true);
                    me->RemoveAura(SPELL_SYLVANAS_ROOT);
                    DoAction(ACTION_ANDUIN_READY);


                    events.SetPhase(PHASE_THREE);
                    DoCastSelf(SPELL_BANSHEE_HEARTSEEKER_AURA, true);
                    //DoCastSelf(SPELL_BANSHEE_BLADES_AURA, true);
                    me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));
                    //me->CastSpell(me, SPELL_BANSHEE_BLADES_AURA);
                    events.ScheduleEvent(EVENT_BANSHEES_FURY, 4s + 500ms, 1, PHASE_THREE);
                    events.ScheduleEvent(EVENT_WITHERING_FIRE, 6s + 2000ms, 1, PHASE_THREE);
                    events.ScheduleEvent(EVENT_BANE_ARROWS, 17s + 500ms, 1, PHASE_THREE);
                    //events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_P3_1, 28s + 50ms, 1, PHASE_THREE);
                    events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_MAIN, 28s + 50ms, 1, PHASE_THREE);
                    events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 33s, 1, PHASE_THREE);
                    //events.ScheduleEvent(EVENT_WITHERING_FIRE, 70s + 200ms, 1, PHASE_THREE);
                    events.ScheduleEvent(EVENT_RAZE, 70s, 1, PHASE_THREE);
                    instance->SetData(DATA_CENTER_PLATFORM, DONE);

                    if (Creature* goliath = instance->GetCreature(DATA_MAWFORGED_GOLIATH))
                        goliath->DespawnOrUnsummon();

                    if (Creature* souljudge = instance->GetCreature(DATA_MAWFORGED_SOULJUDGE))
                        souljudge->DespawnOrUnsummon();

                    if (Creature* summoner = instance->GetCreature(DATA_MAWFORGED_SUMMONER))
                        summoner->DespawnOrUnsummon();

                    if (Creature* hopebreaker = instance->GetCreature(DATA_MAWSWORN_HOPEBREAKER))
                        hopebreaker->DespawnOrUnsummon();

                    if (Creature* vanguard = instance->GetCreature(DATA_MAWSWORN_VANGUARD))
                        vanguard->DespawnOrUnsummon();

                    //for (uint8 i = 0; i < 10; i++)
                        //me->SummonCreature(NPC_SYLVANAS_SHADOW_COPY_FIGHTERS, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);

                    // We need a separated event handler for this because Wailing Arrow is triggered even if Sylvanas is casting
                    _specialEvents.SetPhase(PHASE_THREE);
                    //_specialEvents.ScheduleEvent(EVENT_BANSHEE_WEAPONS, 7s, 1, PHASE_THREE);
                    _specialEvents.ScheduleEvent(EVENT_WAILING_ARROW, 55s, 1, PHASE_THREE);
                    _specialEvents.ScheduleEvent(EVENT_RAZE, 71s + 500ms, 1, PHASE_THREE);
                    //_specialEvents.CancelEvent(EVENT_WAILING_ARROW_MARKER);


                    //_specialEvents.ScheduleEvent(EVENT_WAILING_ARROW_MARKER_PHASE_THREE, 63s, 1, PHASE_THREE);
                });
            break;
        }

        default:
            break;
        }
    }



    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;



        _specialEvents.Update(diff);

        while (uint32 specialEventId = _specialEvents.ExecuteEvent())
        {
            switch (specialEventId)
            {
            case EVENT_WAILING_ARROW:
            {

                std::list<Player*> everyPlayerButCurrentTank;
                GetPlayerListInGrid(everyPlayerButCurrentTank, me, 250.0f);

                if (Unit* currentTank = SelectTarget(SelectTargetMethod::MaxThreat, 0, 250.0f, true, true))
                {
                    ObjectGuid arrowTargetGUID = currentTank->GetGUID();

                    Talk(SAY_ANNOUNCE_WAILING_ARROW_AFFECTED, currentTank);

                    me->CastSpell(currentTank, SPELL_WAILING_ARROW_POINTER, true);

                    if (Player* currentTankToPlayer = currentTank->ToPlayer())
                        everyPlayerButCurrentTank.remove(currentTankToPlayer);

                    if (events.IsInPhase(PHASE_ONE))
                        for (Unit* nonTank : everyPlayerButCurrentTank)
                            Talk(SAY_ANNOUNCE_WAILING_ARROW, nonTank);
                    _wailingArrowCastTimes++;

                    uint32 timerForWailingArrow;

                    switch (_wailingArrowCastTimes)
                    {
                    case 1:
                        timerForWailingArrow = 38.0;
                        break;
                    case 2:
                        timerForWailingArrow = 33.3;
                        break;
                    case 3:
                        timerForWailingArrow = 33.7;
                        break;
                    case 4:
                        timerForWailingArrow = 37.7;
                        break;
                    case 5:
                        timerForWailingArrow = 31.7;
                        break;
                    default:
                        break;
                    }

                    _specialEvents.ScheduleEvent(EVENT_WAILING_ARROW, Seconds(timerForWailingArrow), PHASE_ONE);




                    scheduler.Schedule(events.IsInPhase(PHASE_ONE) ? 5s + 500ms : 6s, [this](TaskContext /*task*/)
                        {
                            me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(750ms));

                            if (!me->HasAura(SPELL_RANGER_BOW_STANCE))
                                DoCastSelf(SPELL_RANGER_BOW_STANCE, false);
                        });

                    scheduler.Schedule(events.IsInPhase(PHASE_ONE) ? 6s + 1000ms : 7s, [this, arrowTargetGUID](TaskContext /*task*/)
                        {
                            Talk(SAY_WAILING_ARROW);

                            if (Player* target = ObjectAccessor::GetPlayer(*me, arrowTargetGUID))
                                me->CastSpell(target, SPELL_WAILING_ARROW, false);


                        });

                    if (events.IsInPhase(PHASE_ONE))
                    {
                        scheduler.Schedule(9s, [this](TaskContext /*task*/)
                            {
                                me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));
                            });
                    }
                }

                if (events.IsInPhase(PHASE_THREE))
                {
                    Trinity::Containers::RandomResize(everyPlayerButCurrentTank, 2);

                    Unit* firstRandomPlayer = everyPlayerButCurrentTank.front();
                    Unit* secondRandomPlayer = everyPlayerButCurrentTank.back();

                    ObjectGuid arrowfirstRandomTargetGUID = firstRandomPlayer->GetGUID();
                    ObjectGuid arrowsecondRandomTargetGUID = secondRandomPlayer->GetGUID();

                    scheduler.Schedule(3s, [this, arrowfirstRandomTargetGUID](TaskContext /*task*/)
                        {
                            if (Player* target = ObjectAccessor::GetPlayer(*me, arrowfirstRandomTargetGUID))
                            {
                                Talk(SAY_ANNOUNCE_WAILING_ARROW_AFFECTED, target);

                                me->CastSpell(target, SPELL_WAILING_ARROW_POINTER, true);

                                scheduler.Schedule(7s, [this, arrowfirstRandomTargetGUID](TaskContext /*task*/)
                                    {
                                        if (Player* target = ObjectAccessor::GetPlayer(*me, arrowfirstRandomTargetGUID))
                                            me->CastSpell(target, SPELL_WAILING_ARROW, false);
                                    });
                            }
                        });

                    scheduler.Schedule(6s, [this, arrowsecondRandomTargetGUID](TaskContext /*task*/)
                        {
                            if (Player* target = ObjectAccessor::GetPlayer(*me, arrowsecondRandomTargetGUID))
                            {
                                Talk(SAY_ANNOUNCE_WAILING_ARROW_AFFECTED, target);

                                me->CastSpell(target, SPELL_WAILING_ARROW_POINTER, true);

                                scheduler.Schedule(7s, [this, arrowsecondRandomTargetGUID](TaskContext /*task*/)
                                    {
                                        if (Player* target = ObjectAccessor::GetPlayer(*me, arrowsecondRandomTargetGUID))
                                            me->CastSpell(target, SPELL_WAILING_ARROW, false);
                                    });

                                scheduler.Schedule(9s, [this](TaskContext /*task*/)
                                    {
                                        me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));

                                        // TODO: this timer is wrong, find correct one.
                                        //_specialEvents.ScheduleEvent(EVENT_WAILING_ARROW, 33s, PHASE_THREE);
                                    });
                            }
                        });
                }

                break;
            }


            case EVENT_WAILING_HOWL:
            {
                scheduler.Schedule(200ms + 400ms, [this](TaskContext /*task*/)
                    {
                        me->InterruptNonMeleeSpells(true);
                        DoCastSelf(SPELL_BANSHEE_WAIL, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 5000));
                        Talk(SAY_ANNOUNCE_BANSHEE_WAIL);
                        Talk(SAY_ANNOUNCE_BANSHEE_WAIL_P2);
                    });



                scheduler.Schedule(4200ms + 400ms, [this](TaskContext /*task*/)
                    {
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_TRANSFORM_INTO_BANSHEE, 0, 0);

                    });
                break;


            }

            case EVENT_HAUNTING_WAVE_COMBAT:
            {

                        me->InterruptNonMeleeSpells(true);
                        Talk(SAY_VEIL_OF_DARKNESS_PHASE_TWO_AND_THREE);
                        DoCastSelf(SPELL_HAUNTING_WAVE, false);

                    
                
                break;
            }

            case EVENT_RAZE:
            {
                me->InterruptNonMeleeSpells(true);
                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(23000ms));
                DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 12000));
                events.ScheduleEvent(EVENT_WITHERING_FIRE, 1ms + 2ms, 1, PHASE_THREE);
                me->SetReactState(REACT_PASSIVE);

                events.CancelEvent(EVENT_BANSHEES_FURY);
                events.CancelEvent(EVENT_SHADOW_DAGGERS);
                events.CancelEvent(EVENT_WITHERING_FIRE);
                events.CancelEvent(EVENT_VEIL_OF_DARKNESS_MAIN);
                events.CancelEvent(EVENT_BANE_ARROWS);
                events.CancelEvent(EVENT_BANSHEE_SCREAM);
                _specialEvents.CancelEvent(EVENT_WAILING_ARROW);


                events.ScheduleEvent(EVENT_BANSHEE_SCREAM, 10s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_BANSHEE_SCREAM_2_2, 58s + 500ms, PHASE_THREE);
                //events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_P3_2, 18s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_MAIN2, 18s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_BANE_ARROWS, 22s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_BANSHEES_FURY_03, 33s + 50ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 39s + 50ms, PHASE_THREE);
                //events.ScheduleEvent(EVENT_WITHERING_FIRE, 15s + 2000ms, 1, PHASE_THREE);

                _specialEvents.ScheduleEvent(EVENT_WAILING_ARROW, 38s + 500ms, 1, PHASE_THREE);
                _specialEvents.ScheduleEvent(EVENT_RAZE_02, 73500ms + 500ms, 1, PHASE_THREE);














                me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_DARK_FOG, 0, 0);
                scheduler.Schedule(50ms, [this](TaskContext /*task*/)
                    {
                        me->CastSpell(GetMiddlePointInCurrentPlatform(), SPELL_RAZE, false);

                    });

                scheduler.Schedule(6100ms + 50ms, [this](TaskContext /*task*/)
                    {
                        for (uint8 covenentPlaform = 0; covenentPlaform < 4; covenentPlaform++)
                        {
                            if (me->IsWithinBox(CovenantPlatformPos[covenentPlaform][DATA_MIDDLE_POS_OUTTER_PLATFORM], 14.0f, 14.0f, 14.0f))
                                DesecrateCurrentPlatform(covenentPlaform);

                        }

                        if (AreaTrigger* areaTrigger = me->GetAreaTrigger(SPELL_INVIGORATING_FIELD_ACTIVATE))
                            areaTrigger->Remove();
                        if (AreaTrigger* areaTrigger = me->GetAreaTrigger(SPELL_INVIGORATING_FIELD_ACTIVATE))
                            areaTrigger->Remove();
                        if (AreaTrigger* areaTrigger = me->GetAreaTrigger(SPELL_INVIGORATING_FIELD_ACTIVATE))
                            areaTrigger->Remove();

                        

                        if (AreaTrigger* areaTrigger = me->GetAreaTrigger(10))
                            areaTrigger->Remove();
                    });


                scheduler.Schedule(6200ms + 50ms, [this](TaskContext /*task*/)
                    {

                        me->NearTeleportTo(SylvanasVeilThreePos, false);
                        me->RemoveAurasDueToSpell(SPELL_WINDRUNNER_DISAPPEAR_02);                        


                    });

                scheduler.Schedule(8000ms + 300ms, [this](TaskContext /*task*/)
                    {
                        me->NearTeleportTo(Platform2Center, false);
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_SWITCH_PLATFORM, 0, 0);


                    });

                scheduler.Schedule(8000ms + 300ms, [this](TaskContext /*task*/)
                    {
                        me->NearTeleportTo(Platform2Center, false);
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_SWITCH_PLATFORM, 0, 0);


                    });






                break;
            }

            case EVENT_RAZE_02:
            {
                me->InterruptNonMeleeSpells(true);
                DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 12000));
                me->SetReactState(REACT_PASSIVE);

                events.CancelEvent(EVENT_BANSHEES_FURY_03);
                events.CancelEvent(EVENT_SHADOW_DAGGERS);
                events.CancelEvent(EVENT_WITHERING_FIRE);
                events.CancelEvent(EVENT_BANSHEE_SCREAM);
                events.CancelEvent(EVENT_BANE_ARROWS);
                events.CancelEvent(EVENT_VEIL_OF_DARKNESS_MAIN2);
                events.ScheduleEvent(EVENT_WITHERING_FIRE, 1ms + 2ms, 1, PHASE_THREE);
                _specialEvents.CancelEvent(EVENT_WAILING_ARROW);

                events.ScheduleEvent(EVENT_BANSHEE_SCREAM_2_3, 38s + 500ms, PHASE_THREE);
                //events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_P3_3, 53s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_MAIN3, 53s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_BANE_ARROWS, 22s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_BANSHEES_FURY_04, 11s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 46s + 500ms, PHASE_THREE);

                _specialEvents.ScheduleEvent(EVENT_WAILING_ARROW, 19s + 50ms, PHASE_THREE);
                _specialEvents.ScheduleEvent(EVENT_RAZE_03, 71s + 500ms, 1, PHASE_THREE);





                me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_DARK_FOG, 0, 0);
                scheduler.Schedule(50ms, [this](TaskContext /*task*/)
                    {
                        me->CastSpell(GetMiddlePointInCurrentPlatform(), SPELL_RAZE, false);

                    });

                scheduler.Schedule(6100ms + 50ms, [this](TaskContext /*task*/)
                    {
                        for (uint8 covenentPlaform = 1; covenentPlaform < 4; covenentPlaform++)
                        {
                            if (me->IsWithinBox(CovenantPlatformPos[covenentPlaform][DATA_MIDDLE_POS_OUTTER_PLATFORM], 14.0f, 14.0f, 14.0f))
                                DesecrateCurrentPlatform(covenentPlaform);

                        }

                        if (AreaTrigger* areaTrigger = me->GetAreaTrigger(SPELL_INVIGORATING_FIELD_ACTIVATE))
                            areaTrigger->Remove();
                        if (AreaTrigger* areaTrigger = me->GetAreaTrigger(SPELL_INVIGORATING_FIELD_ACTIVATE))
                            areaTrigger->Remove();

                    });


                scheduler.Schedule(6200ms + 50ms, [this](TaskContext /*task*/)
                    {

                        me->NearTeleportTo(Platform2Sky, false);
                        me->RemoveAurasDueToSpell(SPELL_WINDRUNNER_DISAPPEAR_02);


                    });

                scheduler.Schedule(8000ms + 300ms, [this](TaskContext /*task*/)
                    {
                        me->NearTeleportTo(Platform3Center, false);
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_SWITCH_PLATFORM, 0, 0);


                    });

                break;


            }

            case EVENT_RAZE_03:
            {
                me->InterruptNonMeleeSpells(true);
                DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 12000));
                me->SetReactState(REACT_PASSIVE);

                events.CancelEvent(EVENT_BANSHEES_FURY_04);
                events.CancelEvent(EVENT_SHADOW_DAGGERS);
                events.CancelEvent(EVENT_WITHERING_FIRE);
                events.CancelEvent(EVENT_BANSHEE_SCREAM);
                events.CancelEvent(EVENT_BANE_ARROWS);
                events.CancelEvent(EVENT_BANE_ARROWS);
                events.CancelEvent(EVENT_VEIL_OF_DARKNESS_MAIN3);
                _specialEvents.CancelEvent(EVENT_WAILING_ARROW);
                events.ScheduleEvent(EVENT_WITHERING_FIRE, 1ms + 2ms, 1, PHASE_THREE);
                events.ScheduleEvent(EVENT_BANSHEE_SCREAM, 18s + 500ms, PHASE_THREE);
                //events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_P3_4, 42s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_MAIN4, 42s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_BANE_ARROWS, 27s + 50ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_BANSHEES_FURY_05, 36s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 48s + 500ms, PHASE_THREE);

                _specialEvents.ScheduleEvent(EVENT_WAILING_ARROW, 2s, 1, PHASE_THREE);
                _specialEvents.ScheduleEvent(EVENT_RAZE_04, 81s, 1, PHASE_THREE);



                me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_DARK_FOG, 0, 0);
                scheduler.Schedule(50ms, [this](TaskContext /*task*/)
                    {
                        me->CastSpell(GetMiddlePointInCurrentPlatform(), SPELL_RAZE, false);

                    });

                scheduler.Schedule(6100ms + 50ms, [this](TaskContext /*task*/)
                    {
                        for (uint8 covenentPlaform = 2; covenentPlaform < 4; covenentPlaform++)
                        {
                            if (me->IsWithinBox(CovenantPlatformPos[covenentPlaform][DATA_MIDDLE_POS_OUTTER_PLATFORM], 14.0f, 14.0f, 14.0f))
                                DesecrateCurrentPlatform(covenentPlaform);

                        }

                        if (AreaTrigger* areaTrigger = me->GetAreaTrigger(SPELL_INVIGORATING_FIELD_ACTIVATE))
                            areaTrigger->Remove();
                        if (AreaTrigger* areaTrigger = me->GetAreaTrigger(SPELL_INVIGORATING_FIELD_ACTIVATE))
                            areaTrigger->Remove();
                        if (AreaTrigger* areaTrigger = me->GetAreaTrigger(SPELL_INVIGORATING_FIELD_ACTIVATE))
                            areaTrigger->Remove();

                    });


                scheduler.Schedule(6200ms + 50ms, [this](TaskContext /*task*/)
                    {

                        me->NearTeleportTo(Platform3Sky, false);
                        me->RemoveAurasDueToSpell(SPELL_WINDRUNNER_DISAPPEAR_02);


                    });

                scheduler.Schedule(8000ms + 300ms, [this](TaskContext /*task*/)
                    {
                        me->NearTeleportTo(Platform4Center, false);
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_SWITCH_PLATFORM, 0, 0);


                    });

                break;

            }

            case EVENT_RAZE_04:
            {
                me->InterruptNonMeleeSpells(true);
                DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 12000));
                me->SetReactState(REACT_PASSIVE);
                events.CancelEvent(EVENT_BANSHEES_FURY_04);
                events.CancelEvent(EVENT_SHADOW_DAGGERS);
                events.CancelEvent(EVENT_WITHERING_FIRE);
                events.CancelEvent(EVENT_BANSHEE_SCREAM);
                events.CancelEvent(EVENT_SHADOW_DAGGERS);
                events.CancelEvent(EVENT_BANE_ARROWS);
                events.CancelEvent(EVENT_VEIL_OF_DARKNESS_MAIN3);

                events.ScheduleEvent(EVENT_BANSHEE_SCREAM, 18s + 50ms, PHASE_THREE);
                //events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_P3_4, 42s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_MAIN4, 42s + 500ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_RAZE_03, 80s + 50ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_BANE_ARROWS, 25s + 50ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_BANSHEES_FURY_06, 35s + 50ms, PHASE_THREE);
                events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 47s + 50ms, PHASE_THREE);


                me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_DARK_FOG, 0, 0);
                scheduler.Schedule(50ms, [this](TaskContext /*task*/)
                    {
                        me->CastSpell(GetMiddlePointInCurrentPlatform(), SPELL_RAZE, false);

                    });

                scheduler.Schedule(6100ms + 50ms, [this](TaskContext /*task*/)
                    {
                        for (uint8 covenentPlaform = 4; covenentPlaform < 4; covenentPlaform++)
                        {
                            if (me->IsWithinBox(CovenantPlatformPos[covenentPlaform][DATA_MIDDLE_POS_OUTTER_PLATFORM], 14.0f, 14.0f, 14.0f))
                                DesecrateCurrentPlatform(covenentPlaform);

                        }


                    });


                scheduler.Schedule(6200ms + 50ms, [this](TaskContext /*task*/)
                    {

                        me->NearTeleportTo(Platform4Sky, false);
                        me->RemoveAurasDueToSpell(SPELL_WINDRUNNER_DISAPPEAR_02);


                    });

                scheduler.Schedule(8000ms + 300ms, [this](TaskContext /*task*/)
                    {
                        me->NearTeleportTo(Platform4Center, false);
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_SWITCH_PLATFORM, 0, 0);


                    });

                break;

            }

            default:
                break;
            }
        }

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_WINDRUNNER:
            {
                Talk(SAY_ANNOUNCE_WINDRUNNER);

                me->AddAura(SPELL_RANGER_BOW_STANCE, me);

                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 250.0f, true, true))
                {
                    me->SetFacingToObject(target);
                }
                

                // NOTE: unsure what happens on the fifth cast, no group takes that much time to phase into intermission, we won't probably know until next expansion launches, so we're resetting to 0 just in case.
                if (_windrunnerCastTimes == 5)
                    _windrunnerCastTimes = 0;
                else
                    _windrunnerCastTimes++;

                if (_windrunnerCastTimes == 1)
                {
                   
                        DoCastSelf(SPELL_WINDRUNNER, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 11000));
                   
                    events.ScheduleEvent(EVENT_WITHERING_FIRE, 5ms, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 3500ms + 141ms, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_DESECRATING_SHOT, 3950ms + 141ms, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_WITHERING_FIRE, 7000ms + 250ms, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_DESECRATING_SHOT, 8500ms + 200ms, 2, PHASE_ONE);
                    //events.Repeat(51s);
                }
                else if (_windrunnerCastTimes == 2) // TODO: fix timers
                {
                    DoCastSelf(SPELL_WINDRUNNER, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 13000));

                    events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 3ms, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_WITHERING_FIRE, 3s + 141ms, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_DESECRATING_SHOT, 3500ms + 141ms, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_WITHERING_FIRE, 6700ms + 750ms, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_DESECRATING_SHOT, 8s + 800ms, 2, PHASE_ONE);
                    //events.Repeat(48s);
                }
                else if (_windrunnerCastTimes == 3) // TODO: fix timers
                {
                    DoCastSelf(SPELL_WINDRUNNER, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 15000));

                    events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 500ms, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_DESECRATING_SHOT, 500ms, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_WITHERING_FIRE, 5s, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_SHADOW_DAGGERS, 7s, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_DESECRATING_SHOT, 9s, 2, PHASE_ONE);
                    events.ScheduleEvent(EVENT_WITHERING_FIRE, 13s, 2, PHASE_ONE);
                    //events.Repeat(47s);
                }
                else if (_windrunnerCastTimes == 4) // TODO: fix timers
                {
                    DoCastSelf(SPELL_WINDRUNNER, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 17000));
                }
                if (_windrunnerCastTimes == 1)
                    events.Repeat(51600ms);
                else if (_windrunnerCastTimes == 2)
                    events.Repeat(50400ms);
                else if (_windrunnerCastTimes == 3)
                    events.Repeat(48600ms);
                else if (_windrunnerCastTimes == 4)
                    events.Repeat(52900ms);
                else if (_windrunnerCastTimes == 5)
                    events.Repeat(49200ms);
                break;
            }

            case EVENT_EVADE_CHECK:
                DoCheckEvade();
                events.Repeat(Seconds(5));
                break;

            case EVENT_WITHERING_FIRE:
            {
                TeleportShadowcopiesToMe();

                if (events.IsInPhase(PHASE_ONE))
                {
                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_01, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 2062));
                    DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 3800));

                    if (Creature* shadowCopy1 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))

                    {
                        if (Creature* shadowCopy2 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[1]))
                        {
                            // Number of casts is dependent on raid's difficulty and size: if mythic, 15; if not, half the raid (min. 5, max. 15)
                            uint32 witheringFires = me->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC_RAID ? 15 :
                                std::max<uint8>(5, std::ceil(float(me->GetMap()->GetPlayersCountExceptGMs()) / 2));

                            std::vector<Position> randomWitheringFirePos;
                            for (uint8 itr = 0; itr < 5; itr++)
                                randomWitheringFirePos.push_back(shadowCopy1->GetFirstCollisionPosition(frand(55.0f, 80.0f), 2.0f * float(M_PI) - frand(-0.5f, 0.5f)));

                            shadowCopy1->NearTeleportTo(randomWitheringFirePos[0].GetPositionX(), randomWitheringFirePos[0].GetPositionY(),
                                randomWitheringFirePos[0].GetPositionZ(), randomWitheringFirePos[0].GetAbsoluteAngle(me), false);

                            scheduler.Schedule(16ms, [this, shadowCopy1, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    me->SendPlayOrphanSpellVisual(randomWitheringFirePos[0], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(31ms, [this, shadowCopy1, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy1->CastSpell(randomWitheringFirePos[0], SPELL_WITHERING_FIRE_COPY, true);
                                });
                            scheduler.Schedule(219ms, [this, shadowCopy1, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy1->NearTeleportTo(randomWitheringFirePos[1].GetPositionX(), randomWitheringFirePos[1].GetPositionY(),
                                        randomWitheringFirePos[1].GetPositionZ(), randomWitheringFirePos[1].GetAbsoluteAngle(me), false);
                                    me->SendPlayOrphanSpellVisual(randomWitheringFirePos[1], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(281ms, [this, shadowCopy1, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy1->CastSpell(randomWitheringFirePos[1], SPELL_WITHERING_FIRE_COPY, true);
                                });

                            scheduler.Schedule(500ms, [this, shadowCopy1, shadowCopy2, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy1->NearTeleportTo(randomWitheringFirePos[2].GetPositionX(), randomWitheringFirePos[2].GetPositionY(),
                                        randomWitheringFirePos[2].GetPositionZ(), randomWitheringFirePos[2].GetAbsoluteAngle(me), false);
                                    me->SendPlayOrphanSpellVisual(randomWitheringFirePos[2], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);

                                    shadowCopy2->NearTeleportTo(randomWitheringFirePos[0].GetPositionX(), randomWitheringFirePos[0].GetPositionY(),
                                        randomWitheringFirePos[0].GetPositionZ(), randomWitheringFirePos[0].GetAbsoluteAngle(me), false);
                                });

                            scheduler.Schedule(560ms, [this, shadowCopy1, shadowCopy2, randomWitheringFirePos, witheringFires](TaskContext /*task*/)
                                {
                                    shadowCopy1->CastSpell(randomWitheringFirePos[2], SPELL_WITHERING_FIRE_COPY, true);

                                    shadowCopy2->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);

                                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(1))
                                        ai->WitheringFire(witheringFires < 11 ? 1 : 2);
                                });

                            scheduler.Schedule(750ms, [this, shadowCopy1, shadowCopy2, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy2->NearTeleportTo(randomWitheringFirePos[1].GetPositionX(), randomWitheringFirePos[1].GetPositionY(),
                                        randomWitheringFirePos[1].GetPositionZ(), randomWitheringFirePos[1].GetAbsoluteAngle(me), false);

                                    shadowCopy1->NearTeleportTo(randomWitheringFirePos[3].GetPositionX(), randomWitheringFirePos[3].GetPositionY(),
                                        randomWitheringFirePos[3].GetPositionZ(), randomWitheringFirePos[3].GetAbsoluteAngle(me), false);
                                    me->SendPlayOrphanSpellVisual(randomWitheringFirePos[3], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(800ms, [this, shadowCopy1, shadowCopy2, randomWitheringFirePos, witheringFires](TaskContext /*task*/)
                                {
                                    shadowCopy1->CastSpell(randomWitheringFirePos[3], SPELL_WITHERING_FIRE_COPY, true);

                                    shadowCopy2->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);

                                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(1))
                                        ai->WitheringFire(witheringFires < 11 ? 1 : 2);
                                });

                            scheduler.Schedule(850ms, [this, shadowCopy1](TaskContext /*task*/)
                                {
                                    shadowCopy1->NearTeleportTo(me->GetPosition(), false);
                                });

                            scheduler.Schedule(925ms, [this, shadowCopy1](TaskContext /*task*/)
                                {
                                    me->SetNameplateAttachToGUID(_shadowCopyGUID[0]);
                                });

                            scheduler.Schedule(950ms, [this, shadowCopy1, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy1->CastSpell(randomWitheringFirePos[2], SPELL_WINDRUNNER_MOVE, true);
                                });

                            scheduler.Schedule(1s + 20ms, [this, shadowCopy1, shadowCopy2, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy2->NearTeleportTo(randomWitheringFirePos[4].GetPositionX(), randomWitheringFirePos[4].GetPositionY(),
                                        randomWitheringFirePos[4].GetPositionZ(), randomWitheringFirePos[4].GetAbsoluteAngle(me), false);
                                    me->SendPlayOrphanSpellVisual(randomWitheringFirePos[4], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);

                                    shadowCopy1->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(1s + 70ms, [this, shadowCopy2, randomWitheringFirePos, witheringFires](TaskContext /*task*/)
                                {
                                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(1))
                                        ai->WitheringFire(witheringFires < 11 ? witheringFires - 4 : witheringFires - 8);

                                    shadowCopy2->CastSpell(randomWitheringFirePos[4], SPELL_WITHERING_FIRE_COPY, true);
                                });

                            scheduler.Schedule(1s + 112ms, [this, witheringFires](TaskContext /*task*/)
                                {
                                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                                        ai->WitheringFire(witheringFires < 11 ? 1 : 2);
                                });

                            scheduler.Schedule(1s + 219ms, [this, shadowCopy1](TaskContext /*task*/)
                                {
                                    shadowCopy1->CastSpell(me, SPELL_WINDRUNNER_MOVE, true);
                                });

                            scheduler.Schedule(1s + 266ms, [this, shadowCopy2, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy2->NearTeleportTo(randomWitheringFirePos[3].GetPositionX(), randomWitheringFirePos[3].GetPositionY(),
                                        randomWitheringFirePos[3].GetPositionZ(), randomWitheringFirePos[3].GetAbsoluteAngle(me), false);
                                    shadowCopy2->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(1s + 406ms, [this](TaskContext /*task*/)
                                {
                                    me->SetNameplateAttachToGUID(ObjectGuid::Empty);
                                });

                            scheduler.Schedule(1s + 531ms, [this, shadowCopy2, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy2->NearTeleportTo(randomWitheringFirePos[4].GetPositionX(), randomWitheringFirePos[4].GetPositionY(),
                                        randomWitheringFirePos[4].GetPositionZ(), randomWitheringFirePos[4].GetAbsoluteAngle(me), false);
                                    shadowCopy2->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(1s + 600ms, [this, witheringFires](TaskContext /*task*/)
                                {
                                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(1))
                                        ai->WitheringFire(witheringFires < 11 ? 1 : 2);
                                });

                            scheduler.Schedule(1s + 650ms, [this, shadowCopy2](TaskContext /*task*/)
                                {
                                    shadowCopy2->NearTeleportTo(me->GetPosition(), false);
                                });

                            scheduler.Schedule(2s + 62ms, [this, shadowCopy2](TaskContext /*task*/)
                                {
                                    me->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);
                                });
                        }
                    }
                }


                else if (events.IsInPhase(PHASE_TWO))
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_WITHERING_FIRE_PHASE_TWO, 0, 0);

                    scheduler.Schedule(312ms, [this](TaskContext /*task*/)
                        {
                            std::list<Player*> targetList;
                            GetPlayerListInGrid(targetList, me, 500.0f);

                            Trinity::Containers::RandomResize(targetList, 4);

                            for (Player* target : targetList)
                            {
                                uint32 randomSpeed = urand(2700, 3500);

                                me->SendPlaySpellVisual(target, SPELL_VISUAL_WITHERING_FIRE_PHASE_TWO, 0, 0, float(randomSpeed / 1500), true);

                                scheduler.Schedule(Milliseconds(randomSpeed), [this, target](TaskContext /*task*/)
                                    {
                                        me->CastSpell(target, SPELL_WITHERING_FIRE, true);
                                    });
                            }
                        });
                    events.Repeat(2s);
                }
                else if (events.IsInPhase(PHASE_THREE))
                {
                    if (Creature* shadowCopy1 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))

                    {
                        if (Creature* shadowCopy2 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[1]))
                        {
                            // Number of casts is dependent on raid's difficulty and size: if mythic, 15; if not, half the raid (min. 5, max. 15)
                            uint32 witheringFires = me->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC_RAID ? 15 :
                                std::max<uint8>(12, std::ceil(float(me->GetMap()->GetPlayersCountExceptGMs()) / 2));

                            std::vector<Position> randomWitheringFirePos;
                            for (uint8 itr = 0; itr < 12; itr++)
                                randomWitheringFirePos.push_back(shadowCopy1->GetFirstCollisionPosition(frand(30.0f, 52.0f), 5.0f - frand(0.0f, 3.5f)));
                            //randomWitheringFirePos.push_back(shadowCopy1->GetFirstCollisionPosition(frand(45.0f, 70.0f), 2.0f * float(M_PI) - frand(-0.5f, 0.5f)));
                            shadowCopy1->NearTeleportTo(randomWitheringFirePos[0].GetPositionX(), randomWitheringFirePos[0].GetPositionY(),
                                randomWitheringFirePos[0].GetPositionZ(), randomWitheringFirePos[0].GetAbsoluteAngle(me), false);

                            scheduler.Schedule(16ms, [this, shadowCopy1, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    me->SendPlayOrphanSpellVisual(randomWitheringFirePos[0], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(31ms, [this, shadowCopy1, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy1->CastSpell(randomWitheringFirePos[0], SPELL_WITHERING_FIRE_COPY, true);
                                });
                            scheduler.Schedule(219ms, [this, shadowCopy1, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy1->NearTeleportTo(randomWitheringFirePos[1].GetPositionX(), randomWitheringFirePos[1].GetPositionY(),
                                        randomWitheringFirePos[1].GetPositionZ(), randomWitheringFirePos[1].GetAbsoluteAngle(me), false);
                                    me->SendPlayOrphanSpellVisual(randomWitheringFirePos[1], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(281ms, [this, shadowCopy1, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy1->CastSpell(randomWitheringFirePos[1], SPELL_WITHERING_FIRE_COPY, true);
                                });

                            scheduler.Schedule(500ms, [this, shadowCopy1, shadowCopy2, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy1->NearTeleportTo(randomWitheringFirePos[2].GetPositionX(), randomWitheringFirePos[2].GetPositionY(),
                                        randomWitheringFirePos[2].GetPositionZ(), randomWitheringFirePos[2].GetAbsoluteAngle(me), false);
                                    me->SendPlayOrphanSpellVisual(randomWitheringFirePos[2], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);

                                    shadowCopy2->NearTeleportTo(randomWitheringFirePos[0].GetPositionX(), randomWitheringFirePos[0].GetPositionY(),
                                        randomWitheringFirePos[0].GetPositionZ(), randomWitheringFirePos[0].GetAbsoluteAngle(me), false);
                                });

                            scheduler.Schedule(560ms, [this, shadowCopy1, shadowCopy2, randomWitheringFirePos, witheringFires](TaskContext /*task*/)
                                {
                                    shadowCopy1->CastSpell(randomWitheringFirePos[2], SPELL_WITHERING_FIRE_COPY, true);

                                    shadowCopy2->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);

                                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(1))
                                        ai->WitheringFire(witheringFires < 11 ? 1 : 2);
                                });

                            scheduler.Schedule(750ms, [this, shadowCopy1, shadowCopy2, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy2->NearTeleportTo(randomWitheringFirePos[1].GetPositionX(), randomWitheringFirePos[1].GetPositionY(),
                                        randomWitheringFirePos[1].GetPositionZ(), randomWitheringFirePos[1].GetAbsoluteAngle(me), false);

                                    shadowCopy1->NearTeleportTo(randomWitheringFirePos[3].GetPositionX(), randomWitheringFirePos[3].GetPositionY(),
                                        randomWitheringFirePos[3].GetPositionZ(), randomWitheringFirePos[3].GetAbsoluteAngle(me), false);
                                    me->SendPlayOrphanSpellVisual(randomWitheringFirePos[3], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(800ms, [this, shadowCopy1, shadowCopy2, randomWitheringFirePos, witheringFires](TaskContext /*task*/)
                                {
                                    shadowCopy1->CastSpell(randomWitheringFirePos[3], SPELL_WITHERING_FIRE_COPY, true);

                                    shadowCopy2->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);

                                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(1))
                                        ai->WitheringFire(witheringFires < 11 ? 1 : 2);
                                });

                            scheduler.Schedule(850ms, [this, shadowCopy1](TaskContext /*task*/)
                                {
                                    shadowCopy1->NearTeleportTo(me->GetPosition(), false);
                                });

                            scheduler.Schedule(925ms, [this, shadowCopy1](TaskContext /*task*/)
                                {
                                    //me->SetNameplateAttachToGUID(_shadowCopyGUID[0]);
                                });

                            scheduler.Schedule(950ms, [this, shadowCopy1, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy1->CastSpell(randomWitheringFirePos[2], SPELL_WINDRUNNER_MOVE, true);
                                });

                            scheduler.Schedule(1s + 20ms, [this, shadowCopy1, shadowCopy2, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy2->NearTeleportTo(randomWitheringFirePos[4].GetPositionX(), randomWitheringFirePos[4].GetPositionY(),
                                        randomWitheringFirePos[4].GetPositionZ(), randomWitheringFirePos[4].GetAbsoluteAngle(me), false);
                                    me->SendPlayOrphanSpellVisual(randomWitheringFirePos[4], SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);

                                    //shadowCopy1->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(1s + 70ms, [this, shadowCopy2, randomWitheringFirePos, witheringFires](TaskContext /*task*/)
                                {
                                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(1))
                                        ai->WitheringFire(witheringFires < 11 ? witheringFires - 4 : witheringFires - 8);

                                    shadowCopy2->CastSpell(randomWitheringFirePos[4], SPELL_WITHERING_FIRE_COPY, true);
                                });

                            scheduler.Schedule(1s + 112ms, [this, witheringFires](TaskContext /*task*/)
                                {
                                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                                        ai->WitheringFire(witheringFires < 11 ? 1 : 2);
                                });

                            scheduler.Schedule(1s + 219ms, [this, shadowCopy1](TaskContext /*task*/)
                                {
                                    //shadowCopy1->CastSpell(me, SPELL_WINDRUNNER_MOVE, true);
                                });

                            scheduler.Schedule(1s + 266ms, [this, shadowCopy2, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy2->NearTeleportTo(randomWitheringFirePos[3].GetPositionX(), randomWitheringFirePos[3].GetPositionY(),
                                        randomWitheringFirePos[3].GetPositionZ(), randomWitheringFirePos[3].GetAbsoluteAngle(me), false);
                                    //shadowCopy2->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(1s + 406ms, [this](TaskContext /*task*/)
                                {
                                    //me->SetNameplateAttachToGUID(ObjectGuid::Empty);
                                });

                            scheduler.Schedule(1s + 531ms, [this, shadowCopy2, randomWitheringFirePos](TaskContext /*task*/)
                                {
                                    shadowCopy2->NearTeleportTo(randomWitheringFirePos[4].GetPositionX(), randomWitheringFirePos[4].GetPositionY(),
                                        randomWitheringFirePos[4].GetPositionZ(), randomWitheringFirePos[4].GetAbsoluteAngle(me), false);
                                    //shadowCopy2->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_02, 0.25f, true, false);
                                });

                            scheduler.Schedule(1s + 600ms, [this, witheringFires](TaskContext /*task*/)
                                {
                                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(1))
                                        ai->WitheringFire(witheringFires < 11 ? 1 : 2);
                                });

                            scheduler.Schedule(1s + 650ms, [this, shadowCopy2](TaskContext /*task*/)
                                {
                                    shadowCopy2->NearTeleportTo(me->GetPosition(), false);
                                });

                            scheduler.Schedule(2s + 62ms, [this, shadowCopy2](TaskContext /*task*/)
                                {
                                    me->SendPlayOrphanSpellVisual(me->GetPosition(), SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);
                                });


                        }

                        events.Repeat(9s);
                    }

                }
                break;
            }

            case EVENT_CONTINUE_ATTACK:
            {

                if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                {
                    AttackStart(sylvanas);

                }

            }

            case EVENT_CHECK_INTERMISSION:
            {
                if(hasPhased == 0)
                {
                    DoCastSelf(SPELL_ACTIVATE_INTERMISSION, true);
                    events.Repeat(5s);
                }
                

                break;
            }

            case EVENT_SHADOW_DAGGERS:
            {
                if (events.IsInPhase(PHASE_ONE))
                {
                    TeleportShadowcopiesToMe();

                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_01, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 4000));
                    DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 3200));

                    uint8 randomCopy = urand(0, 4);

                    for (uint8 itr = 0; itr < 4; itr++)
                    {
                        scheduler.Schedule(Milliseconds(250 * itr), [this, itr, randomCopy](TaskContext /*task*/)
                            {
                                if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(itr))
                                {
                                    Creature* nextShadowCopy = ObjectAccessor::GetCreature(*me, GetShadowCopyJumperGuid(itr + 1));

                                    ai->StartShadowDaggersEvent(itr == randomCopy, itr, nextShadowCopy);
                                }
                            });
                    }
                }
                else if (events.IsInPhase(PHASE_TWO))
                {
                    me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(500ms));

                    std::list<Player*> targetList;
                    GetPlayerListInGrid(targetList, me, 250.0f);

                    for (Player* target : targetList)
                        me->CastSpell(target, SPELL_SHADOW_DAGGER_MISSILE, false);
                    //me->SendPlaySpellVisual(target, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);
                    //me->m_Events.AddEvent(new ShadowDaggersP2(me, target), me->m_Events.CalculateTime(1ms));

                    DoCastAOE(SPELL_SHADOW_DAGGER_MISSILE);
                    //events.Repeat(8s);
                }
                else if (events.IsInPhase(PHASE_THREE))
                {
                    DoCastSelf(SPELL_SHADOW_DAGGER_PHASE_TWO_AND_THREE, false);
                    //me->SendPlaySpellVisual(Platform1Center, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);


                    //Position arrowCenter(me->GetPositionX() + (std::cos(orientation) * distance), me->GetPositionY() + (std::sin(orientation) * distance), me->GetPositionZ());

/*
                    Position lineright1 = { me->GetPositionX() -5.0f, me->GetPositionY() - 5.0f, me->GetPositionZ(), 3.150965f };
                    Position lineright2 = { me->GetPositionX() - 10.0f, me->GetPositionY() - 5.0f, me->GetPositionZ(), 3.150965f };
                    Position lineright3 = { me->GetPositionX() - 15.0f, me->GetPositionY() - 5.0f, me->GetPositionZ(), 3.150965f };
                    Position lineright4 = { me->GetPositionX() - 20.0f, me->GetPositionY() - 5.0f, me->GetPositionZ(), 3.150965f };

                    
                    
                    Position rightmid1 = { me->GetPositionX() - 5.0f, me->GetPositionY() + 5.0f, me->GetPositionZ(), 2.353772f };
                    Position rightmid2 = { me->GetPositionX() - 10.0f, me->GetPositionY() + 10.0f, me->GetPositionZ(), 2.353772f };
                    Position rightmid3 = { me->GetPositionX() - 15.0f, me->GetPositionY() + 15.0f, me->GetPositionZ(), 2.353772f };
                    Position rightmid4 = { me->GetPositionX() - 20.0f, me->GetPositionY() + 20.0f, me->GetPositionZ(), 2.353772f };
                    
                    
                    Position liner2ight1 = { me->GetPositionX() - 5.0f, me->GetPositionY() + 5.0f, me->GetPositionZ(), 1.595873f };
                    Position liner2ight2 = { me->GetPositionX() - 5.0f, me->GetPositionY() + 10.0f, me->GetPositionZ(), 1.595873f };
                    Position liner2ight3 = { me->GetPositionX() - 5.0f, me->GetPositionY() + 15.0f, me->GetPositionZ(), 1.595873f };
                    Position liner2ight4 = { me->GetPositionX() - 5.0f, me->GetPositionY() + 20.0f, me->GetPositionZ(), 1.595873f };


                    me->SendPlaySpellVisual(lineright1, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);
                    me->SendPlaySpellVisual(lineright2, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);
                    me->SendPlaySpellVisual(lineright3, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);
                    me->SendPlaySpellVisual(lineright4, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);

                    me->SendPlaySpellVisual(rightmid1, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);
                    me->SendPlaySpellVisual(rightmid2, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);
                    me->SendPlaySpellVisual(rightmid3, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);
                    me->SendPlaySpellVisual(rightmid4, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);

                    me->SendPlaySpellVisual(liner2ight1, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);
                    me->SendPlaySpellVisual(liner2ight2, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);
                    me->SendPlaySpellVisual(liner2ight3, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);
                    me->SendPlaySpellVisual(liner2ight4, 0.0f, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);




                   

                    


                    */



                    //me->SendPlaySpellVisual(target, SPELL_VISUAL_WITHERING_FIRE_PHASE_ONE, 0, 0, 48.0f, false);
                    //events.Repeat(5s);
                }

                break;
            }

            case EVENT_DESECRATING_SHOT:
            {


                _disecratingShotCastTimes++;

                if (_windrunnerCastTimes == 1)
                    ChooseDesecratingShotPattern(_disecratingShotCastTimes == 1 ? DATA_DESECRATING_SHOT_PATTERN_SCATTERED : DATA_DESECRATING_SHOT_PATTERN_STRAIGHT);
                else if (_windrunnerCastTimes == 2)
                    ChooseDesecratingShotPattern(_disecratingShotCastTimes == 3 ? DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_1 : DATA_DESECRATING_SHOT_PATTERN_SPIRAL);
                else if (_windrunnerCastTimes == 3)
                    ChooseDesecratingShotPattern(_disecratingShotCastTimes == 5 ? DATA_DESECRATING_SHOT_PATTERN_SPIRAL : DATA_DESECRATING_SHOT_PATTERN_SCATTERED);
                else if (_windrunnerCastTimes == 4)
                    ChooseDesecratingShotPattern(_disecratingShotCastTimes == 7 ? DATA_DESECRATING_SHOT_PATTERN_SCATTERED : DATA_DESECRATING_SHOT_PATTERN_JAR);
                    

                //events.Repeat(8s);
                break;
            }

            case EVENT_DOMINATION_CHAINS:
            {
                
                _chainsCastTimes++;
                

                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
                Position const jumpFirstPos = me->GetRandomPoint(SylvanasFirstPhasePlatformCenter, frand(25.0f, 35.0f));
                Position const jumpSecondPos = me->GetRandomPoint(SylvanasFirstPhasePlatformCenter, frand(25.0f, 35.0f));
                Position const jumpThirdPos = me->GetRandomPoint(SylvanasFirstPhasePlatformCenter, frand(25.0f, 35.0f));

                if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                {
                    if (shadowCopy->IsAIEnabled())
                    {
                        shadowCopy->AI()->SetData(DATA_EVENT_TYPE_SHADOWCOPY, DATA_EVENT_COPY_DOMINATION_CHAIN_EVENT);

                        shadowCopy->AI()->DoAction(ACTION_CALCULATE_ARROWS);

                        if (events.GetPhaseMask() == PHASE_INTERMISSION)
                            
                            shadowCopy->AI()->SetData(DATA_EVENT_TYPE_SHADOWCOPY, DATA_EVENT_COPY_DOMINATION_CHAIN_BEFORE_RIVE_EVENT);
                    }

                    TeleportShadowcopiesToMe();

                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);

                    scheduler.Schedule(15ms, [this](TaskContext /*task*/)
                        {
                            DoCastSelf(SPELL_DOMINATION_CHAINS, false);

                            me->SetNameplateAttachToGUID(_shadowCopyGUID[0]);



                            _chainsCastTimes;

                            uint32 timerForChains;

                            switch (_chainsCastTimes)
                            {
                            case 1:
                                timerForChains = 53.5;
                                break;
                            case 2:
                                timerForChains = 49.6;
                                break;
                            case 3:
                                timerForChains = 51.0;
                                break;
                            case 4:
                                timerForChains = 50.0;
                                break;
                            case 5:
                                timerForChains = 53.3;
                                break;
                            default:
                                break;
                            }
                            if (events.GetPhaseMask() == PHASE_ONE)

                                events.ScheduleEvent(EVENT_DOMINATION_CHAINS, Seconds(timerForChains), PHASE_ONE);



                        });

                    scheduler.Schedule(31ms, [this, shadowCopy, jumpFirstPos](TaskContext /*task*/)
                        {
                            shadowCopy->CastSpell(jumpFirstPos, SPELL_DOMINATION_CHAINS_JUMP, true);

                            me->SendPlayOrphanSpellVisual(jumpFirstPos, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);
                        });

                    scheduler.Schedule(151ms, [this](TaskContext /*task*/)
                        {
                            Talk(SAY_ANNOUNCE_DOMINATION_CHAINS);

                            if (events.GetPhaseMask() == PHASE_ONE)
                                Talk(SAY_DOMINATION_CHAINS);
                            else
                                Talk(SAY_INTERMISSION_BEGIN);
                        });

                    scheduler.Schedule(766ms, [this, jumpFirstPos](TaskContext /*task*/)
                        {
                            me->NearTeleportTo(jumpFirstPos, true);
                        });

                    scheduler.Schedule(828ms, [this, shadowCopy, jumpSecondPos](TaskContext /*task*/)
                        {
                            shadowCopy->CastSpell(jumpSecondPos, SPELL_DOMINATION_CHAINS_JUMP, true);

                            me->SendPlayOrphanSpellVisual(jumpSecondPos, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);
                        });

                    scheduler.Schedule(1s + 578ms, [this, jumpSecondPos](TaskContext /*task*/)
                        {
                            me->NearTeleportTo(jumpSecondPos, true);
                        });

                    scheduler.Schedule(1s + 641ms, [this, shadowCopy, jumpThirdPos](TaskContext /*task*/)
                        {
                            shadowCopy->CastSpell(jumpThirdPos, SPELL_DOMINATION_CHAINS_JUMP, true);

                            me->SendPlayOrphanSpellVisual(jumpThirdPos, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);
                        });

                    scheduler.Schedule(2s + 201ms, [this, jumpThirdPos](TaskContext /*task*/)
                        {
                            me->NearTeleportTo(jumpThirdPos, true);
                        });

                    scheduler.Schedule(2s + 281ms, [this](TaskContext /*task*/)
                        {
                            TeleportShadowcopiesToMe();
                        });

                    scheduler.Schedule(2s + 500ms, [this](TaskContext /*task*/)
                        {
                            me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_02);
                            me->SetNameplateAttachToGUID(ObjectGuid::Empty);
                        });

                    scheduler.Schedule(7s + 500ms, [this](TaskContext /*task*/)
                        {
                            if (events.GetPhaseMask() == PHASE_ONE)
                            {
                                me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));

                                for (ObjectGuid const& copiesGUID : _shadowCopyGUID)
                                {
                                    if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, copiesGUID))
                                    {
                                        if (shadowCopy->IsAIEnabled())
                                            shadowCopy->AI()->SetData(DATA_EVENT_TYPE_SHADOWCOPY, DATA_EVENT_COPY_NO_EVENT);
                                    }
                                }

                                DoAction(ACTION_RESET_MELEE_KIT);
                                /*
                                                                if (events.GetTimeUntilEvent(EVENT_VEIL_OF_DARKNESS) <= 2s + 500ms)
                                                                    events.RescheduleEvent(EVENT_VEIL_OF_DARKNESS, 3s + 500ms, 1, PHASE_ONE);
                                                                if (_chainsCastTimes == 1)
                                                                    events.Repeat(53s);
                                                                if (_chainsCastTimes == 2)
                                                                    events.Repeat(49s);
                                                                else
                                                                    events.Repeat(53s); */
                            }

                        });
                }
                break;
            }

            case EVENT_DOMINATION_CHAINS_TRANSITION:
            {


                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
                Position const jumpFirstPos = me->GetRandomPoint(SylvanasFirstPhasePlatformCenter, frand(25.0f, 35.0f));
                Position const jumpSecondPos = me->GetRandomPoint(SylvanasFirstPhasePlatformCenter, frand(25.0f, 35.0f));
                Position const jumpThirdPos = me->GetRandomPoint(SylvanasFirstPhasePlatformCenter, frand(25.0f, 35.0f));

                if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                {
                    if (shadowCopy->IsAIEnabled())
                    {
                            shadowCopy->AI()->SetData(DATA_EVENT_TYPE_SHADOWCOPY, DATA_EVENT_COPY_DOMINATION_CHAIN_EVENT);
                            shadowCopy->AI()->DoAction(ACTION_CALCULATE_ARROWS);
                            shadowCopy->AI()->SetData(DATA_EVENT_TYPE_SHADOWCOPY, DATA_EVENT_COPY_DOMINATION_CHAIN_BEFORE_RIVE_EVENT);
                            
                    }

                    TeleportShadowcopiesToMe();

                    DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, true);

                    scheduler.Schedule(15ms, [this](TaskContext /*task*/)
                        {
                            DoCastSelf(SPELL_DOMINATION_CHAINS, false);

                            me->SetNameplateAttachToGUID(_shadowCopyGUID[0]);

                        });

                    scheduler.Schedule(31ms, [this, shadowCopy, jumpFirstPos](TaskContext /*task*/)
                        {
                            shadowCopy->CastSpell(jumpFirstPos, SPELL_DOMINATION_CHAINS_JUMP, true);

                            me->SendPlayOrphanSpellVisual(jumpFirstPos, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);
                        });

                    scheduler.Schedule(151ms, [this](TaskContext /*task*/)
                        {
                            Talk(SAY_ANNOUNCE_DOMINATION_CHAINS);
                                Talk(SAY_INTERMISSION_BEGIN);
                        });

                    scheduler.Schedule(766ms, [this, jumpFirstPos](TaskContext /*task*/)
                        {
                            me->NearTeleportTo(jumpFirstPos, true);
                        });

                    scheduler.Schedule(828ms, [this, shadowCopy, jumpSecondPos](TaskContext /*task*/)
                        {
                            shadowCopy->CastSpell(jumpSecondPos, SPELL_DOMINATION_CHAINS_JUMP, true);

                            me->SendPlayOrphanSpellVisual(jumpSecondPos, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);
                        });

                    scheduler.Schedule(1s + 578ms, [this, jumpSecondPos](TaskContext /*task*/)
                        {
                            me->NearTeleportTo(jumpSecondPos, true);
                        });

                    scheduler.Schedule(1s + 641ms, [this, shadowCopy, jumpThirdPos](TaskContext /*task*/)
                        {
                            shadowCopy->CastSpell(jumpThirdPos, SPELL_DOMINATION_CHAINS_JUMP, true);

                            me->SendPlayOrphanSpellVisual(jumpThirdPos, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);
                        });

                    scheduler.Schedule(2s + 201ms, [this, jumpThirdPos](TaskContext /*task*/)
                        {
                            me->NearTeleportTo(jumpThirdPos, true);
                        });

                    scheduler.Schedule(2s + 281ms, [this](TaskContext /*task*/)
                        {
                            TeleportShadowcopiesToMe();
                        });

                    scheduler.Schedule(2s + 500ms, [this](TaskContext /*task*/)
                        {
                            me->RemoveAura(SPELL_WINDRUNNER_DISAPPEAR_02);
                            me->SetNameplateAttachToGUID(ObjectGuid::Empty);
                        });

                    scheduler.Schedule(7s + 500ms, [this](TaskContext /*task*/)
                        {
                                for (ObjectGuid const& copiesGUID : _shadowCopyGUID)
                                {
                                    if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, copiesGUID))
                                    {
                                        if (shadowCopy->IsAIEnabled())
                                            shadowCopy->AI()->SetData(DATA_EVENT_TYPE_SHADOWCOPY, DATA_EVENT_COPY_RIVE_EVENT);
                                    }
                                }

                                events.ScheduleEvent(EVENT_RIVE, 1s, PHASE_INTERMISSION);
                                SetCombatMovement(false);
                            
                        });
                }
                break;
            }

            case EVENT_VEIL_OF_DARKNESS:
            {
                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
                DoCastSelf(SPELL_RANGER_BOW_STANCE, false);
                if (events.IsInPhase(PHASE_ONE))
                {
                    if (!me->HasAura(SPELL_RANGER_BOW_STANCE))
                        DoCastSelf(SPELL_RANGER_BOW_STANCE, false);

                    scheduler.Schedule(1s, [this](TaskContext /*task*/)
                        {
                            me->SetPower(me->GetPowerType(), 0);

                            Talk(SAY_ANNOUNCE_VEIL_OF_DARKNESS);
                            Talk(SAY_VEIL_OF_DARKNESS_PHASE_ONE);

                            scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_1_FADE, true);


                                    _veilCastTimes++;

                                    uint32 timerForVeilP1;

                                    switch (_veilCastTimes)
                                    {
                                    case 1:
                                        timerForVeilP1 = 49.4;
                                        break;
                                    case 2:
                                        timerForVeilP1 = 46.5;
                                        break;
                                    case 3:
                                        timerForVeilP1 = 46.3;
                                        break;
                                    case 4:
                                        timerForVeilP1 = 46.0;
                                        break;
                                    case 5:
                                        timerForVeilP1 = 48.0;
                                        break;
                                    default:
                                        break;
                                    }

                                    events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS, Seconds(timerForVeilP1), PHASE_ONE);

                                });


                            scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                                {
                                    me->NearTeleportTo(SylvanasVeilOnePos, false);
                                });

                            scheduler.Schedule(1s + 750ms, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_1_GROW, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 5000));

                                    if (me->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                                    {
                                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, SylvanasNonMeleeSelector(me)))
                                        {
                                            me->NearTeleportTo(target->GetPosition(), false);

                                            // TODO: find out which number for SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_01_MM is.
                                        }
                                    }
                                    else
                                    {
                                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 250.0f, true, true))
                                        {
                                            me->NearTeleportTo(target->GetPosition(), false);

                                            float x = target->GetPositionX();
                                            float y = target->GetPositionY();
                                            float z = target->GetPositionZ() + 0.5f;

                                            if (me->GetMap()->GetDifficultyID() == DIFFICULTY_HEROIC_RAID)
                                                me->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_01_HC, 5.0f, true, false);
                                            else
                                                me->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_01_NM, 5.0f, true, false);
                                        }
                                    }
                                });

                            scheduler.Schedule(2s, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_1, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 4000));

                                });

                            scheduler.Schedule(8s, [this](TaskContext /*task*/)
                                {
                                    TeleportShadowcopiesToMe();
                                });
                        });
                }
                else if (events.IsInPhase(PHASE_TWO))
                {
                    if (!me->HasAura(SPELL_RANGER_BOW_STANCE))
                        DoCastSelf(SPELL_RANGER_BOW_STANCE, false);

                    scheduler.Schedule(1s, [this](TaskContext /*task*/)
                        {
                            me->SetPower(me->GetPowerType(), 0);

                            Talk(SAY_ANNOUNCE_VEIL_OF_DARKNESS);
                            Talk(SAY_VEIL_OF_DARKNESS_PHASE_TWO_AND_THREE);

                            scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_FADE, true);




                                });

                            

                            scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                                {
                                    me->NearTeleportTo(SylvanasVeilOnePos, false);
                                    
                                });

                            scheduler.Schedule(1s + 750ms, [this](TaskContext /*task*/)
                                {
                                    

                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_GROW, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 3200));

                                    if (me->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                                    {
                                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, SylvanasNonMeleeSelector(me)))
                                        {
                                            me->NearTeleportTo(target->GetPosition(), false);

                                            // TODO: find out which number for SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_01_MM is.
                                        }
                                    }
                                    else
                                    {
                                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 250.0f, true, true))
                                        {
                                            me->NearTeleportTo(target->GetPosition(), false);

                                            float x = target->GetPositionX();
                                            float y = target->GetPositionY();
                                            float z = target->GetPositionZ() + 0.5f;

                                            if (me->GetMap()->GetDifficultyID() == DIFFICULTY_HEROIC_RAID)
                                                me->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_3_HC, 3.0f, true, false);
                                            else
                                                me->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_3_HC, 3.0f, true, false);
                                        }
                                    }
                                });


                            

                            scheduler.Schedule(2s, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 3000));


                                });

                            scheduler.Schedule(4s, [this](TaskContext /*task*/)
                                {
                                        uint8 randomCopy = urand(0, 2);

                                        for (uint8 itr = 0; itr < 2; itr++)
                                        {
                                            scheduler.Schedule(Milliseconds(1500), [this, itr, randomCopy](TaskContext /*task*/)
                                                {
                                                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(itr))
                                                    {
                                                        Creature* nextShadowCopy = ObjectAccessor::GetCreature(*me, GetShadowCopyJumperGuid(itr + 1));

                                                        ai->Veil2(itr == randomCopy, itr, nextShadowCopy);
                                                    }
                                                });
                            
                                        }
                                });

                            scheduler.Schedule(5s, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_AREA);

                                });

                            scheduler.Schedule(8s, [this](TaskContext /*task*/)
                                {
                                    me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(750ms));
                                    TeleportShadowcopiesToMe();
                                });
                        });
                    events.Repeat(15s);
                    break;
                }
                //events.Repeat(15s);



                else if (events.IsInPhase(PHASE_THREE)) // std::list<Unit*> targets;
                    //SelectTargetList(targets, 3, SelectTargetMethod::Random, 0, 500.0f, true, true);
                {
                    


                }
                break;


            }

            case EVENT_VEIL_OF_DARKNESS_P2_2:
            {
                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
                DoCastSelf(SPELL_RANGER_BOW_STANCE, false);

                if (events.IsInPhase(PHASE_TWO))
                {
                    if (!me->HasAura(SPELL_RANGER_BOW_STANCE))
                        DoCastSelf(SPELL_RANGER_BOW_STANCE, false);

                    scheduler.Schedule(1s, [this](TaskContext /*task*/)
                        {
                            me->SetPower(me->GetPowerType(), 0);

                            Talk(SAY_ANNOUNCE_VEIL_OF_DARKNESS);
                            Talk(SAY_VEIL_OF_DARKNESS_PHASE_TWO_ONLY);

                            scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_FADE, true);




                                });



                            scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                                {
                                    me->NearTeleportTo(Veil2Pos1, false);

                                });

                            scheduler.Schedule(1s + 750ms, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_GROW, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 3200));

                                    if (me->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                                    {
                                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, SylvanasNonMeleeSelector(me)))
                                        {
                                            me->NearTeleportTo(target->GetPosition(), false);

                                            // TODO: find out which number for SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_01_MM is.
                                        }
                                    }
                                    else
                                    {
                                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 1000.0f, true, true))
                                        {
                                            me->NearTeleportTo(target->GetPosition(), false);

                                            float x = target->GetPositionX();
                                            float y = target->GetPositionY();
                                            float z = target->GetPositionZ() + 0.5f;

                                            if (me->GetMap()->GetDifficultyID() == DIFFICULTY_HEROIC_RAID)
                                                me->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_3_HC, 3.0f, true, false);
                                            else
                                                me->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_3_HC, 3.0f, true, false);
                                        }
                                    }
                                });

                            uint8 randomCopy = urand(0, 4);

                            for (uint8 itr = 0; itr < 4; itr++)
                            {
                                scheduler.Schedule(Milliseconds(2000), [this, itr, randomCopy](TaskContext /*task*/)
                                    {
                                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(itr))
                                        {
                                            Creature* nextShadowCopy = ObjectAccessor::GetCreature(*me, GetShadowCopyJumperGuid(itr + 1));

                                            ai->Veil2(itr == randomCopy, itr, nextShadowCopy);
                                        }
                                    });
                            }




                            scheduler.Schedule(2s, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 3000));

                                });

                            scheduler.Schedule(5s, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_AREA);

                                });

                            scheduler.Schedule(8s, [this](TaskContext /*task*/)
                                {
                                    me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(750ms));
                                    TeleportShadowcopiesToMe();
                                });
                        });
                }
                    
                    break;
                }
                    

            case EVENT_VEIL_OF_DARKNESS_P2_3:
            {
                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
                if (events.IsInPhase(PHASE_TWO))
                {
                    if (!me->HasAura(SPELL_RANGER_BOW_STANCE))
                        DoCastSelf(SPELL_RANGER_BOW_STANCE, false);

                    scheduler.Schedule(1s, [this](TaskContext /*task*/)
                        {
                            me->SetPower(me->GetPowerType(), 0);

                            Talk(SAY_ANNOUNCE_VEIL_OF_DARKNESS);
                            Talk(SAY_VEIL_OF_DARKNESS_PHASE_TWO_ONLY);

                            scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_FADE, true);




                                });



                            scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                                {
                                    me->NearTeleportTo(Veil2Pos3, false);

                                });

                            scheduler.Schedule(1s + 750ms, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_GROW, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 3200));

                                    if (me->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                                    {
                                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, SylvanasNonMeleeSelector(me)))
                                        {
                                            me->NearTeleportTo(target->GetPosition(), false);

                                            // TODO: find out which number for SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_01_MM is.
                                        }
                                    }
                                    else
                                    {
                                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 1000.0f, true, true))
                                        {
                                            me->NearTeleportTo(target->GetPosition(), false);

                                            float x = target->GetPositionX();
                                            float y = target->GetPositionY();
                                            float z = target->GetPositionZ() + 0.5f;

                                            if (me->GetMap()->GetDifficultyID() == DIFFICULTY_HEROIC_RAID)
                                                me->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_3_HC, 3.0f, true, false);
                                            else
                                                me->SendPlayOrphanSpellVisual(Position{ x, y, z }, SPELL_VISUAL_VEIL_OF_DARKNESS_PHASE_3_HC, 3.0f, true, false);
                                        }
                                    }
                                });

                            uint8 randomCopy = urand(0, 4);

                            for (uint8 itr = 0; itr < 4; itr++)
                            {
                                scheduler.Schedule(Milliseconds(2000), [this, itr, randomCopy](TaskContext /*task*/)
                                    {
                                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(itr))
                                        {
                                            Creature* nextShadowCopy = ObjectAccessor::GetCreature(*me, GetShadowCopyJumperGuid(itr + 1));

                                            ai->Veil2(itr == randomCopy, itr, nextShadowCopy);
                                        }
                                    });
                            }

                            scheduler.Schedule(2s, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 3000));

                                });

                            scheduler.Schedule(5s, [this](TaskContext /*task*/)
                                {
                                    DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_2_AREA);

                                });

                            scheduler.Schedule(8s, [this](TaskContext /*task*/)
                                {
                                    me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(750ms));
                                    TeleportShadowcopiesToMe();
                                });
                        });
                }

                break;
            }
            

            case EVENT_VEIL_OF_DARKNESS_MAIN:
            {

                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
                DoCastSelf(SPELL_RANGER_BOW_STANCE, false);
                events.CancelEvent(EVENT_WITHERING_FIRE);
                events.ScheduleEvent(EVENT_WITHERING_FIRE, 1ms + 2ms, 1, PHASE_THREE);

                scheduler.Schedule(100ms, [this](TaskContext /*task*/)
                    {
                        me->SetPower(me->GetPowerType(), 0);

                        Talk(SAY_ANNOUNCE_VEIL_OF_DARKNESS);
                        Talk(SAY_VEIL_OF_DARKNESS_PHASE_THREE_ONLY);
                    });

                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_3_FADE, false);
                        me->NearTeleportTo(SylvanasVeilThreePos, false);
                    });
                


                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 500.0f); 
                Trinity::Containers::RandomResize(playerList, 3);

                Unit* firstRandomPlayer = playerList.front();
                Unit* secondRandomPlayer = playerList.back();
                Unit* thirdRandomPlayer = playerList.back();

                ObjectGuid veilfirstRandomTargetGUID = firstRandomPlayer->GetGUID();
                ObjectGuid veilsecondRandomTargetGUID = secondRandomPlayer->GetGUID();
                ObjectGuid veilthirdRandomTargetGUID = thirdRandomPlayer->GetGUID();

                scheduler.Schedule(2s, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {

                                        target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                                        target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                                        target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);                                        
                                        
                        }
                    });

                scheduler.Schedule(2010ms, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {
                            me->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3, false);

                        }
                    });



                scheduler.Schedule(2100ms, [this, veilsecondRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilsecondRandomTargetGUID))
                        {

                                target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                                target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                                target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);

                        }
                    });

                scheduler.Schedule(2200ms, [this, veilthirdRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilthirdRandomTargetGUID))
                        {
                                target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                                target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                                target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);

                        }
                    });


                scheduler.Schedule(2800ms, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {
                            me->NearTeleportTo(target->GetPosition(), false);
                            
                        }
                    });

                scheduler.Schedule(2900ms, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));
                        me->resetAttackTimer(BASE_ATTACK);
                    });
                            
                break;
            }

            case EVENT_VEIL_OF_DARKNESS_MAIN2:
            {

                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
                DoCastSelf(SPELL_RANGER_BOW_STANCE, false);
                events.ScheduleEvent(EVENT_WITHERING_FIRE, 1ms + 2ms, 1, PHASE_THREE);
                scheduler.Schedule(100ms, [this](TaskContext /*task*/)
                    {
                        me->SetPower(me->GetPowerType(), 0);

                        Talk(SAY_ANNOUNCE_VEIL_OF_DARKNESS);
                        Talk(SAY_VEIL_OF_DARKNESS_PHASE_THREE_ONLY);
                    });

                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_3_FADE, false);
                        me->NearTeleportTo(Platform2Sky, false);
                    });



                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 500.0f);
                Trinity::Containers::RandomResize(playerList, 3);

                Unit* firstRandomPlayer = playerList.front();
                Unit* secondRandomPlayer = playerList.back();
                Unit* thirdRandomPlayer = playerList.back();

                ObjectGuid veilfirstRandomTargetGUID = firstRandomPlayer->GetGUID();
                ObjectGuid veilsecondRandomTargetGUID = secondRandomPlayer->GetGUID();
                ObjectGuid veilthirdRandomTargetGUID = thirdRandomPlayer->GetGUID();

                scheduler.Schedule(2s, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {

                            target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);

                        }
                    });

                scheduler.Schedule(2010ms, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {
                            me->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3, false);

                        }
                    });



                scheduler.Schedule(2100ms, [this, veilsecondRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilsecondRandomTargetGUID))
                        {

                            target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);

                        }
                    });

                scheduler.Schedule(2200ms, [this, veilthirdRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilthirdRandomTargetGUID))
                        {
                            target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);

                        }
                    });


                scheduler.Schedule(2800ms, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {
                            me->NearTeleportTo(target->GetPosition(), false);
                        }
                    });

                scheduler.Schedule(2900ms, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        me->resetAttackTimer(BASE_ATTACK);
                    });

                events.ScheduleEvent(EVENT_VEIL_OF_DARKNESS_MAIN2, 51s + 50ms, PHASE_THREE);
                

                break;
            }

            case EVENT_VEIL_OF_DARKNESS_MAIN3:
            {

                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
                DoCastSelf(SPELL_RANGER_BOW_STANCE, false);
                events.ScheduleEvent(EVENT_WITHERING_FIRE, 1ms + 2ms, 1, PHASE_THREE);
                scheduler.Schedule(100ms, [this](TaskContext /*task*/)
                    {
                        me->SetPower(me->GetPowerType(), 0);

                        Talk(SAY_ANNOUNCE_VEIL_OF_DARKNESS);
                        Talk(SAY_VEIL_OF_DARKNESS_PHASE_THREE_ONLY);
                    });

                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_3_FADE, false);
                        me->NearTeleportTo(Platform3Sky, false);
                    });



                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 500.0f);
                Trinity::Containers::RandomResize(playerList, 3);

                Unit* firstRandomPlayer = playerList.front();
                Unit* secondRandomPlayer = playerList.back();
                Unit* thirdRandomPlayer = playerList.back();

                ObjectGuid veilfirstRandomTargetGUID = firstRandomPlayer->GetGUID();
                ObjectGuid veilsecondRandomTargetGUID = secondRandomPlayer->GetGUID();
                ObjectGuid veilthirdRandomTargetGUID = thirdRandomPlayer->GetGUID();

                scheduler.Schedule(2s, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {

                            target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);

                        }
                    });

                scheduler.Schedule(2010ms, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {
                            me->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3, false);

                        }
                    });



                scheduler.Schedule(2100ms, [this, veilsecondRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilsecondRandomTargetGUID))
                        {

                            target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);

                        }
                    });

                scheduler.Schedule(2200ms, [this, veilthirdRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilthirdRandomTargetGUID))
                        {
                            target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);

                        }
                    });


                scheduler.Schedule(2800ms, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {
                            me->NearTeleportTo(target->GetPosition(), false);
                        }
                    });
                events.Repeat(61s);

                break;
            }

            case EVENT_VEIL_OF_DARKNESS_MAIN4:
            {
                events.ScheduleEvent(EVENT_WITHERING_FIRE, 1ms + 2ms, 1, PHASE_THREE);
                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
                DoCastSelf(SPELL_RANGER_BOW_STANCE, false);

                scheduler.Schedule(100ms, [this](TaskContext /*task*/)
                    {
                        me->SetPower(me->GetPowerType(), 0);

                        Talk(SAY_ANNOUNCE_VEIL_OF_DARKNESS);
                        Talk(SAY_VEIL_OF_DARKNESS_PHASE_THREE_ONLY);
                    });

                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_VEIL_OF_DARKNESS_PHASE_3_FADE, false);
                        me->NearTeleportTo(Platform4Sky, false);
                    });



                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 500.0f);
                Trinity::Containers::RandomResize(playerList, 3);

                Unit* firstRandomPlayer = playerList.front();
                Unit* secondRandomPlayer = playerList.back();
                Unit* thirdRandomPlayer = playerList.back();

                ObjectGuid veilfirstRandomTargetGUID = firstRandomPlayer->GetGUID();
                ObjectGuid veilsecondRandomTargetGUID = secondRandomPlayer->GetGUID();
                ObjectGuid veilthirdRandomTargetGUID = thirdRandomPlayer->GetGUID();

                scheduler.Schedule(2s, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {

                            target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);

                        }
                    });

                scheduler.Schedule(2010ms, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {
                            me->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3, false);

                        }
                    });



                scheduler.Schedule(2100ms, [this, veilsecondRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilsecondRandomTargetGUID))
                        {

                            target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);

                        }
                    });

                scheduler.Schedule(2200ms, [this, veilthirdRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilthirdRandomTargetGUID))
                        {
                            target->CastSpell(target, SPELL_VEIL_OF_DARKNESS_PHASE_3_TARGETED, false);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_01, 0, 0);
                            target->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_02, 0, 0);

                        }
                    });


                scheduler.Schedule(2800ms, [this, veilfirstRandomTargetGUID](TaskContext /*task*/)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, veilfirstRandomTargetGUID))
                        {
                            me->NearTeleportTo(target->GetPosition(), false);
                        }
                    });

                events.Repeat(63s);

                break;
            }
            
            case EVENT_RIVE:
            {
                if (_riveCastTimes < 8)
                {
                    scheduler.Schedule(50ms, [this](TaskContext /*task*/)
                        {
                            me->SetNameplateAttachToGUID(_shadowCopyGUID[1]);

                            DoCastSelf(SPELL_RIVE_DISAPPEAR, true);
                        });

                    scheduler.Schedule(100ms, [this](TaskContext /*task*/)
                        {
                            if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                                shadowCopy->NearTeleportTo(RiveThrowPos[_riveCastTimes], false);

                            if (Creature* shadowCopy2 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[2]))
                                shadowCopy2->NearTeleportTo(RiveThrowPos[_riveCastTimes], false);

                            if (Creature* shadowCopy3 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[3]))
                                shadowCopy3->NearTeleportTo(RiveThrowPos[_riveCastTimes], false);
                        });

                    scheduler.Schedule(200ms, [this](TaskContext /*task*/)
                        {
                            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 250.0f, true, true))
                            {
                                if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                                    shadowCopy->SetFacingToObject(target);

                                if (Creature* shadowCopy2 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[2]))
                                    shadowCopy2->SetFacingToObject(target);

                                if (Creature* shadowCopy3 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[3]))
                                    shadowCopy3->SetFacingToObject(target);
                            }
                        });

                    scheduler.Schedule(300ms, [this](TaskContext /*task*/)
                        {
                            if (Creature* shadowCopy2 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[2]))
                                shadowCopy2->NearTeleportTo(shadowCopy2->GetPositionX(), shadowCopy2->GetPositionY(), 4105.00f, shadowCopy2->GetOrientation(), false);
                        });

                    scheduler.Schedule(400ms, [this](TaskContext /*task*/)
                        {
                            if (Creature* shadowCopy2 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[2]))
                                shadowCopy2->NearTeleportTo(shadowCopy2->GetNearPosition(24.4f, 0.0), false);
                        });

                    scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                        {
                            if (Creature* shadowCopy2 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[2]))
                            {
                                me->SendPlayOrphanSpellVisual(shadowCopy2->GetPosition(), SPELL_VISUAL_WINDRUNNER_03, 0.25f, true, false);

                                if (Creature* shadowCopy1 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[1]))
                                    shadowCopy1->CastSpell(shadowCopy2->GetPosition(), SPELL_DOMINATION_CHAINS_JUMP, true);
                            }
                        });

                    scheduler.Schedule(1s, [this](TaskContext /*task*/)
                        {
                            if (Creature* shadowCopy2 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[2]))
                                me->NearTeleportTo(shadowCopy2->GetPosition(), false);
                        });

                    scheduler.Schedule(1s + 150ms, [this](TaskContext /*task*/)
                        {
                            if (Creature* shadowCopy2 = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[2]))
                                me->CastSpell(shadowCopy2->GetNearPosition(9.0f, 0.0f), SPELL_RIVE_MARKER, true);

                            me->SetNameplateAttachToGUID(ObjectGuid::Empty);

                            DoCastSelf(SPELL_ANCHOR_HERE, true);
                        });

                    scheduler.Schedule(1s + 500ms, [this](TaskContext /*task*/)
                        {
                            if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                                shadowCopy->SetFacingTo(shadowCopy->GetAbsoluteAngle(me) + M_PI + 0.040f);

                            if (_riveCastTimes == 0 || _riveCastTimes == 5)
                            {
                                DoCastSelf(SPELL_RIVE, false);

                                scheduler.Schedule(1s + 750ms, [this](TaskContext /*task*/)
                                    {
                                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_RIVE_BREAK, 0, 0);
                                    });

                                events.ScheduleEvent(EVENT_RIVE, 5s, PHASE_INTERMISSION);
                            }
                            else
                            {
                                DoCastSelf(SPELL_RIVE_FAST, false);

                                me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_RIVE_BREAK_FAST, 0, 0);

                                events.ScheduleEvent(EVENT_RIVE, 2s + 500ms, PHASE_INTERMISSION);
                            }

                            _riveCastTimes++;
                        });
                }
                else
                {
                    for (ObjectGuid const& copiesGUID : _shadowCopyGUID)
                    {
                        if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, copiesGUID))
                        {
                            shadowCopy->NearTeleportTo(me->GetPosition(), false);

                            if (shadowCopy->IsAIEnabled())
                                shadowCopy->AI()->SetData(DATA_EVENT_TYPE_SHADOWCOPY, DATA_EVENT_COPY_FINISH_INTERMISSION_EVENT);
                        }
                    }

                    events.ScheduleEvent(EVENT_FINISH_INTERMISSION, 1s + 500ms, PHASE_INTERMISSION);
                }

                break;
            }

            case EVENT_FINISH_INTERMISSION:
            {
                DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_01, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 1000));

                me->SendPlayOrphanSpellVisual(RiveFinishPos, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);

                me->SetNameplateAttachToGUID(_shadowCopyGUID[0]);

                if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                    shadowCopy->CastSpell(RiveFinishPos, SPELL_DOMINATION_CHAINS_JUMP, true);

                scheduler.Schedule(600ms, [this](TaskContext /*task*/)
                    {
                        me->NearTeleportTo(RiveFinishPos, false);

                        me->SetNameplateAttachToGUID(ObjectGuid::Empty);
                    });

                scheduler.Schedule(1s + 700ms, [this](TaskContext /*task*/)
                    {
                        Talk(SAY_INTERMISSION_END);
                    });

                scheduler.Schedule(2s + 400ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_BANSHEE_WAIL, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 5000));

                        Talk(SAY_ANNOUNCE_BANSHEE_WAIL);
                    });

                scheduler.Schedule(6s + 400ms, [this](TaskContext /*task*/)
                    {
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_TRANSFORM_INTO_BANSHEE, 0, 0);


                    });

                scheduler.Schedule(8s + 500ms, [this](TaskContext /*task*/)
                    {
                        me->GetInstanceScript()->DoCastSpellOnPlayers(SPELL_INTERMISSION_STUN);
                        me->GetInstanceScript()->DoCastSpellOnPlayers(SPELL_INTERMISSION_SCENE);

                        me->GetInstanceScript()->DoUpdateWorldState(WORLD_STATE_SYLVANAS_ENCOUNTER_PHASE, PHASE_TWO);
                        events.SetPhase(PHASE_TWO);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BARBED_ARROW);
                        _specialEvents.SetPhase(PHASE_TWO);
                    });

                scheduler.Schedule(9s + 400ms, [this](TaskContext /*task*/)
                    {
                        me->SendPlayOrphanSpellVisual(SylvanasPhase2PrePos, SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);

                        me->NearTeleportTo(SylvanasPhase2PrePos, false);
                    });

                scheduler.Schedule(38s + 400ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* jaina = instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
                        {
                            if (jaina->IsAIEnabled())
                                jaina->AI()->DoAction(ACTION_OPEN_PORTAL_TO_PHASE_TWO);
                        }
                    });

                break;
            }

            case EVENT_BANE_ARROWS:
            {
                DoCastSelf(SPELL_BANE_ARROWS, false);
                
                scheduler.Schedule(2s + 400ms, [this](TaskContext /*task*/)
                    {
                        me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));
                        me->SetReactState(REACT_AGGRESSIVE);
                    });

                events.Repeat(76s);
                break;
            }



            case EVENT_BANSHEE_SCREAM:
            {


                //me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(14000ms));
                scheduler.Schedule(100ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_BANSHEE_SCREAM, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 5000));
                        Talk(SAY_BANSHEES_SCREAM);

                    });

                scheduler.Schedule(4s + 400ms, [this](TaskContext /*task*/)
                    {
                        //me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_TRANSFORM_INTO_BANSHEE, 0, 0);
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_SCREAM, 0, 0);
                        me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);
                    });

                scheduler.Schedule(6500ms + 400ms, [this](TaskContext /*task*/)
                    {

                        me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
                        me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(300ms));
                    });

                //events.Repeat(47s);
                break;
            }

            case EVENT_BANSHEE_SCREAM_2_2:
            {

                _specialEvents.CancelEvent(EVENT_WAILING_ARROW);
                //me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(14000ms));
                scheduler.Schedule(100ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_BANSHEE_SCREAM, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 5000));
                        Talk(SAY_BANSHEES_SCREAM);

                    });

                scheduler.Schedule(4300ms + 400ms, [this](TaskContext /*task*/)
                    {
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_TRANSFORM_INTO_BANSHEE, 0, 0);
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_SCREAM, 0, 0);
                        me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);
                    });

                scheduler.Schedule(6500ms + 400ms, [this](TaskContext /*task*/)
                    {

                        me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
                        //me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(300ms));
                    });

                scheduler.Schedule(7000ms + 400ms, [this](TaskContext /*task*/)
                    {

                        if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 500.0f, true, true))
                        {
                            me->CastSpell(target, SPELL_BANSHEE_HEARTSEEKER, false);

                        }
                        me->SetReactState(REACT_AGGRESSIVE);
                    });




                //events.Repeat(47s);
                break;
            }

            case EVENT_BANSHEE_SCREAM_2_3:
            {

                _specialEvents.CancelEvent(EVENT_WAILING_ARROW);
                //me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(14000ms));
                scheduler.Schedule(100ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_BANSHEE_SCREAM, CastSpellExtraArgs(TRIGGERED_NONE).AddSpellMod(SPELLVALUE_DURATION, 5000));
                        Talk(SAY_BANSHEES_SCREAM);

                    });

                scheduler.Schedule(4s + 400ms, [this](TaskContext /*task*/)
                    {
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_TRANSFORM_INTO_BANSHEE, 0, 0);
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_SCREAM, 0, 0);
                        me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);
                    });

                scheduler.Schedule(6500ms + 400ms, [this](TaskContext /*task*/)
                    {

                        me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
                        _specialEvents.CancelEvent(EVENT_WAILING_ARROW);
                        me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(300ms));
                    });


                //events.Repeat(47s);
                break;
            }

            case EVENT_BANSHEES_FURY:
            {
                Talk(SAY_BANSHEES_FURY);

                if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                    shadowCopy->NearTeleportTo(me->GetPosition(), false);

                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));

                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 500));

                        me->SendPlayOrphanSpellVisual(GetMiddlePointInCurrentPlatform(), SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);

                        me->m_Events.AddEvent(new SetSheatheStateOrNameplate(me, DATA_CHANGE_SHEATHE_UNARMED, 0), me->m_Events.CalculateTime(16ms));

                        if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                        {
                            me->SetNameplateAttachToGUID(_shadowCopyGUID[0]);

                            shadowCopy->CastSpell(GetMiddlePointInCurrentPlatform(), SPELL_DOMINATION_CHAINS_JUMP, false);
                        }
                    });

                scheduler.Schedule(750ms, [this](TaskContext /*task*/)
                    {
                        me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);

                        me->NearTeleportTo(Platform1Center, false);

                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_TELEPORT, 0, 0);

                        me->SetNameplateAttachToGUID(ObjectGuid::Empty);
                    });

                scheduler.Schedule(800ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* ridingCopy = me->FindNearestCreature(DATA_SYLVANAS_SHADOW_COPY_RIDING, 10.0f, true))
                            me->SetNameplateAttachToGUID(ridingCopy->GetGUID());
                    });

                scheduler.Schedule(850ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 5000));

                        DoCastSelf(SPELL_BANSHEES_FURY, false);

                    });

                scheduler.Schedule(7s, [this](TaskContext /*task*/)
                    {
                        me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));
                    });
                events.Repeat(49s);
                break;
            }

            case EVENT_BANSHEES_FURY_03:
            {
                Talk(SAY_BANSHEES_FURY);

                if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                    shadowCopy->NearTeleportTo(me->GetPosition(), false);

                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));

                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 500));

                        me->SendPlayOrphanSpellVisual(GetMiddlePointInCurrentPlatform(), SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);

                        me->m_Events.AddEvent(new SetSheatheStateOrNameplate(me, DATA_CHANGE_SHEATHE_UNARMED, 0), me->m_Events.CalculateTime(16ms));

                        if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                        {
                            me->SetNameplateAttachToGUID(_shadowCopyGUID[0]);

                            shadowCopy->CastSpell(GetMiddlePointInCurrentPlatform(), SPELL_DOMINATION_CHAINS_JUMP, false);
                        }
                    });

                scheduler.Schedule(750ms, [this](TaskContext /*task*/)
                    {
                        me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);

                        me->NearTeleportTo(Platform2Center, false);

                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_TELEPORT, 0, 0);

                        me->SetNameplateAttachToGUID(ObjectGuid::Empty);
                    });

                scheduler.Schedule(800ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* ridingCopy = me->FindNearestCreature(DATA_SYLVANAS_SHADOW_COPY_RIDING, 10.0f, true))
                            me->SetNameplateAttachToGUID(ridingCopy->GetGUID());
                    });

                scheduler.Schedule(850ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 5000));

                        DoCastSelf(SPELL_BANSHEES_FURY, false);


                    });

                scheduler.Schedule(7s, [this](TaskContext /*task*/)
                    {
                        me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));
                    });

                events.Repeat(52s);
                break;
            }


            case EVENT_BANSHEES_FURY_04:
            {
                Talk(SAY_BANSHEES_FURY);

                if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                    shadowCopy->NearTeleportTo(me->GetPosition(), false);

                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));

                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 500));

                        me->SendPlayOrphanSpellVisual(GetMiddlePointInCurrentPlatform(), SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);

                        me->m_Events.AddEvent(new SetSheatheStateOrNameplate(me, DATA_CHANGE_SHEATHE_UNARMED, 0), me->m_Events.CalculateTime(16ms));

                        if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                        {
                            me->SetNameplateAttachToGUID(_shadowCopyGUID[0]);

                            shadowCopy->CastSpell(GetMiddlePointInCurrentPlatform(), SPELL_DOMINATION_CHAINS_JUMP, false);
                        }
                    });

                scheduler.Schedule(750ms, [this](TaskContext /*task*/)
                    {
                        me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);

                        me->NearTeleportTo(Platform3Center, false);

                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_TELEPORT, 0, 0);

                        me->SetNameplateAttachToGUID(ObjectGuid::Empty);
                    });

                scheduler.Schedule(800ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* ridingCopy = me->FindNearestCreature(DATA_SYLVANAS_SHADOW_COPY_RIDING, 10.0f, true))
                            me->SetNameplateAttachToGUID(ridingCopy->GetGUID());
                    });

                scheduler.Schedule(850ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 5000));

                        DoCastSelf(SPELL_BANSHEES_FURY, false);


                    });

                scheduler.Schedule(7s, [this](TaskContext /*task*/)
                    {
                        me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));
                    });
                events.Repeat(47s);
                break;
            }

            case EVENT_BANSHEES_FURY_05:
            {
                Talk(SAY_BANSHEES_FURY);

                if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                    shadowCopy->NearTeleportTo(me->GetPosition(), false);

                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));

                scheduler.Schedule(250ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_WINDRUNNER_DISAPPEAR_02, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 500));

                        me->SendPlayOrphanSpellVisual(GetMiddlePointInCurrentPlatform(), SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);

                        me->m_Events.AddEvent(new SetSheatheStateOrNameplate(me, DATA_CHANGE_SHEATHE_UNARMED, 0), me->m_Events.CalculateTime(16ms));

                        if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[0]))
                        {
                            me->SetNameplateAttachToGUID(_shadowCopyGUID[0]);

                            shadowCopy->CastSpell(GetMiddlePointInCurrentPlatform(), SPELL_DOMINATION_CHAINS_JUMP, false);
                        }
                    });

                scheduler.Schedule(750ms, [this](TaskContext /*task*/)
                    {
                        me->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);

                        me->NearTeleportTo(Platform4Center, false);

                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_TELEPORT, 0, 0);

                        me->SetNameplateAttachToGUID(ObjectGuid::Empty);
                    });

                scheduler.Schedule(800ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* ridingCopy = me->FindNearestCreature(DATA_SYLVANAS_SHADOW_COPY_RIDING, 10.0f, true))
                            me->SetNameplateAttachToGUID(ridingCopy->GetGUID());
                    });

                scheduler.Schedule(850ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 5000));

                        DoCastSelf(SPELL_BANSHEES_FURY, false);


                    });

                scheduler.Schedule(7s, [this](TaskContext /*task*/)
                    {
                        me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));
                    });
                events.Repeat(58s);
                break;
            }


            default:
                break;
            }
        }

        DoSylvanasAttackIfReady();
    }

    void DoSylvanasAttackIfReady()
    {
        if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_WINDRUNNER) || me->HasAura(SPELL_BANSHEE_SHROUD) || me->HasReactState(REACT_PASSIVE))
            return;

        if (me->isAttackReady(BASE_ATTACK))
        {
            if (IsHeartseekerReady() == true || !me->IsWithinCombatRange(me->GetVictim(), 7.5f))
            {
                if (!me->HasAura(SPELL_RANGER_BOW_STANCE))
                    DoCastSelf(SPELL_RANGER_BOW_STANCE, false);
                else
                {
                    if (!_rangerShotOnCD)
                        DoAction(ACTION_RANGER_SHOT);
                }
            }

            else if (IsBansheesHeartseekerReady() == true || !me->IsWithinCombatRange(me->GetVictim(), 7.5f))
            {
                if (!me->HasAura(SPELL_RANGER_BOW_STANCE))
                    DoCastSelf(SPELL_RANGER_BOW_STANCE, false);
                else
                {
                    if (!_rangerShotOnCD)
                        DoAction(ACTION_RANGER_SHOT);
                }
            }

            else if (AreBladesReady() == true || !me->IsWithinCombatRange(me->GetVictim(), 7.5f))
            {
                if (!me->HasAura(SPELL_RANGER_DAGGERS_STANCE))
                    DoCastSelf(SPELL_RANGER_DAGGERS_STANCE, false);
                else
                {
                    if (!_rangerShotOnCD)
                        DoAction(ACTION_RANGER_SHOT);
                }
            }


            else
            {
                switch (_meleeKitCombo)
                {
                case DATA_MELEE_COMBO_SWITCH_TO_MELEE:
                    if (!me->HasAura(SPELL_RANGER_DAGGERS_STANCE))
                        DoCastSelf(SPELL_RANGER_DAGGERS_STANCE, false);
                    _meleeKitCombo++;
                    break;

                case DATA_MELEE_COMBO_RANGER_STRIKE_01:
                case DATA_MELEE_COMBO_RANGER_STRIKE_02:
                    DoCastVictim(SPELL_RANGER_STRIKE, false);
                    _meleeKitCombo++;
                    break;

                case DATA_MELEE_COMBO_SWITCH_TO_RANGED:
                    if (!me->HasAura(SPELL_RANGER_BOW_STANCE))
                        DoCastSelf(SPELL_RANGER_BOW_STANCE, false);
                    _meleeKitCombo++;
                    break;

                case DATA_MELEE_COMBO_FINISH:
                    DoAction(ACTION_RANGER_SHOT);
                    break;
                default:
                    break;
                }
            }

            me->resetAttackTimer(BASE_ATTACK);
        }
    }


    void ChooseDesecratingShotPattern(int8 pattern)
    {
        switch (pattern)
        {
        case DATA_DESECRATING_SHOT_PATTERN_STRAIGHT:
        {
            int32 step = 1;

            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 500.0f, true, true))
            {
                float orientation = me->GetAbsoluteAngle(target);

                while (DrawDesecratingShotPattern(DATA_DESECRATING_SHOT_PATTERN_STRAIGHT, 0, step, me->GetPosition(), orientation))
                    ++step;

                scheduler.Schedule(2s, [this](TaskContext /*task*/)
                    {
                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                            ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_STRAIGHT, me->GetPosition());
                    });
            }
            break;
        }

        case DATA_DESECRATING_SHOT_PATTERN_SCATTERED:
        {
            int32 step = 1;

            int32 amount = std::max<uint8>(4, std::ceil(float(me->GetMap()->GetPlayersCountExceptGMs()) / 3));

            while (amount > 0 && DrawDesecratingShotPattern(DATA_DESECRATING_SHOT_PATTERN_SCATTERED, amount, step, me->GetPosition(), me->GetOrientation()))
                --amount;

            scheduler.Schedule(2s, [this](TaskContext /*task*/)
                {
                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                        ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_SCATTERED, me->GetPosition());
                });
            break;
        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE:
        {
            int32 step = 0;
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 500.0f, true, true))
            {
                float orientation = me->GetAbsoluteAngle(target);

                while (DrawDesecratingShotPattern(DATA_DESECRATING_SHOT_PATTERN_WAVE, 0, step, me->GetPosition(), orientation))
                    ++step;

                scheduler.Schedule(2s, [this](TaskContext /*task*/)
                    {
                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                            ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_WAVE, me->GetPosition());
                    });
            }
            break;

        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_1:
        {
            int32 step = 0;            

            if (Unit* target = me)
            {
                float orientation = me->GetAbsoluteAngle(target);

                while (DrawDesecratingShotPattern(DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_1, 0, step, me->GetPosition(), orientation))
                    ++step;

                scheduler.Schedule(2250ms, [this](TaskContext /*task*/)
                    {
                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                            ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_1, me->GetPosition());
                    });
            }
            

        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_2:
        {
            int32 step = 0;
            if (Unit* target = me)
            {
                float orientation = me->GetAbsoluteAngle(target);

                while (DrawDesecratingShotPattern(DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_2, 0, step, me->GetPosition(), orientation))
                    ++step;

                scheduler.Schedule(2250ms, [this](TaskContext /*task*/)
                    {
                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                            ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_2, me->GetPosition());
                    });
            }
            

        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_LEFT_1:
        {
            int32 step = 0;
            if (Unit* target = me)
            {
                float orientation = me->GetAbsoluteAngle(target);

                while (DrawDesecratingShotPattern(DATA_DESECRATING_SHOT_PATTERN_WAVE_LEFT_1, 0, step, me->GetPosition(), orientation))
                    ++step;

                scheduler.Schedule(2s, [this](TaskContext /*task*/)
                    {
                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                            ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_WAVE_LEFT_1, me->GetPosition());
                    });
            }
            

        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_LEFT_2:
        {
            int32 step = 0;
            if (Unit* target = me)
            {
                float orientation = me->GetAbsoluteAngle(target);

                while (DrawDesecratingShotPattern(DATA_DESECRATING_SHOT_PATTERN_WAVE_LEFT_2, 0, step, me->GetPosition(), orientation))
                    ++step;

                scheduler.Schedule(2s, [this](TaskContext /*task*/)
                    {
                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                            ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_WAVE_LEFT_2, me->GetPosition());
                    });
            }
            

        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_RIGHT_1:
        {
            int32 step = 0;
            if (Unit* target = me)
            {
                float orientation = me->GetAbsoluteAngle(target);

                while (DrawDesecratingShotPattern(DATA_DESECRATING_SHOT_PATTERN_WAVE_RIGHT_1, 0, step, me->GetPosition(), orientation))
                    ++step;

                scheduler.Schedule(2500ms, [this](TaskContext /*task*/)
                    {
                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                            ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_WAVE_RIGHT_1, me->GetPosition());
                    });
            }
            

        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_RIGHT_2:
        {
            int32 step = 0;
            if (Unit* target = me)
            {
                float orientation = me->GetAbsoluteAngle(target);

                while (DrawDesecratingShotPattern(DATA_DESECRATING_SHOT_PATTERN_WAVE_RIGHT_2, 0, step, me->GetPosition(), orientation))
                    ++step;

                scheduler.Schedule(2500ms, [this](TaskContext /*task*/)
                    {
                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                            ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_WAVE_RIGHT_2, me->GetPosition());
                    });
            }
            break;

        }



        case DATA_DESECRATING_SHOT_PATTERN_SPIRAL:
        {
            int32 step = 10;
            int32 nextCopy = 0;
            std::list<Unit*> targets;
            SelectTargetList(targets, 3, SelectTargetMethod::Random, 0, 500.0f, true, true);
            for (Unit* target : targets)
            {
                while (step > 0 && DrawDesecratingShotPattern(DATA_DESECRATING_SHOT_PATTERN_SPIRAL, 0, step, target->GetPosition(), 1.49f))
                    --step;
                nextCopy++;
                scheduler.Schedule(1s + 500ms + Milliseconds(500 * nextCopy), [this, target](TaskContext /*task*/)
                    {
                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                            ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_SPIRAL, target->GetPosition());
                    });
            }

            SelectTargetList(targets, 3, SelectTargetMethod::Random, 0, 500.0f, true, true);
            for (Unit* target : targets)
            {
                while (step > 0 && DrawDesecratingShotPattern(DATA_DESECRATING_SHOT_PATTERN_SPIRAL, 0, step, target->GetPosition(), 1.49f))
                    --step;
                nextCopy++;
                scheduler.Schedule(1s + 500ms + Milliseconds(500 * nextCopy), [this, target](TaskContext /*task*/)
                    {
                        if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                            ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_SPIRAL, target->GetPosition());
                    });
            }
            break;
        }

        case DATA_DESECRATING_SHOT_PATTERN_JAR:
        {
            int32 step = 0;

            // JAR

            scheduler.Schedule(2s, [this](TaskContext /*task*/)
                {
                    if (npc_sylvanas_windrunner_shadowcopy* ai = GetSylvanasCopyAI(0))
                        ai->StartDesecratingShotEvent(DATA_DESECRATING_SHOT_PATTERN_JAR, me->GetPosition());
                });
            break;
        }

        default:
            break;
        }
    }




    bool DrawDesecratingShotPattern(int8 pattern, int32 amount, int32 step, Position pos, float orientation)
    {
        switch (pattern)
        {
        case DATA_DESECRATING_SHOT_PATTERN_STRAIGHT:
        {
            float distance = 7.0f * step;

            // Let's obtain the arrow's center so we can stop summoning arrows if it goes beyond the boundaries of the platform
            Position arrowCenter(me->GetPositionX() + (std::cos(orientation) * distance), me->GetPositionY() + (std::sin(orientation) * distance), me->GetPositionZ());

            if (!SylvanasFirstPhasePlatformCenter.IsInDist2d(&arrowCenter, PLATFORM_RADIUS))
                return false;

            scheduler.Schedule(Milliseconds(step * 40), [this, arrowCenter](TaskContext /*task*/)
                {
                    me->CastSpell(arrowCenter, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });

            Position arrowInnerLeft(arrowCenter.GetPositionX() + (std::cos(orientation + 135.0f * M_PI / 180) * 2.8284f), arrowCenter.GetPositionY() + (std::sin(orientation + 135.0f * M_PI / 180) * 2.8284f), arrowCenter.GetPositionZ());
            Position arrowInnerRight(arrowCenter.GetPositionX() + (std::cos(orientation + -135.0f * M_PI / 180) * 2.8284f), arrowCenter.GetPositionY() + (std::sin(orientation + -135.0f * M_PI / 180) * 2.8284f), arrowCenter.GetPositionZ());

            scheduler.Schedule(Milliseconds(step * 40), [this, arrowInnerLeft, arrowInnerRight](TaskContext /*task*/)
                {
                    me->CastSpell(arrowInnerLeft, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                    me->CastSpell(arrowInnerRight, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });

            Position arrowOuterLeft(arrowCenter.GetPositionX() + (std::cos(orientation + 135.0f * M_PI / 180) * 5.6568f), arrowCenter.GetPositionY() + (std::sin(orientation + 135.0f * M_PI / 180) * 5.6568f), arrowCenter.GetPositionZ());
            Position arrowOuterRight(arrowCenter.GetPositionX() + (std::cos(orientation + -135.0f * M_PI / 180) * 5.6568f), arrowCenter.GetPositionY() + (std::sin(orientation + -135.0f * M_PI / 180) * 5.6568f), arrowCenter.GetPositionZ());

            scheduler.Schedule(Milliseconds(step * 56), [this, arrowOuterLeft, arrowOuterRight](TaskContext /*task*/)
                {
                    me->CastSpell(arrowOuterLeft, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                    me->CastSpell(arrowOuterRight, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });

            return true;
        }

        case DATA_DESECRATING_SHOT_PATTERN_SCATTERED:
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 250.0f, true, true))
            {
                float distance = frand(2.5f, 5.0f) * step;

                // Let's obtain the arrow's center so we can stop summoning arrows if it goes beyond the boundaries of the platform
                Position arrowCenter(target->GetPositionX() + (std::cos(orientation) * distance), target->GetPositionY() + (std::sin(orientation) * distance), me->GetPositionZ());

                if (!SylvanasFirstPhasePlatformCenter.IsInDist2d(&arrowCenter, PLATFORM_RADIUS))
                    return false;

                scheduler.Schedule(Milliseconds(step * 5), [this, arrowCenter](TaskContext /*task*/)
                    {
                        me->CastSpell(arrowCenter, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                    });

                Position arrowInnerLeft(arrowCenter.GetPositionX() + (std::cos(orientation + 135.0f * M_PI / 180) * 2.8284f), arrowCenter.GetPositionY() + (std::sin(orientation + 135.0f * M_PI / 180) * 2.8284f), arrowCenter.GetPositionZ());

                scheduler.Schedule(Milliseconds(step * 50), [this, arrowInnerLeft](TaskContext /*task*/)
                    {
                        me->CastSpell(arrowInnerLeft, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                    });

                Position arrowInnerRight(arrowCenter.GetPositionX() + (std::cos(orientation + -135.0f * M_PI / 180) * 2.8284f), arrowCenter.GetPositionY() + (std::sin(orientation + -135.0f * M_PI / 180) * 2.8284f), arrowCenter.GetPositionZ());

                scheduler.Schedule(Milliseconds(step * 100), [this, arrowInnerRight](TaskContext /*task*/)
                    {
                        me->CastSpell(arrowInnerRight, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                    });
            }

            return true;
        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE:
        {

            float distance = -4.0f * step;

            // Let's obtain the wave's center so we can stop summoning arrows if it goes beyond the boundaries of the platform
            Position arrowCenter(me->GetPositionX() + (std::cos(orientation) * distance), me->GetPositionY() + (std::sin(orientation) * distance), me->GetPositionZ());

            if (!SylvanasFirstPhasePlatformCenter.IsInDist2d(&arrowCenter, PLATFORM_RADIUS2))
                return false;

            scheduler.Schedule(Milliseconds(step * 25), [this, arrowCenter](TaskContext /*task*/)
                {
                    me->CastSpell(arrowCenter, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });

            Position arrowCenterBack(me->GetPositionX() - (std::cos(orientation) * distance), me->GetPositionY() - (std::sin(orientation) * distance), me->GetPositionZ());

            if (!SylvanasFirstPhasePlatformCenter.IsInDist2d(&arrowCenterBack, PLATFORM_RADIUS2))
                return false;

            scheduler.Schedule(Milliseconds(step * 25), [this, arrowCenterBack](TaskContext /*task*/)
                {
                    me->CastSpell(arrowCenterBack, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });

            Position rightLine(pos.GetPositionX() + (std::cos(orientation - (M_PI / 2)) * 4.0f) + (std::cos(orientation) * distance), pos.GetPositionY() + (std::sin(orientation - (M_PI / 2)) * 4.0f) + (std::sin(orientation) * distance), pos.GetPositionZ());
            scheduler.Schedule(Milliseconds(step * 25), [this, rightLine](TaskContext /*task*/)
                {
                    me->CastSpell(rightLine, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });

            Position rightLineBack(pos.GetPositionX() - (std::cos(orientation - (M_PI / 2)) * 4.0f) - (std::cos(orientation) * distance), pos.GetPositionY() - (std::sin(orientation - (M_PI / 2)) * 4.0f) - (std::sin(orientation) * distance), pos.GetPositionZ());
            scheduler.Schedule(Milliseconds(step * 25), [this, rightLineBack](TaskContext /*task*/)
                {
                    me->CastSpell(rightLineBack, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });

            Position leftLine(pos.GetPositionX() + (std::cos(orientation - (M_PI / 2)) * -4.0f) + (std::cos(orientation) * distance), pos.GetPositionY() + (std::sin(orientation - (M_PI / 2)) * -4.0f) + (std::sin(orientation) * distance), pos.GetPositionZ());
            scheduler.Schedule(Milliseconds(step * 25), [this, leftLine](TaskContext /*task*/)
                {
                    me->CastSpell(leftLine, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });

            Position leftLineBack(pos.GetPositionX() - (std::cos(orientation - (M_PI / 2)) * -4.0f) - (std::cos(orientation) * distance), pos.GetPositionY() - (std::sin(orientation - (M_PI / 2)) * -4.0f) - (std::sin(orientation) * distance), pos.GetPositionZ());
            scheduler.Schedule(Milliseconds(step * 25), [this, leftLineBack](TaskContext /*task*/)
                {
                    me->CastSpell(leftLineBack, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });

            return true;
        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_1:
        {

            float distance = -4.0f * step;
            // Let's obtain the wave's center so we can stop summoning arrows if it goes beyond the boundaries of the platform
            Position arrowCenter(me->GetPositionX() + (std::cos(orientation) * distance), me->GetPositionY() + (std::sin(orientation) * distance), me->GetPositionZ());
            if (!SylvanasFirstPhasePlatformCenter.IsInDist2d(&arrowCenter, PLATFORM_RADIUS))
                return false;
            scheduler.Schedule(Milliseconds(step * 45), [this, arrowCenter](TaskContext /*task*/)
                {
                    me->CastSpell(arrowCenter, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });

            

            return true;
        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_MID_2:
        {

            float distance = -4.0f * step;
            Position arrowCenterBack(me->GetPositionX() - (std::cos(orientation) * distance), me->GetPositionY() - (std::sin(orientation) * distance), me->GetPositionZ());
            if (!SylvanasFirstPhasePlatformCenter.IsInDist2d(&arrowCenterBack, PLATFORM_RADIUS))
                return false;
            scheduler.Schedule(Milliseconds(step * 45), [this, arrowCenterBack](TaskContext /*task*/)
                {
                    me->CastSpell(arrowCenterBack, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });



            return true;
        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_RIGHT_1:
        {
            float distance = -4.0f * step;

            Position rightLine(pos.GetPositionX() + (std::cos(orientation - (M_PI / 2)) * 4.0f) + (std::cos(orientation) * distance), pos.GetPositionY() + (std::sin(orientation - (M_PI / 2)) * 4.0f) + (std::sin(orientation) * distance), pos.GetPositionZ());
            if (!SylvanasFirstPhasePlatformCenter.IsInDist2d(&rightLine, PLATFORM_RADIUS))
                return false;

            scheduler.Schedule(Milliseconds(step * 25), [this, rightLine](TaskContext /*task*/)
                {
                    me->CastSpell(rightLine, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });
            
            return true;
        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_RIGHT_2:
        {
            float distance = -4.0f * step;

            Position rightLineBack(pos.GetPositionX() - (std::cos(orientation - (M_PI / 2)) * 4.0f) - (std::cos(orientation) * distance), pos.GetPositionY() - (std::sin(orientation - (M_PI / 2)) * 4.0f) - (std::sin(orientation) * distance), pos.GetPositionZ());
            if (!SylvanasFirstPhasePlatformCenter.IsInDist2d(&rightLineBack, PLATFORM_RADIUS))
                return false;
            scheduler.Schedule(Milliseconds(step * 25), [this, rightLineBack](TaskContext /*task*/)
                {
                    me->CastSpell(rightLineBack, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });


            return true;
        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_LEFT_1:
        {
            float distance = -4.0f * step;

            Position leftLine(pos.GetPositionX() + (std::cos(orientation - (M_PI / 2)) * -4.0f) + (std::cos(orientation) * distance), pos.GetPositionY() + (std::sin(orientation - (M_PI / 2)) * -4.0f) + (std::sin(orientation) * distance), pos.GetPositionZ());
            if (!SylvanasFirstPhasePlatformCenter.IsInDist2d(&leftLine, PLATFORM_RADIUS))
                return false;
            scheduler.Schedule(Milliseconds(step * 60), [this, leftLine](TaskContext /*task*/)
                {
                    me->CastSpell(leftLine, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });


            return true;
        }

        case DATA_DESECRATING_SHOT_PATTERN_WAVE_LEFT_2:
        {
            float distance = -4.0f * step;         

            Position leftLineBack(pos.GetPositionX() - (std::cos(orientation - (M_PI / 2)) * -4.0f) - (std::cos(orientation) * distance), pos.GetPositionY() - (std::sin(orientation - (M_PI / 2)) * -4.0f) - (std::sin(orientation) * distance), pos.GetPositionZ());
            if (!SylvanasFirstPhasePlatformCenter.IsInDist2d(&leftLineBack, PLATFORM_RADIUS))
                return false;

            scheduler.Schedule(Milliseconds(step * 60), [this, leftLineBack](TaskContext /*task*/)
                {
                    me->CastSpell(leftLineBack, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                });


            return true;
        }

        case DATA_DESECRATING_SHOT_PATTERN_SPIRAL:
        {
            float distance = 4.0f * step;
            uint8 spiralCount = 3;
            float angleOffset = float(M_PI * 2) / spiralCount;

            switch (step)
            {
            case 1:
            {
                me->CastSpell(pos, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                break;
            }

            default:
            {

                for (uint8 i = 0; i < spiralCount; i++)
                {
                    float angle = angleOffset * i;

                    Position finalDest(pos.GetPositionX() + (std::cos(angle + 0.2 * step) * distance), pos.GetPositionY() + (std::sin(angle + 0.2 * step) * distance), pos.GetPositionZ());
                    scheduler.Schedule(Milliseconds(step * 25), [this, finalDest](TaskContext /*task*/)
                        {
                            me->CastSpell(finalDest, SPELL_DESECRATING_SHOT_AREATRIGGER, true);
                        });
                }
                break;
            }
            }
            break;
        }


        case DATA_DESECRATING_SHOT_PATTERN_JAR:
        {
            break;
        }

        default:
            break;
        }

        return true;
    }

    void TeleportShadowcopiesToMe()
    {
        for (ObjectGuid const& copiesGUID : _shadowCopyGUID)
        {
            if (Creature* shadowCopy = ObjectAccessor::GetCreature(*me, copiesGUID))
                shadowCopy->NearTeleportTo(me->GetPosition(), false);
        }
    }

    ObjectGuid GetShadowCopyJumperGuid(int32 index)
    {
        return _shadowCopyGUID[index];
    }

    bool IsHeartseekerReady()
    {
        Aura* heartseekerCharge = me->GetAura(SPELL_RANGER_HEARTSEEKER_CHARGE);

        if (heartseekerCharge && heartseekerCharge->GetStackAmount() >= 3)
            return true;
        else
            return false;

        return true;
    }

    bool IsBansheesHeartseekerReady()
    {
        Aura* heartseekerCharge = me->GetAura(SPELL_BANSHEE_HEARTSEEKER_CHARGE);
        if (heartseekerCharge && heartseekerCharge->GetStackAmount() >= 3)
            return true;

        Aura* bansheeHeartseekerCharge = me->GetAura(SPELL_BANSHEE_BLADES_CHARGE);
        if (bansheeHeartseekerCharge && bansheeHeartseekerCharge->GetStackAmount() == 3)
            return true;
        else
            return false;

        return true;
    }

    bool AreBladesReady()
    {
        Aura* bansheeBladesCharge = me->GetAura(SPELL_BANSHEE_BLADES_CHARGE);

        if (bansheeBladesCharge && bansheeBladesCharge->GetStackAmount() >= 4)
            return true;
        else
            return false;

        return true;
    }



    bool IsPlatformDesecrated(int8 index)
    {
        switch (index)
        {
        case DATA_MALDRAXXI_PLATFORM:
            return _maldraxxiDesecrated;

        case DATA_NIGHTFAE_PLATFORM:
            return _nightfaeDesecrated;

        case DATA_KYRIAN_PLATFORM:
            return _kyrianDesecrated;

        case DATA_VENTHYR_PLATFORM:
            return _venthyrDesecrated;
        default:
            break;
        }
        return true;
    }

    void DesecrateCurrentPlatform(int8 index)
    {
        switch (index)
        {
        case DATA_MALDRAXXI_PLATFORM:

            _maldraxxiDesecrated = true;
            break;

        case DATA_NIGHTFAE_PLATFORM:
            _nightfaeDesecrated = true;
            break;

        case DATA_KYRIAN_PLATFORM:
            _kyrianDesecrated = true;
            break;

        case DATA_VENTHYR_PLATFORM:
            _venthyrDesecrated = true;
            break;

        default:
            break;
        }
    }

    Position const GetMiddlePointInCurrentPlatform()
    {
        for (uint8 covenentPlaform = 0; covenentPlaform < 4; covenentPlaform++)
        {
            if (me->IsWithinBox(CovenantPlatformPos[covenentPlaform][DATA_MIDDLE_POS_OUTTER_PLATFORM], 14.0f, 14.0f, 14.0f))
                return CovenantPlatformPos[covenentPlaform][DATA_MIDDLE_POS_OUTTER_PLATFORM];
        }

        return { };
    }

    Position const GetRandomPointInCurrentPlatform()
    {
        for (uint8 covenentPlaform = 0; covenentPlaform < 4; covenentPlaform++)
        {
            if (me->IsWithinBox(CovenantPlatformPos[covenentPlaform][DATA_MIDDLE_POS_OUTTER_PLATFORM], 14.0f, 14.0f, 14.0f))
                return GetRandomPointInCovenantPlatform(CovenantPlatformPos[covenentPlaform][DATA_BOTTOM_LEFT_POS_VERTEX_PLATFORM], CovenantPlatformPos[covenentPlaform][DATA_TOP_RIGHT_POS_VERTEX_PLATFORM], me->GetPositionZ());
        }

        return { };
    }

    Position const GetRandomPointInNonDesecratedPlatform(int8 index)
    {
        switch (index)
        {
        case DATA_MALDRAXXI_PLATFORM:
        case DATA_NIGHTFAE_PLATFORM:
        case DATA_KYRIAN_PLATFORM:
        case DATA_VENTHYR_PLATFORM:
            return GetRandomPointInCovenantPlatform(CovenantPlatformPos[index][DATA_BOTTOM_LEFT_POS_VERTEX_PLATFORM], CovenantPlatformPos[index][DATA_TOP_RIGHT_POS_VERTEX_PLATFORM], me->GetPositionZ());
            break;
        default:
            break;
        }

        return { };
    }

    npc_sylvanas_windrunner_shadowcopy* GetSylvanasCopyAI(int32 index)
    {
        Creature* shadowCopy = ObjectAccessor::GetCreature(*me, _shadowCopyGUID[index]);

        if (!shadowCopy)
            return nullptr;

        return CAST_AI(npc_sylvanas_windrunner_shadowcopy, shadowCopy->AI());
    }

private:
    EventMap _specialEvents;
    std::vector<ObjectGuid> _shadowCopyGUID;
    std::vector<ObjectGuid> _invigoratingFieldGUID;
    bool _rangerShotOnCD;
    bool _maldraxxiDesecrated;
    bool _nightfaeDesecrated;
    bool _kyrianDesecrated;
    bool _venthyrDesecrated;
    uint8 _sayKilled;
    uint8 _meleeKitCombo;
    uint8 _windrunnerCastTimes;
    uint8 _chainsCastTimes;
    uint8 _veilCastTimes;
    uint8 _ruinCastTimes;
    uint8 _disecratingShotCastTimes;
    uint8 _riveCastTimes;
    uint8 _hauntingWaveTimes;
    uint8 _wailingArrowCastTimes;
    uint8 windsCasted;
    uint8 hasPhased;
    uint8 anduinReady;
};

// Domination Arrow - 176920
struct npc_sylvanas_windrunner_domination_arrow : public ScriptedAI
{
    npc_sylvanas_windrunner_domination_arrow(Creature* creature) : ScriptedAI(creature),
        _instance(creature->GetInstanceScript()) { }

    void JustAppeared() override
    {
        DoZoneInCombat();

        me->SetReactState(REACT_PASSIVE);

        me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
        case ACTION_ACTIVATE_DOMINATION_ARROW:
        {
            me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);

            me->RemoveAura(SPELL_DOMINATION_ARROW_CALAMITY_VISUAL);

            DoCastSelf(SPELL_DOMINATION_ARROW_ACTIVATE, true);

            if (Unit* target = SelectTarget(SelectTargetMethod::MinDistance, 0, 500.0f, true, true, -SPELL_DOMINATION_CHAIN_PLAYER))
            {
                _playerGUID = target->GetGUID();

                me->CastSpell(target, SPELL_DOMINATION_CHAIN_PLAYER, false);

                if (me->GetMap()->GetDifficultyID() == DIFFICULTY_HEROIC_RAID || me->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC_RAID)
                    target->CastSpell(me, SPELL_DOMINATION_ARROW_CALAMITY_AREATRIGGER, true);
            }
            break;
        }

        default:
            break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Player* chainedPlayer = ObjectAccessor::GetPlayer(*me, _playerGUID))
        {
            if (chainedPlayer->HasAura(SPELL_DOMINATION_CHAIN_PLAYER))
                chainedPlayer->RemoveAura(SPELL_DOMINATION_CHAIN_PLAYER);
        }
    }

private:
    InstanceScript* _instance;
    ObjectGuid _playerGUID;

};

// Ranger (Bow) - 347560
class spell_sylvanas_windrunner_ranger_bow : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_ranger_bow);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SYLVANAS_ROOT });
    }

    void OnCast(SpellMissInfo /*missInfo*/)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 1600));

        if (GetCaster()->HasAura(SPELL_RANGER_DAGGERS_STANCE))
            GetCaster()->RemoveAura(SPELL_RANGER_DAGGERS_STANCE);

        if (urand(0, 1))
            GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_UNSHEATHE_BOW_SPIN, 0, 0);
        else
            GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_UNSHEATHE_BOW, 0, 0);

        GetCaster()->ApplyAttackTimePercentMod(WeaponAttackType::MAX_ATTACK, 1.494999885559082031f, true);

        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_SHEATHE_UNARMED, 0), GetCaster()->m_Events.CalculateTime(16ms));
        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_SHEATHE_BOW, 0), GetCaster()->m_Events.CalculateTime(328ms));
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_sylvanas_windrunner_ranger_bow::OnCast);
    }
};

// Ranger (Dagger) - 348010
class spell_sylvanas_windrunner_ranger_dagger : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_ranger_dagger);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SYLVANAS_ROOT });
    }

    void OnCast(SpellMissInfo /*missInfo*/)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 2500));

        if (GetCaster()->HasAura(SPELL_RANGER_BOW_STANCE))
            GetCaster()->RemoveAura(SPELL_RANGER_BOW_STANCE);

        if (urand(0, 1))
            GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_UNSHEATHE_DAGGERS_SPIN, 0, 0);
        else
            GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_UNSHEATHE_DAGGERS, 0, 0);

        GetCaster()->ApplyAttackTimePercentMod(WeaponAttackType::MAX_ATTACK, 1.149999976158142089f, true);

        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_SHEATHE_UNARMED, 0), GetCaster()->m_Events.CalculateTime(16ms));
        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_SHEATHE_DAGGERS, 0), GetCaster()->m_Events.CalculateTime(313ms));
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_sylvanas_windrunner_ranger_dagger::OnCast);
    }
};

// Ranger Shot - 347548
class spell_sylvanas_windrunner_ranger_shot : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_ranger_shot);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SYLVANAS_ROOT });
    }

    void OnPrecast() override
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 2500));
    }

    void Register() override { }
};

// Ranger Strike - 348299
class spell_sylvanas_windrunner_ranger_strike : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_ranger_strike);

    void OnCast(SpellMissInfo /*missInfo*/)
    {
        if (urand(0, 1))
            GetCaster()->SendPlaySpellVisual(GetCaster()->GetVictim(), SPELL_VISUAL_RANGER_STRIKE_RIGHT, 0, 0, 1.0f, true);
        else
            GetCaster()->SendPlaySpellVisual(GetCaster()->GetVictim(), SPELL_VISUAL_RANGER_STRIKE_LEFT, 0, 0, 1.0f, true);
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_sylvanas_windrunner_ranger_strike::OnCast);
    }
};

// Shadow Daggers P2 - 348089
class spell_sylvanas_windrunner_shadow_dagger_p2 : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_shadow_dagger_p2);

    void OnPreCast(SpellMissInfo /*missInfo*/)
    {
        GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_SHADOW_DAGGER, 0, 0);

    }

    void OnCast(SpellMissInfo /*missInfo*/)
    {
        GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_SHADOW_DAGGER_SOUND, 0, 0);
    }

    void Register() override
    {

        BeforeHit += BeforeSpellHitFn(spell_sylvanas_windrunner_shadow_dagger_p2::OnCast);

    }
};

// Blasphemy Stun - 357728
class spell_sylvanas_windrunner_blasphemy_stun : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_blasphemy_stun);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        //GetCaster()->SetUnitFlag(UNIT_FLAG_STUNNED);
        //Getcaster()->CastSpell(GetCaster(), SPELL_INTERMISSION_STUN);
        GetCaster()->CastSpell(GetTarget(), SPELL_SERVERSIDE_STUN, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 10000));

    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveUnitFlag(UNIT_FLAG_STUNNED);
        GetTarget()->RemoveAura(SPELL_INTERMISSION_STUN);

    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sylvanas_windrunner_blasphemy_stun::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_blasphemy_stun::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};
 
// Windrunner - 347504
class spell_sylvanas_windrunner_windrunner : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_windrunner);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->m_Events.AddEvent(new PauseAttackState(GetTarget(), true), GetTarget()->m_Events.CalculateTime(1ms));

    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->m_Events.AddEvent(new PauseAttackState(GetTarget(), false), GetTarget()->m_Events.CalculateTime(1ms));
        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_SHEATHE_BOW, 0), GetCaster()->m_Events.CalculateTime(1ms));
        GetTarget()->resetAttackTimer(BASE_ATTACK);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sylvanas_windrunner_windrunner::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_windrunner::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Windrunner (Disappear 01) - 352303
// Windrunner (Disappear 02) - 358975
class spell_sylvanas_windrunner_disappear : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_disappear);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SYLVANAS_ROOT });
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;

        // TODO: the first one is used for Windrunner, the duration is dynamic, most likely dependent on the action performed
        // whereas the second is only for Domination Chains and rest of the phases
        if (GetSpellInfo()->Id == SPELL_WINDRUNNER_DISAPPEAR_02)
            GetCaster()->CastSpell(GetCaster(), SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 3600));
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sylvanas_windrunner_disappear::OnApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Withering Fire - 347928
class spell_sylvanas_windrunner_withering_fire : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_withering_fire);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (sylvanas->GetAreaId() == AREA_PINNACLE_OF_DOMINANCE)
                    sylvanas->CastSpell(GetHitUnit(), GetEffectInfo(EFFECT_0).TriggerSpell, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_withering_fire::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Desecrating Shot - 348627
class spell_sylvanas_windrunner_desecrating_shot : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_desecrating_shot);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), GetEffectInfo(EFFECT_0).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_desecrating_shot::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class HeartseekerDamageEvent : public BasicEvent
{
public:
    HeartseekerDamageEvent(Unit* actor, Unit* victim) : _actor(actor), _victim(victim) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {

        if (_actor->GetAreaId() == AREA_THE_CRUCIBLE)
        {
            _actor->CastSpell(_victim, SPELL_BANSHEE_HEARTSEEKER_PHYSICAL_DAMAGE, true);
            _actor->CastSpell(_victim, SPELL_BANSHEE_HEARTSEEKER_SHADOW_DAMAGE, true);

            return true;
        }

        else
        {
            _actor->CastSpell(_victim, SPELL_RANGER_HEARTSEEKER_PHYSICAL_DAMAGE, true);
            _actor->CastSpell(_victim, SPELL_RANGER_HEARTSEEKER_SHADOW_DAMAGE, true);

            return true;
        }


        return true;
    }

private:
    Unit* _actor;
    Unit* _victim;
};

class HeartseekerMissileEvent : public BasicEvent
{
public:
    HeartseekerMissileEvent(Unit* actor, Unit* victim) : _actor(actor), _victim(victim) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        _actor->SendPlaySpellVisual(_victim, SPELL_VISUAL_HEARTSEEKER, 0, 0, 36.0f, false);

        uint32 timeToTarget = _actor->GetDistance(_victim) * (0.0277 * 1000);

        _actor->m_Events.AddEvent(new HeartseekerDamageEvent(_actor, _victim), _actor->m_Events.CalculateTime(Milliseconds(timeToTarget)));

        if (Aura* rangersHeartseeker = _actor->GetAura(SPELL_RANGER_HEARTSEEKER_CHARGE))
            rangersHeartseeker->ModStackAmount(-1, AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);

        if (Aura* bansheesHeartseeker = _actor->GetAura(SPELL_BANSHEE_HEARTSEEKER_CHARGE))
            bansheesHeartseeker->ModStackAmount(-1, AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);

        if (Aura* bansheeBlades = _actor->GetAura(SPELL_BANSHEE_BLADES_CHARGE))
            bansheeBlades->ModStackAmount(-1, AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);

        return true;
    }

private:
    Unit* _actor;
    Unit* _victim;
};

class ShadowDaggersP2Missile : public BasicEvent
{
public:
    ShadowDaggersP2Missile(Unit* actor, Unit* victim) : _actor(actor), _victim(victim) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {

        _actor->CastSpell(_victim, SPELL_SHADOW_DAGGER_MISSILE, true);

        return true;


    }

private:
    Unit* _actor;
    Unit* _victim;
};

class ShadowDaggersP2 : public BasicEvent
{
public:
    ShadowDaggersP2(Unit* actor, Unit* victim) : _actor(actor), _victim(victim) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        _actor->SendPlaySpellVisual(_victim, SPELL_VISUAL_SHADOW_DAGGER, 0, 0, 100.0f, false);

        return true;
    }

private:
    Unit* _actor;
    Unit* _victim;
};


class BansheeBladesEvent : public BasicEvent
{
public:
    BansheeBladesEvent(Unit* caster, Unit* victim) : _caster(caster), _victim(victim) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {


        _caster->CastSpell(_victim, SPELL_BANSHEE_BLADES_PHYSICAL_DAMAGE, true);
        _caster->CastSpell(_victim, SPELL_BANSHEE_BLADES_SHADOW_DAMAGE, true);


        if (Aura* bansheeBlades = _caster->GetAura(SPELL_BANSHEE_BLADES_CHARGE))
            bansheeBlades->ModStackAmount(-1, AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);

        return true;


        // _caster->SendPlaySpellVisual(_victim, SPELL_VISUAL_RANGER_HEARTSEEKER, 0, 0, 36.0f, false);
         //if (Aura* bansheeHeartseeker = _caster->GetAura(SPELL_BANSHEE_HEARTSEEKER_CHARGE))
           //  bansheeHeartseeker->ModStackAmount(-1, AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);
         //return true;
    }

private:
    Unit* _caster;
    Unit* _victim;
};

// SPELL_SHADOW_DAGGER_MISSILE = 348089
class spell_sylvanas_windrunner_daggers_p2 : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_daggers_p2);

    void OnPrecast() override
    {

            GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_SHADOW_DAGGER, 0, 0);
            GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_SHADOW_DAGGER_SOUND, 0, 0);

    }

    void HandleAfterCast()
    {

    }
    void Register() override
    {

        AfterCast += SpellCastFn(spell_sylvanas_windrunner_daggers_p2::HandleAfterCast);
    }
};



// Ranger's Heartseeker - 352663
class spell_sylvanas_windrunner_ranger_heartseeker : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_ranger_heartseeker);

    void OnPrecast() override
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->m_Events.AddEvent(new SetSheatheStateOrNameplate(caster, DATA_CHANGE_SHEATHE_UNARMED, 0), caster->m_Events.CalculateTime(16ms));
        caster->m_Events.AddEvent(new SetSheatheStateOrNameplate(caster, DATA_CHANGE_SHEATHE_BOW, 0), caster->m_Events.CalculateTime(328ms));
        caster->m_Events.AddEvent(new SetSheatheStateOrNameplate(caster, DATA_SYLVANAS_SHADOW_COPY_RIDING, 0), caster->m_Events.CalculateTime(343ms));
        caster->m_Events.AddEvent(new SetSheatheStateOrNameplate(caster, DATA_CHANGE_NAMEPLATE_TO_SYLVANAS, 0), caster->m_Events.CalculateTime(2s));
    }

    void Register() override { }
};

class spell_sylvanas_windrunner_ranger_heartseeker_aura : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_ranger_heartseeker_aura);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        Unit* actualTank = caster->GetVictim();

        caster->SetFacingToObject(actualTank);

        caster->m_Events.AddEvent(new HeartseekerMissileEvent(caster, actualTank), caster->m_Events.CalculateTime(1ms));
        caster->m_Events.AddEvent(new HeartseekerMissileEvent(caster, actualTank), caster->m_Events.CalculateTime(281ms));
        caster->m_Events.AddEvent(new HeartseekerMissileEvent(caster, actualTank), caster->m_Events.CalculateTime(562ms));
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (sylvanas->IsAIEnabled())
                    sylvanas->AI()->DoAction(ACTION_RANGER_SHOT);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sylvanas_windrunner_ranger_heartseeker_aura::OnApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_ranger_heartseeker_aura::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Ranger's Heartseeker (Shadow damage) - 352652
class spell_sylvanas_windrunner_ranger_heartseeker_shadow_damage : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_ranger_heartseeker_shadow_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_BANSHEE_MARK,
            SPELL_BANSHEES_BANE
            });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        // NOTE: not the best way to check, we could use phase, but it works great
        if (GetCaster()->GetAreaId() == AREA_THE_CRUCIBLE)
            GetCaster()->CastSpell(GetHitUnit(), SPELL_BANSHEES_BANE, true);
        else
            GetCaster()->CastSpell(GetHitUnit(), SPELL_BANSHEE_MARK, true);
    }

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;


        SetHitDamage(35804.0f);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_ranger_heartseeker_shadow_damage::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_ranger_heartseeker_shadow_damage::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Ranger's Heartseeker (Physical damage) - 352651
class spell_sylvanas_windrunner_ranger_heartseeker_physical_damage : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_ranger_heartseeker_physical_damage);


    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;

        // NOTE: this is completely based off research on different sniffs. The reduction might be inaccurate, but it's pretty close to Blizzard's.
        uint32 damage = GetHitDamage();

        float distanceDmgMod = caster->GetExactDist2d(GetHitUnit()) / 4.5f;

        if (distanceDmgMod < 1.0f)
            return;

        damage /= distanceDmgMod;

        SetHitDamage(35804.0f);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_ranger_heartseeker_physical_damage::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }

};


// Banshee's Heartseeker - 353969
class spell_sylvanas_windrunner_banshee_heartseeker : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_heartseeker);

    void OnPrecast() override
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->m_Events.AddEvent(new SetSheatheStateOrNameplate(caster, DATA_CHANGE_SHEATHE_UNARMED, 0), caster->m_Events.CalculateTime(16ms));
        caster->m_Events.AddEvent(new SetSheatheStateOrNameplate(caster, DATA_CHANGE_SHEATHE_BOW, 0), caster->m_Events.CalculateTime(328ms));
        caster->m_Events.AddEvent(new SetSheatheStateOrNameplate(caster, DATA_SYLVANAS_SHADOW_COPY_RIDING, 0), caster->m_Events.CalculateTime(343ms));
        caster->m_Events.AddEvent(new SetSheatheStateOrNameplate(caster, DATA_CHANGE_NAMEPLATE_TO_SYLVANAS, 0), caster->m_Events.CalculateTime(2s));
    }

    void Register() override { }
};


class spell_sylvanas_windrunner_banshee_heartseeker_aura : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_banshee_heartseeker_aura);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        Unit* actualTank = caster->GetVictim();

        caster->SetFacingToObject(actualTank);

        caster->m_Events.AddEvent(new HeartseekerMissileEvent(caster, actualTank), caster->m_Events.CalculateTime(1ms));
        caster->m_Events.AddEvent(new HeartseekerMissileEvent(caster, actualTank), caster->m_Events.CalculateTime(281ms));
        caster->m_Events.AddEvent(new HeartseekerMissileEvent(caster, actualTank), caster->m_Events.CalculateTime(562ms));
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (sylvanas->IsAIEnabled())
                    sylvanas->AI()->DoAction(ACTION_RANGER_SHOT);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sylvanas_windrunner_banshee_heartseeker_aura::OnApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_banshee_heartseeker_aura::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Banshee's Heartseeker (Physical damage) - 353968
class spell_sylvanas_windrunner_banshee_heartseeker_physical_damage : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_heartseeker_physical_damage);


    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;

        // NOTE: this is completely based off research on different sniffs. The reduction might be inaccurate, but it's pretty close to Blizzard's.
        uint32 damage = GetHitDamage();


        SetHitDamage(33100.0f);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_heartseeker_physical_damage::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }

};

// Banshee's Heartseeker (Shadow damage) - 353967
class spell_sylvanas_windrunner_banshee_heartseeker_shadow_damage : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_heartseeker_shadow_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_BANSHEE_MARK,
            SPELL_BANSHEES_BANE
            });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        // NOTE: not the best way to check, we could use phase, but it works great
        if (GetCaster()->GetAreaId() == AREA_THE_CRUCIBLE)
            GetCaster()->CastSpell(GetHitUnit(), SPELL_BANSHEES_BANE, true);
        else
            GetCaster()->CastSpell(GetHitUnit(), SPELL_BANSHEE_MARK, true);
    }

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;

        // NOTE: this is completely based off research on different sniffs. The reduction might be inaccurate, but it's pretty close to Blizzard's.
        uint32 damage = GetHitDamage();


        SetHitDamage(10344.0f);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_heartseeker_shadow_damage::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_heartseeker_shadow_damage::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Banshee's Blades - 358181
class spell_sylvanas_windrunner_banshee_blades : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_blades);

    void OnPrecast() override
    {

        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_SHEATHE_UNARMED, 0), GetCaster()->m_Events.CalculateTime(16ms));
        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_SHEATHE_DAGGERS, 0), GetCaster()->m_Events.CalculateTime(328ms));
    }

    void HandleAfterCast()
    {
        GetCaster()->RemoveAurasDueToSpell(SPELL_SYLVANAS_ROOT);
        GetCaster()->m_Events.AddEvent(new BansheeBladesEvent(GetCaster(), GetCaster()->GetVictim()), GetCaster()->m_Events.CalculateTime(1ms));
        GetCaster()->m_Events.AddEvent(new BansheeBladesEvent(GetCaster(), GetCaster()->GetVictim()), GetCaster()->m_Events.CalculateTime(281ms));
        GetCaster()->m_Events.AddEvent(new BansheeBladesEvent(GetCaster(), GetCaster()->GetVictim()), GetCaster()->m_Events.CalculateTime(562ms));
        GetCaster()->m_Events.AddEvent(new BansheeBladesEvent(GetCaster(), GetCaster()->GetVictim()), GetCaster()->m_Events.CalculateTime(1043ms));
    }
    void Register() override
    {

        AfterCast += SpellCastFn(spell_sylvanas_windrunner_banshee_blades::HandleAfterCast);
    }
};

class spell_sylvanas_windrunner_banshee_weapons_aura : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_banshee_weapons_aura);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;

        GetCaster()->m_Events.AddEvent(new BansheeBladesEvent(GetCaster(), GetCaster()->GetVictim()), GetCaster()->m_Events.CalculateTime(1ms));
        GetCaster()->m_Events.AddEvent(new BansheeBladesEvent(GetCaster(), GetCaster()->GetVictim()), GetCaster()->m_Events.CalculateTime(281ms));
        GetCaster()->m_Events.AddEvent(new BansheeBladesEvent(GetCaster(), GetCaster()->GetVictim()), GetCaster()->m_Events.CalculateTime(562ms));
        //GetCaster()->m_Events.AddEvent(new RangerHeartseekerMissileEvent(GetCaster(), GetCaster()->GetVictim()), GetCaster()->m_Events.CalculateTime(843ms));
        //GetCaster()->CastSpell(GetHitUnit(), SPELL_BANSHEE_BLADES_PHYSICAL_DAMAGE, true);

    }
    void HandleAfterCast()
    {
        GetCaster()->m_Events.AddEvent(new BansheeBladesEvent(GetCaster(), GetCaster()->GetVictim()), GetCaster()->m_Events.CalculateTime(1ms));
        GetCaster()->m_Events.AddEvent(new BansheeBladesEvent(GetCaster(), GetCaster()->GetVictim()), GetCaster()->m_Events.CalculateTime(281ms));
        GetCaster()->m_Events.AddEvent(new BansheeBladesEvent(GetCaster(), GetCaster()->GetVictim()), GetCaster()->m_Events.CalculateTime(562ms));
    }
    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sylvanas_windrunner_banshee_weapons_aura::OnApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);

    }
};

// Banshee's Blades (Shadow damage) - 358183
class spell_sylvanas_windrunner_banshee_blades_shadow_damage : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_blades_shadow_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_BANSHEE_MARK,
            SPELL_BANSHEES_BANE
            });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        // NOTE: not the best way to check, we could use phase, but it works great
        if (GetCaster()->GetAreaId() == AREA_THE_CRUCIBLE)
            GetCaster()->CastSpell(GetHitUnit(), SPELL_BANSHEES_BANE, true);
        else
            GetCaster()->CastSpell(GetHitUnit(), SPELL_BANSHEE_MARK, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_blades_shadow_damage::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Banshee's Blades (Physical damage) - 358182
class spell_sylvanas_windrunner_banshee_blades_physical_damage : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_blades_physical_damage);



    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;

        // NOTE: this is completely based off research on different sniffs. The reduction might be inaccurate, but it's pretty close to Blizzard's.
        uint32 damage = GetHitDamage();


        SetHitDamage(6886.0f);
    }



    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_blades_physical_damage::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);


    }
};

// Domination Chains - 349419
class spell_sylvanas_windrunner_domination_chains : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_domination_chains);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DOMINATION_ARROW_ACTIVATE });
    }

    void OnCast(SpellMissInfo /*missInfo*/)
    {
        std::list<Creature*> arrowList;
        GetCreatureListWithEntryInGrid(arrowList, GetCaster(), NPC_DOMINATION_ARROW, 500.0f);

        for (Creature* arrow : arrowList)
        {
            if (arrow->IsAIEnabled())
                arrow->AI()->DoAction(ACTION_ACTIVATE_DOMINATION_ARROW);
        }
    }

    void HandleAfterCast()
    {
        GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_NORMAL_CAST, 0, 0);
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_sylvanas_windrunner_domination_chains::OnCast);
        AfterCast += SpellCastFn(spell_sylvanas_windrunner_domination_chains::HandleAfterCast);
    }
};

// Domination Arrow (Fall) - 352317
// Domination Arrow (Fall and Spawn) - 352319
class spell_sylvanas_windrunner_domination_arrow : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_domination_arrow);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), GetEffectInfo(EFFECT_0).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_domination_arrow::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Domination Chain (Player) - 349451
class spell_sylvanas_windrunner_domination_chain : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_domination_chain);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DOMINATION_CHAIN_PERIODIC });
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;

        _playerGUID = GetTarget()->GetGUID();

        if (GetTarget()->HasAura(SPELL_BARBED_ARROW))
            GetTarget()->RemoveAura(SPELL_BARBED_ARROW);

        GetCaster()->CastSpell(GetTarget(), SPELL_DOMINATION_CHAIN_PERIODIC, true);

        _arrowAreaTriggerGUID = AreaTrigger::CreateNewMovementForceId(GetCaster()->GetMap(), DATA_AREATRIGGER_DOMINATION_ARROW);

        GetTarget()->ApplyMovementForce(_arrowAreaTriggerGUID, GetCaster()->GetPosition(), 3.20000004768371582f, MovementForceType::Gravity);

        Aura* chainPull = GetTarget()->GetAura(SPELL_DOMINATION_CHAIN_PERIODIC);

        if (chainPull && chainPull->GetStackAmount() == 2)
            _arrowAreaTriggerGUID = AreaTrigger::CreateNewMovementForceId(GetCaster()->GetMap(), DATA_AREATRIGGER_DOMINATION_ARROW);

        GetTarget()->ApplyMovementForce(_arrowAreaTriggerGUID, GetCaster()->GetPosition(), 6.40000009537f, MovementForceType::Gravity);

        if (chainPull && chainPull->GetStackAmount() == 3)
            _arrowAreaTriggerGUID = AreaTrigger::CreateNewMovementForceId(GetCaster()->GetMap(), DATA_AREATRIGGER_DOMINATION_ARROW);

        GetTarget()->ApplyMovementForce(_arrowAreaTriggerGUID, GetCaster()->GetPosition(), 9.60000014305f, MovementForceType::Gravity);

        if (chainPull && chainPull->GetStackAmount() == 4)
            _arrowAreaTriggerGUID = AreaTrigger::CreateNewMovementForceId(GetCaster()->GetMap(), DATA_AREATRIGGER_DOMINATION_ARROW);

        GetTarget()->ApplyMovementForce(_arrowAreaTriggerGUID, GetCaster()->GetPosition(), 12.8000001907f, MovementForceType::Gravity);

        if (chainPull && chainPull->GetStackAmount() == 5)
            _arrowAreaTriggerGUID = AreaTrigger::CreateNewMovementForceId(GetCaster()->GetMap(), DATA_AREATRIGGER_DOMINATION_ARROW);

        GetTarget()->ApplyMovementForce(_arrowAreaTriggerGUID, GetCaster()->GetPosition(), 16.0000002384f, MovementForceType::Gravity);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DOMINATION_CHAIN_PERIODIC);

        GetTarget()->RemoveMovementForce(_arrowAreaTriggerGUID);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sylvanas_windrunner_domination_chain::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_domination_chain::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }

private:
    ObjectGuid _playerGUID;
    ObjectGuid _arrowAreaTriggerGUID;
};

// Domination Chain (Periodic) - 349458
class spell_sylvanas_windrunner_domination_chain_periodic : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_domination_chain_periodic);

    void HandleEffectPeriodic(AuraEffect const* aurEff)
    {
        if (!GetCaster())
            return;

        if (aurEff->GetTickNumber() == 3)
            GetCaster()->CastSpell(GetTarget(), SPELL_DOMINATION_CHAIN_PERIODIC, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sylvanas_windrunner_domination_chain_periodic::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// Wailing Arrow - 347609
class spell_sylvanas_windrunner_wailing_arrow : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_wailing_arrow);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    int32 CalcCastTime(int32 castTime) override
    {
        if (GetCaster()->GetAreaId() == AREA_THE_CRUCIBLE)
            return 1500;

        return castTime;
    }

    void OnPrecast() override
    {


        if (GetCaster()->GetAreaId() == AREA_PINNACLE_OF_DOMINANCE)
        { 
            GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_SYLVANAS_SHADOW_COPY_RIDING, 0), GetCaster()->m_Events.CalculateTime(2s));
            GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_NAMEPLATE_TO_SYLVANAS, 0), GetCaster()->m_Events.CalculateTime(3100ms + 250ms));
        }

        else if (GetCaster()->GetAreaId() == AREA_THE_CRUCIBLE)
        { 
                GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_WAILING_ARROW_CHARGE, 0, 0);
                GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_SYLVANAS_SHADOW_COPY_RIDING, 0), GetCaster()->m_Events.CalculateTime(840ms));
                GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_NAMEPLATE_TO_SYLVANAS, 0), GetCaster()->m_Events.CalculateTime(1500ms + 250ms));
        }



    }

    void OnCast(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), GetEffectInfo(EFFECT_0).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_sylvanas_windrunner_wailing_arrow::OnCast, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Wailing Arrow (Trigger) - 348056
class spell_sylvanas_windrunner_wailing_arrow_trigger : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_wailing_arrow_trigger);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo
        ({
            spellInfo->GetEffect(EFFECT_0).TriggerSpell,
            spellInfo->GetEffect(EFFECT_1).TriggerSpell
            });
    }

    void HandleDummyEffect(SpellEffIndex effIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), GetEffectInfo(effIndex).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_wailing_arrow_trigger::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_wailing_arrow_trigger::HandleDummyEffect, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// Wailing Arrow (Raid damage) - 357618
class spell_sylvanas_windrunner_wailing_arrow_raid_damage : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_wailing_arrow_raid_damage);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;

        // NOTE: this is completely based off research on different sniffs. The reduction might be inaccurate, but it's pretty close to Blizzard's.
        uint32 damage = GetHitDamage();

        float distanceDmgMod = caster->GetExactDist2d(GetHitUnit()) / 4.5f;

        if (distanceDmgMod < 1.0f)
            return;

        damage /= distanceDmgMod;

        SetHitDamage(damage);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_wailing_arrow_raid_damage::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};



// Veil of Darkness (Phase 1 - Fade) - 352470
// Veil of Darkness (Phase 2 - Fade) - 353273
// Veil of Darkness (Phase 3 - Fade) - 354168
class spell_sylvanas_windrunner_veil_of_darkness_fade : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_veil_of_darkness_fade);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_SYLVANAS_ROOT,
            SPELL_VEIL_OF_DARKNESS_DESELECT,
            SPELL_VEIL_OF_DARKNESS_SCREEN_FOG
            });
    }

    void OnPrecast() override
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 9750));
    }

    void OnCast(SpellMissInfo /*missInfo*/)
    {
        GetCaster()->GetInstanceScript()->DoCastSpellOnPlayers(SPELL_VEIL_OF_DARKNESS_SCREEN_FOG);
        GetCaster()->CastSpell(GetCaster(), SPELL_VEIL_OF_DARKNESS_DESELECT, true);
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_sylvanas_windrunner_veil_of_darkness_fade::OnCast);
    }
};


// Veil of Darkness Phase Three Pointer - 357876
class spell_sylvanas_windrunner_veil_of_darkness_pointer : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_veil_of_darkness_pointer);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        //if (Unit* caster = GetCaster())

        //GetCaster()->CastSpell(GetTarget(), SPELL_VEIL_OF_DARKNESS_P3_ONLY, false);
        //GetCaster()->CastSpell(GetTarget(), SPELL_VEIL_OF_DARKNESS_PHASE_THREE_EMPTY, true);
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                
                    sylvanas->CastSpell(GetTarget(), SPELL_VEIL_OF_DARKNESS_PHASE_THREE_EMPTY, true);
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_veil_of_darkness_pointer::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Veil Of Darkness Empty - 354141

class spell_sylvanas_windrunner_veil_of_darkness_phase_three_swap : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_veil_of_darkness_phase_three_swap);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        //if (Unit* caster = GetCaster())

            
            GetCaster()->CastSpell(GetTarget(), SPELL_VEIL_OF_DARKNESS_ABSORB_AURA, false);
            GetCaster()->CastSpell(GetTarget(), SPELL_VEIL_OF_DARKNESS_DAMAGE, false);
            GetTarget()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS_3_03, 0, 0);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_veil_of_darkness_phase_three_swap::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};


// Veil of Darkness (Phase 1) - 347726
class spell_sylvanas_windrunner_veil_of_darkness_phase_1 : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_veil_of_darkness_phase_1);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo
        ({
            SPELL_VEIL_OF_DARKNESS_PHASE_1_GROW,
            spellInfo->GetEffect(EFFECT_0).TriggerSpell,
            spellInfo->GetEffect(EFFECT_1).TriggerSpell
            });
    }

    void OnPrecast() override
    {
        GetCaster()->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);
        GetCaster()->SetAnimTier(AnimTier::Fly);

        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_SHEATHE_UNARMED, 0), GetCaster()->m_Events.CalculateTime(1ms));
        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_SYLVANAS_SHADOW_COPY_RIDING, 0), GetCaster()->m_Events.CalculateTime(25ms));

        GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_VEIL_OF_DARKNESS, 0, 0);
    }

    void HandleDummyEffect(SpellEffIndex effIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), GetEffectInfo(effIndex).TriggerSpell, true);
        //GetCaster()->CastSpell(GetHitUnit(), SPELL_VEIL_OF_DARKNESS_ABSORB_AURA, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_AURA_STACK, + 5));
        if (Aura* veilAbsorb = GetHitUnit()->GetAura(SPELL_VEIL_OF_DARKNESS_ABSORB_AURA))
            veilAbsorb->SetStackAmount((5));
        GetCaster()->CastSpell(GetHitUnit(), SPELL_VEIL_OF_DARKNESS_VISUAL_SPREAD);
    }

    void HandleAfterCast()
    {
        GetCaster()->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
        GetCaster()->SetAnimTier(AnimTier::Ground);

        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_NAMEPLATE_TO_SYLVANAS, 0), GetCaster()->m_Events.CalculateTime(25ms));

        GetCaster()->m_Events.AddEvent(new PauseAttackState(GetCaster(), false), GetCaster()->m_Events.CalculateTime(250ms));
        GetCaster()->resetAttackTimer(BASE_ATTACK);
    }




    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_veil_of_darkness_phase_1::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_veil_of_darkness_phase_1::HandleDummyEffect, EFFECT_1, SPELL_EFFECT_DUMMY);
        AfterCast += SpellCastFn(spell_sylvanas_windrunner_veil_of_darkness_phase_1::HandleAfterCast);
    }
};



// Veil Of Darkness Absorb Aura - 347704
class spell_sylvanas_windrunner_veil_of_darkness_visual_circle : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_veil_of_darkness_visual_circle);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;
        // if (Cloud && !Cloud->IsWithinDist(target, 4, false))
        Aura* veilAbsorb = GetTarget()->GetAura(SPELL_VEIL_OF_DARKNESS_ABSORB_AURA);
        if (veilAbsorb && veilAbsorb->GetStackAmount() <= 1)
            GetTarget()->RemoveAurasDueToSpell(SPELL_VEIL_OF_DARKNESS_VISUAL_SPREAD);
    }



    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sylvanas_windrunner_veil_of_darkness_visual_circle::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        //AfterEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_veil_of_darkness_visual_circle::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Veil of Darkness (Phase 2) - 347741
class spell_sylvanas_windrunner_veil_of_darkness_phase_2 : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_veil_of_darkness_phase_2);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo
        ({
            SPELL_VEIL_OF_DARKNESS_PHASE_2_GROW,
            spellInfo->GetEffect(EFFECT_0).TriggerSpell
            });
    }

    void OnPrecast() override
    {
        GetCaster()->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);
        GetCaster()->SetAnimTier(AnimTier::Fly);

    }

    void HandleDummyEffect(SpellEffIndex effIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), GetEffectInfo(effIndex).TriggerSpell, true);
        if (Aura* veilAbsorb = GetHitUnit()->GetAura(SPELL_VEIL_OF_DARKNESS_ABSORB_AURA))
            veilAbsorb->SetStackAmount((5));
        GetCaster()->CastSpell(GetHitUnit(), SPELL_VEIL_OF_DARKNESS_VISUAL_SPREAD);
    }

    void OnCast(SpellMissInfo /*missInfo*/)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_VEIL_OF_DARKNESS_PHASE_2_AREA, true);
    }

    void HandleAfterCast()
    {

        GetCaster()->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
        GetCaster()->SetAnimTier(AnimTier::Ground);
        GetCaster()->CastSpell(GetCaster(), SPELL_VEIL_OF_DARKNESS_PHASE_2_AREA, true);

    }


    void Register() override
    {

        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_veil_of_darkness_phase_2::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        BeforeHit += BeforeSpellHitFn(spell_sylvanas_windrunner_veil_of_darkness_phase_2::OnCast);
        AfterCast += SpellCastFn(spell_sylvanas_windrunner_veil_of_darkness_phase_2::HandleAfterCast);
    }
};
// Veil of Darkness (Phase 2) - 352225
class spell_sylvanas_windrunner_veil_of_darkness_phase_2_fog : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_veil_of_darkness_phase_2_fog);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo
        ({ SPELL_VEIL_OF_DARKNESS_ABSORB_AURA });
    }



    void HandleDummyEffect(SpellEffIndex effIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_VEIL_OF_DARKNESS_ABSORB_AURA, true);
        if (Aura* veilAbsorb = GetHitUnit()->GetAura(SPELL_VEIL_OF_DARKNESS_ABSORB_AURA))
            veilAbsorb->SetStackAmount((5));
        GetCaster()->CastSpell(GetHitUnit(), SPELL_VEIL_OF_DARKNESS_VISUAL_SPREAD);
    }


    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_veil_of_darkness_phase_2_fog::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }




};

// Veil of Darkness (Phase 3) - 354142
class spell_sylvanas_windrunner_veil_of_darkness_phase_3 : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_veil_of_darkness_phase_3);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo
        ({
            SPELL_VEIL_OF_DARKNESS_PHASE_3_GROW

            });
    }

    void OnPrecast() override
    {
        GetCaster()->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);
        GetCaster()->SetAnimTier(AnimTier::Fly);
        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_SHEATHE_UNARMED, 0), GetCaster()->m_Events.CalculateTime(1ms));


    }

    void HandleDummyEffect(SpellEffIndex effIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_VEIL_OF_DARKNESS_DAMAGE, true);


    }


    void HandleAfterCast()
    {
        GetCaster()->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
        GetCaster()->SetAnimTier(AnimTier::Ground);
        GetCaster()->m_Events.AddEvent(new PauseAttackState(GetCaster(), false), GetCaster()->m_Events.CalculateTime(250ms));
        GetCaster()->resetAttackTimer(BASE_ATTACK);
        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_SHEATHE_BOW, 0), GetCaster()->m_Events.CalculateTime(1ms));
    }

    void Register() override
    {

        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_veil_of_darkness_phase_3::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        AfterCast += SpellCastFn(spell_sylvanas_windrunner_veil_of_darkness_phase_3::HandleAfterCast);
    }
};


// Rive - 353417
class spell_sylvanas_windrunner_rive : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_rive);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo
        ({
            SPELL_RIVE_CHAIN,
            spellInfo->GetEffect(EFFECT_0).TriggerSpell
            });
    }

    void OnPrecast() override
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    if (Creature* shadowCopy3 = ObjectAccessor::GetCreature(*sylvanas, ai->GetShadowCopyJumperGuid(3)))
                    {
                        shadowCopy3->CastSpell(sylvanas, SPELL_RIVE_CHAIN, false);

                        if (GameObject* platformSpear = shadowCopy3->FindNearestGameObjectOfType(GameobjectTypes::GAMEOBJECT_TYPE_GOOBER, 15.0f))
                            platformSpear->SetSpellVisualId(SPELL_VISUAL_TORGHAST_SPIRE_BREAK, sylvanas->GetGUID());
                    }
                }
            }
        }
    }

    void HandleAfterCast()
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    if (Creature* shadowCopy = ObjectAccessor::GetCreature(*sylvanas, ai->GetShadowCopyJumperGuid(0)))
                        sylvanas->CastSpell(shadowCopy->GetPosition(), GetEffectInfo(EFFECT_0).TriggerSpell, true);
                }
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sylvanas_windrunner_rive::HandleAfterCast);
    }
};

// Rive (Fast) - 353418
class spell_sylvanas_windrunner_rive_fast : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_rive_fast);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo
        ({
            SPELL_RIVE_CHAIN_FAST,
            spellInfo->GetEffect(EFFECT_0).TriggerSpell
            });
    }

    void OnPrecast() override
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    if (Creature* shadowCopy3 = ObjectAccessor::GetCreature(*sylvanas, ai->GetShadowCopyJumperGuid(3)))
                    {
                        shadowCopy3->CastSpell(sylvanas, SPELL_RIVE_CHAIN_FAST, false);

                        if (GameObject* platformSpear = shadowCopy3->FindNearestGameObjectOfType(GameobjectTypes::GAMEOBJECT_TYPE_GOOBER, 15.0f))
                            platformSpear->SetSpellVisualId(SPELL_VISUAL_TORGHAST_SPIRE_BREAK_FAST, sylvanas->GetGUID());
                    }
                }
            }
        }
    }

    void HandleAfterCast()
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    if (Creature* shadowCopy = ObjectAccessor::GetCreature(*sylvanas, ai->GetShadowCopyJumperGuid(0)))
                        sylvanas->CastSpell(shadowCopy->GetPosition(), GetEffectInfo(EFFECT_0).TriggerSpell, true);
                }
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sylvanas_windrunner_rive_fast::HandleAfterCast);
    }
};

// Banshee Wail - 348094
class spell_sylvanas_windrunner_banshee_wail : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_wail);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo
        ({
            SPELL_BANSHEE_WAIL_EXPIRE,
            SPELL_BANSHEE_WAIL_MARKER,
            spellInfo->GetEffect(EFFECT_0).TriggerSpell
            });
    }

    void OnPrecast() override
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_BANSHEE_WAIL_EXPIRE, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 5000));
        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_SYLVANAS_SHADOW_COPY_RIDING, 0), GetCaster()->m_Events.CalculateTime(4600ms));
        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_NAMEPLATE_TO_SYLVANAS, 0), GetCaster()->m_Events.CalculateTime(7s));

        std::list<Player*> targetList;
        GetPlayerListInGrid(targetList, GetCaster(), 250.0f);

        for (Player* target : targetList)
            GetCaster()->CastSpell(target, SPELL_BANSHEE_WAIL_MARKER, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 4650));
    }

    void HandleAfterCast()
    {
        std::list<Player*> targetList;
        GetPlayerListInGrid(targetList, GetCaster(), 200.0f);

        for (Player* target : targetList)
            GetCaster()->CastSpell(target, GetEffectInfo(EFFECT_0).TriggerSpell, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sylvanas_windrunner_banshee_wail::HandleAfterCast);
    }
};

// Banshee Wail (Marker) - 357719
class spell_sylvanas_windrunner_banshee_wail_marker : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_banshee_wail_marker);

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_SCREAM_EXPIRE, 0, 0);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_banshee_wail_marker::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Banshee Wail (Triggered Missile) - 348108
class spell_sylvanas_windrunner_banshee_wail_triggered_missile : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_wail_triggered_missile);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo
        ({
            spellInfo->GetEffect(EFFECT_0).TriggerSpell,
            spellInfo->GetEffect(EFFECT_1).TriggerSpell
            });
    }

    void HandleDummyEffect(SpellEffIndex effIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), GetEffectInfo(effIndex).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_wail_triggered_missile::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_wail_triggered_missile::HandleDummyEffect, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// Banshee Wail (Interrupt) - 351252
class spell_sylvanas_windrunner_banshee_wail_interrupt : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_wail_interrupt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_BANSHEE_WAIL_SILENCE });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        if (GetHitUnit()->HasUnitState(UNIT_STATE_CASTING))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_BANSHEE_WAIL_SILENCE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_wail_interrupt::HandleHit, EFFECT_0, SPELL_EFFECT_INTERRUPT_CAST);
    }
};




// Banshee Scream (Cast)- 353952
class spell_sylvanas_windrunner_banshee_scream : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_scream);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo
        ({
            SPELL_BANSHEE_SCREAM_EXPIRE,
            SPELL_BANSHEE_SCREAM_MARKER,
            spellInfo->GetEffect(EFFECT_0).TriggerSpell
            });
    }

    void OnPrecast() override
    {
        //GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_SHEATHE_UNARMED), GetCaster()->m_Events.CalculateTime(16ms));
        //GetCaster()->m_Events.AddEvent(new SetDisplayIdBanshee(GetCaster(), DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL), GetCaster()->m_Events.CalculateTime(4650ms));
        //GetCaster()->m_Events.AddEvent(new SetDisplayIdElf(GetCaster(), DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL), GetCaster()->m_Events.CalculateTime(5028ms));
        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_SYLVANAS_SHADOW_COPY_RIDING, 0), GetCaster()->m_Events.CalculateTime(4600ms));
        GetCaster()->m_Events.AddEvent(new SetSheatheStateOrNameplate(GetCaster(), DATA_CHANGE_NAMEPLATE_TO_SYLVANAS, 0), GetCaster()->m_Events.CalculateTime(7s));
        //GetCaster()->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL);
        GetCaster()->CastSpell(GetCaster(), SPELL_BANSHEE_SCREAM_EXPIRE, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 5000));
        GetCaster()->m_Events.AddEvent(new PauseAttackState(GetCaster(), false), GetCaster()->m_Events.CalculateTime(7550ms));
        std::list<Player*> targetList;
        GetPlayerListInGrid(targetList, GetCaster(), 250.0f);

        for (Player* target : targetList)
            GetCaster()->CastSpell(target, SPELL_BANSHEE_SCREAM_MARKER, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 4650));
    }

    void HandleAfterCast()
    {
        std::list<Player*> targetList;
        GetPlayerListInGrid(targetList, GetCaster(), 200.0f);
        //GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_TRANSFORM_INTO_BANSHEE, 0, 0);

        //GetCaster()->m_Events.AddEvent(new SetDisplayIdBanshee(GetCaster(), DATA_DISPLAY_ID_SYLVANAS_BANSHEE_MODEL), GetCaster()->m_Events.CalculateTime(4650ms));
        //GetCaster()->m_Events.AddEvent(new SetDisplayIdBanshee(GetCaster(), DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL), GetCaster()->m_Events.CalculateTime(5250ms));

        for (Player* target : targetList)
            GetCaster()->CastSpell(target, GetEffectInfo(EFFECT_0).TriggerSpell, true);






    }



    void Register() override
    {
        AfterCast += SpellCastFn(spell_sylvanas_windrunner_banshee_scream::HandleAfterCast);
    }
};

// Banshee Scream (Visual) - 357720
class spell_sylvanas_windrunner_banshee_scream_marker : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_banshee_scream_marker);

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_SCREAM_EXPIRE, 0, 0);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_banshee_scream_marker::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Banshee Scream (Not-Triggered Missile) - 353958
class spell_sylvanas_windrunner_banshee_scream_triggered_missile : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_scream_triggered_missile);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo
        ({
            spellInfo->GetEffect(EFFECT_0).TriggerSpell,
            spellInfo->GetEffect(EFFECT_1).TriggerSpell
            });
    }

    void HandleDummyEffect(SpellEffIndex effIndex)
    {
        GetCaster()->CastSpell(GetHitUnit(), GetEffectInfo(effIndex).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_scream_triggered_missile::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_scream_triggered_missile::HandleDummyEffect, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// Banshee Scream (Interrupt Trigger) - 353956
class spell_sylvanas_windrunner_banshee_scream_interrupt : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_scream_interrupt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_BANSHEE_SCREAM_SILENCE });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {

        if (GetHitUnit()->HasUnitState(UNIT_STATE_CASTING))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_BANSHEE_SCREAM_SILENCE, false);

    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_scream_interrupt::HandleHit, EFFECT_0, SPELL_EFFECT_INTERRUPT_CAST);
    }
};

// Banshee Scream (%HP Scaling) - 353955
class spell_sylvanas_windrunner_banshee_scream_damage : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_scream_damage);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {




        uint32 mod = 200;
        uint32 pctmod = 100 - (uint32)floor(GetHitUnit()->GetHealthPct());
        SetHitDamage(GetHitDamage() + mod * pctmod);



    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_scream_damage::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);

    }
};

class HauntingWaveEvent : public BasicEvent
{
public:
    HauntingWaveEvent(Unit* actor, Position const hauntingWaveDest, uint32 hauntingWaveAreaTriggerSpell) : _actor(actor),
        _hauntingWaveDest(hauntingWaveDest), _hauntingWaveAreaTriggerSpell(hauntingWaveAreaTriggerSpell) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        _actor->CastSpell(_hauntingWaveDest, _hauntingWaveAreaTriggerSpell, true);

        return true;
    }

private:
    Unit* _actor;
    Position _hauntingWaveDest;
    uint32 _hauntingWaveAreaTriggerSpell;
};

// Haunting Wave - 352271
class spell_sylvanas_windrunner_haunting_wave : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_haunting_wave);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        for (uint8 itr = 0; itr < 2; itr++)
        {
            for (uint8 i = 0; i < _waveCount; ++i)
            {
                float angle;

                if (itr == 1)
                    angle = _angleOffset * i;
                else
                    angle = 3.0f + _angleOffset * i;

                Position dest = { GetCaster()->GetPositionX(), GetCaster()->GetPositionY(), GetCaster()->GetPositionZ(), angle };

                if (itr == 1)
                    GetCaster()->m_Events.AddEvent(new HauntingWaveEvent(GetCaster(), dest, GetEffectInfo(EFFECT_0).TriggerSpell), GetCaster()->m_Events.CalculateTime(26ms * i));
                else
                    GetCaster()->m_Events.AddEvent(new HauntingWaveEvent(GetCaster(), dest, GetEffectInfo(EFFECT_0).TriggerSpell), GetCaster()->m_Events.CalculateTime(658ms + 26ms * i));
            }
        }
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_sylvanas_windrunner_haunting_wave::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }

private:
    uint8 _waveCount = 12;
    float _angleOffset = float(M_PI * 2) / _waveCount;
};



// Ruin - 355540
class spell_sylvanas_windrunner_ruin : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_ruin);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        if (!GetCaster())
            return;

        std::list<Player*> playerList;
        GetPlayerListInGrid(playerList, GetCaster(), 500.0f);

        if (playerList.size() > 2)
            Trinity::Containers::RandomResize(playerList, 2);

        for (Player* target : playerList)
        {
            GetCaster()->CastSpell(*target, SPELL_RUIN_VISUAL, true);
            GetCaster()->GetInstanceScript()->DoCastSpellOnPlayers(SPELL_RUIN_DAMAGE);
            GetCaster()->CastSpell(*target, aurEff->GetSpellEffectInfo().TriggerSpell, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sylvanas_windrunner_ruin::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

class BaneArrowEvent : public BasicEvent
{
public:
    BaneArrowEvent(Unit* actor, Position const arrowDestPos) : _actor(actor), _arrowDestPos(arrowDestPos) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        _actor->CastSpell(_arrowDestPos, SPELL_BANE_ARROWS_DAMAGE, true);
        _actor->CastSpell(_arrowDestPos, SPELL_BANSHEES_BANE_AREATRIGGER, true);

        return true;
    }

private:
    Unit* _actor;
    Position _arrowDestPos;
};

// Bane Arrows - 354011
class spell_sylvanas_windrunner_bane_arrows : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_bane_arrows);

    void OnPrecast() override
    {
        GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_BANE_ARROWS, 0, 0);
    }

    void HandleAfterCast()
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    for (uint8 covenantPlatform = 0; covenantPlatform < 4; covenantPlatform++)
                    {
                        if (!ai->IsPlatformDesecrated(covenantPlatform))
                        {
                            for (uint8 i = 0; i < 7; i++)
                            {
                                Position const baneArrowPos = ai->GetRandomPointInNonDesecratedPlatform(covenantPlatform);

                                uint32 randomSpeed = urand(2040, 3200);

                                sylvanas->SendPlaySpellVisual(baneArrowPos, 0.0f, SPELL_VISUAL_BANE_ARROW, 0, 0, float(randomSpeed / 1000), true);

                                sylvanas->m_Events.AddEvent(new BaneArrowEvent(sylvanas, baneArrowPos), sylvanas->m_Events.CalculateTime(Milliseconds(randomSpeed)));
                            }
                        }
                    }
                }
            }
        }
        GetCaster()->m_Events.AddEvent(new PauseAttackState(GetCaster(), false), GetCaster()->m_Events.CalculateTime(1ms));
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sylvanas_windrunner_bane_arrows::HandleAfterCast);
    }
};

class BansheeBaneEvent : public BasicEvent
{
public:
    BansheeBaneEvent(Unit* actor, Position const bansheeBanePos) : _actor(actor), _bansheeBanePos(bansheeBanePos) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        _actor->CastSpell(_bansheeBanePos, SPELL_BANSHEES_BANE_AREATRIGGER, true);

        return true;
    }

private:
    Unit* _actor;
    Position _bansheeBanePos;
};

// Banshee's Bane - 353929
class spell_sylvanas_windrunner_banshee_bane : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_banshee_bane);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        uint8 stackAmount = GetStackAmount();

        float angleOffset = float(M_PI * 2) / stackAmount;

        for (uint8 i = 0; i < stackAmount; ++i)
        {
            Position bansheeBaneDest = GetTarget()->GetNearPosition(3.6f, angleOffset * i);

            if (InstanceScript* instance = GetTarget()->GetInstanceScript())
            {
                if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                {
                    if (sylvanas->IsInCombat())
                    {
                        GetTarget()->SendPlaySpellVisual(bansheeBaneDest, 0.0f, SPELL_VISUAL_BANSHEES_BANE_DROP, 0, 0, 0.349999994039535522f, true);

                        sylvanas->m_Events.AddEvent(new BansheeBaneEvent(sylvanas, bansheeBaneDest), sylvanas->m_Events.CalculateTime(500ms));
                    }
                }
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_banshee_bane::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

class BansheesFuryEvent : public BasicEvent
{
public:
    BansheesFuryEvent(Unit* actor, uint8 covenantPlatform) : _actor(actor), _covenantPlatform(covenantPlatform) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        if (InstanceScript* instance = _actor->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    if (sylvanas->HasAura(SPELL_BANSHEES_FURY))
                        sylvanas->SendPlaySpellVisual(ai->GetRandomPointInNonDesecratedPlatform(_covenantPlatform), 0.0f, SPELL_VISUAL_BANSHEE_FURY, 0, 0, 0.100000001490116119f, true);
                }
            }
        }

        return true;
    }

private:
    Unit* _actor;
    uint8 _covenantPlatform;
};

// Banshee's Fury - 354068
class spell_sylvanas_windrunner_banshee_fury : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_banshee_fury);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;

        GetCaster()->SetNameplateAttachToGUID(ObjectGuid::Empty);
        GetCaster()->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
        GetCaster()->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_BANSHEE_FURY_END, 0, 0);

        // TODO: check on every player if Banshee's Bane is on; if yes, trigger explode on them.

        std::list<Player*> targetList;
        GetPlayerListInGrid(targetList, GetCaster(), 500.0f);
            for (Player* target: targetList)
            if (target->HasAura(SPELL_BANSHEES_BANE))
            { 
                GetCaster()->CastSpell(target, SPELL_BANSHEES_FURY_EXPLODE, true);
            }
        
    }    

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!GetCaster())
            return;

        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    for (uint8 covenantPlatform = 0; covenantPlatform < 4; covenantPlatform++)
                    {
                        if (!ai->IsPlatformDesecrated(covenantPlatform))
                        {
                            for (uint8 i = 0; i < 10; i++)
                                GetCaster()->m_Events.AddEvent(new BansheesFuryEvent(GetCaster(), covenantPlatform), GetCaster()->m_Events.CalculateTime(50ms * i));
                        }
                    }
                }
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sylvanas_windrunner_banshee_fury::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_banshee_fury::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Banshee Fury Explode - 357526
class spell_sylvanas_windrunner_banshee_fury_explode : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_banshee_fury_explode);


    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;

        std::list<Player*> targetList;
        GetPlayerListInGrid(targetList, GetCaster(), 500.0f);
        for (Player* target : targetList)
            if (target->HasAura(SPELL_BANSHEES_BANE))
            {
                Aura* bane = target->GetAura(SPELL_BANSHEES_BANE);
                uint8 baneStacks = bane->GetStackAmount();
                SetHitDamage(14200 * (uint8)baneStacks);
            }
    }


    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_banshee_fury_explode::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }


};
class RazeEvent : public BasicEvent
{
public:
    RazeEvent(Unit* actor) : _actor(actor) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        if (InstanceScript* instance = _actor->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                    sylvanas->SendPlaySpellVisual(ai->GetRandomPointInCurrentPlatform(), 0.0f, SPELL_VISUAL_RAZE, 0, 0, 0.100000001490116119f, true);
            }
        }

        return true;
    }

private:
    Unit* _actor;
};

// Raze - 354147
class spell_sylvanas_windrunner_raze : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_raze);


    void OnPrecast() override
    {

        for (uint8 i = 0; i < 100; i++)
            GetCaster()->m_Events.AddEvent(new RazeEvent(GetCaster()), GetCaster()->m_Events.CalculateTime(50ms * i));
    }




    void HandleAfterCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_WINDRUNNER_DISAPPEAR_02, true);

    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sylvanas_windrunner_raze::HandleAfterCast);
    }
};

enum BolvarSpells
{
    SPELL_RUNIC_MARK = 354926,
    SPELL_RUNIC_MARK_TRIGGERED = 354927,
    SPELL_GLYPH_OF_DESINTEGRATION = 354932,
    SPELL_CHARGE_TOWARDS_SYLVANAS = 357046,
    SPELL_WINDS_OF_ICECROWN = 356941,
    SPELL_WINDS_OF_ICECROWN_INTERRUPT = 356986
};

enum BolvarEvents
{
    EVENT_RUNIC_MARK = 1,
    EVENT_GLYPH_OF_DESINTEGRATION
};

enum BolvarActions
{
    ACTION_WINDS_OF_ICECROWN = 1
};

enum BolvarTexts
{
    SAY_FIRST_CHAIN = 0,
    SAY_SECOND_CHAIN = 1,
    SAY_FIRST_RUIN = 2,
    SAY_WINDS_OF_ICECROWN_01 = 3,
    SAY_THIRD_CHAIN_START = 4,
    SAY_THIRD_CHAIN_FINISH = 5,
    SAY_WINDS_OF_ICECROWN_02 = 9,
    SAY_THIS_TIME_YOU_WILL_LOSE = 10,
    SAY_PREPARE_PHASE_THREE = 11
};

enum BolvarMisc
{
    POINT_JUMP_SECOND_CHAIN = 2,
    POINT_GOLIATH_SOULJUDGE = 3,
    POINT_GOLIATH_SUMMONER = 4,
    POINT_SOULJUDGE_SUMMONER = 5,
    POINT_LAST_EARTH_BRIDGE = 6,

};


Position const BolvarPrePhaseTwoPos = { 204.64757f, -842.77606f, 4999.9956f,  0.7806f };
Position const GoliathSoulJudge = { 47.2036f, -1014.1976f, 4999.9858f, 5.0281f };
Position const GoliathSummoner = { -82.8170f, -1128.7692f, 4999.98f, 2.7151f };
Position const BolvarRuin3 = { -160.4722f, -1088.9347f, 4999.983f, 5.7167f };
Position const SouljudgeSummoner = { -226.7686f, -1160.7883f, 4999.9858f, 5.373671f };
Position const LastEarthBridge = { -218.13716f, -1162.092f, 5000.01f, 3.9772f };


// HACKFIX: Vmaps on gameobjects is NYI, I'm adding custom points for jumping from one chain to the other.
Position const BolvarPhaseTwoPos[10] =
{
    { 204.6475f, -842.7760f, 4999.995f, 0.7806f },
    { 205.5017f, -839.9323f, 5000.162f, 0.9862f },
    { 195.3038f, -843.8055f, 5000.038f, 3.0062f },
    { 112.9909f, -821.6018f, 4999.974f, 3.9780f }, // HACKFIX
    { 48.6562f,  -877.9184f, 5000.093f, 3.9858f }
};

Position const BolvarPrePhaseThreePos = { -249.54861f, -1278.5382f, 5667.1157f, 1.5865f };

// Highlord Bolvar Fordragon - 178081
struct npc_sylvanas_windrunner_bolvar : public ScriptedAI
{
    npc_sylvanas_windrunner_bolvar(Creature* creature) : ScriptedAI(creature, DATA_BOLVAR_FORDRAGON_PINNACLE),
        _instance(creature->GetInstanceScript()), _windsOfIcecrown(0) { }

    void JustAppeared() override
    {
        _scheduler.ClearValidator();
    }

    void Reset() override
    {
        _events.Reset();

        _windsOfIcecrown = 0;
        //me->GetMap()->SetZoneMusic(AREA_SANCTUM_OF_DOMINATION, SOUND_BOLVAR_MUSIC);
        //me->GetMap()->SetZoneMusic(AREA_SANCTUM_OF_DOMINATION, MUSIC_SYLVANAS_FREE_WILL);
    }

    void MovementInform(uint32 /*type*/, uint32 id) override
    {
        switch (id)
        {
        case POINT_JUMP_SECOND_CHAIN:
            me->GetMotionMaster()->MovePoint(0, BolvarPhaseTwoPos[4], false);
            break;

        case POINT_GOLIATH_SOULJUDGE:
            me->GetMotionMaster()->MovePoint(0, BolvarPhaseTwoPos[4], false);
            break;

        default:
            break;
        }

    }

    void SpellHitTarget(WorldObject* /*target*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_WINDS_OF_ICECROWN)
        {
            if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (_windsOfIcecrown == 1)
                {
                    sylvanas->RemoveAura(SPELL_BANSHEE_READY_STANCE);
                    sylvanas->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_WINDS_01, 0, 0);


                    me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));


                    _scheduler.Schedule(47ms, [this, sylvanas](TaskContext /*task*/)
                        {
                            sylvanas->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_RUIN_INTERRUPTED, 0, 0);

                        });

                    _scheduler.Schedule(1s, [this, sylvanas](TaskContext /*task*/)
                        {
                            if (sylvanas->IsAIEnabled())
                                sylvanas->m_Events.AddEvent(new PauseAttackState(sylvanas, false), sylvanas->m_Events.CalculateTime(1ms));
                            //sylvanas->AI()->DoAction(ACTION_START_PHASE_);


                        });

                    _scheduler.Schedule(1s + 454ms, [this, sylvanas](TaskContext /*task*/)
                        {
                            if (sylvanas->IsAIEnabled())
                                sylvanas->AI()->Talk(SAY_WINDS_OF_ICECROWN_AFTER_01);
                            sylvanas->AI()->DoAction(ACTION_START_PHASE_TWO_ONE);
                            sylvanas->AI()->DoAction(ACTION_PREPARE_JAINA);


                        });

                    _scheduler.Schedule(12s + 454ms, [this, sylvanas](TaskContext /*task*/)
                        {
                            Talk(SAY_THIRD_CHAIN_START);
                        });

                }
                else if (_windsOfIcecrown == 2)
                {
                    sylvanas->RemoveAura(SPELL_BANSHEE_READY_STANCE);
                    sylvanas->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_WINDS_02, 0, 0);

                    me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));

                    _scheduler.Schedule(47ms, [this, sylvanas](TaskContext /*task*/)
                        {
                            sylvanas->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_SYLVANAS_RUIN_INTERRUPTED, 0, 0);
                        });

                    _scheduler.Schedule(1s, [this, sylvanas](TaskContext /*task*/)
                        {
                            if (sylvanas->IsAIEnabled())
                                sylvanas->m_Events.AddEvent(new PauseAttackState(sylvanas, false), sylvanas->m_Events.CalculateTime(1ms));
                        });

                    _scheduler.Schedule(1s + 454ms, [this, sylvanas](TaskContext /*task*/)
                        {
                            if (sylvanas->IsAIEnabled())
                                sylvanas->AI()->Talk(SAY_WINDS_OF_ICECROWN_AFTER_02);
                            sylvanas->AI()->DoAction(ACTION_START_PHASE_TWO_TWO);
                            sylvanas->AI()->DoAction(ACTION_PREPARE_JAINA_02);

                        });
                }
            }
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        // HACKFIX: sparring system is not implemented yet, this is a workaround
        if (me->HealthBelowPctDamaged(85.0f, damage))
            damage = 0;
    }

    void DamageDealt(Unit* /*victim*/, uint32& damage, DamageEffectType /*damageType*/) override
    {
        // HACKFIX: sparring system is not implemented yet, this is a workaround
        damage = 0;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _events.ScheduleEvent(EVENT_RUNIC_MARK, 1s, 1);
        _events.ScheduleEvent(EVENT_GLYPH_OF_DESINTEGRATION, 5s, 1);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
        case ACTION_WINDS_OF_ICECROWN_PRE:
        {
            // HACKFIX: GameObject pathing NYI
            me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);

            if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if
                    (_windsOfIcecrown == 0)
                {
                    Talk(SAY_WINDS_OF_ICECROWN_01);
                    sylvanas->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(5750ms));
                    sylvanas->AI()->DoAction(ACTION_WINDS_OF_ICECROWN_PRE);
                    me->CastSpell(sylvanas, SPELL_CHARGE_TOWARDS_SYLVANAS, false);
                    _windsOfIcecrown++;
                }


                else if (_windsOfIcecrown == 1)
                {
                    _windsOfIcecrown++;
                    Talk(SAY_WINDS_OF_ICECROWN_02);
                    sylvanas->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(5750ms));
                    sylvanas->AI()->DoAction(ACTION_WINDS_OF_ICECROWN_PRE);
                    me->CastSpell(sylvanas, SPELL_CHARGE_TOWARDS_SYLVANAS, false);

                }

            }
            break;
        }


        case ACTION_WINDS_OF_ICECROWN:
        {
            _scheduler.Schedule(500ms, [this](TaskContext /*task*/)
                {
                    if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                        me->CastSpell(sylvanas, SPELL_WINDS_OF_ICECROWN, false);
                });

            // HACKFIX: GameObject pathing NYI
            _scheduler.Schedule(1s, [this](TaskContext /*task*/)
                {
                    me->ClearUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                });
            break;
        }

        default:
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_RUNIC_MARK:
            {
                if (!me->IsWithinMeleeRange(me->GetVictim()))
                    DoCastVictim(SPELL_RUNIC_MARK, false);
                else
                    DoCastVictim(SPELL_RUNIC_MARK_TRIGGERED, false);

                _events.Repeat(6s, 8s);
                break;
            }

            case EVENT_GLYPH_OF_DESINTEGRATION:
            {
                DoCastVictim(SPELL_GLYPH_OF_DESINTEGRATION, false);
                _events.Repeat(10s, 12s);
                break;
            }

            default:
                break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    InstanceScript* _instance;
    EventMap _events;
    TaskScheduler _scheduler;
    uint8 _windsOfIcecrown;
};

// Runic Mark (Triggered) - 354928
class spell_sylvanas_windrunner_runic_mark_triggered : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_runic_mark_triggered);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), GetEffectInfo(EFFECT_0).TriggerSpell, false);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_sylvanas_windrunner_runic_mark_triggered::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Charge Towards Sylvanas - 357046
class spell_sylvanas_windrunner_charge_towards_sylvanas : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_charge_towards_sylvanas);

    void HandleCharge(SpellEffIndex /*effIndex*/)
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* bolvar = instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
            {
                if (bolvar->IsAIEnabled())
                    bolvar->AI()->DoAction(ACTION_WINDS_OF_ICECROWN);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_charge_towards_sylvanas::HandleCharge, EFFECT_0, SPELL_EFFECT_CHARGE);
    }
};

// Winds of Icecrown - 356941
class spell_sylvanas_windrunner_winds_of_icecrown : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_winds_of_icecrown);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WINDS_OF_ICECROWN_INTERRUPT });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_WINDS_OF_ICECROWN_INTERRUPT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_winds_of_icecrown::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum ThrallSpells
{
    SPELL_FLAMEAXE = 354898,
    SPELL_PULVERIZE = 354918,
    SPELL_STONECRASH = 357139,
    SPELL_EARTHEN_GRASP = 354900,

    SPELL_JUMP_PHASE_TWO_MASTER = 351862,
    SPELL_CALL_EARTH = 352842,
    SPELL_CALL_EARTH_AREATRIGGER_01 = 354576,
    SPELL_CALL_EARTH_AREATRIGGER_02 = 354575,
    SPELL_CALL_EARTH_AREATRIGGER_03 = 354577,
    SPELL_CALL_EARTH_BRIDGE_01 = 351840,
    SPELL_CALL_EARTH_BRIDGE_02 = 348093,
    SPELL_CALL_EARTH_BRIDGE_03 = 351841
};

enum ThrallEvents
{
    EVENT_FLAMEAXE = 1,
    EVENT_PULVERIZE,
    EVENT_STONECRASH_PHASE_ONE_AND_THREE,
    EVENT_EARTHEN_GRASP
};

enum ThrallActions
{
    ACTION_PREPARE_EARTH_BRIDGE_1 = 1,
    ACTION_PREPARE_EARTH_BRIDGE_2 = 2,
    ACTION_PREPARE_EARTH_BRIDGE_3 = 3,
};

enum ThrallMovePoints
{
    POINT_RUIN_1 = 1,
    POINT_SCOUT = 2,
    POINT_SCOUT_1 = 3,
    POINT_THRALL_RUIN_1 = 4,
};

enum ThrallTexts
{
    SAY_CROSSING_FIRST_BRIDGE = 0,
    SAY_FORMING_SECOND_BRIDGE = 1,
    SAY_YIELD_SYLVANAS = 2,
    SAY_FORMING_THIRD_BRIDGE = 3,
    SAY_FOR_SAURFANG = 9,
};

Position const ThrallPrePhaseTwoPos = { 209.80556f, -823.7917f, 4999.985f, 4.52320f };
Position const ThrallScout = { -91.0363f, -1018.9352f, 4999.9819f, 3.8586f };
Position const ThrallRuin = { -153.6781, -1078.3079f, 4999.9834f, 3.9882f };
Position const AfterRuin = { -218.13716f, -1162.092f, 5000.01f, 3.9772f };

// HACKFIX: pathfinding on gameobject is NYI, I'm adding custom points for jumping from one chain to the other
Position const ThrallPhaseTwoPos[10] =
{
    { 207.5f,    -831.4184f, 5000.203f, -1.399f }, //0
    { 198.9548f, -845.8125f, 5000.038f, -1.399f },
    { 120.4910f, -820.9802f, 4999.974f, 3.9544f }, // HACKFIX
    { 55.6406f,  -878.5989f, 5000.149f, 3.9858f }, // Thrall to move here
    { 45.70656f, -885.2656f, 5000.005f, 3.7368f }, // Thrall bridge 1 casting position
    { 66.66008f, -976.307f, 5000.1074f, 4.9667f }, // Jaina to move here
    { -27.79167f, -982.194f, 4999.974f, 4.3873f }, // Thrall Second Earth bridge casting position
    { -218.13716f, -1162.092f, 5000.01f, 3.9772f } // third bridge
};



Position const ThrallCallEarthTargetPos[3] =
{
    { 13.3576f, -913.349f, 4999.5f, 5.4977f },
    { 9.36632f, -995.7847f, 4999.5f, 1.2217f },
    { -207.16f, -1198.63f, 4999.5f, 0.3491f }
};

Position const ThrallFirstRuin = { -19.9390f, -954.4670f, 4999.9839f, 1.2920f };

Position const ThrallPrePhaseThreePos = { -242.277f, -1282.972f, 5667.1157f, 1.53310f };

// Thrall - 176532
struct npc_sylvanas_windrunner_thrall : public ScriptedAI
{
    npc_sylvanas_windrunner_thrall(Creature* creature) : ScriptedAI(creature, DATA_THRALL_PINNACLE),
        _instance(creature->GetInstanceScript()) { }

    void JustAppeared() override
    {
        _scheduler.ClearValidator();
    }

    void Reset() override
    {
        _events.Reset();
    }

    void PrepareThrall()
    {
        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
        {
            if (thrall->IsAIEnabled())
                thrall->AI()->DoAction(ACTION_PREPARE_EARTH_BRIDGE_3);


        }
    }

    void MovementInform(uint32 /*type*/, uint32 id) override
    {


        switch (id)
        {
        case POINT_JUMP_SECOND_CHAIN:
            me->GetMotionMaster()->MovePoint(0, ThrallPhaseTwoPos[3], false);
            break;


        default:
            break;
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        // HACKFIX: sparring system is not implemented yet, this is a workaround
        if (me->HealthBelowPctDamaged(85.0f, damage))
            damage = 0;
    }

    void DamageDealt(Unit* /*victim*/, uint32& damage, DamageEffectType /*damageType*/) override
    {
        // HACKFIX: sparring system is not implemented yet, this is a workaround
        damage = 0;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _events.ScheduleEvent(EVENT_FLAMEAXE, 1s + 300ms, 1);
        _events.ScheduleEvent(EVENT_PULVERIZE, 7s, 1);
        _events.ScheduleEvent(EVENT_STONECRASH_PHASE_ONE_AND_THREE, 8s + 400ms, 1, PHASE_ONE);
        _events.ScheduleEvent(EVENT_EARTHEN_GRASP, 9s + 800ms, 1);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
        case ACTION_PREPARE_EARTH_BRIDGE_1:
        {
            FormEarthBridge(ThrallPhaseTwoPos[4], ThrallCallEarthTargetPos[0]);

            _scheduler.Schedule(4s + 281ms, [this](TaskContext /*task*/)
                {
                    Talk(SAY_FORMING_SECOND_BRIDGE);
                });

            _scheduler.Schedule(8s + 47ms, [this](TaskContext /*task*/)
                {
                    if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                    {
                        if (bolvar->IsAIEnabled())
                            bolvar->AI()->Talk(SAY_FIRST_RUIN);
                    }
                });
            break;
        }

        case ACTION_PREPARE_EARTH_BRIDGE_2:
        {
            FormEarthBridge(ThrallPhaseTwoPos[6], ThrallCallEarthTargetPos[1]);
            _scheduler.Schedule(4s + 281ms, [this](TaskContext /*task*/)
                {
                    Talk(SAY_FORMING_THIRD_BRIDGE);
                });
            break;
        }

        case ACTION_PREPARE_EARTH_BRIDGE_3:
        {
            FormEarthBridge(ThrallPhaseTwoPos[7], ThrallCallEarthTargetPos[2]);



            break;
        }




        default:
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_FLAMEAXE:
            {
                DoCastSelf(SPELL_FLAMEAXE, false);
                _events.Repeat(18s, 20s);
                break;
            }

            case EVENT_PULVERIZE:
            {
                DoCastVictim(SPELL_PULVERIZE, false);
                _events.Repeat(9s + 750ms, 15s + 800ms);
                break;
            }

            case EVENT_STONECRASH_PHASE_ONE_AND_THREE:
            {
                DoCastVictim(SPELL_STONECRASH, false);
                _events.Repeat(12s, 18s);
                break;
            }

            case EVENT_EARTHEN_GRASP:
            {
                DoCastVictim(SPELL_EARTHEN_GRASP, false);
                _events.Repeat(8s, 10s);
                break;
            }
            default:
                break;
            }
        }

        DoMeleeAttackIfReady();
    }

    void FormEarthBridge(Position jumpPos, Position bridgePos)
    {
        me->CastSpell(jumpPos, SPELL_JUMP_PHASE_TWO_MASTER, false);

        uint32 bridgeAreaTriggerSpell;
        uint32 bridgeGameObjectSpell;

        switch (uint8 randomBridge = urand(DATA_BRIDGE_PHASE_TWO_1, DATA_BRIDGE_PHASE_TWO_3))
        {
        case DATA_BRIDGE_PHASE_TWO_1:
            bridgeAreaTriggerSpell = SPELL_CALL_EARTH_AREATRIGGER_01;
            bridgeGameObjectSpell = SPELL_CALL_EARTH_BRIDGE_01;
            break;
        case DATA_BRIDGE_PHASE_TWO_2:
            bridgeAreaTriggerSpell = SPELL_CALL_EARTH_AREATRIGGER_02;
            bridgeGameObjectSpell = SPELL_CALL_EARTH_BRIDGE_02;
            break;
        case DATA_BRIDGE_PHASE_TWO_3:
            bridgeAreaTriggerSpell = SPELL_CALL_EARTH_AREATRIGGER_03;
            bridgeGameObjectSpell = SPELL_CALL_EARTH_BRIDGE_03;
            break;
        default:
            break;
        }

        _scheduler.Schedule(1s + 266ms, [this](TaskContext /*task*/)
            {
                DoCastSelf(SPELL_CALL_EARTH, false);
            });

        _scheduler.Schedule(2s + 250ms, [this, bridgePos, bridgeAreaTriggerSpell](TaskContext /*task*/)
            {
                me->CastSpell(bridgePos, bridgeAreaTriggerSpell, true);
            });

        _scheduler.Schedule(6s + 266ms, [this, bridgePos, bridgeGameObjectSpell](TaskContext /*task*/)
            {
                me->CastSpell(bridgePos, bridgeGameObjectSpell, true);
            });
    }

private:
    InstanceScript* _instance;
    EventMap _events;
    TaskScheduler _scheduler;
};

// Pulverize - 354918
class spell_sylvanas_windrunner_pulverize : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_pulverize);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), GetEffectInfo(EFFECT_0).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sylvanas_windrunner_pulverize::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Pulverize (triggered) - 354923
class spell_sylvanas_windrunner_pulverize_triggered : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_pulverize_triggered);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitDest()->GetPosition(), GetEffectInfo(EFFECT_0).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_sylvanas_windrunner_pulverize_triggered::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Stonecrash (Phase 1 and 3) - 357139
class spell_sylvanas_windrunner_stonecrash_phase_one_and_three : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_stonecrash_phase_one_and_three);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitDest()->GetPosition(), GetEffectInfo(EFFECT_0).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_sylvanas_windrunner_stonecrash_phase_one_and_three::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Stonecrash (Phase 2) - 357137
class spell_sylvanas_windrunner_stonecrash_phase_two : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_stonecrash_phase_two);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_1).TriggerSpell });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitDest()->GetPosition(), GetEffectInfo(EFFECT_1).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_sylvanas_windrunner_stonecrash_phase_two::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

enum JainaSpells
{
    SPELL_ICE_BOLT = 350002,
    SPELL_COMET_BARRAGE = 354938,
    SPELL_FRIGID_SHARDS = 354933,
    SPELL_CONE_OF_COLD = 350003,
    SPELL_SEARING_BLAST = 355507,

    SPELL_TELEPORT_TO_PHASE_2 = 355073,
    SPELL_TELEPORT_PHASE_TWO_MASTER = 351890,
    SPELL_CHANNEL_ICE = 352843,
    SPELL_CHANNEL_ICE_AREATRIGGER_01 = 354476,
    SPELL_CHANNEL_ICE_AREATRIGGER_02 = 354573,
    SPELL_CHANNEL_ICE_AREATRIGGER_03 = 354574,
    SPELL_CHANNEL_ICE_BRIDGE_01 = 348148,
    SPELL_CHANNEL_ICE_BRIDGE_02 = 351837,
    SPELL_CHANNEL_ICE_BRIDGE_03 = 351838,
    SPELL_PORTAL_TO_ORIBOS_PHASE_3 = 357102,
    SPELL_ARCANE_STASIS_WAVE = 357109,
    SPELL_TELEPORT_TO_PHASE_3 = 350906,
    SPELL_TELEPORT_TO_PHASE_3_DEST = 357103,
    SPELL_FROZEN_SHELL = 351762,
    SPELL_BLASPHEMY_STUN = 357728
};

enum JainaEvents
{
    EVENT_FRIGID_SHARDS = 1,
    EVENT_CONE_OF_COLD,
    EVENT_COMET_BARRAGE
};

enum JainaActions
{
    ACTION_PREPARE_ICE_BRIDGE_1 = 1,
    ACTION_PREPARE_ICE_BRIDGE_2,
    ACTION_PREPARE_ICE_BRIDGE_3,
    ACTION_THRALL_BRIDGE_2,
    ACTION_DESPAWN_ADDS
};

enum JainaTexts
{
    SAY_PREPARE_PHASE_TWO = 0,
    SAY_START_PHASE_TWO = 1,
    SAY_CALL_FOR_AID = 2,
    SAY_OVERRUN = 3,
    SAY_THANKS = 4,
    SAY_ENCOURAGE_PLAYERS = 5,
    SAY_AID_THRALL = 6,
    SAY_FIGHTING_SYLVANAS = 7,
    SAY_ASK_FOR_ANDUIN = 8,
    SAY_PREPARING_PORTAL_TO_PHASE_THREE = 9,
    SAY_FINISHING_PORTAL_TO_PHASE_THREE = 10,
    SAY_FINISHED_PORTAL_TO_PHASE_THREE = 11
};

enum JainaVisuals
{
    SPELL_VISUAL_KIT_JAINA_BREAK_BLASPHEMY = 149324,
    SPELL_VISUAL_KIT_JAINA_TELEPORT_PLAYERS = 149323
};

enum JainaPoints
{
    POINT_TELEPORT_TO_THIRD_CHAIN = 1,
    POINT_TELEPORT_TO_SIXTH_CHAIN = 2
};

Position const JainaPhaseTwoPos[10] =
{
    { 216.5781f, -831.4253f, 4999.991f, 3.7259f },
    { 211.3368f, -836.1528f, 5000.154f, 2.7889f },
    { 194.3541f, -846.6875f, 4999.989f, 2.7889f }, // Channel Ice 1
    { 182.8524f, -884.8073f, 5000.056f, 4.0016f },
    { 127.7951f, -944.7674f, 5000.001f, 3.6245f },
    { 38.60067f, -1026.189f, 5000.003f, 3.9523f }, // Channel Ice 2 She must be positioned
    { -65.3701f, -1113.404f, 4999.985f, 3.8916f },
    { -83.2934f, -1126.055f, 4999.991f, 2.8005f }, // Channel Ice 3 She must be positioned
    { -96.5086f, -1169.178f, 4999.938f, 3.9523f },
    { -153.118f, -1222.013f, 4999.987f, 3.7989f }
};

Position const JainaChannelIceTargetPos[3] =
{
    { 158.634f, -832.84f,  4999.5f, 4.3633f },
    { 15.0278f, -1055.03f, 4999.5f, 2.3561f }, // Cast position of Ice 2
    { -121.38f, -1112.85f, 4999.5f, 4.3633f }
    // .go xyz  -123.51543 -1113.573  4999.528 2.8948211669921875
};

Position const JainaPrePhaseThreePos = { -258.666f, -1284.191f, 5667.1157f, 1.58863f };

Position const PlayerPrePhaseThreePos = { -250.200f, -1292.985f, 5667.1147f, 1.53310f };

// Jaina Proudmoore - 176533
struct npc_sylvanas_windrunner_jaina : public ScriptedAI
{
    npc_sylvanas_windrunner_jaina(Creature* creature) : ScriptedAI(creature, DATA_JAINA_PROUDMOORE_PINNACLE),
        _instance(creature->GetInstanceScript()), goliathAlive(true), souljudgeAlive(true), summonerAlive(true), goliathDeath(0), souljudgeDeath(0), summonerDeath(0), windsCasted(0) { }

    void JustAppeared() override
    {
        _scheduler.ClearValidator();
    }

    void Reset() override
    {
        _events.Reset();
    }

    void ChannelIceIfPossible()
    {


        if (goliathDeath == 1 && souljudgeDeath == 1 && summonerDeath == 0)
        { 
            DoAction(ACTION_PREPARE_ICE_BRIDGE_2);
            if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (sylvanas->IsAIEnabled())
                    sylvanas->AI()->DoAction(ACTION_SECOND_WAVE_VEIL);
            }
        }

        else if (goliathDeath == 2 && summonerDeath == 1 && souljudgeDeath == 1)
        { 
            DoAction(ACTION_PREPARE_ICE_BRIDGE_3);
            if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                //if (sylvanas->IsAIEnabled())
                   // sylvanas->AI()->DoAction(ACTION_SECOND_WAVE_WAIL);
            }
        }
        else if (summonerDeath == 2 && souljudgeDeath == 2 && goliathDeath == 2)
        {
            if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
            {
                if (thrall->IsAIEnabled())
                    thrall->AI()->DoAction(ACTION_PREPARE_EARTH_BRIDGE_3);
            }
        }
    }


    void CastRuin()
    {
        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {
            if (goliathDeath == 1 && souljudgeDeath == 1 && summonerDeath == 0)
                sylvanas->AI()->DoAction(ACTION_RUIN_2);

            else if (goliathDeath == 2 && summonerDeath == 1 && souljudgeDeath == 1)
                sylvanas->AI()->DoAction(ACTION_RUIN_3);

            else if (summonerDeath == 2 && souljudgeDeath == 2 && goliathDeath == 2)
                sylvanas->AI()->DoAction(ACTION_RUIN_4);


        }
    }

    void WindsCasted()
    {
        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {

        }

    }






    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == POINT_MOTION_TYPE)
        {
            if (id == POINT_TELEPORT_TO_THIRD_CHAIN)
                me->CastSpell(JainaPhaseTwoPos[4], SPELL_TELEPORT_PHASE_TWO_MASTER, true);

            if (id == POINT_TELEPORT_TO_SIXTH_CHAIN)
                me->CastSpell(JainaPhaseTwoPos[8], SPELL_TELEPORT_PHASE_TWO_MASTER, true);
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        // HACKFIX: sparring system is not implemented yet, this is a workaround
        if (me->HealthBelowPctDamaged(85.0f, damage))
            damage = 0;
    }

    void DamageDealt(Unit* /*victim*/, uint32& damage, DamageEffectType /*damageType*/) override
    {
        // HACKFIX: sparring system is not implemented yet, this is a workaround
        damage = 0;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _events.ScheduleEvent(EVENT_COMET_BARRAGE, 5s, 1);
        _events.ScheduleEvent(EVENT_FRIGID_SHARDS, 10s, 1);
        _events.ScheduleEvent(EVENT_CONE_OF_COLD, 14s, 1);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
        case ACTION_OPEN_PORTAL_TO_PHASE_TWO:
        {
            me->RemoveAurasDueToSpell(SPELL_CHAMPIONS_MOD_FACTION);
            if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));

                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 250.0f);

                me->NearTeleportTo(JainaPhaseTwoPos[0], false);

                if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                {
                    bolvar->RemoveAurasDueToSpell(SPELL_CHAMPIONS_MOD_FACTION);
                    bolvar->NearTeleportTo(BolvarPrePhaseTwoPos, false);

                    if (bolvar->IsAIEnabled())
                        bolvar->m_Events.AddEvent(new PauseAttackState(me, true), bolvar->m_Events.CalculateTime(1ms));
                }

                if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                {
                    thrall->RemoveAurasDueToSpell(SPELL_CHAMPIONS_MOD_FACTION);
                    thrall->NearTeleportTo(ThrallPrePhaseTwoPos, false);

                    if (thrall->IsAIEnabled())
                        thrall->m_Events.AddEvent(new PauseAttackState(thrall, true), thrall->m_Events.CalculateTime(1ms));
                }

                _scheduler.Schedule(50ms, [this, playerList](TaskContext /*task*/)
                    {
                        for (Player* player : playerList)
                            player->CastSpell(player, SPELL_TELEPORT_TO_PHASE_TWO, true);
                    });

                _scheduler.Schedule(100ms, [this](TaskContext /*task*/)
                    {
                        DoCastSelf(SPELL_ANCHOR_HERE, true);

                        me->HandleEmoteCommand(EMOTE_STATE_READY1H_ALLOW_MOVEMENT);

                        if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                            bolvar->CastSpell(bolvar, SPELL_ANCHOR_HERE, true);

                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                            thrall->CastSpell(thrall, SPELL_ANCHOR_HERE, true);
                    });

                _scheduler.Schedule(150ms, [this, playerList](TaskContext /*task*/)
                    {
                        for (Player* player : playerList)
                            player->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_GENERIC_TELEPORT, 0, 0);

                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_GENERIC_TELEPORT, 0, 0);
                        me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_JAINA_KNEEL_THEN_STAND, 0, 0);

                        if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                            bolvar->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_GENERIC_TELEPORT, 0, 0);

                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        {
                            thrall->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_GENERIC_TELEPORT, 0, 0);
                            thrall->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_THRALL_KNEEL_THEN_STAND, 0, 0);
                        }
                    });

                _scheduler.Schedule(1s, [this, sylvanas](TaskContext /*task*/)
                    {
                        if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                            ai->TeleportShadowcopiesToMe();
                    });

                _scheduler.Schedule(2s + 750ms, [this](TaskContext /*task*/)
                    {
                        Talk(SAY_PREPARE_PHASE_TWO);
                    });

                _scheduler.Schedule(3s + 500ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                        {
                            if (Creature* shadowCopy = ObjectAccessor::GetCreature(*sylvanas, ai->GetShadowCopyJumperGuid(0)))
                                sylvanas->SetNameplateAttachToGUID(ai->GetShadowCopyJumperGuid(0));

                            sylvanas->CastSpell(sylvanas, SPELL_WINDRUNNER_DISAPPEAR_02, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 500));

                            sylvanas->SendPlayOrphanSpellVisual(SylvanasWavePos[0], SPELL_VISUAL_WINDRUNNER_01, 0.5f, true, false);

                            if (Creature* shadowCopy = ObjectAccessor::GetCreature(*sylvanas, ai->GetShadowCopyJumperGuid(0)))
                                shadowCopy->CastSpell(SylvanasWavePos[0], SPELL_WINDRUNNER_MOVE, true);
                        }
                    });

                _scheduler.Schedule(4s, [this, sylvanas](TaskContext /*task*/)
                    {
                        sylvanas->NearTeleportTo(SylvanasWavePos[0], false);

                        sylvanas->SetNameplateAttachToGUID(ObjectGuid::Empty);

                        sylvanas->CastSpell(sylvanas, SPELL_BANSHEE_READY_STANCE, true);

                        if (Creature* jaina = _instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
                            sylvanas->SetFacingToObject(jaina);

                        sylvanas->CastSpell(sylvanas, SPELL_SYLVANAS_ROOT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_DURATION, 25050));
                    });

                _scheduler.Schedule(5s + 578ms, [this](TaskContext /*task*/)
                    {
                        me->GetMotionMaster()->MovePoint(0, JainaPhaseTwoPos[1], false);

                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                            thrall->GetMotionMaster()->MovePoint(0, ThrallPhaseTwoPos[0], false);
                    });

                _scheduler.Schedule(6s + 200ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                            bolvar->AI()->Talk(SAY_FIRST_CHAIN);
                    });

                _scheduler.Schedule(6s + 890ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                            bolvar->GetMotionMaster()->MovePoint(0, BolvarPhaseTwoPos[1], false);
                    });

                _scheduler.Schedule(8s + 93ms, [this](TaskContext /*task*/)
                    {
                        DoAction(ACTION_PREPARE_ICE_BRIDGE_1);
                    });
            }
            break;
        }

        case ACTION_PREPARE_ICE_BRIDGE_1:
        {
            FormFrozenBridge(JainaPhaseTwoPos[2], JainaChannelIceTargetPos[0]);

            _scheduler.Schedule(493ms, [this](TaskContext /*task*/)
                {
                    if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        thrall->GetMotionMaster()->MovePoint(0, ThrallPhaseTwoPos[1], false);
                });

            _scheduler.Schedule(900ms, [this](TaskContext /*task*/)
                {
                    if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        bolvar->GetMotionMaster()->MovePoint(0, BolvarPhaseTwoPos[2], false);
                });

            _scheduler.Schedule(3s + 322ms, [this](TaskContext /*task*/)
                {
                    Talk(SAY_START_PHASE_TWO);
                });

            _scheduler.Schedule(5s + 593ms, [this](TaskContext /*task*/)
                {
                    if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->DoAction(ACTION_HAUNTING_WAVE_SECOND_CHAIN);
                        sylvanas->SetCanFly(true);
                        sylvanas->SetDisableGravity(true);
                    }
                });

            _scheduler.Schedule(6s + 811ms, [this](TaskContext /*task*/)
                {
                    me->GetMotionMaster()->MovePoint(POINT_TELEPORT_TO_THIRD_CHAIN, JainaPhaseTwoPos[3], false);

                    if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        bolvar->GetMotionMaster()->MoveJump(BolvarPhaseTwoPos[3], bolvar->GetSpeed(MOVE_RUN), Movement::gravity * 1.4125f, POINT_JUMP_SECOND_CHAIN);

                    if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        thrall->GetMotionMaster()->MoveJump(ThrallPhaseTwoPos[2], thrall->GetSpeed(MOVE_RUN), Movement::gravity * 1.4125f, POINT_JUMP_SECOND_CHAIN);
                });

            _scheduler.Schedule(10s + 639ms, [this](TaskContext /*task*/)
                {
                    if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                    {
                        if (thrall->IsAIEnabled())
                            thrall->AI()->Talk(SAY_CROSSING_FIRST_BRIDGE);

                        thrall->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_GENERIC_CHEERS, 0, 0);
                    }
                });

            _scheduler.Schedule(20s + 639ms, [this](TaskContext /*task*/)
                {
                    if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                    {
                        if (bolvar->IsAIEnabled())
                            bolvar->AI()->Talk(SAY_SECOND_CHAIN);
                    }
                });

            _scheduler.Schedule(29s + 685ms, [this](TaskContext /*task*/)
                {
                    if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                    {
                        if (thrall->IsAIEnabled())
                            thrall->AI()->DoAction(ACTION_PREPARE_EARTH_BRIDGE_1);
                            thrall->CastSpell(thrall, SPELL_CHAMPIONS_MOD_FACTION, true);
                    }
                });

            break;
        }


        case ACTION_PREPARE_ICE_BRIDGE_2:
        {
            FormFrozenBridge(JainaPhaseTwoPos[5], JainaChannelIceTargetPos[1]);
            Talk(SAY_THANKS);
            _scheduler.Schedule(8s + 685ms, [this](TaskContext /*task*/)
                {
                    if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                    {
                        bolvar->NearTeleportTo(SylvanasRuin3, false);
                    }
                });

            break;
        }

        case ACTION_PREPARE_ICE_BRIDGE_3:
        {
            FormFrozenBridge(JainaPhaseTwoPos[7], JainaChannelIceTargetPos[2]);

            _scheduler.Schedule(8s + 685ms, [this](TaskContext /*task*/)
                {
                    if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                    {
                        bolvar->GetMotionMaster()->MovePoint(POINT_SOULJUDGE_SUMMONER, SouljudgeSummoner);

                    }
                });

            _scheduler.Schedule(8s + 685ms, [this](TaskContext /*task*/)
                {
                    if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                    {
                        bolvar->GetMotionMaster()->MovePoint(POINT_SOULJUDGE_SUMMONER, SouljudgeSummoner);

                    }
                });

            break;
        }



        case ACTION_GOLIATH_DIED:
        {
            goliathDeath++;
            ChannelIceIfPossible();
            CastRuin();
            break;

        }

        case ACTION_SOULJUDGE_DIED:
        {

            souljudgeDeath++;
            ChannelIceIfPossible();
            CastRuin();
            break;

        }



        case ACTION_SUMMONER_DIED:
        {
            summonerDeath++;
            ChannelIceIfPossible();
            CastRuin();
            break;

        }

        case ACTION_JAINA_TWO_ONE:
        {
            if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            { 

                _scheduler.Schedule(1s + 500ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        {
                            
                            thrall->CastSpell(SylvanasPhase2PrePos, SPELL_JUMP_PHASE_TWO_MASTER, false);
                            thrall->CastSpell(thrall, SPELL_CHAMPIONS_MOD_FACTION, true);
                            thrall->AI()->AttackStart(sylvanas);
                            thrall->SetReactState(REACT_AGGRESSIVE);
                            
                        }

                        if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        {
                            bolvar->CastSpell(bolvar, SPELL_CHAMPIONS_MOD_FACTION, true);
                            bolvar->AI()->AttackStart(sylvanas);
                        }
                    });

                _scheduler.Schedule(26s + 500ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        {
                            if (thrall->IsAIEnabled())
                                thrall->AI()->Talk(SAY_YIELD_SYLVANAS);
                        }
                    });

                _scheduler.Schedule(33s + 400ms, [this](TaskContext /*task*/)
                    {
                        Talk(SAY_CALL_FOR_AID);
                    });

                _scheduler.Schedule(45s + 500ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        {
                            if (thrall->IsAIEnabled())
                                thrall->AI()->DoAction(ACTION_PREPARE_EARTH_BRIDGE_2);
                        }
                    });

                _scheduler.Schedule(53s + 400ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        {
                            if (bolvar->IsAIEnabled())
                                bolvar->AI()->Talk(SAY_THIRD_CHAIN_FINISH);
                        }
                    });

                _scheduler.Schedule(56s + 500ms, [this](TaskContext /*task*/)
                    {
                        Talk(SAY_OVERRUN);

                    });

                _scheduler.Schedule(57s + 500ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))

                            if (thrall->IsAIEnabled())
                                thrall->NearTeleportTo(SylvanasRuin3, false);

                    });



                _scheduler.Schedule(59s + 400ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        {
                            if (bolvar->IsAIEnabled())
                                bolvar->NearTeleportTo(GoliathSoulJudge, false);
                                bolvar->m_Events.AddEvent(new PauseAttackState(bolvar, true), bolvar->m_Events.CalculateTime(1ms));
                        }
                    });
            }


            break;

        }

        case ACTION_JAINA_TWO_TWO:
        {
            if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                me->NearTeleportTo(SylvanasRuin4, false);

                

                _scheduler.Schedule(1s + 100ms, [this, sylvanas](TaskContext /*task*/)
                    {
                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        {
                            thrall->CastSpell(SylvanasRuin4, SPELL_JUMP_PHASE_TWO_MASTER, false);
                            thrall->CastSpell(thrall, SPELL_CHAMPIONS_MOD_FACTION, true);
                            thrall->AI()->AttackStart(sylvanas);
                        }
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoCastSelf(SPELL_CHAMPIONS_MOD_FACTION, true);
                        AttackStart(sylvanas);

                    });

                _scheduler.Schedule(6s + 100ms, [this](TaskContext /*task*/)
                    {
                        Talk(SAY_FIGHTING_SYLVANAS);
                    });

                _scheduler.Schedule(16s + 500ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        {
                            if (thrall->IsAIEnabled())
                                thrall->AI()->Talk(SAY_FOR_SAURFANG);
                        }
                    });

                _scheduler.Schedule(26s + 500ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        {
                            if (bolvar->IsAIEnabled())
                                bolvar->AI()->Talk(SAY_THIS_TIME_YOU_WILL_LOSE);
                        }
                    });

                _scheduler.Schedule(37s + 500ms, [this](TaskContext /*task*/)
                    {
                        Talk(SAY_ASK_FOR_ANDUIN);
                    });

                _scheduler.Schedule(47s + 500ms, [this](TaskContext /*task*/)
                    {
                        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                            sylvanas->AI()->DoAction(ACTION_PREPARE_PHASE_THREE);

                    });
            }



            break;

        }

        case ACTION_JAINA_START_ATTACKING:
        {
            _scheduler.Schedule(3s, [this](TaskContext /*task*/)
                {
                    
                    me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));

                    if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                    { 

                        DoCastSelf(SPELL_CHAMPIONS_MOD_FACTION, true);
                        AttackStart(sylvanas);

                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        {
                            thrall->CastSpell(thrall, SPELL_CHAMPIONS_MOD_FACTION, true);
                            AttackStart(thrall);


                            if (thrall->IsAIEnabled())
                                thrall->AI()->AttackStart(sylvanas);
                        }

                        if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        {
                            bolvar->CastSpell(bolvar, SPELL_CHAMPIONS_MOD_FACTION, true);
                            bolvar->AI()->AttackStart(sylvanas);
                        }
                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        {
                            AttackStart(thrall);
                        }
                    }

                });
            break;
        }

        case ACTION_DESPAWN_ADDS:
        {
            for (uint8 i = 0; i < 26; i++)
            {
                if (Creature* goliath = _instance->GetCreature(DATA_MAWFORGED_GOLIATH))
                    goliath->DespawnOrUnsummon(1ms);

                if (Creature* souljudge = _instance->GetCreature(DATA_MAWFORGED_SOULJUDGE))
                    souljudge->DespawnOrUnsummon(1ms);

                if (Creature* summoner = _instance->GetCreature(DATA_MAWFORGED_SUMMONER))
                    summoner->DespawnOrUnsummon(1ms);

                if (Creature* hopebreaker = _instance->GetCreature(DATA_MAWSWORN_HOPEBREAKER))
                    hopebreaker->DespawnOrUnsummon(1ms);

                if (Creature* vanguard = _instance->GetCreature(DATA_MAWSWORN_VANGUARD))
                    vanguard->DespawnOrUnsummon(1ms);

                if (Creature* orbs = _instance->GetCreature(DATA_DECREPIT_ORB))
                    orbs->DespawnOrUnsummon(1ms);
            }


            
            break;
        }

        case ACTION_OPEN_PORTAL_TO_PHASE_THREE:
        {
            me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));

            if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
            {
                if (bolvar->IsAIEnabled())
                    bolvar->m_Events.AddEvent(new PauseAttackState(bolvar, true), bolvar->m_Events.CalculateTime(1ms));
            }

            if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
            {
                if (thrall->IsAIEnabled())
                    thrall->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
            }

            me->setActive(true);

            Talk(SAY_PREPARING_PORTAL_TO_PHASE_THREE);

            std::list<Player*> playerList;
            GetPlayerListInGrid(playerList, me, 250.0f);

            _scheduler.Schedule(500ms, [this, playerList](TaskContext /*task*/)
                {
                    me->NearTeleportTo(-233.971f, -1304.676f, 4999.984f, false);
                    me->GetInstanceScript()->DoCastSpellOnPlayers(SPELL_ABSORB_VEIL_2);
                    me->RemoveAura(SPELL_CHAMPIONS_MOD_FACTION);

                    for (Player* player : playerList)
                        player->CastSpell(player, SPELL_ABSORB_VEIL_2, false);
                });

            _scheduler.Schedule(980ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_ANCHOR_HERE, true);
                    
                });

            _scheduler.Schedule(1s + 800ms, [this](TaskContext /*task*/)
                {
                    Position const frontJainaPos = me->GetNearPosition(5.0f, 0.0f);

                    me->CastSpell(frontJainaPos, SPELL_PORTAL_TO_ORIBOS_PHASE_3, false);
                });

            _scheduler.Schedule(5s, [this](TaskContext /*task*/)
                {
                    Talk(SAY_FINISHING_PORTAL_TO_PHASE_THREE);
                    DoAction(ACTION_DESPAWN_ADDS);
                    if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->DoAction(ACTION_INITIATE_PHASE_THREE);
                    }

                    if (Creature* hopebreaker = _instance->GetCreature(DATA_MAWSWORN_HOPEBREAKER))
                        hopebreaker->DespawnOrUnsummon();

                    if (Creature* vanguard = _instance->GetCreature(DATA_MAWSWORN_VANGUARD))
                        vanguard->DespawnOrUnsummon();
                });

            _scheduler.Schedule(12s, [this](TaskContext /*task*/)
                {
                    Talk(SAY_FINISHED_PORTAL_TO_PHASE_THREE);

                    if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                    { 
                        sylvanas->SetFacingTo(4.8989f);
                        sylvanas->RemoveAura(SPELL_BLASPHEMY_STUN);
                    }
                        
                });

            _scheduler.Schedule(12s + 250ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_GENERIC_TELEPORT, 0, 0);
                    me->NearTeleportTo(JainaPrePhaseThreePos, false);

                    if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                    {
                        bolvar->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_GENERIC_TELEPORT, 0, 0);
                        bolvar->NearTeleportTo(BolvarPrePhaseThreePos, false);
                    }

                    if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                    {
                        thrall->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_GENERIC_TELEPORT, 0, 0);
                        thrall->NearTeleportTo(ThrallPrePhaseThreePos, false);
                    }
                });

            _scheduler.Schedule(12s + 300ms, [this, playerList](TaskContext /*task*/)
                {
                    for (Player* player : playerList)
                        player->CastSpell(me->GetPosition(), SPELL_TELEPORT_TO_PHASE_3, true);
                });

            _scheduler.Schedule(13s, [this, playerList](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_ANCHOR_HERE, true);
                    me->GetInstanceScript()->DoCastSpellOnPlayers(SPELL_BLASPHEMY_STUN);
                    me->GetInstanceScript()->DoCastSpellOnPlayers(SPELL_SERVERSIDE_STUN);
                    
                });

            _scheduler.Schedule(13s + 200ms, [this, playerList](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_CHAMPIONS_MOD_FACTION, true);
                    

                    for (Player* player : playerList)
                        player->CastSpell(player, SPELL_PLATFORMS_SCENE, true);

                    if (Creature* anduin = _instance->GetCreature(DATA_ANDUIN_CRUCIBLE))
                    {
                        if (anduin->IsAIEnabled())
                            anduin->AI()->DoAction(ACTION_INITIATE_PHASE_THREE);
                    }



                });

            _scheduler.Schedule(15s, [this](TaskContext /*task*/)
                {
                    if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                    {
                        if (bolvar->IsAIEnabled())
                            bolvar->AI()->Talk(SAY_PREPARE_PHASE_THREE);
                    }

                    if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                    {
                        sylvanas->RemoveAura(SPELL_BLASPHEMY_STUN);
                    }
                });

            _scheduler.Schedule(18s + 950ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_FROZEN_SHELL, false);
                });

            _scheduler.Schedule(20s, [this](TaskContext /*task*/)
                {
                    me->RemoveAura(SPELL_BLASPHEMY_STUN);
                });

            _scheduler.Schedule(20s + 200ms, [this](TaskContext /*task*/)
                {
                    me->CastStop();
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_JAINA_BREAK_BLASPHEMY, 0, 0);
                });

            _scheduler.Schedule(20s + 950ms, [this](TaskContext /*task*/)
                {
                    me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_JAINA_TELEPORT_PLAYERS, 0, 0);
                });

            _scheduler.Schedule(22s + 122ms, [this](TaskContext /*task*/)
                {
                    Talk(SAY_ENCOURAGE_PLAYERS);
                });

            _scheduler.Schedule(23s + 122ms, [this, playerList](TaskContext /*task*/)  // CHANGE
                {
                    for (Player* player : playerList)
                    {
                        player->CastSpell(CovenantPlatformPos[0][0].GetPositionWithOffset(frand(3.0f, 10.0f)), SPELL_TELEPORT_TO_PHASE_3_DEST, true);
                        player->RemoveAura(SPELL_BLASPHEMY_STUN);
                        player->RemoveAura(SPELL_INTERMISSION_STUN);
                        player->RemoveAura(SPELL_SERVERSIDE_STUN);
                    }

                    if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        bolvar->RemoveAura(SPELL_BLASPHEMY_STUN);

                    if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        thrall->RemoveAura(SPELL_BLASPHEMY_STUN);
                });

            _scheduler.Schedule(24s, [this, playerList](TaskContext /*task*/)
                {
                    me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));
                    SetCombatMovement(false);
                    if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                        sylvanas->SetFacingTo(2.3582f);

                    if (Creature* anduin = _instance->GetCreature(DATA_ANDUIN_CRUCIBLE))
                    {
                        
                        AttackStart(anduin);
                        me->GetThreatManager().AddThreat(anduin, 90000.0f);

                        me->CastSpell(anduin, SPELL_SEARING_BLAST, false);
                        AddThreat(anduin, 500000.0f);
                        
                        if (Creature* thrall = _instance->GetCreature(DATA_THRALL_PINNACLE))
                        { 
                            thrall->m_Events.AddEvent(new PauseAttackState(thrall, false), thrall->m_Events.CalculateTime(1ms));
                            //thrall->CastSpell(thrall, SPELL_CHAMPIONS_MOD_FACTION, true);
                            thrall->AI()->AttackStart(anduin);
                            thrall->GetThreatManager().AddThreat(anduin, 500000.0f);
                        }
                        if (Creature* bolvar = _instance->GetCreature(DATA_BOLVAR_FORDRAGON_PINNACLE))
                        { 
                            bolvar->m_Events.AddEvent(new PauseAttackState(bolvar, false), bolvar->m_Events.CalculateTime(1ms));
                            //bolvar->CastSpell(bolvar, SPELL_CHAMPIONS_MOD_FACTION, true);
                            bolvar->AI()->AttackStart(anduin);
                            bolvar->GetThreatManager().AddThreat(anduin, 500000.0f);
                            
                                
                        }

                        if (anduin->IsAIEnabled())
                        { 
                            anduin->AI()->AttackStart(me);
                            anduin->GetThreatManager().AddThreat(me, 90000.0f);
                        }
                    }


                });

            _scheduler.Schedule(25s + 522ms, [this, playerList](TaskContext /*task*/)
                {
                    
                    if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->DoAction(ACTION_START_PHASE_THREE);

                        for (uint8 i = 0; i < 8; i++)
                            sylvanas->CastSpell(InvigoratingFieldPos[i], SPELL_INVIGORATING_FIELD_ACTIVATE, true);
                        
                    }

                    //for (Player* player : playerList)
                        //player->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_PLAYER_ACTIVATE_FIELDS, 0, 0);
                });
            break;
        }

        default:
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_FRIGID_SHARDS:
            {
                DoCastVictim(SPELL_FRIGID_SHARDS, false);
                _events.Repeat(6s, 8s);
                break;
            }

            case EVENT_CONE_OF_COLD:
            {
                DoCastVictim(SPELL_CONE_OF_COLD, false);
                _events.Repeat(12s, 13s);
                break;
            }

            case EVENT_COMET_BARRAGE:
            {
                DoCastVictim(SPELL_COMET_BARRAGE, false);
                _events.Repeat(16s, 18s);
                break;
            }

            default:
                break;
            }
        }

        DoSpellAttackIfReady(SPELL_ICE_BOLT);
    }

    void FormFrozenBridge(Position teleportPos, Position bridgePos)
    {
        me->CastSpell(teleportPos, SPELL_TELEPORT_PHASE_TWO_MASTER, false);

        uint32 bridgeAreaTriggerSpell;
        uint32 bridgeGameObjectSpell;



        switch (uint8 randomBridge = urand(DATA_BRIDGE_PHASE_TWO_1, DATA_BRIDGE_PHASE_TWO_3))
        {
        case DATA_BRIDGE_PHASE_TWO_1:
            bridgeAreaTriggerSpell = SPELL_CHANNEL_ICE_AREATRIGGER_01;
            bridgeGameObjectSpell = SPELL_CHANNEL_ICE_BRIDGE_01;
            break;
        case DATA_BRIDGE_PHASE_TWO_2:
            bridgeAreaTriggerSpell = SPELL_CHANNEL_ICE_AREATRIGGER_02;
            bridgeGameObjectSpell = SPELL_CHANNEL_ICE_BRIDGE_02;
            break;
        case DATA_BRIDGE_PHASE_TWO_3:
            bridgeAreaTriggerSpell = SPELL_CHANNEL_ICE_AREATRIGGER_03;
            bridgeGameObjectSpell = SPELL_CHANNEL_ICE_BRIDGE_03;
            break;
        default:
            break;
        }

        _scheduler.Schedule(500ms, [this](TaskContext /*task*/)
            {
                DoCastSelf(SPELL_CHANNEL_ICE, false);
            });

        _scheduler.Schedule(1s + 500ms, [this, bridgePos, bridgeAreaTriggerSpell](TaskContext /*task*/)
            {
                me->CastSpell(bridgePos, bridgeAreaTriggerSpell, true);
            });

        _scheduler.Schedule(5s + 500ms, [this, bridgePos, bridgeGameObjectSpell](TaskContext /*task*/)
            {
                me->CastSpell(bridgePos, bridgeGameObjectSpell, true);
            });
    }

private:
    InstanceScript* _instance;
    EventMap _events;
    TaskScheduler _scheduler;
    bool goliathAlive;
    bool souljudgeAlive;
    bool summonerAlive;
    uint8 goliathDeath = 0;
    uint8 souljudgeDeath = 0;
    uint8 summonerDeath = 0;
    uint8 windsCasted = 0;

};



// Frigid Shards - 354933
class spell_sylvanas_windrunner_frigid_shards : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_frigid_shards);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        if (!GetCaster())
            return;

        GetCaster()->CastSpell(GetTarget(), aurEff->GetSpellEffectInfo().TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sylvanas_windrunner_frigid_shards::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Comet Barrage - 354938
class spell_sylvanas_windrunner_comet_barrage : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_comet_barrage);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitDest()->GetPosition(), GetEffectInfo(EFFECT_0).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_sylvanas_windrunner_comet_barrage::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Teleport to Phase Two - 350903
class spell_sylvanas_windrunner_teleport_to_phase_two : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_teleport_to_phase_two);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* jaina = instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
                GetCaster()->CastSpell(jaina->GetRandomPoint(jaina->GetPosition(), frand(2.0f, 8.0f)), GetEffectInfo(EFFECT_0).TriggerSpell, true);
        }
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_sylvanas_windrunner_teleport_to_phase_two::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Teleport to Phase Three - 350906
class spell_sylvanas_windrunner_teleport_to_phase_three : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_teleport_to_phase_three);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* jaina = instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
                GetCaster()->CastSpell(jaina->GetRandomPoint(jaina->GetPosition(), frand(3.0f, 10.0f)), GetEffectInfo(EFFECT_0).TriggerSpell, true);
        }
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_sylvanas_windrunner_teleport_to_phase_three::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Teleport - 357103
class spell_sylvanas_windrunner_teleport : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_teleport);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitDest()->GetPosition(), GetEffectInfo(EFFECT_0).TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_sylvanas_windrunner_teleport::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum AnduinSpells
{
    SPELL_BLASPHEMY_PRE = 357729,
    SPELL_BLASPHEMY = 357730,
    SPELL_BLASPHEMY_DAMAGE = 357731,
    SPELL_CRIPPLING_DEFEAT = 354176,
    SPELL_BREAK_PLAYER_TARGETTING = 140562
};

enum AnduinEvents
{
    EVENT_BLASPHEMY = 1,
    EVENT_LIGHT_BLAST = 2,
    EVENT_CRIPPLING_DEFEAT = 3
};

enum AnduinActions
{

};



// Mawforged Goliath - 177892
struct npc_mawforged_goliath : public ScriptedAI
{
public:
    npc_mawforged_goliath(Creature* creature) : ScriptedAI(creature, DATA_MAWFORGED_GOLIATH),
        instance(creature->GetInstanceScript()) { }


    void JustAppeared() override
    {
        DoCastSelf(SPELL_PORTAL_INTERACT, false);

        me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        me->SetReactState(REACT_AGGRESSIVE);
        DoZoneInCombat();

    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!me->HasReactState(REACT_PASSIVE))
        {
            ScriptedAI::MoveInLineOfSight(who);
            return;
        }

        if (me->CanStartAttack(who, false) && me->IsWithinDistInMap(who, me->GetAttackDistance(who) + me->m_CombatDistance))
            JustEngagedWith(who);
    }

    void SpellHit(WorldObject* /*target*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_ARCANE_STASIS_WAVE)
        {
            me->DespawnOrUnsummon();
        }
    }

    void JustEngagedWith(Unit* who) override
    {
        DoCastSelf(SPELL_GOLIATH_FURY_02, false);

    }



    void JustDied(Unit* /*killer*/) override
    {

        if (Creature* jaina = instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
        {

            if (jaina->IsAIEnabled())
                jaina->AI()->DoAction(ACTION_GOLIATH_DIED);
        }
    }




    void UpdateAI(uint32 diff) override
    {

        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {

            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

private:



    EventMap _events;
    InstanceScript* instance;
};

enum SoulJudgeEvents
{
    EVENT_CRUSHING_DREAD = 1,
    EVENT_LASHING_STRIKE = 2,
};

enum SoulJudgeSpells
{
    SPELL_CRUSHING_DREAD_CIRCLE = 351117,
    SPELL_CRUSHING_DREAD_DAMAGE = 351118,
    SPELL_LASHING_WOUND = 351180,
};

// Mawforged Souljudge - 177889
struct npc_mawforged_souljudge : public ScriptedAI
{
    npc_mawforged_souljudge(Creature* creature) : ScriptedAI(creature, DATA_MAWFORGED_SOULJUDGE),
        _instance(creature->GetInstanceScript()) { }

    void JustAppeared() override
    {

        DoCastSelf(SPELL_PORTAL_INTERACT, false);
        _events.Reset();
        me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);

        DoZoneInCombat();
    }

    void JustEngagedWith(Unit* who) override

    {
        _events.ScheduleEvent(EVENT_CRUSHING_DREAD, 6s, 8s);
        _events.ScheduleEvent(EVENT_LASHING_STRIKE, 3s, 10s);
    }

    void SpellHit(WorldObject* /*target*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_ARCANE_STASIS_WAVE)
        {
            me->DespawnOrUnsummon();
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!me->HasReactState(REACT_PASSIVE))
        {
            ScriptedAI::MoveInLineOfSight(who);
            return;
        }

        if (me->CanStartAttack(who, false) && me->IsWithinDistInMap(who, me->GetAttackDistance(who) + me->m_CombatDistance))
            JustEngagedWith(who);
    }

    void JustDied(Unit* /*killer*/) override
    {

        if (Creature* jaina = _instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
        {
            if (jaina->IsAIEnabled())
                jaina->AI()->DoAction(ACTION_SOULJUDGE_DIED);
        }
    }


    void DamageTaken(Unit* /*who*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {

    }


    void UpdateAI(uint32 diff) override
    {

        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_CRUSHING_DREAD:
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 350.0f, true, true, -SPELL_CRUSHING_DREAD_CIRCLE))
                {
                    DoCast(target, SPELL_CRUSHING_DREAD_CIRCLE);


                }

                _events.ScheduleEvent(EVENT_CRUSHING_DREAD, 15s, 20s);
                break;
            }

            case EVENT_LASHING_STRIKE:
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 350.0f, true, true))
                {
                    DoCast(target, SPELL_LASHING_WOUND);

                }

                _events.ScheduleEvent(EVENT_LASHING_STRIKE, 12s, 18s);
                break;

            }

            default:
                break;
            }

        }

        DoMeleeAttackIfReady();
    }

private:



    EventMap _events;
    InstanceScript* _instance;
};


enum SummonerEvents
{
    EVENT_DECREPIT_ORBS = 1,
    EVENT_CURSE_OF_LETHARGY = 2,
    EVENT_DETONATE_CHARGE = 3,
    EVENT_DETONATE_CAST = 4,
    EVENT_SUMMON_DECREPIT_ORBS = 5,

    
};

enum SummonerSpells
{
    SPELL_SUMMON_DECREPIT_ORBS_1 = 351353,
    SPELL_CURSE_OF_LETHARGY_INSTANT = 351451,
    SPELL_CURSE_OF_LETHARGY = 351939,
    SPELL_DECREPIT_ORB_SIZE = 351317, // aura it
    SPELL_DECREPIT_ORB_VISUAL = 351327,
    SPELL_DETONATE_CAST = 351323,
    SPELL_DETONATE_DEATH = 351324,
};

// Mawforged Summoner - 177891
struct npc_mawforged_summoner : public ScriptedAI
{
    npc_mawforged_summoner(Creature* creature) : ScriptedAI(creature, DATA_MAWFORGED_SUMMONER),
        _instance(creature->GetInstanceScript()) { }


    void JustAppeared() override
    {
        DoCastSelf(SPELL_PORTAL_INTERACT, false);
        me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        _events.ScheduleEvent(EVENT_SUMMON_DECREPIT_ORBS, 3s, 5s);
        DoZoneInCombat();
    }

    void JustEngagedWith(Unit* who) override

    {


    }

    void MoveInLineOfSight(Unit* /*who*/) override { }

    void JustDied(Unit* /*killer*/) override
    {

        if (Creature* jaina = _instance->GetCreature(DATA_JAINA_PROUDMOORE_PINNACLE))
        {
            if (jaina->IsAIEnabled())
                jaina->AI()->DoAction(ACTION_SUMMONER_DIED);
        }
    }

    void SpellHitTarget(WorldObject* /*target*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_ARCANE_STASIS_WAVE)
        {
            me->DespawnOrUnsummon();
        }
    }

    void DamageTaken(Unit* /*who*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {

    }

    void UpdateAI(uint32 diff) override
    {

        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SUMMON_DECREPIT_ORBS:
                {
                    DoCastSelf(SPELL_SUMMON_DECREPIT_ORBS, false);

                    _events.ScheduleEvent(EVENT_SUMMON_DECREPIT_ORBS, 15s, 20s);
                    break;
                }
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

private:



    EventMap _events;
    InstanceScript* _instance;
};

// Decrepit Orb - 178008
struct npc_decrepit_orb : public ScriptedAI
{
    npc_decrepit_orb(Creature* creature) : ScriptedAI(creature, DATA_DECREPIT_ORB),
        _instance(creature->GetInstanceScript()) { }


    void JustAppeared() override
    {
        DoCastSelf(SPELL_DECREPIT_ORB_VISUAL, false);
        _events.ScheduleEvent(EVENT_DETONATE_CHARGE, 3s);
        //_events.ScheduleEvent(EVENT_DETONATE_CAST, 4s);
        DoZoneInCombat();
        SetCombatMovement(false);
        me->SetReactState(REACT_PASSIVE);
        me->AddAura(SPELL_DECREPIT_ORB_SIZE, me);
    }

    void JustEngagedWith(Unit* who) override

    {


    }

    void MoveInLineOfSight(Unit* /*who*/) override { }





    void DamageTaken(Unit* /*who*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {

    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSelf(SPELL_DETONATE_DEATH, false);
    }

    void UpdateAI(uint32 diff) override
    {

        if (!UpdateVictim())
            return;

        _events.Update(diff);



        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DETONATE_CHARGE:
                {
                    me->AddAura(SPELL_DECREPIT_ORB_SIZE, me);

                    _events.ScheduleEvent(EVENT_DETONATE_CHARGE, 3s, 4s);
                    break;
                }

                case EVENT_DETONATE_CAST:
                {
                    DoCastSelf(SPELL_DETONATE_CAST, false);


                    break;
                }

            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        
    }

private:



    EventMap _events;
    InstanceScript* _instance;
};





enum VanguardEvents
{
    EVENT_ACCURSED_MIGHT = 1,
    EVENT_UNSTOPPABLE_FORCE = 2,
};

enum VanguardSpells
{
    SPELL_ACCURSED_MIGHT = 350865,
    SPELL_UNSTOPPABLE_FORCE = 351075,
};


// Mawsworn Vanguard 177154
struct npc_mawsworn_vanguard : public ScriptedAI
{
public:
    npc_mawsworn_vanguard(Creature* creature) : ScriptedAI(creature, DATA_MAWSWORN_VANGUARD),
        _instance(creature->GetInstanceScript()) { }


    void JustAppeared() override
    {
        DoCastSelf(SPELL_PORTAL_INTERACT, false);
        me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        DoZoneInCombat();
    }

    void SpellHitTarget(WorldObject* /*target*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_ARCANE_STASIS_WAVE)
        {
            me->DespawnOrUnsummon();
        }
    }

    void JustEngagedWith(Unit* who) override
    {
        _events.ScheduleEvent(EVENT_ACCURSED_MIGHT, 1s, 3s, PHASE_TWO);
        _events.ScheduleEvent(EVENT_UNSTOPPABLE_FORCE, 5s, 10s, PHASE_TWO);

    }

    void JustDied(Unit* /*killer*/) override
    {


    }





    void UpdateAI(uint32 diff) override
    {

        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_ACCURSED_MIGHT:
            {
                DoCastSelf(SPELL_ACCURSED_MIGHT, false);
                _events.ScheduleEvent(EVENT_ACCURSED_MIGHT, 5s, 12s, PHASE_TWO);


                break;
            }

            case EVENT_UNSTOPPABLE_FORCE:
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 350.0f, true, true))
                {
                    DoCast(target, SPELL_UNSTOPPABLE_FORCE);

                }

                _events.ScheduleEvent(EVENT_UNSTOPPABLE_FORCE, 5s, 10s, PHASE_TWO);
                break;

            }

            default:
                break;
            }

        }

        DoMeleeAttackIfReady();
    }

private:



    EventMap _events;
    InstanceScript* _instance;
};

enum HopebreakerEvents
{
    EVENT_ENFLAME = 1,
    EVENT_DESTABILIZE = 2,
};


// Mawsworn Hopebreaker 177787
struct npc_mawsworn_hopebreaker : public ScriptedAI
{
public:
    npc_mawsworn_hopebreaker(Creature* creature) : ScriptedAI(creature, DATA_MAWSWORN_HOPEBREAKER),
        instance(creature->GetInstanceScript()), isFeignDeath(false) { }


    void JustAppeared() override
    {
        DoCastSelf(SPELL_PORTAL_INTERACT, false);
        me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        DoZoneInCombat();

        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 500.0f, true, true))
        {
            AttackStart(target);

        }

    }

    void SpellHitTarget(WorldObject* /*target*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_ARCANE_STASIS_WAVE)
        {
            me->DespawnOrUnsummon();
        }
    }

    void JustEngagedWith(Unit* who) override
    {
        me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        _events.ScheduleEvent(EVENT_ENFLAME, 5s, 1, PHASE_TWO);
        _events.ScheduleEvent(EVENT_DESTABILIZE, 10s, 1, PHASE_TWO);


    }

    void JustDied(Unit* /*killer*/) override
    {


    }





    void UpdateAI(uint32 diff) override
    {

        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_ENFLAME:
            {
                DoCastSelf(SPELL_ENFLAME, false);


                break;
            }

            case EVENT_DESTABILIZE:
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 350.0f, true, true))
                {
                    DoCast(target, SPELL_DESTABILIZE);

                }

                _events.ScheduleEvent(EVENT_LASHING_STRIKE, 12s, 18s);
                break;

            }

            default:
                break;
            }

        }

        DoMeleeAttackIfReady();
    }

private:



    EventMap _events;
    InstanceScript* instance;
    bool isFeignDeath;
};


// Anduin Wrynn - 178072
struct npc_sylvanas_windrunner_anduin : public ScriptedAI
{
    npc_sylvanas_windrunner_anduin(Creature* creature) : ScriptedAI(creature, DATA_ANDUIN_CRUCIBLE),
        _instance(creature->GetInstanceScript()) { }

    void JustAppeared() override
    {
        _scheduler.ClearValidator();

        me->m_Events.AddEvent(new PauseAttackState(me, true), me->m_Events.CalculateTime(1ms));
    }

    void Reset() override
    {
        _events.Reset();
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        // HACKFIX: sparring system is not implemented yet, this is a workaround
        if (me->HealthBelowPctDamaged(85.0f, damage))
            damage = 0;
    }

    void DamageDealt(Unit* /*victim*/, uint32& damage, DamageEffectType /*damageType*/) override
    {
        // HACKFIX: sparring system is not implemented yet, this is a workaround
        //damage = 0;
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
        case ACTION_INITIATE_PHASE_THREE:
        {
            _events.SetPhase(PHASE_THREE);

            me->RemoveUnitFlag2(UNIT_FLAG2_UNTARGETABLE_BY_CLIENT);

            DoCastSelf(SPELL_BLASPHEMY_PRE, false);

            _scheduler.Schedule(11s + 800ms, [this](TaskContext /*task*/)
                {
                    DoCastSelf(SPELL_BREAK_PLAYER_TARGETTING, true);
                });

            _scheduler.Schedule(12s, [this](TaskContext /*task*/)
                {
                    me->SetUnitFlag2(UNIT_FLAG2_UNTARGETABLE_BY_CLIENT);

                    me->m_Events.AddEvent(new PauseAttackState(me, false), me->m_Events.CalculateTime(1ms));

                    _events.ScheduleEvent(EVENT_LIGHT_BLAST, 8s, 12s, PHASE_THREE);
                    _events.ScheduleEvent(EVENT_BLASPHEMY, 18s, 20s, PHASE_THREE);
                });
            break;
        }

        default:
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_BLASPHEMY:
            {
                DoCastSelf(SPELL_BLASPHEMY, false);

                _events.Repeat(25s, 45s);
                break;
            }

            case EVENT_LIGHT_BLAST:
            {
                me->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_ANDUIN_FINISH_BLASPHEMY, 0, 0);

                _events.Repeat(8s, 12s);
                break;
            }

            case EVENT_CRIPPLING_DEFEAT:
            {
                DoCastVictim(SPELL_CRIPPLING_DEFEAT, false);
                break;
            }

            default:
                break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    InstanceScript* _instance;
    EventMap _events;
    TaskScheduler _scheduler;
};

Position const MiddlePlatformNegativeYVertexPos = { -285.9056f, -1276.4102f, 5666.6479f, 0.0f };

static Position GetRandomPointInMiddlePlatform()
{
    G3D::Vector3 point;
    G3D::CoordinateFrame{ G3D::Matrix3::fromEulerAnglesZYX(DegToRad(-45.0f), 0.0f, 0.0f), { MiddlePlatformNegativeYVertexPos.GetPositionX(), MiddlePlatformNegativeYVertexPos.GetPositionY(), MiddlePlatformNegativeYVertexPos.GetPositionZ()} }
        .toWorldSpace(G3D::Box{ { 0.0f, 0.0f, 0.0f }, { 50.0f, 50.0f, 0.0f } })
        .getRandomSurfacePoint(point);

    Position position = Vector3ToPosition(point);

    return Position(position.GetPositionX(), position.GetPositionY(), position.GetPositionZ());
}

class BlasphemyEvent : public BasicEvent
{
public:
    BlasphemyEvent(Unit* actor, Position const blasphemyDestPos, uint8 queuedSpell) : _actor(actor), _blasphemyDestPos(blasphemyDestPos),
        _queuedSpell(queuedSpell) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        if (_queuedSpell == 0)
            _actor->SendPlaySpellVisualKit(SPELL_VISUAL_KIT_ANDUIN_FINISH_BLASPHEMY, 0, 0);
        else
            _actor->CastSpell(_blasphemyDestPos, SPELL_BLASPHEMY_DAMAGE, true);

        return true;
    }

private:
    Unit* _actor;
    Position _blasphemyDestPos;
    uint8 _queuedSpell;
};

// Blasphemy - 357729
// Blasphemy - 357730
class spell_sylvanas_windrunner_blasphemy : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_blasphemy);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;

        GetCaster()->m_Events.AddEvent(new BlasphemyEvent(GetCaster(), GetCaster()->GetPosition(), 0), GetCaster()->m_Events.CalculateTime(5s));
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;

        for (uint8 i = 0; i < 20; i++)
        {
            Position const blasphemyDestPos = GetRandomPointInMiddlePlatform();

            uint32 speedAsTime = urand(800, 1500);

            GetCaster()->SendPlaySpellVisual(blasphemyDestPos, 0.0f, SPELL_VISUAL_BLASPHEMY, 0, 0, float(speedAsTime / 1000), true);

            GetCaster()->m_Events.AddEvent(new BlasphemyEvent(GetCaster(), blasphemyDestPos, 1), GetCaster()->m_Events.CalculateTime(Milliseconds(speedAsTime)));
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_sylvanas_windrunner_blasphemy::OnApply, EFFECT_0, SPELL_AURA_AREA_TRIGGER, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_sylvanas_windrunner_blasphemy::OnRemove, EFFECT_0, SPELL_AURA_AREA_TRIGGER, AURA_EFFECT_HANDLE_REAL);
    }
};

// Energize Power Aura (Sylvanas) - 352312
class spell_sylvanas_windrunner_energize_power_aura : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_energize_power_aura);

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!GetCaster())
            return;

        uint8 powerRegenCycle = 0;
        uint32 powerGained = 0;

        powerGained = _sylvanasPowerRegenCycle[powerRegenCycle++];

        if (powerRegenCycle >= 3)
            powerRegenCycle = 0;

        GetTarget()->ModifyPower(GetTarget()->GetPowerType(), powerGained);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sylvanas_windrunner_energize_power_aura::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }

private:
    /// NOTE: Sylvanas regenerates 10 energy points every 3s roughly.
    static constexpr std::array<int32, 3> _sylvanasPowerRegenCycle =
    {
        3, 3, 4
    };
};

// Activate Phase Intermission - 359429
class spell_sylvanas_windrunner_activate_phase_intermission : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_activate_phase_intermission);

    void OnCast(SpellMissInfo /*missInfo*/)
    {
        
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            { 

                Map::PlayerList const& players = sylvanas->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
                        if (Player* player = i->GetSource())
                        {
                            if (player->HasAura(SPELL_WAILING_ARROW_POINTER) || sylvanas->HasAura(SPELL_WINDRUNNER) || sylvanas->HasUnitState(UNIT_STATE_CASTING))
                            {
                                sylvanas->AI()->DoAction(ACTION_CHECK_INTERMISSION);
                                return;
                            }



                            else
                            {
                                sylvanas->AI()->DoAction(ACTION_PREPARE_INTERMISSION);
                                GetCaster()->SetDisplayId(DATA_DISPLAY_ID_SYLVANAS_ELF_MODEL);
                                GetCaster()->SetAnimTier(AnimTier::Ground);
                            }
                        }
                           
                        
            }

        
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_sylvanas_windrunner_activate_phase_intermission::OnCast);
    }
};

// Goliath Fury 02
class spell_sylvanas_windrunner_goliath_fury_aura : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_goliath_fury_aura);

    ObjectGuid currentTarget;

    bool Load() override
    {
        currentTarget = ObjectGuid::Empty;
        return true;
    }

    void OnProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        Unit* caster = eventInfo.GetActor(); // mob
        Unit* target = eventInfo.GetActionTarget(); // current target
        if (!caster || !target)
            return;

        if (target->GetGUID() == currentTarget)
            return;

        PreventDefaultAction();
        caster->RemoveAura(SPELL_GOLIATH_FURY);
        currentTarget = target->GetGUID();
    }

    void Register() override
    {

        OnEffectProc += AuraEffectProcFn(spell_sylvanas_windrunner_goliath_fury_aura::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);

    }

    AuraScript* GetAuraScript() const
    {
        return new spell_sylvanas_windrunner_goliath_fury_aura();
    }

};

// SoulJudge Crushing Dread - 351117
//SPELL_CRUSHING_DREAD_CIRCLE = 351117,
//SPELL_CRUSHING_DREAD_DAMAGE = 351118,
class spell_sylvanas_windrunner_souljudge_crushing_dread_aura : public AuraScript
{
    PrepareAuraScript(spell_sylvanas_windrunner_souljudge_crushing_dread_aura);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_CRUSHING_DREAD_DAMAGE });
    }

    void OnPeriodic(AuraEffect const* aurEff)
    {
        if (!GetCaster())
            return;

        PreventDefaultAction();

        if (aurEff->GetTickNumber() == 2)
        {
            if (Unit* caster = GetCaster())
            {
                GetCaster()->CastSpell(GetTarget(), SPELL_CRUSHING_DREAD_DAMAGE, true);
                Aura* crushingDread = GetTarget()->GetAura(SPELL_CRUSHING_DREAD_CIRCLE);
                if (crushingDread && crushingDread->GetStackAmount() >= 0)
                    crushingDread->ModStackAmount(+1);

            }
        }
    }


    void Register() override
    {

        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sylvanas_windrunner_souljudge_crushing_dread_aura::OnPeriodic, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);

    }

    AuraScript* GetAuraScript() const
    {
        return new spell_sylvanas_windrunner_souljudge_crushing_dread_aura();
    }

};

class spell_sylvanas_windrunner_summoner_summon_decrepit_orbs : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_summoner_summon_decrepit_orbs);

    void HandleCast()
    {

        if (Unit* caster = GetCaster())
        {
            Position pos = GetCaster()->GetPosition().GetPositionWithOffset(frand(3.0f, 10.0f));

            for (uint8 i = 0; i < 4; i++)
            {

                caster->SummonCreature(NPC_DECREPIT_ORBS, pos, TEMPSUMMON_MANUAL_DESPAWN);

            }

        }
        

    }

   
    void Register() override
    {
        OnCast += SpellCastFn(spell_sylvanas_windrunner_summoner_summon_decrepit_orbs::HandleCast);
    }
};

// Activate Finish Boss - 359431
class spell_sylvanas_windrunner_activate_finish_boss : public SpellScript
{
    PrepareSpellScript(spell_sylvanas_windrunner_activate_finish_boss);

    void OnCast(SpellMissInfo /*missInfo*/)
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (sylvanas->IsAIEnabled())
                    sylvanas->AI()->DoAction(ACTION_PREPARE_FINISH_BOSS);
            }
        }
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_sylvanas_windrunner_activate_finish_boss::OnCast);
    }
};



// Desecrating Shot - 22400
struct at_sylvanas_windrunner_disecrating_shot : AreaTriggerAI
{
    at_sylvanas_windrunner_disecrating_shot(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger),
        _instance(at->GetInstanceScript()) { }

    void OnCreate() override
    {
        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {
            _scheduler.Schedule(2s + 500ms, [this, sylvanas](TaskContext /*task*/)
                {
                    if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                    {
                        if (Creature* shadowCopy = ObjectAccessor::GetCreature(*at, ai->GetShadowCopyJumperGuid(0)))
                            shadowCopy->SendPlaySpellVisual(at->GetPosition(), 0, SPELL_VISUAL_DESECRATING_ARROW, 0, 0, 0.280999988317489624f, true);
                    }
                });

            _scheduler.Schedule(2s + 780ms, [this, sylvanas](TaskContext /*task*/)
                {
                    sylvanas->CastSpell(at->GetPosition(), SPELL_DESECRATING_SHOT_TRIGGERED, true);

                    at->Remove();
                });
        }
    }

    void OnUpdate(uint32 diff) override
    {
        if (!_instance)
            return;

        _scheduler.Update(diff);
    }

private:
    InstanceScript* _instance;
    TaskScheduler _scheduler;
};

// Calamity - 23389
struct at_sylvanas_windrunner_calamity : AreaTriggerAI
{
    at_sylvanas_windrunner_calamity(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger),
        _instance(at->GetInstanceScript()) { }

    void OnCreate() override
    {
        if (!_instance)
            return;
    }

    void OnUnitEnter(Unit* unit) override
    {
        if (!_instance || !unit->IsPlayer() || unit->GetGUID() != at->GetCasterGuid())
            return;

        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {
            sylvanas->CastSpell(unit, SPELL_DOMINATION_ARROW_CALAMITY_DAMAGE, true);

            at->GetTarget()->KillSelf();

            at->Remove();
        }
    }

private:
    InstanceScript* _instance;
    ObjectGuid _chainedPlayer;
};

class DebrisEvent : public BasicEvent
{
public:
    DebrisEvent(Unit* actor, Position const debrisDestPos) : _actor(actor), _debrisDestPos(debrisDestPos) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        _actor->CastSpell(_debrisDestPos, SPELL_RIVEN_DEBRIS, true);

        return true;
    }

private:
    Unit* _actor;
    Position _debrisDestPos;
};

// Rive - 23028
struct at_sylvanas_windrunner_rive : AreaTriggerAI
{
    at_sylvanas_windrunner_rive(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger),
        _instance(at->GetInstanceScript()) { }

    void OnSplineIndexReached(int splineIndex) override
    {
        if (!_instance)
            return;

        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {
            if (splineIndex == DATA_SPLINEPOINT_RIVE_MARKER_DISAPPEAR)
            {
                std::list<WorldObject*> riveMarkerAreaTriggers;
                Trinity::AllWorldObjectsInRange objects(sylvanas, 250.0f);
                Trinity::WorldObjectListSearcher<Trinity::AllWorldObjectsInRange> searcher(sylvanas, riveMarkerAreaTriggers, objects);
                Cell::VisitAllObjects(sylvanas, searcher, 250.0f);

                for (std::list<WorldObject*>::const_iterator itr = riveMarkerAreaTriggers.begin(); itr != riveMarkerAreaTriggers.end(); ++itr)
                {
                    if (AreaTrigger* riveMarkerAreaTrigger = (*itr)->ToAreaTrigger())
                    {
                        if (riveMarkerAreaTrigger->GetEntry() == DATA_AREATRIGGER_RIVE_MARKER)
                            riveMarkerAreaTrigger->Remove();
                    }
                }
            }
            else
            {
                for (uint8 i = 0; i < 5; i++)
                {
                    Position const debrisPos = at->GetRandomNearPosition(30.0f);

                    at->SendPlayOrphanSpellVisual(debrisPos, SPELL_VISUAL_RIVEN_DEBRIS, 1.50f, true, false);

                    sylvanas->m_Events.AddEvent(new DebrisEvent(sylvanas, debrisPos), sylvanas->m_Events.CalculateTime(1s + 500ms));
                }
            }
        }
    }

    void OnUnitEnter(Unit* unit) override
    {
        if (!_instance || !unit->IsPlayer())
            return;

        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            sylvanas->CastSpell(unit, SPELL_RIVE_DAMAGE, true);
    }

private:
    InstanceScript* _instance;
};

// Frozen Bridge - 5428, Earthen Bridge - 5428
struct at_sylvanas_windrunner_bridges : AreaTriggerAI
{
    at_sylvanas_windrunner_bridges(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger),
        _instance(at->GetInstanceScript()) { }

    void OnCreate() override
    {
        if (!_instance)
            return;

        // NOTE: we modify the duration as it lasts for the rest of the encounter.
        at->SetDuration(-1);
    }

private:
    InstanceScript* _instance;
};

// Haunting Wave - 22874, 23694, 23693, 23673	
struct at_sylvanas_windrunner_haunting_wave : AreaTriggerAI
{
    at_sylvanas_windrunner_haunting_wave(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger),
        _instance(at->GetInstanceScript()) { }
    void OnUnitEnter(Unit* unit) override
    {
        if (!_instance || !unit->IsPlayer())
            return;
        switch (at->GetMap()->GetDifficultyID())
        {
        case DIFFICULTY_LFR_NEW:
            _movementForceMagnitude = 4.0f;
            break;
        case DIFFICULTY_NORMAL_RAID:
            _movementForceMagnitude = 5.0f;
            break;
        case DIFFICULTY_HEROIC_RAID:
            _movementForceMagnitude = 6.0f;
            break;
        case DIFFICULTY_MYTHIC_RAID:
            _movementForceMagnitude = 8.0f;
            break;
        default:
            break;
        }
        unit->ApplyMovementForce(at->GetGUID(), HauntingPull, _movementForceMagnitude, MovementForceType::Gravity);
        at->GetCaster()->CastSpell(unit, SPELL_HAUNTING_WAVE_DAMAGE, true);
        //unit->ApplyMovementForce(at->GetGUID(), *at->GetCaster(), -5.f, 0);


    }
    void OnUnitExit(Unit* unit) override
    {
        if (!_instance || !unit->IsPlayer())
            return;
        unit->RemoveMovementForce(at->GetGUID());
    }
private:
    InstanceScript* _instance;
    float _movementForceMagnitude = 0.0f;
};

// Blasphemy (Pre-Phase 3) - 23506
struct at_sylvanas_windrunner_blasphemy_pre : AreaTriggerAI
{
    at_sylvanas_windrunner_blasphemy_pre(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger),
        _instance(at->GetInstanceScript()) { }

    

    void OnUnitEnter(Unit* unit) override
    {
        if (!_instance || !unit->IsPlayer())
            return;        

            

        at->GetCaster()->CastSpell(unit, SPELL_BLASPHEMY_STUN, true);
        at->GetCaster()->CastSpell(unit, SPELL_INTERMISSION_STUN, true);
        unit->CastSpell(unit, SPELL_INTERMISSION_STUN, true);
    }

    void OnUnitExit(Unit* unit) override
    {

        if (unit->HasAura(SPELL_INTERMISSION_STUN))
            unit->RemoveAura(SPELL_INTERMISSION_STUN);
    }

private:
    InstanceScript* _instance;
};

// Banshee's Bane - 27461
struct at_sylvanas_windrunner_banshee_bane : AreaTriggerAI
{
    at_sylvanas_windrunner_banshee_bane(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger),
        _instance(at->GetInstanceScript()), _readyToPick(false), _updateDiff(0) { }

    void OnUpdate(uint32 diff) override
    {
        if (!_instance)
            return;

        _scheduler.Update(diff);

        if (!_readyToPick)
        {
            _scheduler.Schedule(1s, [this](TaskContext /*task*/)
                {
                    _readyToPick = true;
                });
        }
    }

    void OnUnitEnter(Unit* unit) override
    {
        if (!_instance || !unit->IsPlayer() || !_readyToPick)
            return;

        if (Creature* sylvanas = _instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
        {
            unit->SendPlaySpellVisual(at->GetPosition(), 0.0f, SPELL_VISUAL_BANSHEES_BANE_ABSORB, 0, 0, 0.5f, true);

            _scheduler.Schedule(500ms, [this, sylvanas, unit](TaskContext /*task*/)
                {
                    sylvanas->CastSpell(unit, SPELL_BANSHEES_BANE, true);

                    at->Remove();
                });
        }
    }

private:
    InstanceScript* _instance;
    TaskScheduler _scheduler;
    bool _readyToPick;
    uint32 _updateDiff;
};

// Raze - 23117
struct at_sylvanas_windrunner_raze : AreaTriggerAI
{
    at_sylvanas_windrunner_raze(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger),
        _instance(at->GetInstanceScript()) { }

    void OnUnitEnter(Unit* unit) override
    {
        if (!_instance || !unit->IsPlayer())
            return;

        at->GetCaster()->CastSpell(unit, SPELL_RAZE_PERIODIC, true);
    }

    void OnUnitExit(Unit* unit) override
    {
        if (!_instance || !unit->IsPlayer())
            return;

        if (unit->HasAura(SPELL_RAZE_PERIODIC))
            unit->RemoveAura(SPELL_RAZE_PERIODIC);
    }

private:
    InstanceScript* _instance;
};

// AT 10
struct at_invigorating_field_1 : public AreaTriggerAI
{

    at_invigorating_field_1(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger),
        _instance(at->GetInstanceScript()) { }


    void OnUnitEnter(Unit* unit) override
    {
        if (unit->HasUnitFlag(UNIT_FLAG_PACIFIED))
            return;

        if (InstanceScript* instance = at->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    for (uint8 covenantPlatform = 0; covenantPlatform < 1; covenantPlatform++)
                    {
                        if (ai->IsPlatformDesecrated(covenantPlatform))
                        {
                            return;
                        }
                    }

                }

            }

        }

        float x = unit->GetPositionX();
        float y = unit->GetPositionY() - 55.1f;
        float z = unit->GetPositionZ();

        unit->CastSpell(Position{ x, y, z }, SPELL_INVIGORATING_FIELD_JUMP, false);

        

    }
private:
    InstanceScript* _instance;
    bool _maldraxxiDesecrated;
};
// AT 12
struct at_invigorating_field_10 : public AreaTriggerAI
{

    at_invigorating_field_10(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger),
        _instance(at->GetInstanceScript()) { }


    void OnUnitEnter(Unit* unit) override
    {
        if (unit->HasUnitFlag(UNIT_FLAG_PACIFIED))
            return;

        if (InstanceScript* instance = at->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    for (uint8 covenantPlatform = 2; covenantPlatform < 3; covenantPlatform++)
                    {
                        if (ai->IsPlatformDesecrated(covenantPlatform))
                        {
                            return;
                        }
                    }

                }

            }

        }


        float x = unit->GetPositionX();
        float y = unit->GetPositionY() - 55.1f;
        float z = unit->GetPositionZ();

        unit->CastSpell(Position{ x, y, z }, SPELL_INVIGORATING_FIELD_JUMP, false);



    }
private:
    InstanceScript* _instance;
    bool _maldraxxiDesecrated;
};
// AT 11
class at_invigorating_field_2 : public AreaTriggerAI
{
public:
    at_invigorating_field_2(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {

        if (unit->HasUnitFlag(UNIT_FLAG_PACIFIED))
            return;

        if (InstanceScript* instance = at->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    for (uint8 covenantPlatform = 0; covenantPlatform < 1; covenantPlatform++)
                    {
                        if (ai->IsPlatformDesecrated(covenantPlatform))
                        {
                            return;
                        }
                    }

                }

            }

        }
        float x = unit->GetPositionX();
        float y = unit->GetPositionY() + 55.1f;
        float z = unit->GetPositionZ();

        unit->CastSpell(Position{ x, y, z }, SPELL_INVIGORATING_FIELD_JUMP, false);



    }
};
// AT 13
class at_invigorating_field_20 : public AreaTriggerAI
{
public:
    at_invigorating_field_20(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {

        if (unit->HasUnitFlag(UNIT_FLAG_PACIFIED))
            return;

        if (InstanceScript* instance = at->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    for (uint8 covenantPlatform = 2; covenantPlatform < 3; covenantPlatform++)
                    {
                        if (ai->IsPlatformDesecrated(covenantPlatform))
                        {
                            return;
                        }
                    }

                }

            }

        }

        float x = unit->GetPositionX();
        float y = unit->GetPositionY() + 55.1f;
        float z = unit->GetPositionZ();

        unit->CastSpell(Position{ x, y, z }, SPELL_INVIGORATING_FIELD_JUMP, false);



    }
};
// AT 14
class at_invigorating_field_3 : public AreaTriggerAI
{
public:
    at_invigorating_field_3(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        if (unit->HasUnitFlag(UNIT_FLAG_PACIFIED))
            return;

        if (InstanceScript* instance = at->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    for (uint8 covenantPlatform = 0; covenantPlatform < 1; covenantPlatform++)
                    {
                        if (ai->IsPlatformDesecrated(covenantPlatform))
                        {
                            return;
                        }
                    }

                }

            }

        }


        float x = unit->GetPositionX() + 55.1f;
        float y = unit->GetPositionY();
        float z = unit->GetPositionZ();

        unit->CastSpell(Position{ x, y, z }, SPELL_INVIGORATING_FIELD_JUMP, false);



    }
};
// AT 16
class at_invigorating_field_30 : public AreaTriggerAI
{
public:
    at_invigorating_field_30(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        if (unit->HasUnitFlag(UNIT_FLAG_PACIFIED))
            return;

        if (InstanceScript* instance = at->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    for (uint8 covenantPlatform = 1; covenantPlatform < 2; covenantPlatform++)
                    {
                        if (ai->IsPlatformDesecrated(covenantPlatform))
                        {
                            return;
                        }
                    }

                }

            }

        }


        float x = unit->GetPositionX() + 55.1f;
        float y = unit->GetPositionY();
        float z = unit->GetPositionZ();

        unit->CastSpell(Position{ x, y, z }, SPELL_INVIGORATING_FIELD_JUMP, false);



    }
};
// AT 17
class at_invigorating_field_4 : public AreaTriggerAI
{
public:
    at_invigorating_field_4(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {

        if (unit->HasUnitFlag(UNIT_FLAG_PACIFIED))
            return;

        if (InstanceScript* instance = at->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    for (uint8 covenantPlatform = 1; covenantPlatform < 2; covenantPlatform++)
                    {
                        if (ai->IsPlatformDesecrated(covenantPlatform))
                        {
                            return;
                        }
                    }

                }

            }

        }
        float x = unit->GetPositionX() - 55.1f;
        float y = unit->GetPositionY();
        float z = unit->GetPositionZ();
        unit->CastSpell(Position{ x, y, z }, SPELL_INVIGORATING_FIELD_JUMP, false);


    }
};
// AT 15
class at_invigorating_field_40 : public AreaTriggerAI
{
public:
    at_invigorating_field_40(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {

        if (unit->HasUnitFlag(UNIT_FLAG_PACIFIED))
            return;

        if (InstanceScript* instance = at->GetInstanceScript())
        {
            if (Creature* sylvanas = instance->GetCreature(DATA_SYLVANAS_WINDRUNNER))
            {
                if (boss_sylvanas_windrunner* ai = CAST_AI(boss_sylvanas_windrunner, sylvanas->AI()))
                {
                    for (uint8 covenantPlatform = 0; covenantPlatform < 1; covenantPlatform++)
                    {
                        if (ai->IsPlatformDesecrated(covenantPlatform))
                        {
                            return;
                        }
                    }

                }

            }

        }

        float x = unit->GetPositionX() - 55.1f;
        float y = unit->GetPositionY();
        float z = unit->GetPositionZ();
        unit->CastSpell(Position{ x, y, z }, SPELL_INVIGORATING_FIELD_JUMP, false);


    }
};

class at_anduin_center_platform : public AreaTriggerAI
{
public:
    at_anduin_center_platform(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        if (InstanceScript* instance = at->GetInstanceScript())
        {
            if (instance->GetData(DATA_CENTER_PLATFORM) != DONE)

            return;
        }
    

        else if (InstanceScript* instance = at->GetInstanceScript())
        {
            if (Creature* anduin = instance->GetCreature(DATA_ANDUIN_CRUCIBLE))
            {
                anduin->CastSpell(unit, SPELL_CRIPPLING_DEFEAT, false);
            }
        }
    }

};



void AddSC_boss_sylvanas_windrunner()
{
    RegisterSanctumOfDominationCreatureAI(boss_sylvanas_windrunner);
    RegisterSanctumOfDominationCreatureAI(npc_sylvanas_windrunner_shadowcopy);
    RegisterSanctumOfDominationCreatureAI(npc_sylvanas_windrunner_shadowcopy_riding);
    RegisterSanctumOfDominationCreatureAI(npc_sylvanas_windrunner_domination_arrow);


    RegisterSpellScript(spell_sylvanas_windrunner_ranger_bow);
    RegisterSpellScript(spell_sylvanas_windrunner_ranger_dagger);
    RegisterSpellScript(spell_sylvanas_windrunner_ranger_shot);
    RegisterSpellScript(spell_sylvanas_windrunner_ranger_strike);
    RegisterSpellScript(spell_sylvanas_windrunner_windrunner);
    RegisterSpellScript(spell_sylvanas_windrunner_disappear);
    RegisterSpellScript(spell_sylvanas_windrunner_withering_fire);
    RegisterSpellScript(spell_sylvanas_windrunner_desecrating_shot);
    RegisterSpellAndAuraScriptPair(spell_sylvanas_windrunner_ranger_heartseeker, spell_sylvanas_windrunner_ranger_heartseeker_aura);
    RegisterSpellScript(spell_sylvanas_windrunner_ranger_heartseeker_shadow_damage);
    RegisterSpellScript(spell_sylvanas_windrunner_ranger_heartseeker_physical_damage);
    RegisterSpellScript(spell_sylvanas_windrunner_domination_chains);
    RegisterSpellScript(spell_sylvanas_windrunner_domination_arrow);
    RegisterSpellScript(spell_sylvanas_windrunner_domination_chain);
    RegisterSpellScript(spell_sylvanas_windrunner_domination_chain_periodic);
    RegisterSpellScript(spell_sylvanas_windrunner_wailing_arrow);
    RegisterSpellScript(spell_sylvanas_windrunner_wailing_arrow_trigger);
    RegisterSpellScript(spell_sylvanas_windrunner_wailing_arrow_raid_damage);
    RegisterSpellScript(spell_sylvanas_windrunner_veil_of_darkness_fade);
    RegisterSpellScript(spell_sylvanas_windrunner_veil_of_darkness_phase_1);
    RegisterSpellScript(spell_sylvanas_windrunner_veil_of_darkness_phase_2);
    RegisterSpellScript(spell_sylvanas_windrunner_veil_of_darkness_visual_circle);
    RegisterSpellScript(spell_sylvanas_windrunner_rive);
    RegisterSpellScript(spell_sylvanas_windrunner_rive_fast);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_wail);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_wail_marker);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_wail_triggered_missile);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_wail_interrupt);
    RegisterSpellScript(spell_sylvanas_windrunner_daggers_p2);

    RegisterSpellScript(spell_sylvanas_windrunner_haunting_wave);
    RegisterSpellScript(spell_sylvanas_windrunner_ruin);

    RegisterSanctumOfDominationCreatureAI(npc_mawforged_goliath);
    RegisterSpellScript(spell_sylvanas_windrunner_goliath_fury_aura);

    RegisterSanctumOfDominationCreatureAI(npc_mawforged_summoner);
    RegisterSanctumOfDominationCreatureAI(npc_decrepit_orb);
    RegisterSpellScript(spell_sylvanas_windrunner_summoner_summon_decrepit_orbs);
    RegisterSanctumOfDominationCreatureAI(npc_mawforged_souljudge);
    RegisterSpellScript(spell_sylvanas_windrunner_souljudge_crushing_dread_aura);

    RegisterSanctumOfDominationCreatureAI(npc_mawsworn_vanguard);

    RegisterSanctumOfDominationCreatureAI(npc_mawsworn_hopebreaker);




    RegisterSpellScript(spell_sylvanas_windrunner_energize_power_aura);
    RegisterSpellScript(spell_sylvanas_windrunner_activate_phase_intermission);
    RegisterSpellScript(spell_sylvanas_windrunner_activate_finish_boss);

    RegisterSanctumOfDominationCreatureAI(npc_sylvanas_windrunner_bolvar);
    RegisterSpellScript(spell_sylvanas_windrunner_runic_mark_triggered);
    RegisterSpellScript(spell_sylvanas_windrunner_charge_towards_sylvanas);
    RegisterSpellScript(spell_sylvanas_windrunner_winds_of_icecrown);

    RegisterSanctumOfDominationCreatureAI(npc_sylvanas_windrunner_thrall);
    RegisterSpellScript(spell_sylvanas_windrunner_pulverize);
    RegisterSpellScript(spell_sylvanas_windrunner_pulverize_triggered);
    RegisterSpellScript(spell_sylvanas_windrunner_stonecrash_phase_one_and_three);
    RegisterSpellScript(spell_sylvanas_windrunner_stonecrash_phase_two);

    RegisterSanctumOfDominationCreatureAI(npc_sylvanas_windrunner_jaina);
    RegisterSpellScript(spell_sylvanas_windrunner_frigid_shards);
    RegisterSpellScript(spell_sylvanas_windrunner_comet_barrage);
    RegisterSpellScript(spell_sylvanas_windrunner_teleport_to_phase_two);

    RegisterSpellScript(spell_sylvanas_windrunner_teleport);

    RegisterSanctumOfDominationCreatureAI(npc_sylvanas_windrunner_anduin);


    RegisterAreaTriggerAI(at_sylvanas_windrunner_disecrating_shot);
    RegisterAreaTriggerAI(at_sylvanas_windrunner_calamity);
    RegisterAreaTriggerAI(at_sylvanas_windrunner_rive);
    RegisterAreaTriggerAI(at_sylvanas_windrunner_bridges);
    RegisterAreaTriggerAI(at_sylvanas_windrunner_haunting_wave);
    RegisterSpellScript(spell_sylvanas_windrunner_veil_of_darkness_phase_2_fog);



    // PHASE_THREE
    RegisterSpellScript(spell_sylvanas_windrunner_teleport_to_phase_three);
    RegisterAreaTriggerAI(at_sylvanas_windrunner_blasphemy_pre);
    RegisterSpellScript(spell_sylvanas_windrunner_blasphemy);
    RegisterSpellScript(spell_sylvanas_windrunner_blasphemy_stun);
    RegisterAreaTriggerAI(at_sylvanas_windrunner_raze);
    RegisterSpellScript(spell_sylvanas_windrunner_raze);
    RegisterAreaTriggerAI(at_sylvanas_windrunner_banshee_bane);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_bane);
    RegisterSpellScript(spell_sylvanas_windrunner_bane_arrows);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_scream);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_scream_marker);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_scream_triggered_missile);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_scream_interrupt);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_scream_damage);
    RegisterAreaTriggerAI(at_invigorating_field_1);
    RegisterAreaTriggerAI(at_invigorating_field_10);
    RegisterAreaTriggerAI(at_invigorating_field_2);
    RegisterAreaTriggerAI(at_invigorating_field_20);
    RegisterAreaTriggerAI(at_invigorating_field_3);
    RegisterAreaTriggerAI(at_invigorating_field_30);
    RegisterAreaTriggerAI(at_invigorating_field_4);
    RegisterAreaTriggerAI(at_invigorating_field_40);
    RegisterSpellScript(spell_sylvanas_windrunner_veil_of_darkness_phase_3);
    RegisterSpellScript(spell_sylvanas_windrunner_veil_of_darkness_pointer);
    RegisterSpellScript(spell_sylvanas_windrunner_veil_of_darkness_phase_three_swap);
    RegisterSpellAndAuraScriptPair(spell_sylvanas_windrunner_banshee_heartseeker, spell_sylvanas_windrunner_banshee_heartseeker_aura);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_heartseeker_physical_damage);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_heartseeker_shadow_damage);
    RegisterSpellAndAuraScriptPair(spell_sylvanas_windrunner_banshee_blades, spell_sylvanas_windrunner_banshee_weapons_aura);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_blades_physical_damage);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_blades_shadow_damage);
    RegisterAreaTriggerAI(at_anduin_center_platform);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_fury);
    RegisterSpellScript(spell_sylvanas_windrunner_banshee_fury_explode);



}
