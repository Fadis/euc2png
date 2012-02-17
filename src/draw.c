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

#include <draw.h>
#include <map.h>
#include <glyph.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <IL/il.h>


// ページ情報を初期化
void clearPage ( Page *_page ) {
  // ページを真っ白にする
  memset ( _page->buffer, 255, sizeof ( uint8_t ) * PAGE_SIZE_X * 6 * PAGE_SIZE_Y * 8 );
  // 現在のカーソル位置を左上にする
  _page->current_x = 0;
  _page->current_y = 0;
  // 現在のページを0ページ目にする
  _page->current_page = 0;
}

// 改ページ
void nextPage ( Page *_page ) {
  // ページを真っ白にする
  memset ( _page->buffer, 255, sizeof ( uint8_t ) * PAGE_SIZE_X * 6 * PAGE_SIZE_Y * 8 );
  // 現在のカーソル位置を左上にする
  _page->current_x = 0;
  _page->current_y = 0;
  // 現在のページを進める
  _page->current_page++;
}

// 改行
void newLinePage ( Page *_page ) {
  // 現在のカーソル一を一つ下の行の左端にする
  _page->current_x = 0;
  _page->current_y++;
}

// カーソル前進
void advancePage ( Page *_page ) {
  // カーソルを右に一つ進める
  _page->current_x++;
  // カーソルがページの右端を越えていたら
  if ( _page->current_x == PAGE_SIZE_X )
    // 改行する
    newLinePage ( _page );
}

// 改ページ必要性のチェック
int isFullPage ( Page *_page ) {
  // カーソルがページの下端を越えていたら1を返す
  if ( _page->current_y == PAGE_SIZE_Y )
    return 1;
  else
    return 0;
}

// ページに点を打つ座標は現在のカーソル位置からの相対座標
int dotPage ( Page *_page, int _x, int _y ) {
  // ページ上の絶対座標を求める
  int abs_x = _page->current_x * 6 + _x;
  int abs_y = ( PAGE_SIZE_Y - _page->current_y - 1 ) * 8 + _y;
  // バッファの対応する箇所を黒にする
  _page->buffer[ abs_x + abs_y * PAGE_SIZE_X * 6 ] = 0;
}

// ページに文字を描く
void typePage ( Page *_page, uint16_t _index ) {
  int hindex;
  int vindex;
  // インデックスで指定されたグリフを読んで対応箇所に点を打つ
  for ( hindex = 0; hindex != 5; hindex++ ) {
    uint8_t temp = glyph[ _index * 5 + hindex ];
    for ( vindex = 0; vindex != 8; vindex++, temp <<= 1 )
      if ( temp & 0x80 )
        dotPage ( _page, hindex, vindex );
  }
}

// ページを書き出す
void dumpPage ( Page *_page, const char *_name ) {
  // 画像出力はDevIL任せ
  ilTexImage ( PAGE_SIZE_X * 6, PAGE_SIZE_Y * 8, 1, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, ( void* ) ( _page->buffer ) );
  char filename[ MAX_FILENAME_LENGTH ];
  snprintf ( filename, 256, "%s_%02d.png", _name, _page->current_page );
  ilSaveImage ( filename );
}
