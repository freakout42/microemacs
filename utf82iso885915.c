/* UTF-8 to ISO-8859-1/ISO-8859-15 mapper.
 * Return 0..255 for valid ISO-8859-15 code points, 256 otherwise.
 */
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#define MAXMAPS 32
static int usednoniso;
static int nonisofree = 0;

static int iso2ucodet[128] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0xa0,0xa1,0xa2,0xa3,0x20ac,0xa5,0x0160,0xa7,0x0161,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
  0xb0,0xb1,0xb2,0xb3,0x017d,0xb5,0xb6,0xb7,0x017e,0xb9,0xba,0xbb,0x0152,0x0153,0x0175,0xbf,
  0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
  0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
  0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
  0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
};

int iso2ucode(unsigned char cod) {
  return cod < 128U ? cod : iso2ucodet[cod - 128];
}

int utf162utf8(char* out, int c) {
int bits;
char *out1;
out1 = out;
if      (c <    0x80) {  *out++=  c;                         bits = -6; }
else if (c <   0x800) {  *out++= ((c >>  6) & 0x1F) | 0xC0;  bits =  0; }
else if (c < 0x10000) {  *out++= ((c >> 12) & 0x0F) | 0xE0;  bits =  6; }
else                  {  *out++= ((c >> 18) & 0x07) | 0xF0;  bits = 12; }
for ( ; bits >= 0; bits -= 6) *out++= ((c >> bits) & 0x3F) | 0x80;
return out - out1;
}

int to_utf8(char *buf, int nbuf) {
  char *buf2;
  char *src;
  char *tgt;
  buf2 = src = malloc(nbuf + 1);
  strncpy(buf2, buf, nbuf);
  tgt = buf;
  while ((src - buf2) < nbuf) tgt += utf162utf8(tgt, iso2ucode(*src++));
  free(buf2);
  return tgt - buf;
}

int to_latin9(const unsigned int code) {
  int i;
  int u = 0;
  if (code < 256U) return code;
  for (i=0; i < 128; i++) if (iso2ucodet[i] == code) { return i + 128; }
  if (nonisofree < MAXMAPS) { iso2ucodet[nonisofree++] = code; return nonisofree + 127; }
  else { usednoniso = -1; return 191U; } /* U+00BF = 0xBF: ? inverted */
}

/* Convert an UTF-8 string to ISO-8859-15.
 * All invalid sequences are ignored.
 * Note: output == input is allowed,
 * but   input < output < input + length is not.
 * Output has to have room for (length+1) chars, including the trailing NUL byte.
 */
size_t utf8_to_latin9(char *output, char *input, size_t length) {
    unsigned char *out;
    unsigned char *in;
    unsigned char *end;
    unsigned int  c;

    out = output;
    in  = input;
    end = input + length;
    while (in < end)
        if (*in < 128)
            *(out++) = *(in++); /* Valid codepoint */

        else
        if (*in < 192)
            in++;               /* 10000000 .. 10111111 are invalid */

        else
        if (*in < 224) {        /* 110xxxxx 10xxxxxx */
            if (in + 1 >= end)
                break;
            if ((in[1] & 192U) == 128U) {
                c = to_latin9( (((unsigned int)(in[0] & 0x1FU)) << 6U)
                             |  ((unsigned int)(in[1] & 0x3FU)) );
                if (c < 256)
                    *(out++) = c;
            }
            in += 2;

        } else
        if (*in < 240) {        /* 1110xxxx 10xxxxxx 10xxxxxx */
            if (in + 2 >= end)
                break;
            if ((in[1] & 192U) == 128U &&
                (in[2] & 192U) == 128U) {
                c = to_latin9( (((unsigned int)(in[0] & 0x0FU)) << 12U)
                             | (((unsigned int)(in[1] & 0x3FU)) << 6U)
                             |  ((unsigned int)(in[2] & 0x3FU)) );
                if (c < 256)
                    *(out++) = c;
            }
            in += 3;

        } else
        if (*in < 248) {        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
            if (in + 3 >= end)
                break;
            if ((in[1] & 192U) == 128U &&
                (in[2] & 192U) == 128U &&
                (in[3] & 192U) == 128U) {
                c = to_latin9( (((unsigned int)(in[0] & 0x07U)) << 18U)
                             | (((unsigned int)(in[1] & 0x3FU)) << 12U)
                             | (((unsigned int)(in[2] & 0x3FU)) << 6U)
                             |  ((unsigned int)(in[3] & 0x3FU)) );
                if (c < 256)
                    *(out++) = c;
            }
            in += 4;

        } else
        if (*in < 252) {        /* 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
            if (in + 4 >= end)
                break;
            if ((in[1] & 192U) == 128U &&
                (in[2] & 192U) == 128U &&
                (in[3] & 192U) == 128U &&
                (in[4] & 192U) == 128U) {
                c = to_latin9( (((unsigned int)(in[0] & 0x03U)) << 24U)
                             | (((unsigned int)(in[1] & 0x3FU)) << 18U)
                             | (((unsigned int)(in[2] & 0x3FU)) << 12U)
                             | (((unsigned int)(in[3] & 0x3FU)) << 6U)
                             |  ((unsigned int)(in[4] & 0x3FU)) );
                if (c < 256)
                    *(out++) = c;
            }
            in += 5;

        } else
        if (*in < 254) {        /* 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
            if (in + 5 >= end)
                break;
            if ((in[1] & 192U) == 128U &&
                (in[2] & 192U) == 128U &&
                (in[3] & 192U) == 128U &&
                (in[4] & 192U) == 128U &&
                (in[5] & 192U) == 128U) {
                c = to_latin9( (((unsigned int)(in[0] & 0x01U)) << 30U)
                             | (((unsigned int)(in[1] & 0x3FU)) << 24U)
                             | (((unsigned int)(in[2] & 0x3FU)) << 18U)
                             | (((unsigned int)(in[3] & 0x3FU)) << 12U)
                             | (((unsigned int)(in[4] & 0x3FU)) << 6U)
                             |  ((unsigned int)(in[5] & 0x3FU)) );
                if (c < 256)
                    *(out++) = c;
            }
            in += 6;

        } else
            in++;               /* 11111110 and 11111111 are invalid */

    *out = '\0';

    return (size_t)(out - (unsigned char *)output);
}

int to_utf16(char *buf, int nbuf) {
  char *buf2;
  buf2 = strdup(buf);
  int i, len;
  usednoniso = 1;
  len = utf8_to_latin9(buf, buf2, strlen(buf2));
  free(buf2);
  return len * usednoniso;
}

#ifdef LIBVERSION
unsigned int to_latin9(const unsigned int code)
{
    /* Code points 0 to U+00FF are the same in both. */
    if (code < 256U) return code;
    switch (code) {
    case 0x0152U:    return 188U; /* U+0152 = 0xBC: OE ligature */
    case 0x0153U:    return 189U; /* U+0153 = 0xBD: oe ligature */
    case 0x0160U:    return 166U; /* U+0160 = 0xA6: S with caron */
    case 0x0161U:    return 168U; /* U+0161 = 0xA8: s with caron */
    case 0x0178U:    return 190U; /* U+0178 = 0xBE: Y with diaresis */
    case 0x017DU:    return 180U; /* U+017D = 0xB4: Z with caron */
    case 0x017EU:    return 184U; /* U+017E = 0xB8: z with caron */
    case 0x20ACU:    return 164U; /* U+20AC = 0xA4: Euro */
    default: usednoniso = -1;
                     return 191U; /* U+00BF = 0xBF: ? inverted */
    }
}

unsigned int to_ucpoint(const unsigned int code)
{
    /* only ISO-8859-15 exceptions are converted */
    if (code < 128U || code > 256U) return code;
    switch (code) {
    case 188U:    return 0x0152U; /* U+0152 = 0xBC: OE ligature */
    case 189U:    return 0x0153U; /* U+0153 = 0xBD: oe ligature */
    case 166U:    return 0x0160U; /* U+0160 = 0xA6: S with caron */
    case 168U:    return 0x0161U; /* U+0161 = 0xA8: s with caron */
    case 190U:    return 0x0178U; /* U+0178 = 0xBE: Y with diaresis */
    case 180U:    return 0x017DU; /* U+017D = 0xB4: Z with caron */
    case 184U:    return 0x017EU; /* U+017E = 0xB8: z with caron */
    case 164U:    return 0x20ACU; /* U+20AC = 0xA4: Euro */
    default:      return code;
    }
}

int to_utf8(char *buf, int nbuf) {
  wchar_t *se;
  int len, i;
  se = malloc((nbuf+2) * sizeof(wchar_t));
  for (i=0; i<nbuf; i++) se[i] = to_ucpoint((unsigned char)buf[i]);
                         se[i] = '\0';
  len = wcstombs(buf, se, nbuf*2);
  free(se);
  return len;
}

int to_utf16(char *buf, int nbuf) {
  wchar_t *se;
  int i, len;
  se = malloc((nbuf+2) * sizeof(wchar_t));
  len = mbstowcs(se, buf, nbuf+2);
  usednoniso = 1;
  for (i=0; i<=len; i++) buf[i] = to_latin9(se[i]);
  free(se);
  return len * usednoniso;
}
#endif
