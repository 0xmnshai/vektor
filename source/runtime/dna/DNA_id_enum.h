

#pragma once

namespace blender {

enum eIconSizes {
  ICON_SIZE_ICON = 0,
  ICON_SIZE_PREVIEW = 1,

  NUM_ICON_SIZES,
};

enum eIDPropertyType {
  IDP_STRING = 0,
  IDP_INT = 1,
  IDP_FLOAT = 2,

  IDP_ARRAY = 5,
  IDP_GROUP = 6,
  IDP_ID = 7,
  IDP_DOUBLE = 8,
  IDP_IDPARRAY = 9,

  IDP_BOOLEAN = 10,
};
#define IDP_NUMTYPES 11

enum {
  IDP_TYPE_FILTER_STRING = 1 << IDP_STRING,
  IDP_TYPE_FILTER_INT = 1 << IDP_INT,
  IDP_TYPE_FILTER_FLOAT = 1 << IDP_FLOAT,
  IDP_TYPE_FILTER_ARRAY = 1 << IDP_ARRAY,
  IDP_TYPE_FILTER_GROUP = 1 << IDP_GROUP,
  IDP_TYPE_FILTER_ID = 1 << IDP_ID,
  IDP_TYPE_FILTER_DOUBLE = 1 << IDP_DOUBLE,
  IDP_TYPE_FILTER_IDPARRAY = 1 << IDP_IDPARRAY,
  IDP_TYPE_FILTER_BOOLEAN = 1 << IDP_BOOLEAN,
};

enum eIDPropertySubType {
  IDP_STRING_SUB_UTF8 = 0,
  IDP_STRING_SUB_BYTE = 1,
};

enum eIDPropertyFlag {

  IDP_FLAG_OVERRIDABLE_LIBRARY = 1 << 0,

  IDP_FLAG_OVERRIDELIBRARY_LOCAL = 1 << 1,

  IDP_FLAG_STATIC_TYPE = 1 << 4,

  IDP_FLAG_GHOST = 1 << 7,
};

#define MAKE_ID2(c, d) ((d) << 8 | (c))

enum ID_Type {
  ID_SCE = MAKE_ID2('S', 'C'),
  ID_LI = MAKE_ID2('L', 'I'),
  ID_OB = MAKE_ID2('O', 'B'),
  ID_ME = MAKE_ID2('M', 'E'),
  ID_CU_LEGACY = MAKE_ID2('C', 'U'),
  ID_MB = MAKE_ID2('M', 'B'),
  ID_MA = MAKE_ID2('M', 'A'),
  ID_TE = MAKE_ID2('T', 'E'),
  ID_IM = MAKE_ID2('I', 'M'),
  ID_LT = MAKE_ID2('L', 'T'),
  ID_LA = MAKE_ID2('L', 'A'),
  ID_CA = MAKE_ID2('C', 'A'),
  ID_KE = MAKE_ID2('K', 'E'),
  ID_WO = MAKE_ID2('W', 'O'),
  ID_SCR = MAKE_ID2('S', 'R'),
  ID_VF = MAKE_ID2('V', 'F'),
  ID_TXT = MAKE_ID2('T', 'X'),
  ID_SPK = MAKE_ID2('S', 'K'),
  ID_SO = MAKE_ID2('S', 'O'),
  ID_GR = MAKE_ID2('G', 'R'),
  ID_AR = MAKE_ID2('A', 'R'),
  ID_AC = MAKE_ID2('A', 'C'),
  ID_NT = MAKE_ID2('N', 'T'),
  ID_BR = MAKE_ID2('B', 'R'),
  ID_PA = MAKE_ID2('P', 'A'),
  ID_GD_LEGACY = MAKE_ID2('G', 'D'),
  ID_WM = MAKE_ID2('W', 'M'),
  ID_MC = MAKE_ID2('M', 'C'),
  ID_MSK = MAKE_ID2('M', 'S'),
  ID_LS = MAKE_ID2('L', 'S'),
  ID_PAL = MAKE_ID2('P', 'L'),
  ID_PC = MAKE_ID2('P', 'C'),
  ID_CF = MAKE_ID2('C', 'F'),
  ID_WS = MAKE_ID2('W', 'S'),
  ID_LP = MAKE_ID2('L', 'P'),
  ID_CV = MAKE_ID2('C', 'V'),
  ID_PT = MAKE_ID2('P', 'T'),
  ID_VO = MAKE_ID2('V', 'O'),
  ID_GP = MAKE_ID2('G', 'P'),
};

#define ID_LINK_PLACEHOLDER MAKE_ID2('I', 'D')

#define ID_SCRN MAKE_ID2('S', 'N')

#define ID_SEQ MAKE_ID2('S', 'Q')

#define ID_CO MAKE_ID2('C', 'O')

#define ID_PO MAKE_ID2('A', 'C')

#define ID_NLA MAKE_ID2('N', 'L')

#define ID_FLUIDSIM MAKE_ID2('F', 'S')

}
