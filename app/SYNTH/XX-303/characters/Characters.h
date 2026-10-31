#pragma once

// The characters of SYNTH/XX-303 (the parameter Character), for lib/open303's Open303::setCharacter(): its own Open303,
// the TB-03's and the RB303's, fitted to recordings of the TB-03 and of ReBirth RB-338's 303 (CharacterTB03.h and
// CharacterRB303.h), the analog TB-303's, from its circuit and fitted to the TB-03 where that leaves open
// (CharacterTB303.h), and AudioRealism ABL3's, fitted to recordings of it (CharacterABL3.h). In this order: a patch
// keeps the index, so new ones are appended.

#include "open303/src/rosic_CharacterOpen303.h"
#include "CharacterTB03.h"
#include "CharacterRB303.h"   // ReBirth RB-338's 303
#include "CharacterTB303.h"   // the analog TB-303, from its circuit (the rest fitted to the TB-03)
#include "CharacterABL3.h"    // AudioRealism ABL3

static const rosic::Character *const characters[] = {&rosic::characterOpen303, &characterTB03, &characterRB303,
                                                     &characterTB303, &characterABL3};
static const char *characterNames[] = {"Open303", "TB03", "RB303", "TB303", "ABL3"};
static constexpr int numCharacters = sizeof(characters) / sizeof(characters[0]);
