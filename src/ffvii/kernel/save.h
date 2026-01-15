#pragma once

#include "psyq/typedef.h"



#define GAME_CHARACTERS_N 0x9



struct Save
{
    // +   0x4 []       save preview lead character's level.
    // +   0x5 []       save preview char 0.
    // +   0x6 []       save preview char 1.
    // +   0x7 []       save preview char 2.
    // +   0x8 save preview lead character's name (0x10 size).
    // +  0x18 [][]     save preview lead character's current HP
    // +  0x1a [][]     save preview lead character's max HP
    // +  0x1c [][]     save preview lead character's current MP
    // +  0x1e [][]     save preview lead character's max MP
    // +  0x20 [][][][] save preview amount of gil.
    // +  0x24 [][][][] save preview number of seconds played.
    // +  0x28 save preview location name (0x18 size).
    // +  0x48 save preview window color (0xc size).
    // +  0x54          savemap character data (0x84 size).
    //     +0x00 []       character id.
    //     +0x01 []       level.
    //     +0x02 []       strength.
    //     +0x03 []       vitality
    //     +0x04 []       magic
    //     +0x05 []       spirit
    //     +0x06 []       dexterity
    //     +0x07 []       luck
    //     +0x08 []       strength bonus
    //     +0x09 []       vitality bonus
    //     +0x0a []       magic bonus
    //     +0x0b []       spirit bonus
    //     +0x0c []       dexterity bonus
    //     +0x0d []       luck bonus
    //     +0x0e []       current limit level
    //     +0x0f []       current limit bar (0xFF = limit break)
    //     +0x10          name string.
    //     +0x1c []       equipped weapon.
    //     +0x1d []       equipped armor.
    //     +0x1e []       equipped accessory.
    //     +0x1f []       flags (0x10 sadness, 0x20 fury).
    //     +0x20 []       char order (0xff normal, 0xfe back row).
    //     +0x21 []       level progress bar (0-255)
    //     +0x22 [][]     mask of learned limit skills
    //                     0x0001: Limit Lv. 1-1
    //                     0x0002: Limit Lv. 1-2
    //                     0x0004: Always 0 (reserved bit or spacer/breaker/end of limit)
    //                     0x0008: Limit Lv. 2-1
    //                     0x0010: Limit Lv. 2-2
    //                     0x0020: Always 0 (reserved bit or spacer/breaker/end of limit)
    //                     0x0040: Limit Lv. 3-1
    //                     0x0080: Limit Lv. 3-2
    //                     0x0100: Always 0 (reserved bit or spacer/breaker/end of limit)
    //                     0x0200: Limit Lv. 4
    //     +0x24 [][]     number of kills.
    //     +0x2c [][]     current hp.
    //     +0x2e [][]     max hp (before materia alterations).
    //     +0x30 [][]     current mp.
    //     +0x32 [][]     max mp (before materia alterations).
    //     +0x38 [][]     max hp (after materia alterations).
    //     +0x3a [][]     max mp (after materia alterations).
    //     +0x3c [][][][] current exp.
    //     +0x40 [][][][] weapon materia slot 1.
    //     +0x44 [][][][] weapon materia slot 2.
    //     +0x48 [][][][] weapon materia slot 3.
    //     +0x4c [][][][] weapon materia slot 4.
    //     +0x50 [][][][] weapon materia slot 5.
    //     +0x54 [][][][] weapon materia slot 6.
    //     +0x58 [][][][] weapon materia slot 7.
    //     +0x5c [][][][] weapon materia slot 8.
    //     +0x60 [][][][] armor materia slot 1.
    //     +0x64 [][][][] armor materia slot 2.
    //     +0x68 [][][][] armor materia slot 3.
    //     +0x6c [][][][] armor materia slot 4.
    //     +0x70 [][][][] armor materia slot 5.
    //     +0x74 [][][][] armor materia slot 6.
    //     +0x78 [][][][] armor materia slot 7.
    //     +0x7c [][][][] armor materia slot 8.
    //     +0x80 [][][][] exp to next level.
    // + 0x4f8 []       party member slot 1 (used in battle).
    // + 0x4f9 []       party member slot 2 (used in battle).
    // + 0x4fa []       party member slot 3 (used in battle).
    // + 0x4fc party item stock array.
    // + 0x77c party materia stock (0x4 size 0xc8 items).
    // + 0xa9c materia stolen by yuffie (0x4 size 0x30 items).
    //         0xffffff00 materia exp (0xffffff - mastered).
    //         0x000000ff materia id.
    // + 0xb5c end of array
    // + 0xb7c [][][][] party gil amount.
    // + 0xb80 [][][][] total seconds played.
    // + 0xb84 [][][][] countdown seconds for timer (set in STTIM).
    // + 0xb88 [][][][] seconds fraction.
    // + 0xb90 [][][][] ???
    // + 0xb94 [][]     ???
    // + 0xb96 [][]     field id.
    // + 0xba4 savemap memory bank 1/2 start
    // + 0xba4 (+0x00) [][]     game progress
    // + 0xbb4 (+0x10) []       played hours.
    // + 0xbb5 (+0x11) []       played minutes.
    // + 0xbb6 (+0x12) []       played seconds.
    // + 0xbb7 (+0x13) []       played frames. From 0 to 30 (dec) in one sec.
    // + 0xbb8 (+0x14) []       countdown timer hours.
    // + 0xbb9 (+0x15) []       countdown timer minutes.
    // + 0xbba (+0x16) []       countdown timer seconds.
    // + 0xbbb (+0x17) []       countdown timer frames. From 0 to 30 (dec) in one sec.
    // + 0xbbc (+0x18) [][]     number of battles.
    // + 0xbbe (+0x1a) [][]     number of escapes.
    // + 0xbc0 (+0x1c) [][]     menu visibility mask.
    // + 0xbc2 (+0x1e) [][]     menu locking mask.
    //                         0x0001 - item
    //                         0x0002 - magic
    //                         0x0004 - materia
    //                         0x0008 - equip
    //                         0x0010 - status
    //                         0x0020 - order
    //                         0x0040 - limit
    //                         0x0080 - config
    //                         0x0100 - PHS
    //                         0x0200 - SAVE
    // + 0xbe4 bit array of key items (0x1bit size 0x40 items 0x8 bytes total)
    // + 0xbef       []       some flags.
    //                         0x01 set when any summon equiped.
    //                         0x02 set when enemy skill equipped.
    // + 0xc03       []       init with 0 on worldmap loading.
    // + 0xc1e (+0x7a) []       flags
    //                         0x01 - used in world map.
    //                         0x40 - tagret text shown in battle.
    //                         0x80 - underwater timer in battle.
    // + 0xca4 savemap memory bank 3/4 start
    // + 0xcad (+0x09) []       character id in slot 1.
    // + 0xcae (+0x0a) []       character id in slot 2.
    // + 0xcaf (+0x0b) []       character id in slot 3.
    // + 0xda4 savemap memory bank b/c start
    // + 0xea4 savemap memory bank d/e start
    // + 0xeaa (+0x06) []       some value that increments each battle check.
    // + 0xeab (+0x07) []       some value that increments each battle check.
    // + 0xf04         []       ??? Northern Cave - Progress (TODO: more info)
    // + 0xf05         []       ??? Northern Cave - Progress (TODO: more info)
    //                             0x30 - some music effects.
    // + 0xf0c field map name for menu (0x18 size).
    // + 0xf24 
    // + 0xf2a (+0x86) []       flags
    //                         0x10 - used in world map
    // + 0xf5a [][]     ????
    // + 0xf5c array of coords for vehicles left on map (0x8 size 0x6 items) (cloud, wild chokobo, bronco/chocobo, buggy/highwind, submarine, diamond/ultimate/ruby).
    //     +0x00 [][][][] 0x0007ffff x.
    //                  0x00f80000 model id.
    //                  0xff000000 direction.
    //     +0x04 [][][][] 0x0003ffff z.
    //                  0xfffc0000 y.
    // + 0xf98 [][][][] ???
    // + 0xf9c [][]     0x0fff camera rotation.
    //                0x3000 camera view.
    //                0xc000 camera mode.
    // + 0xfa6
    // + 0xfa2 []       set id from here as world map to load (planet, underwater, snowfield)
    // + 0xfa4 savemap memory bank 7/f start
    // +0x1013 (+0x6f) []       all mastered materia for given type exist.
    // +0x1027 (+0x83) []       flags.
    //                      0x80 - enable rain on field. If set it gradually become stronger. If unset if gradually fades away.
    // +0x10a4 [][]     character lock mask. You cant move character from and out party (in game methods)
    // +0x10a6 [][]     character availability mask. Add character here when adding it to party or enable it with opcode.
    //                     0x0001 - Cloud.
    //                     0x0002 - Barret.
    //                     0x0004 - Tifa.
    //                     0x0008 - Aerith.
    //                     0x0010 - Red XIII.
    //                     0x0020 - Yuffie.
    //                     0x0040 - Cait Sith.
    //                     0x0080 - Vincent.
    //                     0x0100 - Cid.
    //                     0x0200 - U (unknown).
    // +0x10d8 []       battle speed.
    // +0x10d9 []       battle message speed.
    // +0x10da [][]     config flags
    //                     0x0003 - sound (0x0-mono,0x1-stereo,0x2-wide)
    //                     0x000c - conroller (0x0-normal,0x1-customize)
    //                     0x0030 - cursor (0x0-initial,0x1-memory)
    //                     0x00c0 - atb/recommended/wait (0x0-active,0x1-recommended,0x2-wait)
    //                     0x0300 - camera (0x0-auto,0x1-fixed)
    //                     0x1c00 - magic order
    // +0x10dc array of button remap. Each record represent one button in buttons mask (0x10 items 0x1 size)
    //     +0x00 [] new button bit
    // +0x10ec []       field message speed.
};
