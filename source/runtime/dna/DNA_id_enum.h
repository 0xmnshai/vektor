#pragma once

namespace vektor::dna {
#define MAKE_ID2(c, d) ((d) << 8 | (c))

enum ID_Type {
  ID_SCE = MAKE_ID2('S', 'C'), /* Scene */
  ID_OB = MAKE_ID2('O', 'B'),  /* Object */
  ID_ME = MAKE_ID2('M', 'E'),  /* Mesh */
  ID_MA = MAKE_ID2('M', 'A'),  /* Material */
  ID_TE = MAKE_ID2('T', 'E'),  /* Tex (Texture) */
  ID_IM = MAKE_ID2('I', 'M'),  /* Image */
  ID_LA = MAKE_ID2('L', 'A'),  /* Light */
  ID_CA = MAKE_ID2('C', 'A'),  /* Camera */
  ID_KE = MAKE_ID2('K', 'E'),  /* Key (shape key) */
  ID_WO = MAKE_ID2('W', 'O'),  /* World */
  ID_TXT = MAKE_ID2('T', 'X'), /* Text */
  ID_SO = MAKE_ID2('S', 'O'),  /* Sound */
  ID_WM = MAKE_ID2('W', 'M'),  /* wmWindowManager */
  ID_CF = MAKE_ID2('C', 'F'),  /* CacheFile */
  ID_WS = MAKE_ID2('W', 'S'),  /* WorkSpace */
};

}  // namespace vektor::dna
