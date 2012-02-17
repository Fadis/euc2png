/***************************************************************************
 *   Copyright (C) 2010 by Naomasa Matsubayashi   *
 *   fadis@quaternion.sakura.ne.jp   *
 *                                                                         *
 *   All rights reserved.                                                  *
 *                                                                         *
 * Redistribution and use in source and binary forms, with or without      *
 * modification, are permitted provided that the following conditions are  *
 * met:                                                                    *
 *                                                                         *
 *  1. Redistributions of source code must retain the above copyright      *
 *     notice, this list of conditions and the following disclaimer.       *
 *  2. Redistributions in binary form must reproduce the above copyright   *
 *     notice, this list of conditions and the following disclaimer in the *
 *     documentation and/or other materials provided with the distribution.*
 *                                                                         *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT      *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    *
 ***************************************************************************/

#ifndef EUC2PNG_DRAW_H
#define EUC2PNG_DRAW_H

#include <stdint.h>

// ファイル名の最大長
#define MAX_FILENAME_LENGTH 256

// ページの縦横の文字数
#define PAGE_SIZE_X 40
#define PAGE_SIZE_Y 40

// 出力ページ情報
typedef struct {
  // ページ画像を一時的に保持するバッファ
  uint8_t buffer[ PAGE_SIZE_X * 6 * PAGE_SIZE_Y * 8 ];
  // 現在のX軸方向のカーソル位置(n文字目)
  int current_x;
  // 現在のY軸方向のカーソル位置(n文字目)
  int current_y;
  // 現在のページ
  int current_page;
} Page;

void clearPage ( Page *_page );
void nextPage ( Page *_page );
void newLinePage ( Page *_page );
void advancePage ( Page *_page );
int isFullPage ( Page *_page );
int dotPage ( Page *_page, int _x, int _y );
void typePage ( Page *_page, uint16_t _index );
void dumpPage ( Page *_page, const char *_name );

#endif
