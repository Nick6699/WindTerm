/**********************************************************************
  utf32_be.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2007  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "regenc.h"

static int
utf32be_mbc_enc_len(const UChar* p ARG_UNUSED)
{
  return 4;
}

static int
utf32be_mbc_enc_len_se(OnigIterator* it ARG_UNUSED, OnigPosition p ARG_UNUSED)
{
  return 4;
}

static int
utf32be_is_mbc_newline(const UChar* p, const UChar* end)
{
  if (p + 3 < end) {
    if (*(p+3) == 0x0a && *(p+2) == 0 && *(p+1) == 0 && *p == 0)
      return 1;
#ifdef USE_UNICODE_ALL_LINE_TERMINATORS
    if ((*(p+3) == 0x0b || *(p+3) == 0x0c || *(p+3) == 0x0d || *(p+3) == 0x85)
	&& *(p+2) == 0 && *(p+1) == 0 && *p == 0x00)
      return 1;
    if (*(p+2) == 0x20 && (*(p+3) == 0x29 || *(p+3) == 0x28)
	&& *(p+1) == 0 && *p == 0)
      return 1;
#endif
  }
  return 0;
}

static int
utf32be_is_mbc_newline_se(OnigIterator* it, OnigPosition p, OnigPosition end)
{
  if (p + 3 < end) {
	const UChar c2 = ONIG_CHARAT(p+2);
    const UChar c3 = ONIG_CHARAT(p+3);

    if (c3 == 0x0a && c2 == 0 && ONIG_CHARAT(p+1) == 0 && ONIG_CHARAT(p) == 0)
      return 1;
#ifdef USE_UNICODE_ALL_LINE_TERMINATORS
    if ((c3 == 0x0b || c3 == 0x0c || c3 == 0x0d || c3 == 0x85)
	&& c2 == 0 && ONIG_CHARAT(p+1) == 0 && ONIG_CHARAT(p) == 0x00)
      return 1;
    if (c2 == 0x20 && (c3 == 0x29 || c3 == 0x28)
	&& ONIG_CHARAT(p+1) == 0 && ONIG_CHARAT(p) == 0)
      return 1;
#endif
  }
  return 0;
}

static OnigCodePoint
utf32be_mbc_to_code(const UChar* p, const UChar* end ARG_UNUSED)
{
  return (OnigCodePoint )(((p[0] * 256 + p[1]) * 256 + p[2]) * 256 + p[3]);
}

static OnigCodePoint
utf32be_mbc_to_code_se(OnigIterator* it, OnigPosition p, OnigPosition end ARG_UNUSED)
{
  return (OnigCodePoint )(((ONIG_CHARAT(p) * 256 + ONIG_CHARAT(p+1)) * 256 + ONIG_CHARAT(p+2)) * 256 + ONIG_CHARAT(p+3));
}

static int
utf32be_code_to_mbclen(OnigCodePoint code ARG_UNUSED)
{
  return 4;
}

static int
utf32be_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  UChar* p = buf;

  *p++ = (UChar )((code & 0xff000000) >>24);
  *p++ = (UChar )((code & 0xff0000)   >>16);
  *p++ = (UChar )((code & 0xff00)     >> 8);
  *p++ = (UChar ) (code & 0xff);
  return 4;
}

static int
utf32be_mbc_case_fold(OnigCaseFoldType flag,
		      const UChar** pp, const UChar* end, UChar* fold)
{
  const UChar* p = *pp;

  if (ONIGENC_IS_ASCII_CODE(*(p+3)) && *(p+2) == 0 && *(p+1) == 0 && *p == 0) {
    *fold++ = 0;
    *fold++ = 0;

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
      if (*(p+3) == 0x49) {
	*fold++ = 0x01;
	*fold   = 0x31;
	(*pp) += 4;
	return 4;
      }
    }
#endif

    *fold++ = 0;
    *fold   = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*(p+3));
    *pp += 4;
    return 4;
  }
  else
    return onigenc_unicode_mbc_case_fold(ONIG_ENCODING_UTF32_BE, flag, pp, end,
					 fold);
}

static int
utf32be_mbc_case_fold_se(OnigIterator* it, OnigCaseFoldType flag,
		      OnigPosition* pp, OnigPosition end, UChar* fold)
{
  OnigPosition p = *pp;
  const UChar c3 = ONIG_CHARAT(p+3);

  if (ONIGENC_IS_ASCII_CODE(c3) && ONIG_CHARAT(p+2) == 0 && ONIG_CHARAT(p+1) == 0 && ONIG_CHARAT(p) == 0) {
    *fold++ = 0;
    *fold++ = 0;

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
      if (c3 == 0x49) {
	*fold++ = 0x01;
	*fold   = 0x31;
	(*pp) += 4;
	return 4;
      }
    }
#endif

    *fold++ = 0;
    *fold   = ONIGENC_ASCII_CODE_TO_LOWER_CASE(c3);
    *pp += 4;
    return 4;
  }
  else
    return onigenc_unicode_mbc_case_fold_se(it, ONIG_ENCODING_UTF32_BE, flag, pp, end,
					 fold);
}

#if 0
static int
utf32be_is_mbc_ambiguous(OnigCaseFoldType flag, const UChar** pp, const UChar* end)
{
  const UChar* p = *pp;

  (*pp) += 4;

  if (*(p+2) == 0 && *(p+1) == 0 && *p == 0) {
    int c, v;

    p += 3;
    if (*p == 0xdf && (flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
      return TRUE;
    }

    c = *p;
    v = ONIGENC_IS_UNICODE_ISO_8859_1_BIT_CTYPE(c,
                       (BIT_CTYPE_UPPER | BIT_CTYPE_LOWER));
    if ((v | BIT_CTYPE_LOWER) != 0) {
      /* 0xaa, 0xb5, 0xba are lower case letter, but can't convert. */
      if (c >= 0xaa && c <= 0xba)
	return FALSE;
      else
	return TRUE;
    }
    return (v != 0 ? TRUE : FALSE);
  }

  return FALSE;
}
#endif

static UChar*
utf32be_left_adjust_char_head(const UChar* start, const UChar* s)
{
  int rem;

  if (s <= start) return (UChar* )s;

  rem = (int )((s - start) % 4);
  return (UChar* )(s - rem);
}

static OnigPosition
utf32be_left_adjust_char_head_se(OnigIterator* it, OnigPosition start, OnigPosition s)
{
  int rem;

  if (s <= start) return s;

  rem = (int )((s - start) % 4);
  return (s - rem);
}

static int
utf32be_get_case_fold_codes_by_str(OnigCaseFoldType flag,
    const OnigUChar* p, const OnigUChar* end, OnigCaseFoldCodeItem items[])
{
  return onigenc_unicode_get_case_fold_codes_by_str(ONIG_ENCODING_UTF32_BE,
						    flag, p, end, items);
}

OnigEncodingType OnigEncodingUTF32_BE = {
  utf32be_mbc_enc_len,
  utf32be_mbc_enc_len_se,
  "UTF-32BE",   /* name */
  4,            /* max byte length */
  4,            /* min byte length */
  utf32be_is_mbc_newline,
  utf32be_is_mbc_newline_se,
  utf32be_mbc_to_code,
  utf32be_mbc_to_code_se,
  utf32be_code_to_mbclen,
  utf32be_code_to_mbc,
  utf32be_mbc_case_fold,
  utf32be_mbc_case_fold_se,
  onigenc_unicode_apply_all_case_fold,
  utf32be_get_case_fold_codes_by_str,
  onigenc_unicode_property_name_to_ctype,
  onigenc_unicode_is_code_ctype,
  onigenc_utf16_32_get_ctype_code_range,
  utf32be_left_adjust_char_head,
  utf32be_left_adjust_char_head_se,
  onigenc_always_false_is_allowed_reverse_match,
  ONIGENC_FLAG_UNICODE,
};
