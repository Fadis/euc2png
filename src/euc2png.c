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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <map.h>
#include <glyph.h>
#include <draw.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <IL/il.h>

// マルチバイト文字が途中で切れると処理が面倒なので入力テキストファイルはmmapする
typedef struct {
  // mmap先頭アドレス
  uint8_t *addr;
  // ファイルディスクリプタ
  int fd;
  // mmapされた領域の長さ
  unsigned int length;
} Map;

// テキストを画像にする
void draw( Page *_page, const char *_name, Map *_map ) {
  // 現在処理している文字の先頭バイトを指すポインタ
  uint8_t *string = _map->addr;
  // テキストの終端を指すポインタ
  uint8_t *end = _map->addr += _map->length;
  while( string != end ) {
    // UNIX/Windowsスタイル改行コードを見つけたら
    if( string[ 0 ] == 0x0A ) {
      // 改行する
      newLinePage( _page );
      // Windowsスタイル改行コードの場合改行コードが2バイトなので次のバイトを読み飛ばす
      if( string[ 1 ] == 0x0D )
        string += 2;
      else
        string++;
    }
    // MacOSスタイルの改行コードを見つけたら
    else if( string[ 0 ] == 0x0D ) {
      // 改行する
      newLinePage( _page );
      string++;
    }
    // それ以外の文字だったら
    else {
      // 描く
      typePage( _page, getGlyphEUC( string ) );
      // カーソルを進める
      advancePage( _page );
      // 現在処理している文字の先頭バイトを進める
      string = nextCharEUC( string );
    }
    // ページが一杯になっていたら
    if( isFullPage( _page ) ) {
      // 現在のページを書き出して
      dumpPage( _page, _name );
      // 改ページする
      nextPage( _page );
    }
  }
  // 最後にバッファに残った分を書き出す
  dumpPage( _page, _name );
}

// 入力テキストファイルをマップする
void mapFile( Map *_map, const char *_name ) {
  struct stat file_stat;
  stat( _name, &file_stat );
  _map->length = file_stat.st_size;
  _map->fd = open( _name, O_RDONLY );
  _map->addr = (uint8_t*)mmap( NULL, _map->length, PROT_READ, MAP_SHARED, _map->fd, 0 );
}

// 入力テキストファイルをアンマップする
void unmapFile( Map *_map ) {
  munmap( _map->addr, _map->length );
  close( _map->fd );
}

// 引数を調べる
void parseOptions( int argc, char * const argv[], char *_input_filename, char *_output_filename ) {
  static struct option long_options[ 2 ] = {
    { "input", 1, 0, 'i'},
    { "output", 1, 0, 'o'},
  };
  strncpy( _input_filename, "source.txt", MAX_FILENAME_LENGTH );
  strncpy( _output_filename, "dump", MAX_FILENAME_LENGTH );
  int value;
  while( ( value = getopt_long( argc, argv, "i:o:", long_options, NULL ) ) != -1 ) {
    switch( value ) {
      case 'i':
        strncpy( _input_filename, optarg, MAX_FILENAME_LENGTH );
        break;
      case 'o':
        strncpy( _output_filename, optarg, MAX_FILENAME_LENGTH );
        break;
    };
  };
}

// main関数
// エラーチェックとかしてないから常用するならもう少しきちんと書き直した方が良い
int main( int argc, char *argv[] ) {
  Map map;
  char input_filename[ MAX_FILENAME_LENGTH ];
  char output_filename[ MAX_FILENAME_LENGTH ];
  parseOptions( argc, argv, input_filename, output_filename );
  mapFile( &map, input_filename );
  ilInit();
  Page page;
  clearPage( &page );
  draw( &page, output_filename, &map );
  unmapFile( &map );
}
