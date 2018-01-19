/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "nsSBCharSetProber.h"

/********* Language model for: Spanish *********/

/**
 * Generated by BuildLangModel.py
 * On: 2015-12-12 18:39:02.290370
 **/

/* Character Mapping Table:
 * ILL: illegal character.
 * CTR: control character specific to the charset.
 * RET: carriage/return.
 * SYM: symbol (punctuation) that does not belong to word.
 * NUM: 0 - 9.
 *
 * Other characters are ordered by probabilities
 * (0 is the most common character in the language).
 *
 * Orders are generic to a language. So the codepoint with order X in
 * CHARSET1 maps to the same character as the codepoint with the same
 * order X in CHARSET2 for the same language.
 * As such, it is possible to get missing order. For instance the
 * ligature of 'o' and 'e' exists in ISO-8859-15 but not in ISO-8859-1
 * even though they are both used for French. Same for the euro sign.
 */
static const unsigned char Iso_8859_1_CharToOrderMap[] =
{
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,RET,CTR,CTR,RET,CTR,CTR, /* 0X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 1X */
  SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* 2X */
  NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,SYM,SYM,SYM,SYM,SYM,SYM, /* 3X */
  SYM,  1, 14, 10,  8,  0, 16, 15, 20,  5, 23, 27,  7, 12,  3,  2, /* 4X */
   13, 21,  6,  4,  9, 11, 18, 31, 28, 17, 24,SYM,SYM,SYM,SYM,SYM, /* 5X */
  SYM,  1, 14, 10,  8,  0, 16, 15, 20,  5, 23, 27,  7, 12,  3,  2, /* 6X */
   13, 21,  6,  4,  9, 11, 18, 31, 28, 17, 24,SYM,SYM,SYM,SYM,CTR, /* 7X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 8X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 9X */
  SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* AX */
  SYM,SYM,SYM,SYM,SYM, 52,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* BX */
   33, 25, 39, 46, 37, 45, 47, 35, 36, 26, 48, 40, 53, 22, 41, 43, /* CX */
   49, 29, 38, 19, 50, 54, 34,SYM, 44, 51, 30, 55, 32, 42, 56, 57, /* DX */
   33, 25, 39, 46, 37, 45, 47, 35, 36, 26, 48, 40, 58, 22, 41, 43, /* EX */
   49, 29, 38, 19, 50, 59, 34,SYM, 44, 51, 30, 60, 32, 42, 61, 62, /* FX */
};
/*X0  X1  X2  X3  X4  X5  X6  X7  X8  X9  XA  XB  XC  XD  XE  XF */

static const unsigned char Iso_8859_15_CharToOrderMap[] =
{
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,RET,CTR,CTR,RET,CTR,CTR, /* 0X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 1X */
  SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* 2X */
  NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,SYM,SYM,SYM,SYM,SYM,SYM, /* 3X */
  SYM,  1, 14, 10,  8,  0, 16, 15, 20,  5, 23, 27,  7, 12,  3,  2, /* 4X */
   13, 21,  6,  4,  9, 11, 18, 31, 28, 17, 24,SYM,SYM,SYM,SYM,SYM, /* 5X */
  SYM,  1, 14, 10,  8,  0, 16, 15, 20,  5, 23, 27,  7, 12,  3,  2, /* 6X */
   13, 21,  6,  4,  9, 11, 18, 31, 28, 17, 24,SYM,SYM,SYM,SYM,CTR, /* 7X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 8X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 9X */
  SYM,SYM,SYM,SYM,SYM,SYM, 63,SYM, 64,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* AX */
  SYM,SYM,SYM,SYM, 65, 66,SYM,SYM, 67,SYM,SYM,SYM, 68, 69, 70,SYM, /* BX */
   33, 25, 39, 46, 37, 45, 47, 35, 36, 26, 48, 40, 71, 22, 41, 43, /* CX */
   49, 29, 38, 19, 50, 72, 34,SYM, 44, 51, 30, 73, 32, 42, 74, 75, /* DX */
   33, 25, 39, 46, 37, 45, 47, 35, 36, 26, 48, 40, 76, 22, 41, 43, /* EX */
   49, 29, 38, 19, 50, 77, 34,SYM, 44, 51, 30, 78, 32, 42, 79, 80, /* FX */
};
/*X0  X1  X2  X3  X4  X5  X6  X7  X8  X9  XA  XB  XC  XD  XE  XF */

static const unsigned char Windows_1252_CharToOrderMap[] =
{
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,RET,CTR,CTR,RET,CTR,CTR, /* 0X */
  CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR,CTR, /* 1X */
  SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* 2X */
  NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,NUM,SYM,SYM,SYM,SYM,SYM,SYM, /* 3X */
  SYM,  1, 14, 10,  8,  0, 16, 15, 20,  5, 23, 27,  7, 12,  3,  2, /* 4X */
   13, 21,  6,  4,  9, 11, 18, 31, 28, 17, 24,SYM,SYM,SYM,SYM,SYM, /* 5X */
  SYM,  1, 14, 10,  8,  0, 16, 15, 20,  5, 23, 27,  7, 12,  3,  2, /* 6X */
   13, 21,  6,  4,  9, 11, 18, 31, 28, 17, 24,SYM,SYM,SYM,SYM,CTR, /* 7X */
  SYM,ILL,SYM, 81,SYM,SYM,SYM,SYM,SYM,SYM, 82,SYM, 83,ILL, 84,ILL, /* 8X */
  ILL,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, 85,SYM, 86,ILL, 87, 88, /* 9X */
  SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* AX */
  SYM,SYM,SYM,SYM,SYM, 89,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM,SYM, /* BX */
   33, 25, 39, 46, 37, 45, 47, 35, 36, 26, 48, 40, 90, 22, 41, 43, /* CX */
   49, 29, 38, 19, 50, 91, 34,SYM, 44, 51, 30, 92, 32, 42, 93, 94, /* DX */
   33, 25, 39, 46, 37, 45, 47, 35, 36, 26, 48, 40, 95, 22, 41, 43, /* EX */
   49, 29, 38, 19, 50, 96, 34,SYM, 44, 51, 30, 97, 32, 42, 98, 99, /* FX */
};
/*X0  X1  X2  X3  X4  X5  X6  X7  X8  X9  XA  XB  XC  XD  XE  XF */


/* Model Table:
 * Total sequences: 897
 * First 512 sequences: 0.9970385677528184
 * Next 512 sequences (512-1024): 0.0029614322471815486
 * Rest: 4.597017211338539e-17
 * Negative sequences: TODO
 */
static const PRUint8 SpanishLangModel[] =
{
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,0,3,3,3,2,3,0,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,3,3,3,3,3,2,3,3,3,3,3,3,0,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,3,3,2,3,3,2,2,3,3,2,2,3,0,
  3,3,3,3,3,3,3,3,3,3,3,3,3,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,3,2,0,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,3,3,3,3,0,2,3,3,3,0,0,2,2,0,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,0,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,3,3,2,3,3,3,3,2,0,3,2,2,
  3,3,3,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,3,3,2,2,3,3,2,2,0,2,2,0,
  3,3,3,2,3,3,3,3,2,2,2,3,3,2,2,3,2,3,3,3,3,2,3,2,2,3,3,2,0,0,2,2,2,
  3,3,3,3,3,3,3,3,2,3,3,3,2,2,3,2,2,3,2,3,3,0,3,2,2,3,3,0,0,0,2,2,2,
  3,3,3,3,3,3,3,3,2,3,3,3,2,2,2,2,2,3,0,3,3,2,3,0,2,3,3,3,0,0,2,0,0,
  3,3,3,3,3,3,3,3,3,3,3,2,3,3,3,3,3,3,3,3,2,3,3,3,3,3,3,3,2,3,0,2,0,
  3,3,3,3,3,3,2,2,2,2,2,3,3,3,3,2,2,3,0,3,2,0,3,2,0,3,3,2,2,0,3,2,2,
  3,3,3,2,3,3,3,3,2,3,3,3,2,3,3,0,2,2,2,3,3,0,3,2,0,3,3,2,0,0,3,0,0,
  3,3,3,3,3,3,3,3,3,3,3,3,2,2,3,2,2,3,2,3,2,2,3,3,0,3,2,2,0,0,2,2,0,
  3,3,3,3,3,3,3,3,3,3,0,3,3,0,2,2,2,2,2,3,3,0,3,2,2,2,3,2,0,0,3,2,3,
  3,3,3,2,2,3,3,3,2,3,2,3,2,2,2,2,3,2,0,3,0,0,3,2,0,2,2,2,0,0,3,2,0,
  3,3,3,3,3,3,3,3,2,2,2,3,2,2,2,2,2,2,0,3,2,0,0,2,2,2,2,2,0,0,2,2,0,
  3,3,3,2,2,3,2,2,2,0,2,3,0,2,0,2,2,2,2,3,0,0,3,0,0,2,3,2,0,0,0,0,0,
  0,0,0,3,3,0,3,3,3,3,3,0,3,3,2,3,2,0,3,0,0,0,0,0,0,0,0,0,2,0,0,0,0,
  3,3,3,3,2,3,3,3,3,3,2,3,3,0,2,0,2,3,2,2,2,0,3,2,2,2,3,0,2,0,2,2,2,
  2,3,2,0,2,2,0,2,2,2,0,3,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,
  3,3,3,3,3,0,2,2,3,3,3,2,3,2,3,3,3,0,2,0,0,2,0,2,2,0,0,0,0,0,0,0,0,
  3,3,3,2,0,3,2,2,2,2,0,3,2,2,0,0,0,0,0,3,0,0,2,2,0,2,3,0,0,0,2,0,2,
  3,3,3,2,0,3,2,0,2,2,2,3,2,2,2,3,0,2,0,3,2,3,2,0,3,3,2,2,0,0,2,0,0,
  2,0,0,3,3,2,3,3,2,3,3,2,3,3,2,3,3,2,2,0,2,2,0,2,2,0,0,0,2,2,0,0,0,
  2,3,2,3,3,2,3,3,3,3,3,2,2,3,2,3,2,2,2,0,0,0,0,2,0,0,0,0,3,0,0,0,0,
  3,3,3,2,3,3,3,3,2,2,2,3,3,0,2,2,2,3,2,0,2,0,2,0,0,0,0,2,0,0,2,2,0,
  3,3,3,2,2,3,2,2,2,3,3,3,2,3,2,0,2,2,3,2,2,2,0,2,0,2,2,2,3,0,0,2,0,
  3,3,3,0,0,2,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,3,0,0,2,0,0,0,0,0,0,0,
  2,3,2,3,3,0,2,3,2,3,2,0,3,2,3,0,2,0,0,0,0,0,0,2,2,0,0,0,0,2,0,0,0,
  3,3,3,3,2,3,2,2,2,2,2,2,0,0,2,0,2,2,0,0,2,0,0,2,0,2,0,2,0,0,0,2,0,
  3,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,
};


const SequenceModel Iso_8859_1SpanishModel =
{
  Iso_8859_1_CharToOrderMap,
  SpanishLangModel,
  33,
  (float)0.9970385677528184,
  PR_TRUE,
  "ISO-8859-1"
};

const SequenceModel Iso_8859_15SpanishModel =
{
  Iso_8859_15_CharToOrderMap,
  SpanishLangModel,
  33,
  (float)0.9970385677528184,
  PR_TRUE,
  "ISO-8859-15"
};

const SequenceModel Windows_1252SpanishModel =
{
  Windows_1252_CharToOrderMap,
  SpanishLangModel,
  33,
  (float)0.9970385677528184,
  PR_TRUE,
  "WINDOWS-1252"
};